// *** Generated by com.sos.scheduler.engine.cplusplus.generator ***

#include "_precompiled.h"

#include "com__sos__scheduler__engine__kernel__order__jobchain__OrderQueueNode.h"
#include "com__sos__scheduler__engine__kernel__order__jobchain__Node.h"
#include "java__lang__String.h"

namespace javaproxy { namespace com { namespace sos { namespace scheduler { namespace engine { namespace kernel { namespace order { namespace jobchain { 

struct OrderQueueNode__class : ::zschimmer::javabridge::Class
{
    OrderQueueNode__class(const string& class_name);
   ~OrderQueueNode__class();

    ::zschimmer::javabridge::Method const __constructor____method;
    ::zschimmer::javabridge::Method const _orderCount____method;

    static const ::zschimmer::javabridge::class_factory< OrderQueueNode__class > class_factory;
};

const ::zschimmer::javabridge::class_factory< OrderQueueNode__class > OrderQueueNode__class::class_factory ("com.sos.scheduler.engine.kernel.order.jobchain.OrderQueueNode");

OrderQueueNode__class::OrderQueueNode__class(const string& class_name) :
    ::zschimmer::javabridge::Class(class_name)
    ,__constructor____method(this, "<init>", "()V")
    ,_orderCount____method(this, "orderCount", "()I"){}

OrderQueueNode__class::~OrderQueueNode__class() {}



OrderQueueNode OrderQueueNode::new_instance() {
    OrderQueueNode result;
    result.java_object_allocate_();
    ::zschimmer::javabridge::raw_parameter_list<0> parameter_list;
    OrderQueueNode__class* cls = result._class.get();
    cls->__constructor____method.call(result.get_jobject(), parameter_list);
    return result;
}


OrderQueueNode::OrderQueueNode(jobject jo) { if (jo) assign_(jo); }

OrderQueueNode::OrderQueueNode(const OrderQueueNode& o) { assign_(o.get_jobject()); }

#ifdef Z_HAS_MOVE_CONSTRUCTOR
    OrderQueueNode::OrderQueueNode(OrderQueueNode&& o) { set_jobject(o.get_jobject());  o.set_jobject(NULL); }
#endif

OrderQueueNode::~OrderQueueNode() { assign_(NULL); }




jint OrderQueueNode::orderCount() const {
    ::zschimmer::javabridge::raw_parameter_list<0> parameter_list;
    OrderQueueNode__class* cls = _class.get();
    return cls->_orderCount____method.int_call(get_jobject(), parameter_list);
}


::zschimmer::javabridge::Class* OrderQueueNode::java_object_class_() const { return _class.get(); }

::zschimmer::javabridge::Class* OrderQueueNode::java_class_() { return OrderQueueNode__class::class_factory.clas(); }


void OrderQueueNode::Lazy_class::initialize() const {
    _value = OrderQueueNode__class::class_factory.clas();
}


}}}}}}}}
