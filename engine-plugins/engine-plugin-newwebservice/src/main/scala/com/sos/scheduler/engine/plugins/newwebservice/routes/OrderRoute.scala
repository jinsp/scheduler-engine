package com.sos.scheduler.engine.plugins.newwebservice.routes

import com.sos.scheduler.engine.client.api.{OrderClient, SchedulerOverviewClient}
import com.sos.scheduler.engine.client.web.common.QueryHttp._
import com.sos.scheduler.engine.common.sprayutils.SprayJsonOrYamlSupport._
import com.sos.scheduler.engine.common.sprayutils.SprayUtils.asFromStringOptionDeserializer
import com.sos.scheduler.engine.data.event._
import com.sos.scheduler.engine.data.events.SchedulerAnyKeyedEventJsonFormat
import com.sos.scheduler.engine.data.events.SchedulerAnyKeyedEventJsonFormat.anyEventJsonFormat
import com.sos.scheduler.engine.data.jobchain.JobChainPath
import com.sos.scheduler.engine.data.order.{OrderDetailed, OrderKey, OrderOverview, OrderStatisticsChanged, Orders}
import com.sos.scheduler.engine.data.queries.{OrderQuery, PathQuery}
import com.sos.scheduler.engine.kernel.event.{DirectEventClient, OrderStatisticsChangedSource}
import com.sos.scheduler.engine.kernel.order.OrderSubsystemClient
import com.sos.scheduler.engine.plugins.newwebservice.html.HtmlDirectives._
import com.sos.scheduler.engine.plugins.newwebservice.html.WebServiceContext
import com.sos.scheduler.engine.plugins.newwebservice.routes.OrderRoute._
import com.sos.scheduler.engine.plugins.newwebservice.routes.SchedulerDirectives.typedPath
import com.sos.scheduler.engine.plugins.newwebservice.routes.event.EventRoutes._
import com.sos.scheduler.engine.plugins.newwebservice.routes.log.LogRoute
import com.sos.scheduler.engine.plugins.newwebservice.simplegui.YamlHtmlPage.implicits.jsonToYamlHtmlPage
import com.sos.scheduler.engine.plugins.newwebservice.simplegui.{OrdersHtmlPage, SingleKeyEventHtmlPage}
import scala.collection.immutable.Seq
import scala.concurrent.ExecutionContext
import spray.httpx.marshalling.ToResponseMarshallable.isMarshallable
import spray.routing.Directives._
import spray.routing._

/**
  * @author Joacim Zschimmer
  */
trait OrderRoute extends LogRoute {

  protected def orderSubsystem: OrderSubsystemClient
  protected def orderStatisticsChangedSource: OrderStatisticsChangedSource
  protected implicit def client: OrderClient with SchedulerOverviewClient with DirectEventClient
  protected implicit def webServiceContext: WebServiceContext
  protected implicit def executionContext: ExecutionContext

  protected final def orderRoute: Route =
    (getRequiresSlash(webServiceContext) | pass) {
      parameter("return".?) {
        case Some("OrderStatistics") ⇒
          jobChainNodeQuery { query ⇒
            completeTryHtml(client.orderStatistics(query))
          }
        case Some("OrderStatisticsChanged") ⇒
          pathQuery(JobChainPath)(orderStatisticsChanged)
        case returnTypeOption ⇒
          typedPath(OrderKey) { query ⇒
            singleOrder(returnTypeOption, query)
          } ~
          orderQuery { query ⇒
            queriedOrders(returnTypeOption, query)
          }
      }
    }

  private def singleOrder(returnTypeOption: Option[String], orderKey: OrderKey): Route =
    returnTypeOption getOrElse "OrderDetailed" match {
      case "log" ⇒
        logRoute(orderSubsystem.order(orderKey).log)

      case "OrderOverview" ⇒
        completeTryHtml(client.order[OrderOverview](orderKey))

      case "OrderDetailed" ⇒
        completeTryHtml(client.order[OrderDetailed](orderKey))

      case _ ⇒
        orderEvents(orderKey)
    }

  private def queriedOrders(returnTypeOption: Option[String], query: OrderQuery): Route =
    returnTypeOption match {
      case Some(ReturnTypeRegex(OrderTreeComplementedName, OrderOverview.name)
                | OrderTreeComplementedName) ⇒
        completeTryHtml(client.orderTreeComplementedBy[OrderOverview](query))

      case Some(ReturnTypeRegex(OrderTreeComplementedName, OrderDetailed.name)) ⇒
        completeTryHtml(client.orderTreeComplementedBy[OrderDetailed](query))

      case Some(ReturnTypeRegex(OrdersComplementedName, OrderOverview.name)
                | OrdersComplementedName)  ⇒
        completeTryHtml(client.ordersComplementedBy[OrderOverview](query))

      case Some(ReturnTypeRegex(OrdersComplementedName, OrderDetailed.name)) ⇒
        completeTryHtml(client.ordersComplementedBy[OrderDetailed](query))

      case Some(OrderOverview.name) ⇒
        completeTryHtml(client.ordersBy[OrderOverview](query) map { _ map Orders.apply })

      case Some(OrderDetailed.name) ⇒
        completeTryHtml(client.ordersBy[OrderDetailed](query) map { _ map Orders.apply })

      case Some(_) ⇒
        query.orderKeyOption match {
          case Some(orderKey) ⇒
            orderEvents(orderKey)
          case None ⇒
            orderEvents(query.jobChainQuery.pathQuery)  // Events are only selected by JobChainPath !!!
        }

      case None ⇒
        htmlPreferred(webServiceContext) {
          requestUri { uri ⇒
            complete(
              for (o ← client.ordersComplementedBy[OrderOverview](query)) yield
                OrdersHtmlPage.toHtmlPage(o, uri, query, client, webServiceContext))
          }
        } ~
          complete(client.orderTreeComplementedBy[OrderOverview](query))
    }

  private def orderEvents(orderKey: OrderKey): Route = {
    eventRequest() { request ⇒
      completeTryHtml {
        implicit val toHtmlPage = SingleKeyEventHtmlPage.singleKeyEventToHtmlPage[AnyEvent](orderKey)
        request match {
          case request: EventRequest[_] ⇒
            client.eventsForKey[AnyEvent](request.asInstanceOf[EventRequest[AnyEvent]], orderKey)
          case request: ReverseEventRequest[_] ⇒
            for (responseSnapshot ← client.eventsReverseForKey[AnyEvent](request.asInstanceOf[ReverseEventRequest[AnyEvent]], orderKey)) yield
              for (events ← responseSnapshot) yield
                EventSeq.NonEmpty(events)
        }
      }
    }
  }

  private def orderEvents(query: PathQuery): Route =
    eventRequest() {
      case request: EventRequest[_] ⇒
        completeTryHtml {
          client.events[Event](
            request.asInstanceOf[EventRequest[Event]],
            predicate = PartialFunction[KeyedEvent[Event], Boolean] {
              case KeyedEvent(OrderKey(jobChainPath, _), _) ⇒ query.matches(jobChainPath)
              case _ ⇒ false
            })
        }
      case _ ⇒
        reject
    }

  private def orderStatisticsChanged(query: PathQuery): Route = {
    eventRequest() {
      case EventRequest(eventClass, afterEventId, timeout, limit) if eventClass == classOf[OrderStatisticsChanged] ⇒
        completeTryHtml[EventSeq[Seq, AnyKeyedEvent]] {
          for (snapshot ← orderStatisticsChangedSource.whenOrderStatisticsChanged(after = afterEventId, timeout, query))
            yield nestIntoSeqSnapshot(snapshot)
        }
      case _ ⇒
        reject
    }
  }
}

object OrderRoute {
  private val OrderTreeComplementedName = "OrderTreeComplemented"
  private val OrdersComplementedName = "OrdersComplemented"
  private val ReturnTypeRegex = {
    val word = "([A-Za-z]+)"
    s"$word(?:/$word)?".r
  }
}