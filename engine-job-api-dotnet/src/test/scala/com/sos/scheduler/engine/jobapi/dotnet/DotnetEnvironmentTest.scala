package com.sos.scheduler.engine.jobapi.dotnet

import com.sos.scheduler.engine.common.scalautil.FileUtils.implicits._
import com.sos.scheduler.engine.common.scalautil.Logger
import com.sos.scheduler.engine.jobapi.dotnet.DotnetEnvironmentTest._
import java.nio.file.Files.{createTempDirectory, delete}
import org.junit.runner.RunWith
import org.scalatest.FreeSpec
import org.scalatest.junit.JUnitRunner

/**
  * @author Joacim Zschimmer
  */
@RunWith(classOf[JUnitRunner])
final class DotnetEnvironmentTest extends FreeSpec {

  "DotnetEnvironment provides files and cleans up after use" in {
    val baseDir = createTempDirectory("DotnetEnvironmentTest-")  // Double nesting of directories to get a clear test
    val dotnetEnvironment = new DotnetEnvironment(baseDir)
    assert(baseDir.pathSet == Set(dotnetEnvironment.directory))
    for (o ← dotnetEnvironment.directory.pathSet) logger.info(s"$o")
    assert(dotnetEnvironment.directory.pathSet ==
      (dlls.DotnetDlls.DllsResourcePaths map { o ⇒ dotnetEnvironment.directory resolve o.simpleName }))
    dotnetEnvironment.close()
    assert(baseDir.pathSet == Set())
    delete(baseDir)
  }
}

private object DotnetEnvironmentTest {
  private val logger = Logger(getClass)
}