package com.sos.scheduler.engine.common.scalautil

/**
 * @author Joacim Zschimmer
 */
trait CloseOnError {
  final def closeOnError[A](body: ⇒ A): A =
    try body
    catch {
      case t: Throwable ⇒
        close()
        throw t
    }

  def close()
}