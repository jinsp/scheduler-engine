package com.sos.scheduler.engine.taskserver.modules.shell

import com.sos.scheduler.engine.agent.data.ProcessKillScript
import com.sos.scheduler.engine.base.process.ProcessSignal
import com.sos.scheduler.engine.base.process.ProcessSignal._
import com.sos.scheduler.engine.common.process.StdoutStderr.StdoutStderrType
import com.sos.scheduler.engine.common.process.windows.{Logon, WindowsProcess, WindowsProcessCredentials}
import com.sos.scheduler.engine.common.scalautil.AutoClosing.autoClosing
import com.sos.scheduler.engine.common.scalautil.Closers.implicits.RichClosersAutoCloseable
import com.sos.scheduler.engine.common.scalautil.FileUtils.implicits._
import com.sos.scheduler.engine.common.scalautil.{HasCloser, Logger, SetOnce}
import com.sos.scheduler.engine.common.utils.JavaShutdownHook
import com.sos.scheduler.engine.common.xml.VariableSets
import com.sos.scheduler.engine.taskserver.common.ConcurrentStdoutAndStderrWell
import com.sos.scheduler.engine.taskserver.data.TaskServerConfiguration._
import com.sos.scheduler.engine.taskserver.data.TaskServerMainTerminated
import com.sos.scheduler.engine.taskserver.modules.common.{CommonArguments, Task}
import com.sos.scheduler.engine.taskserver.modules.monitor.MonitorProcessor
import com.sos.scheduler.engine.taskserver.modules.shell.ShellProcessTask._
import com.sos.scheduler.engine.taskserver.modules.shell.TaskVariables.taskStartVariables
import com.sos.scheduler.engine.taskserver.task.process.ShellScriptProcess.startShellScript
import com.sos.scheduler.engine.taskserver.task.process.{ProcessConfiguration, RichProcess}
import java.io.FileInputStream
import java.nio.file.Files._
import java.nio.file.Path
import java.time.Instant.now
import java.util.Locale
import org.jetbrains.annotations.TestOnly
import org.scalactic.Requirements._
import scala.concurrent.duration.Duration.Inf
import scala.concurrent.{Await, ExecutionContext, Future}
import scala.io

/**
 * @author Joacim Zschimmer
  * @see spooler_module_process.cxx, C++ class Process_module_instance
 */
private[taskserver] final class ShellProcessTask(
  module: ShellModule,
  protected val commonArguments: CommonArguments,
  environment: Map[String, String],
  variablePrefix: String,
  logDirectory: Path,
  logFilenamePart: String,
  killScriptOption: Option[ProcessKillScript],
  synchronizedStartProcess: RichProcessStartSynchronizer,
  taskServerMainTerminatedOption: Option[Future[TaskServerMainTerminated.type]] = None)
  (implicit executionContext: ExecutionContext)
extends HasCloser with Task {

  import commonArguments.{agentTaskId, hasOrder, jobName, monitors, namedIDispatches, stdFiles}
  import namedIDispatches.spoolerTask

  private val monitorProcessor = MonitorProcessor.create(monitors, namedIDispatches, commonArguments.stdFiles.stderrLogLevel).closeWithCloser
  private var orderParamsFile: Path = null
  private var processStdFileMap: Map[StdoutStderrType, Path] = Map()
  private var concurrentStdoutStderrWell: ConcurrentStdoutAndStderrWell = null
  private var startCalled = false
  private val richProcessOnce = new SetOnce[RichProcess]
  private val logger = Logger.withPrefix[ShellProcessTask](toString)
  private var sigtermForwarder: Option[JavaShutdownHook] = None
  @volatile private var errorLogged = false

  def start() = {
    requireState(!startCalled)
    startCalled = true
    val precondition = monitorProcessor.preTask() && {
      onClose {
        monitorProcessor.postTask()
      }
      monitorProcessor.preStep()
    }
    if (precondition)
      startProcess()
    else
      Future.successful(false)
  }

  private def startProcess(): Future[Boolean] = {
    sigtermForwarder = for (terminated ← taskServerMainTerminatedOption) yield
      JavaShutdownHook.add(ShellProcessTask.getClass.getName) {
        sendProcessSignal(SIGTERM)
        Await.ready(terminated, Inf)  // Delay until TaskServer has been terminated
      }
    val logon = for (logon ← commonArguments.logon) yield
      Logon(WindowsProcessCredentials.byKey(logon.credentialsKey), withUserProfile = logon.withUserProfile)
    logon map { _.user } match {
      case Some(user) ⇒
        orderParamsFile = createTempFile("sos-", ".tmp")
        WindowsProcess.makeFileAppendableForUser(orderParamsFile, user)
      case None ⇒
        orderParamsFile = createTempFile("sos-", ".tmp")
    }
    val env = {
      val params = // First upper-case, then merge, to handle case conflicts properly
        spoolerTask.parameterMap.map { case (k, v) ⇒ k.toUpperCase(Locale.ROOT) → v } ++
        spoolerTask.orderParameterMap.map { case (k, v) ⇒ k.toUpperCase(Locale.ROOT) → v }
      val paramEnv = params map { case (k, v) ⇒ (variablePrefix concat k) → v }
      environment +
        (ReturnValuesFileEnvironmentVariableName → orderParamsFile.toAbsolutePath.toString) ++
        taskStartVariables(now) ++
        paramEnv
    }
    val (agentTaskIdOption, killScriptFileOption) =
      if (taskServerMainTerminatedOption.nonEmpty) (None, None)
      else (Some(agentTaskId), killScriptOption)  // No idString if this is an own process (due to a monitor), already started with idString
    if (stdFiles.isEmpty) {
      processStdFileMap = RichProcess.createStdFiles(logDirectory, id = logFilenamePart, logon map { _.user })
    }
    concurrentStdoutStderrWell = new ConcurrentStdoutAndStderrWell(s"Job $jobName",
      stdFiles.copy(
        stdFileMap = processStdFileMap ++ stdFiles.stdFileMap,
        onErrorLogged = { () ⇒ errorLogged = true } :: stdFiles.onErrorLogged)
    ).closeWithCloser
    val processConfiguration = ProcessConfiguration(
      processStdFileMap,
      additionalEnvironment = env,
      agentTaskIdOption = agentTaskIdOption,
      killScriptOption = killScriptFileOption,
      logon = logon)
    synchronizedStartProcess {
      startShellScript(processConfiguration, name = jobName, scriptString = module.script.string.trim).closeWithCloser
    } map { richProcess ⇒
      richProcessOnce := richProcess
      deleteFilesWhenProcessClosed(List(orderParamsFile))
      concurrentStdoutStderrWell.start()
      true
    }
  }

  def end() = {}  // Not called

  def step() = {
    requireState(startCalled)
    richProcessOnce.get match {
      case None ⇒
        logger.warn("step, but no process has been started")
        <process.result spooler_process_result="false" exit_code="999888999"/>.toString()
      case Some(richProcess) ⇒
        val returnCode = richProcess.waitForTermination()
        for (o ← sigtermForwarder) o.close()
        concurrentStdoutStderrWell.finish()
        transferReturnValuesToMaster()
        val success = monitorProcessor.postStep(returnCode.isSuccess)
        <process.result
          spooler_process_result={(!errorLogged && success).toString}
          exit_code={returnCode.number.toString}
          state_text={concurrentStdoutStderrWell.firstStdoutLine}/>
        .toString()
    }
  }

  def callIfExists(javaSignature: String) = {
    requireState(startCalled)
    logger.debug(s"Ignoring call $javaSignature")
    true
  }

  private def transferReturnValuesToMaster(): Unit = {
    val variables = fetchReturnValues()
    if (variables.nonEmpty) {
      val xmlString = VariableSets.toXmlElem(fetchReturnValues()).toString()
      if (hasOrder)
        spoolerTask.orderParamsXml = xmlString
      else
        spoolerTask.paramsXml = xmlString
    }
  }

  private def fetchReturnValues() =
    autoClosing(new FileInputStream(orderParamsFile)) { in ⇒
      io.Source.fromInputStream(in)(Encoding).getLines.map(lineToKeyValue).toMap
    }

  def sendProcessSignal(signal: ProcessSignal): Unit = {
    logger.trace(s"sendProcessSignal $signal")
    for (p ← richProcessOnce) p.sendProcessSignal(signal)
  }

  def deleteLogFiles() = deleteFilesWhenProcessClosed(processStdFileMap.values)

  private def deleteFilesWhenProcessClosed(files: Iterable[Path]): Unit = {
    if (concurrentStdoutStderrWell != null) {
      if (files.nonEmpty) {
        for (richProcess ← richProcessOnce;
             _ ← richProcess.closed;
             _ ← concurrentStdoutStderrWell.closed)
        {
          RichProcess.tryDeleteFiles(files)
        }
      }
    }
  }

  @TestOnly
  def files = {
    requireState(startCalled)
    richProcessOnce.toOption match {
      case None ⇒ Nil
      case Some(o) ⇒ o.processConfiguration.files
    }
  }

  override def toString = List(super.toString) ++ (richProcessOnce map { _.toString }) mkString " "

  def pidOption = richProcessOnce flatMap { _.pidOption }
}

private object ShellProcessTask {
  private val ReturnValuesFileEnvironmentVariableName = "SCHEDULER_RETURN_VALUES"
  private val ReturnValuesRegex = "([^=]+)=(.*)".r

  private def lineToKeyValue(line: String): (String, String) = line match {
    case ReturnValuesRegex(name, value) ⇒ name.trim → value.trim
    case _ ⇒ throw new IllegalArgumentException(s"Not the expected syntax NAME=VALUE in file denoted by environment variable $ReturnValuesFileEnvironmentVariableName: $line")
  }
}
