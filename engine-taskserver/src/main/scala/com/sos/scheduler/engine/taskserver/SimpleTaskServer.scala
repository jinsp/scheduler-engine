package com.sos.scheduler.engine.taskserver

import com.google.inject.Guice
import com.sos.scheduler.engine.base.process.ProcessSignal
import com.sos.scheduler.engine.common.guice.ScalaAbstractModule
import com.sos.scheduler.engine.common.scalautil.AutoClosing.autoClosing
import com.sos.scheduler.engine.common.scalautil.Closers.implicits._
import com.sos.scheduler.engine.common.scalautil.Futures._
import com.sos.scheduler.engine.common.scalautil.{HasCloser, Logger}
import com.sos.scheduler.engine.minicom.remoting.{DialogConnection, Remoting}
import com.sos.scheduler.engine.taskserver.SimpleTaskServer._
import com.sos.scheduler.engine.taskserver.spoolerapi.{ProxySpooler, ProxySpoolerLog, ProxySpoolerTask}
import com.sos.scheduler.engine.taskserver.task.{RemoteModuleInstanceServer, TaskStartArguments}
import scala.concurrent.ExecutionContext.Implicits.global
import scala.concurrent._
import scala.concurrent.duration.Duration

/**
 * A blocking [[TaskServer]], running in a own thread.
 *
 * @author Joacim Zschimmer
 */
final class SimpleTaskServer(startArguments: TaskStartArguments) extends TaskServer with HasCloser {

  private val controllingSchedulerConnection = new TcpConnection(startArguments.controllerInetSocketAddress).closeWithCloser
  private val injector = Guice.createInjector(new ScalaAbstractModule {
    def configure() = bindInstance[TaskStartArguments](startArguments)
  })
  private val remoting = new Remoting(injector, new DialogConnection(controllingSchedulerConnection), IDispatchFactories, ProxyIDispatchFactories)

  private val terminatedPromise = Promise[Unit]()
  def terminated = terminatedPromise.future

  def start(): Unit =
    Future {
      blocking {
        controllingSchedulerConnection.connect()
        try remoting.run()
        catch {
          case t: Throwable ⇒
            logger.error(t.toString, t)
            throw t
        }
      }
    } onComplete { o ⇒
      terminatedPromise.complete(o)
    }

  def sendProcessSignal(signal: ProcessSignal) = {
    logger.trace(s"sendProcessSignal $signal")
    remoting.invocables[HasSendProcessSignal] foreach { _.sendProcessSignal(signal) }
  }

  override def toString = s"TaskServer(controller=${startArguments.controllerAddress})"
}

object SimpleTaskServer {
  private val IDispatchFactories = List(RemoteModuleInstanceServer)
  private val ProxyIDispatchFactories = List(ProxySpooler, ProxySpoolerLog, ProxySpoolerTask)
  private val logger = Logger(getClass)

  def run(startArguments: TaskStartArguments): Unit =
    autoClosing(new SimpleTaskServer(startArguments)) { taskServer ⇒
      taskServer.start()
      awaitResult(taskServer.terminated, Duration.Inf)
    }
}