package com.sos.scheduler.engine.tests.jira.js1354

import com.google.common.base.Charsets.UTF_8
import com.google.common.io.Files.touch
import com.sos.scheduler.engine.common.scalautil.FileUtils.implicits._
import com.sos.scheduler.engine.common.time.ScalaJoda._
import com.sos.scheduler.engine.data.job.TaskId
import com.sos.scheduler.engine.data.jobchain.JobChainPath
import com.sos.scheduler.engine.data.order.OrderFinishedEvent
import com.sos.scheduler.engine.kernel.folder.FolderSubsystem
import com.sos.scheduler.engine.kernel.persistence.hibernate.HibernateVariableStore
import com.sos.scheduler.engine.kernel.persistence.hibernate.ScalaHibernate.transaction
import com.sos.scheduler.engine.test.EventBusTestFutures.implicits.RichEventBus
import com.sos.scheduler.engine.test.scala.ScalaSchedulerTest
import com.sos.scheduler.engine.tests.jira.js1354.JS1354IT._
import javax.persistence.EntityManagerFactory
import org.junit.runner.RunWith
import org.scalatest.FreeSpec
import org.scalatest.junit.JUnitRunner
import scala.concurrent.ExecutionContext.Implicits.global
import scala.concurrent.duration._
import scala.concurrent.{Await, Future}

/**
 * JS-1354, JS-1390, JS-1391 file_order_source should not lead to creation of needless task IDs
 *
 * @author Joacim Zschimmer
 */
@RunWith(classOf[JUnitRunner])
final class JS1354IT extends FreeSpec with ScalaSchedulerTest {

  import controller.{getEventBus ⇒ eventBus}

  private lazy val fileOrderDir = testEnvironment.newFileOrderSourceDirectory()
  private implicit lazy val entityManagerFactory: EntityManagerFactory = instance[EntityManagerFactory]

  "JS-1391 No needless task ID creation while the limit of a job chain with file_order_source is reached" - {
    "job_chain max_order=1" in {
      runFiles("TESTFILE-1a", "TESTFILE-1b") {
        testEnvironment.fileFromPath(TestJobChainPath).write(
          <job_chain max_orders="1">
            <file_order_source directory={fileOrderDir.getPath} repeat="1"/>
            <job_chain_node state="100" job="/test-100"/>
            <job_chain_node state="200" job="/test-200"/>
          </job_chain>.toString(),
          UTF_8)
        instance[FolderSubsystem].updateFolders()
      }
    }

    "Second files" in {
      runFiles("TESTFILE-2a", "TESTFILE-2b") {}
    }
  }

  "JS-1354 No needless task ID creation while waiting for a new file order" - {
    "job_chain max_order=unlimited" in {
      testEnvironment.fileFromPath(TestJobChainPath).write(
        <job_chain>
          <file_order_source directory={fileOrderDir.getPath} repeat="1"/>
          <job_chain_node state="100" job="/test-100"/>
          <job_chain_node state="200" job="/test-200"/>
        </job_chain>.toString(),
        UTF_8)
      instance[FolderSubsystem].updateFolders()
      runFiles("TESTFILE-1") {}
    }
  }

  "JS-1390 Don't fall asleep after first file order" - {
    "Second files" in {
      runFiles("TESTFILE-2a", "TESTFILE-2b") {}
    }

    "Third file" in {
      runFiles("TESTFILE-3") {}
    }
  }

  private def runFiles(names: String*)(body: ⇒ Unit): Unit = {
    val preId = nextTaskId
    val files = names map { o ⇒ fileOrderDir / o }
    val ordersFinished = Future.sequence(files map { f ⇒ eventBus.keyedEventFuture[OrderFinishedEvent](TestJobChainPath orderKey f.getPath) })
    files foreach touch
    body
    Await.result(ordersFinished, 60.seconds)
    val postId = nextTaskId
    assert(postId == TaskId(preId.value + files.size * JobChainTaskCount))
    sleep(1.s)
    assert(nextTaskId == postId)
  }

  private def nextTaskId = transaction { implicit entityManager ⇒ instance[HibernateVariableStore].nextTaskId }
}

private object JS1354IT {
  private val TestJobChainPath = JobChainPath("/test")
  private val JobChainTaskCount = 2   // Job chain contains two jobs, for each a task will be started
}