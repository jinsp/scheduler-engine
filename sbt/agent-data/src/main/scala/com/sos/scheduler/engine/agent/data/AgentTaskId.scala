package com.sos.scheduler.engine.agent.data

import com.sos.scheduler.engine.agent.data
import com.sos.scheduler.engine.base.generic.IsString
import scala.math.abs
import scala.util.Random

/**
 * @see C++ Process_id
 * @author Joacim Zschimmer
 */
final case class AgentTaskId(string: String) extends IsString {
  import data.AgentTaskId._

  if (!pattern.matcher(string).matches) throw new IllegalArgumentException(s"Invalid AgentTaskId($string)")

  /**
   *  Index, only valid if string starts with number followed by a hypen, as generated by Agent.
   */
  def index = (string takeWhile { _ != '-' }).toLong

  override def toString = s"AgentTaskId($string)"
}

object AgentTaskId extends IsString.HasJsonFormat[AgentTaskId] {

  private val pattern = "[A-Za-z0-9-][A-Za-z0-9._-]*".r.pattern  // Try to exclude any meta characters

  def apply(index: Long, salt: Long) = new AgentTaskId(s"$index-$salt")

  /**
   * Delivers [[AgentTaskId]] with recognizable strictly increasing numbers.
   * The numbers itself are meaningless.
   */
  def newGenerator(start: Int = 1): Iterator[AgentTaskId] =
    new IncreasingPositiveLongs(start = start, maximum = Int.MaxValue) map { n ⇒ AgentTaskId(index = n, salt = abs(Random.nextLong())) }
}