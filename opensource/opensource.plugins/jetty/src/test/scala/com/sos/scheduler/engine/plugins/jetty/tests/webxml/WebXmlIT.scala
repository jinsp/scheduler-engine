package com.sos.scheduler.engine.plugins.jetty.tests.webxml

import com.google.common.base.Charsets.UTF_8
import com.google.common.io.{Resources, Files}
import com.sos.scheduler.engine.kernel.plugin.PluginSubsystem
import com.sos.scheduler.engine.plugins.jetty.JettyPlugin
import com.sos.scheduler.engine.plugins.jetty.test.JettyPluginJerseyTester
import com.sos.scheduler.engine.test.scala.ScalaSchedulerTest
import java.io.File
import org.junit.runner.RunWith
import org.scalatest.FreeSpec
import org.scalatest.Matchers._
import org.scalatest.junit.JUnitRunner

@RunWith(classOf[JUnitRunner])
final class WebXmlIT extends FreeSpec with ScalaSchedulerTest with JettyPluginJerseyTester {
  private val baseDir = controller.environment.directory
  private val configDir = controller.environment.configDirectory

  override protected def checkedBeforeAll() {
    controller.activateScheduler()
    prepareWebXml()
    val pluginSubsystem = injector.getInstance(classOf[PluginSubsystem])
    pluginSubsystem.activatePlugin(classOf[JettyPlugin])
    super.checkedBeforeAll()
  }

  private def prepareWebXml() {
    val webXmlFile = new File(configDir, "web.xml")
    val a = readFile(webXmlFile).replace("{{SCHEDULER_WORK_URL}}", baseDir.toURI.toURL.toString)
    Files.write(a, webXmlFile, UTF_8)
  }

//  ignore("(for debugging only)") {
//    controller.waitForTermination(1.hours)
//  }

  "Web server should deliver integrated resource as without a web.xml" in {
    get[String]("/jobscheduler/z/index.html") shouldEqual Resources.toString(getClass.getResource("/com/sos/scheduler/engine/web/z/index.html"), UTF_8)
  }

  "Web server should deliver external files described in web.xml" in {
    get[String](s"/jobscheduler/${configDir.getName}/scheduler.xml") shouldEqual readFile(new File(configDir, "scheduler.xml"))
  }

  private def readFile(f: File) = Files.toString(f, UTF_8)
}