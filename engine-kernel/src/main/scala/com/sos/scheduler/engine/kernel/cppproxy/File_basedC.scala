package com.sos.scheduler.engine.kernel.cppproxy

import com.sos.scheduler.engine.cplusplus.runtime.Sister
import com.sos.scheduler.engine.kernel.filebased.FileBased
import javax.annotation.Nullable

trait File_basedC[S <: Sister] {
  def path: String
  def name: String
  def file_based_state: Int
  def file_based_error_string: String
  def file: String
  def file_modification_time_t: Long
  def source_xml_bytes: Array[Byte]
  def log: Prefix_logC
  def set_force_file_reread(): Unit
  def is_file_based_reread: Boolean
  def is_visible: Boolean
  def is_file_based: Boolean
  def has_base_file: Boolean
  def is_to_be_removed: Boolean
  def name_is_fixed: Boolean
  @Nullable def replacement_java: FileBased
  def missing_requisites_java: Array[String]
}
