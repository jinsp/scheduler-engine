package com.sos.scheduler.engine.kernel.event

import com.google.inject.ImplementedBy
import com.sos.scheduler.engine.common.scalautil.HasCloser
import com.sos.scheduler.engine.data.event._
import com.sos.scheduler.engine.data.order.{OrderEvent, OrderKey, OrderStatistics, OrderStatisticsChanged}
import com.sos.scheduler.engine.data.queries.{JobChainNodeQuery, JobChainQuery, PathQuery}
import com.sos.scheduler.engine.kernel.event.OrderStatisticsChangedSource._
import com.sos.scheduler.engine.kernel.event.collector.EventCollector
import java.time.Duration
import scala.PartialFunction.cond
import scala.concurrent.{ExecutionContext, Future}

/**
  * @author Joacim Zschimmer
  */
@ImplementedBy(classOf[OrderStatisticsChangedSourceInjectible])
trait OrderStatisticsChangedSource
extends HasCloser {

  protected def eventCollector: EventCollector
  protected def orderStatistics(query: JobChainNodeQuery): Future[Snapshot[OrderStatistics]]
  protected implicit def executionContext: ExecutionContext

  def whenOrderStatisticsChanged(after: EventId, timeout: Duration, query: PathQuery = PathQuery.All): Future[Snapshot[OrderStatisticsChanged]] =
    for (eventSeq ← eventCollector.when(EventRequest[OrderEvent](after = after, timeout), pathPredicate(query));
         snapshot ← orderStatistics(JobChainNodeQuery(JobChainQuery(query, isDistributed = Some(false/*No database access*/)))))
      yield snapshot map OrderStatisticsChanged.apply
}

object OrderStatisticsChangedSource {
  private def pathPredicate(query: PathQuery)(e: KeyedEvent[Event]) =
    cond(e) {
      case KeyedEvent(key: OrderKey, _) ⇒ query matches key.jobChainPath
    }
}