// *** Generated by com.sos.scheduler.engine.cplusplus.generator ***

#include "_precompiled.h"

#include "com__sos__scheduler__engine__newkernel__job__CppNewJob.h"
#include "com__sos__scheduler__engine__cplusplus__runtime__Sister.h"
#include "com__sos__scheduler__engine__data__job__TaskPersistentState.h"
#include "com__sos__scheduler__engine__kernel__cppproxy__Job_nodeC.h"
#include "com__sos__scheduler__engine__kernel__cppproxy__SpoolerC.h"
#include "com__sos__scheduler__engine__kernel__cppproxy__Variable_setC.h"
#include "com__sos__scheduler__engine__kernel__job__Job.h"
#include "com__sos__scheduler__engine__kernel__order__jobchain__JobChain.h"
#include "java__lang__Object.h"
#include "java__lang__String.h"
#include "org__w3c__dom__Document.h"
#include "org__w3c__dom__Element.h"

namespace javaproxy { namespace com { namespace sos { namespace scheduler { namespace engine { namespace newkernel { namespace job { 

struct CppNewJob__class : ::zschimmer::javabridge::Class
{
    CppNewJob__class(const string& class_name);
   ~CppNewJob__class();

    ::zschimmer::javabridge::Method const __constructor__Lcom_sos_scheduler_engine_kernel_cppproxy_SpoolerC_2Lcom_sos_scheduler_engine_kernel_job_Job_2__method;
    ::zschimmer::javabridge::Method const _appendCalendarDOMElements__Lorg_w3c_dom_Document_2__method;
    ::zschimmer::javabridge::Method const _canBeRemovedNow____method;
    ::zschimmer::javabridge::Method const _close____method;
    ::zschimmer::javabridge::Method const _connectJobNode__Lcom_sos_scheduler_engine_kernel_cppproxy_Job_1nodeC_2__method;
    ::zschimmer::javabridge::Method const _description____method;
    ::zschimmer::javabridge::Method const _disconnectJobNode__Lcom_sos_scheduler_engine_kernel_cppproxy_Job_1nodeC_2__method;
    ::zschimmer::javabridge::Method const _domElement__Lorg_w3c_dom_Document_2Lcom_sos_scheduler_engine_kernel_order_jobchain_JobChain_2__method;
    ::zschimmer::javabridge::Method const _enqueueTask__Lcom_sos_scheduler_engine_data_job_TaskPersistentState_2__method;
    ::zschimmer::javabridge::Method const _executeStateCommand__Ljava_lang_String_2__method;
    ::zschimmer::javabridge::Method const _hasTask____method;
    ::zschimmer::javabridge::Method const _isInJobChain____method;
    ::zschimmer::javabridge::Method const _isPermanentlyStopped____method;
    ::zschimmer::javabridge::Method const _isWaitingForProcess____method;
    ::zschimmer::javabridge::Method const _killTask__IZ__method;
    ::zschimmer::javabridge::Method const _onActivate____method;
    ::zschimmer::javabridge::Method const _onInitialize____method;
    ::zschimmer::javabridge::Method const _onLoad____method;
    ::zschimmer::javabridge::Method const _onOrderPossiblyAvailable____method;
    ::zschimmer::javabridge::Method const _onPrepareToRemove____method;
    ::zschimmer::javabridge::Method const _onProcessIsIdle____method;
    ::zschimmer::javabridge::Method const _onReferencedScheduleLoaded____method;
    ::zschimmer::javabridge::Method const _onReferencedScheduleModified____method;
    ::zschimmer::javabridge::Method const _onReferencedScheduleToBeRemoved____method;
    ::zschimmer::javabridge::Method const _onRemoveNow____method;
    ::zschimmer::javabridge::Method const _orderSetbackMaximum____method;
    ::zschimmer::javabridge::Method const _processClassPath____method;
    ::zschimmer::javabridge::Method const _readHistory__Lorg_w3c_dom_Document_2II__method;
    ::zschimmer::javabridge::Method const _removalError____method;
    ::zschimmer::javabridge::Method const _removeRunningTask__I__method;
    ::zschimmer::javabridge::Method const _setIdleTimeout__J__method;
    ::zschimmer::javabridge::Method const _setOrderControlled____method;
    ::zschimmer::javabridge::Method const _setScheduleDOM__Lorg_w3c_dom_Element_2__method;
    ::zschimmer::javabridge::Method const _setStateText__Ljava_lang_String_2__method;
    ::zschimmer::javabridge::Method const _setXmlBytes___3B__method;
    ::zschimmer::javabridge::Method const _signalEarlierOrder__JLjava_lang_String_2Ljava_lang_String_2__method;
    ::zschimmer::javabridge::Method const _stateString____method;
    ::zschimmer::javabridge::Method const _stop____method;
    ::zschimmer::javabridge::Method const _stopSimply____method;
    ::zschimmer::javabridge::Method const _title____method;
    ::zschimmer::javabridge::Method const _tryToEndATask____method;

    static const ::zschimmer::javabridge::class_factory< CppNewJob__class > class_factory;
};

const ::zschimmer::javabridge::class_factory< CppNewJob__class > CppNewJob__class::class_factory ("com.sos.scheduler.engine.newkernel.job.CppNewJob");

CppNewJob__class::CppNewJob__class(const string& class_name) :
    ::zschimmer::javabridge::Class(class_name)
    ,__constructor__Lcom_sos_scheduler_engine_kernel_cppproxy_SpoolerC_2Lcom_sos_scheduler_engine_kernel_job_Job_2__method(this, "<init>", "(Lcom/sos/scheduler/engine/kernel/cppproxy/SpoolerC;Lcom/sos/scheduler/engine/kernel/job/Job;)V")
    ,_appendCalendarDOMElements__Lorg_w3c_dom_Document_2__method(this, "appendCalendarDOMElements", "(Lorg/w3c/dom/Document;)Lorg/w3c/dom/Element;")
    ,_canBeRemovedNow____method(this, "canBeRemovedNow", "()Z")
    ,_close____method(this, "close", "()V")
    ,_connectJobNode__Lcom_sos_scheduler_engine_kernel_cppproxy_Job_1nodeC_2__method(this, "connectJobNode", "(Lcom/sos/scheduler/engine/kernel/cppproxy/Job_nodeC;)Z")
    ,_description____method(this, "description", "()Ljava/lang/String;")
    ,_disconnectJobNode__Lcom_sos_scheduler_engine_kernel_cppproxy_Job_1nodeC_2__method(this, "disconnectJobNode", "(Lcom/sos/scheduler/engine/kernel/cppproxy/Job_nodeC;)V")
    ,_domElement__Lorg_w3c_dom_Document_2Lcom_sos_scheduler_engine_kernel_order_jobchain_JobChain_2__method(this, "domElement", "(Lorg/w3c/dom/Document;Lcom/sos/scheduler/engine/kernel/order/jobchain/JobChain;)Lorg/w3c/dom/Element;")
    ,_enqueueTask__Lcom_sos_scheduler_engine_data_job_TaskPersistentState_2__method(this, "enqueueTask", "(Lcom/sos/scheduler/engine/data/job/TaskPersistentState;)V")
    ,_executeStateCommand__Ljava_lang_String_2__method(this, "executeStateCommand", "(Ljava/lang/String;)V")
    ,_hasTask____method(this, "hasTask", "()Z")
    ,_isInJobChain____method(this, "isInJobChain", "()Z")
    ,_isPermanentlyStopped____method(this, "isPermanentlyStopped", "()Z")
    ,_isWaitingForProcess____method(this, "isWaitingForProcess", "()Z")
    ,_killTask__IZ__method(this, "killTask", "(IZ)V")
    ,_onActivate____method(this, "onActivate", "()Z")
    ,_onInitialize____method(this, "onInitialize", "()Z")
    ,_onLoad____method(this, "onLoad", "()Z")
    ,_onOrderPossiblyAvailable____method(this, "onOrderPossiblyAvailable", "()V")
    ,_onPrepareToRemove____method(this, "onPrepareToRemove", "()V")
    ,_onProcessIsIdle____method(this, "onProcessIsIdle", "()V")
    ,_onReferencedScheduleLoaded____method(this, "onReferencedScheduleLoaded", "()V")
    ,_onReferencedScheduleModified____method(this, "onReferencedScheduleModified", "()V")
    ,_onReferencedScheduleToBeRemoved____method(this, "onReferencedScheduleToBeRemoved", "()Z")
    ,_onRemoveNow____method(this, "onRemoveNow", "()V")
    ,_orderSetbackMaximum____method(this, "orderSetbackMaximum", "()I")
    ,_processClassPath____method(this, "processClassPath", "()Ljava/lang/String;")
    ,_readHistory__Lorg_w3c_dom_Document_2II__method(this, "readHistory", "(Lorg/w3c/dom/Document;II)Lorg/w3c/dom/Element;")
    ,_removalError____method(this, "removalError", "()Ljava/lang/String;")
    ,_removeRunningTask__I__method(this, "removeRunningTask", "(I)V")
    ,_setIdleTimeout__J__method(this, "setIdleTimeout", "(J)V")
    ,_setOrderControlled____method(this, "setOrderControlled", "()V")
    ,_setScheduleDOM__Lorg_w3c_dom_Element_2__method(this, "setScheduleDOM", "(Lorg/w3c/dom/Element;)V")
    ,_setStateText__Ljava_lang_String_2__method(this, "setStateText", "(Ljava/lang/String;)V")
    ,_setXmlBytes___3B__method(this, "setXmlBytes", "([B)V")
    ,_signalEarlierOrder__JLjava_lang_String_2Ljava_lang_String_2__method(this, "signalEarlierOrder", "(JLjava/lang/String;Ljava/lang/String;)V")
    ,_stateString____method(this, "stateString", "()Ljava/lang/String;")
    ,_stop____method(this, "stop", "()V")
    ,_stopSimply____method(this, "stopSimply", "()V")
    ,_title____method(this, "title", "()Ljava/lang/String;")
    ,_tryToEndATask____method(this, "tryToEndATask", "()Z"){}

CppNewJob__class::~CppNewJob__class() {}



CppNewJob CppNewJob::new_instance(const ::zschimmer::javabridge::proxy_jobject< ::javaproxy::com::sos::scheduler::engine::kernel::cppproxy::SpoolerC >& p0, const ::zschimmer::javabridge::proxy_jobject< ::javaproxy::com::sos::scheduler::engine::kernel::job::Job >& p1) {
    CppNewJob result;
    result.java_object_allocate_();
    ::zschimmer::javabridge::raw_parameter_list<2> parameter_list;
    parameter_list._jvalues[0].l = p0.get_jobject();
    parameter_list._jvalues[1].l = p1.get_jobject();
    CppNewJob__class* cls = result._class.get();
    cls->__constructor__Lcom_sos_scheduler_engine_kernel_cppproxy_SpoolerC_2Lcom_sos_scheduler_engine_kernel_job_Job_2__method.call(result.get_jobject(), parameter_list);
    return result;
}


CppNewJob::CppNewJob(jobject jo) { if (jo) assign_(jo); }

CppNewJob::CppNewJob(const CppNewJob& o) { assign_(o.get_jobject()); }

#ifdef Z_HAS_MOVE_CONSTRUCTOR
    CppNewJob::CppNewJob(CppNewJob&& o) { set_jobject(o.get_jobject());  o.set_jobject(NULL); }
#endif

CppNewJob::~CppNewJob() { assign_(NULL); }




::javaproxy::org::w3c::dom::Element CppNewJob::appendCalendarDOMElements(const ::zschimmer::javabridge::proxy_jobject< ::javaproxy::org::w3c::dom::Document >& p0) const {
    ::zschimmer::javabridge::raw_parameter_list<1> parameter_list;
    parameter_list._jvalues[0].l = p0.get_jobject();
    CppNewJob__class* cls = _class.get();
    ::javaproxy::org::w3c::dom::Element result;
    result.steal_local_ref(cls->_appendCalendarDOMElements__Lorg_w3c_dom_Document_2__method.jobject_call(get_jobject(), parameter_list));
    return result;
}

bool CppNewJob::canBeRemovedNow() const {
    ::zschimmer::javabridge::raw_parameter_list<0> parameter_list;
    CppNewJob__class* cls = _class.get();
    return 0 != cls->_canBeRemovedNow____method.bool_call(get_jobject(), parameter_list);
}

void CppNewJob::close() const {
    ::zschimmer::javabridge::raw_parameter_list<0> parameter_list;
    CppNewJob__class* cls = _class.get();
    cls->_close____method.call(get_jobject(), parameter_list);
}

bool CppNewJob::connectJobNode(const ::zschimmer::javabridge::proxy_jobject< ::javaproxy::com::sos::scheduler::engine::kernel::cppproxy::Job_nodeC >& p0) const {
    ::zschimmer::javabridge::raw_parameter_list<1> parameter_list;
    parameter_list._jvalues[0].l = p0.get_jobject();
    CppNewJob__class* cls = _class.get();
    return 0 != cls->_connectJobNode__Lcom_sos_scheduler_engine_kernel_cppproxy_Job_1nodeC_2__method.bool_call(get_jobject(), parameter_list);
}

::javaproxy::java::lang::String CppNewJob::description() const {
    ::zschimmer::javabridge::raw_parameter_list<0> parameter_list;
    CppNewJob__class* cls = _class.get();
    ::javaproxy::java::lang::String result;
    result.steal_local_ref(cls->_description____method.jobject_call(get_jobject(), parameter_list));
    return result;
}

void CppNewJob::disconnectJobNode(const ::zschimmer::javabridge::proxy_jobject< ::javaproxy::com::sos::scheduler::engine::kernel::cppproxy::Job_nodeC >& p0) const {
    ::zschimmer::javabridge::raw_parameter_list<1> parameter_list;
    parameter_list._jvalues[0].l = p0.get_jobject();
    CppNewJob__class* cls = _class.get();
    cls->_disconnectJobNode__Lcom_sos_scheduler_engine_kernel_cppproxy_Job_1nodeC_2__method.call(get_jobject(), parameter_list);
}

::javaproxy::org::w3c::dom::Element CppNewJob::domElement(const ::zschimmer::javabridge::proxy_jobject< ::javaproxy::org::w3c::dom::Document >& p0, const ::zschimmer::javabridge::proxy_jobject< ::javaproxy::com::sos::scheduler::engine::kernel::order::jobchain::JobChain >& p1) const {
    ::zschimmer::javabridge::raw_parameter_list<2> parameter_list;
    parameter_list._jvalues[0].l = p0.get_jobject();
    parameter_list._jvalues[1].l = p1.get_jobject();
    CppNewJob__class* cls = _class.get();
    ::javaproxy::org::w3c::dom::Element result;
    result.steal_local_ref(cls->_domElement__Lorg_w3c_dom_Document_2Lcom_sos_scheduler_engine_kernel_order_jobchain_JobChain_2__method.jobject_call(get_jobject(), parameter_list));
    return result;
}

void CppNewJob::enqueueTask(const ::zschimmer::javabridge::proxy_jobject< ::javaproxy::com::sos::scheduler::engine::data::job::TaskPersistentState >& p0) const {
    ::zschimmer::javabridge::raw_parameter_list<1> parameter_list;
    parameter_list._jvalues[0].l = p0.get_jobject();
    CppNewJob__class* cls = _class.get();
    cls->_enqueueTask__Lcom_sos_scheduler_engine_data_job_TaskPersistentState_2__method.call(get_jobject(), parameter_list);
}

void CppNewJob::executeStateCommand(const ::zschimmer::javabridge::proxy_jobject< ::javaproxy::java::lang::String >& p0) const {
    ::zschimmer::javabridge::raw_parameter_list<1> parameter_list;
    parameter_list._jvalues[0].l = p0.get_jobject();
    CppNewJob__class* cls = _class.get();
    cls->_executeStateCommand__Ljava_lang_String_2__method.call(get_jobject(), parameter_list);
}

bool CppNewJob::hasTask() const {
    ::zschimmer::javabridge::raw_parameter_list<0> parameter_list;
    CppNewJob__class* cls = _class.get();
    return 0 != cls->_hasTask____method.bool_call(get_jobject(), parameter_list);
}

bool CppNewJob::isInJobChain() const {
    ::zschimmer::javabridge::raw_parameter_list<0> parameter_list;
    CppNewJob__class* cls = _class.get();
    return 0 != cls->_isInJobChain____method.bool_call(get_jobject(), parameter_list);
}

bool CppNewJob::isPermanentlyStopped() const {
    ::zschimmer::javabridge::raw_parameter_list<0> parameter_list;
    CppNewJob__class* cls = _class.get();
    return 0 != cls->_isPermanentlyStopped____method.bool_call(get_jobject(), parameter_list);
}

bool CppNewJob::isWaitingForProcess() const {
    ::zschimmer::javabridge::raw_parameter_list<0> parameter_list;
    CppNewJob__class* cls = _class.get();
    return 0 != cls->_isWaitingForProcess____method.bool_call(get_jobject(), parameter_list);
}

void CppNewJob::killTask(jint p0, jboolean p1) const {
    ::zschimmer::javabridge::raw_parameter_list<2> parameter_list;
    parameter_list._jvalues[0].i = p0;
    parameter_list._jvalues[1].z = p1;
    CppNewJob__class* cls = _class.get();
    cls->_killTask__IZ__method.call(get_jobject(), parameter_list);
}

bool CppNewJob::onActivate() const {
    ::zschimmer::javabridge::raw_parameter_list<0> parameter_list;
    CppNewJob__class* cls = _class.get();
    return 0 != cls->_onActivate____method.bool_call(get_jobject(), parameter_list);
}

bool CppNewJob::onInitialize() const {
    ::zschimmer::javabridge::raw_parameter_list<0> parameter_list;
    CppNewJob__class* cls = _class.get();
    return 0 != cls->_onInitialize____method.bool_call(get_jobject(), parameter_list);
}

bool CppNewJob::onLoad() const {
    ::zschimmer::javabridge::raw_parameter_list<0> parameter_list;
    CppNewJob__class* cls = _class.get();
    return 0 != cls->_onLoad____method.bool_call(get_jobject(), parameter_list);
}

void CppNewJob::onOrderPossiblyAvailable() const {
    ::zschimmer::javabridge::raw_parameter_list<0> parameter_list;
    CppNewJob__class* cls = _class.get();
    cls->_onOrderPossiblyAvailable____method.call(get_jobject(), parameter_list);
}

void CppNewJob::onPrepareToRemove() const {
    ::zschimmer::javabridge::raw_parameter_list<0> parameter_list;
    CppNewJob__class* cls = _class.get();
    cls->_onPrepareToRemove____method.call(get_jobject(), parameter_list);
}

void CppNewJob::onProcessIsIdle() const {
    ::zschimmer::javabridge::raw_parameter_list<0> parameter_list;
    CppNewJob__class* cls = _class.get();
    cls->_onProcessIsIdle____method.call(get_jobject(), parameter_list);
}

void CppNewJob::onReferencedScheduleLoaded() const {
    ::zschimmer::javabridge::raw_parameter_list<0> parameter_list;
    CppNewJob__class* cls = _class.get();
    cls->_onReferencedScheduleLoaded____method.call(get_jobject(), parameter_list);
}

void CppNewJob::onReferencedScheduleModified() const {
    ::zschimmer::javabridge::raw_parameter_list<0> parameter_list;
    CppNewJob__class* cls = _class.get();
    cls->_onReferencedScheduleModified____method.call(get_jobject(), parameter_list);
}

bool CppNewJob::onReferencedScheduleToBeRemoved() const {
    ::zschimmer::javabridge::raw_parameter_list<0> parameter_list;
    CppNewJob__class* cls = _class.get();
    return 0 != cls->_onReferencedScheduleToBeRemoved____method.bool_call(get_jobject(), parameter_list);
}

void CppNewJob::onRemoveNow() const {
    ::zschimmer::javabridge::raw_parameter_list<0> parameter_list;
    CppNewJob__class* cls = _class.get();
    cls->_onRemoveNow____method.call(get_jobject(), parameter_list);
}

jint CppNewJob::orderSetbackMaximum() const {
    ::zschimmer::javabridge::raw_parameter_list<0> parameter_list;
    CppNewJob__class* cls = _class.get();
    return cls->_orderSetbackMaximum____method.int_call(get_jobject(), parameter_list);
}

::javaproxy::java::lang::String CppNewJob::processClassPath() const {
    ::zschimmer::javabridge::raw_parameter_list<0> parameter_list;
    CppNewJob__class* cls = _class.get();
    ::javaproxy::java::lang::String result;
    result.steal_local_ref(cls->_processClassPath____method.jobject_call(get_jobject(), parameter_list));
    return result;
}

::javaproxy::org::w3c::dom::Element CppNewJob::readHistory(const ::zschimmer::javabridge::proxy_jobject< ::javaproxy::org::w3c::dom::Document >& p0, jint p1, jint p2) const {
    ::zschimmer::javabridge::raw_parameter_list<3> parameter_list;
    parameter_list._jvalues[0].l = p0.get_jobject();
    parameter_list._jvalues[1].i = p1;
    parameter_list._jvalues[2].i = p2;
    CppNewJob__class* cls = _class.get();
    ::javaproxy::org::w3c::dom::Element result;
    result.steal_local_ref(cls->_readHistory__Lorg_w3c_dom_Document_2II__method.jobject_call(get_jobject(), parameter_list));
    return result;
}

::javaproxy::java::lang::String CppNewJob::removalError() const {
    ::zschimmer::javabridge::raw_parameter_list<0> parameter_list;
    CppNewJob__class* cls = _class.get();
    ::javaproxy::java::lang::String result;
    result.steal_local_ref(cls->_removalError____method.jobject_call(get_jobject(), parameter_list));
    return result;
}

void CppNewJob::removeRunningTask(jint p0) const {
    ::zschimmer::javabridge::raw_parameter_list<1> parameter_list;
    parameter_list._jvalues[0].i = p0;
    CppNewJob__class* cls = _class.get();
    cls->_removeRunningTask__I__method.call(get_jobject(), parameter_list);
}

void CppNewJob::setIdleTimeout(jlong p0) const {
    ::zschimmer::javabridge::raw_parameter_list<1> parameter_list;
    parameter_list._jvalues[0].j = p0;
    CppNewJob__class* cls = _class.get();
    cls->_setIdleTimeout__J__method.call(get_jobject(), parameter_list);
}

void CppNewJob::setOrderControlled() const {
    ::zschimmer::javabridge::raw_parameter_list<0> parameter_list;
    CppNewJob__class* cls = _class.get();
    cls->_setOrderControlled____method.call(get_jobject(), parameter_list);
}

void CppNewJob::setScheduleDOM(const ::zschimmer::javabridge::proxy_jobject< ::javaproxy::org::w3c::dom::Element >& p0) const {
    ::zschimmer::javabridge::raw_parameter_list<1> parameter_list;
    parameter_list._jvalues[0].l = p0.get_jobject();
    CppNewJob__class* cls = _class.get();
    cls->_setScheduleDOM__Lorg_w3c_dom_Element_2__method.call(get_jobject(), parameter_list);
}

void CppNewJob::setStateText(const ::zschimmer::javabridge::proxy_jobject< ::javaproxy::java::lang::String >& p0) const {
    ::zschimmer::javabridge::raw_parameter_list<1> parameter_list;
    parameter_list._jvalues[0].l = p0.get_jobject();
    CppNewJob__class* cls = _class.get();
    cls->_setStateText__Ljava_lang_String_2__method.call(get_jobject(), parameter_list);
}

void CppNewJob::setXmlBytes(const ::zschimmer::javabridge::Local_java_byte_array& p0) const {
    ::zschimmer::javabridge::raw_parameter_list<1> parameter_list;
    parameter_list._jvalues[0].l = p0.get_jbyteArray();
    CppNewJob__class* cls = _class.get();
    cls->_setXmlBytes___3B__method.call(get_jobject(), parameter_list);
}

void CppNewJob::signalEarlierOrder(jlong p0, const ::zschimmer::javabridge::proxy_jobject< ::javaproxy::java::lang::String >& p1, const ::zschimmer::javabridge::proxy_jobject< ::javaproxy::java::lang::String >& p2) const {
    ::zschimmer::javabridge::raw_parameter_list<3> parameter_list;
    parameter_list._jvalues[0].j = p0;
    parameter_list._jvalues[1].l = p1.get_jobject();
    parameter_list._jvalues[2].l = p2.get_jobject();
    CppNewJob__class* cls = _class.get();
    cls->_signalEarlierOrder__JLjava_lang_String_2Ljava_lang_String_2__method.call(get_jobject(), parameter_list);
}

::javaproxy::java::lang::String CppNewJob::stateString() const {
    ::zschimmer::javabridge::raw_parameter_list<0> parameter_list;
    CppNewJob__class* cls = _class.get();
    ::javaproxy::java::lang::String result;
    result.steal_local_ref(cls->_stateString____method.jobject_call(get_jobject(), parameter_list));
    return result;
}

void CppNewJob::stop() const {
    ::zschimmer::javabridge::raw_parameter_list<0> parameter_list;
    CppNewJob__class* cls = _class.get();
    cls->_stop____method.call(get_jobject(), parameter_list);
}

void CppNewJob::stopSimply() const {
    ::zschimmer::javabridge::raw_parameter_list<0> parameter_list;
    CppNewJob__class* cls = _class.get();
    cls->_stopSimply____method.call(get_jobject(), parameter_list);
}

::javaproxy::java::lang::String CppNewJob::title() const {
    ::zschimmer::javabridge::raw_parameter_list<0> parameter_list;
    CppNewJob__class* cls = _class.get();
    ::javaproxy::java::lang::String result;
    result.steal_local_ref(cls->_title____method.jobject_call(get_jobject(), parameter_list));
    return result;
}

bool CppNewJob::tryToEndATask() const {
    ::zschimmer::javabridge::raw_parameter_list<0> parameter_list;
    CppNewJob__class* cls = _class.get();
    return 0 != cls->_tryToEndATask____method.bool_call(get_jobject(), parameter_list);
}


::zschimmer::javabridge::Class* CppNewJob::java_object_class_() const { return _class.get(); }

::zschimmer::javabridge::Class* CppNewJob::java_class_() { return CppNewJob__class::class_factory.clas(); }


void CppNewJob::Lazy_class::initialize() const {
    _value = CppNewJob__class::class_factory.clas();
}


}}}}}}}
