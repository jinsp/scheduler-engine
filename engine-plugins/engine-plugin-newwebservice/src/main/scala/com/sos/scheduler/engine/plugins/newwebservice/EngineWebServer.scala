package com.sos.scheduler.engine.plugins.newwebservice

import akka.actor.ActorSystem
import com.google.inject.Injector
import com.sos.scheduler.engine.common.sprayutils.WebServerBinding
import com.sos.scheduler.engine.common.sprayutils.web.SprayWebServer
import com.sos.scheduler.engine.common.sprayutils.web.auth.{CSRF, GateKeeper}
import scala.collection.immutable
import scala.concurrent.ExecutionContext

/**
 * @author Joacim Zschimmer
 */
final class EngineWebServer(
  protected val bindings: immutable.Iterable[WebServerBinding],
  gateKeeperConfiguration: GateKeeper.Configuration,
  csrf: CSRF,
  injector: Injector)
  (implicit
    protected val actorSystem: ActorSystem,
    protected val executionContext: ExecutionContext)
extends SprayWebServer with SprayWebServer.HasUri {

  protected def uriPathPrefix = ""

  protected def newRouteActorRef(binding: WebServerBinding) =
    actorSystem.actorOf(
      WebServiceActor.props(
        new GateKeeper(gateKeeperConfiguration, csrf, isUnsecuredHttp = binding.isUnsecuredHttp),
        injector),
      name = SprayWebServer.actorName("EngineWebServer", binding))
}
