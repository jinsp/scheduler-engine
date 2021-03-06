package com.sos.scheduler.engine.data.jobchain

import com.sos.scheduler.engine.base.sprayjson.typed.{Subtype, TypedJsonFormat}

trait NodeOverview {
  def jobChainPath: JobChainPath
  def nodeId: NodeId

  def nodeKey = NodeKey(jobChainPath, nodeId)
}

object NodeOverview {
  implicit val MyJsonFormat = TypedJsonFormat[NodeOverview](
    Subtype[SimpleJobNodeOverview]("Job"),
    Subtype[SinkNodeOverview]("Sink"),
    Subtype[NestedJobChainNodeOverview]("NestedJobChain"),
    Subtype[EndNodeOverview]("End"))
}
