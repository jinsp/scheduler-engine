// *** Generated by com.sos.scheduler.engine.cplusplus.generator ***

#include "_precompiled.h"

#include "com__sos__scheduler__engine__kernel__cppproxy__Order_subsystemC.h"
#include "java__lang__Object.h"
#include "java__lang__String.h"

namespace javaproxy { namespace com { namespace sos { namespace scheduler { namespace engine { namespace kernel { namespace cppproxy { 

struct Order_subsystemC__class : ::zschimmer::javabridge::Class
{
    Order_subsystemC__class(const string& class_name);
   ~Order_subsystemC__class();


    static const ::zschimmer::javabridge::class_factory< Order_subsystemC__class > class_factory;
};

const ::zschimmer::javabridge::class_factory< Order_subsystemC__class > Order_subsystemC__class::class_factory ("com.sos.scheduler.engine.kernel.cppproxy.Order_subsystemC");

Order_subsystemC__class::Order_subsystemC__class(const string& class_name) :
    ::zschimmer::javabridge::Class(class_name)
{}

Order_subsystemC__class::~Order_subsystemC__class() {}




Order_subsystemC::Order_subsystemC(jobject jo) { if (jo) assign_(jo); }

Order_subsystemC::Order_subsystemC(const Order_subsystemC& o) { assign_(o.get_jobject()); }

#ifdef Z_HAS_MOVE_CONSTRUCTOR
    Order_subsystemC::Order_subsystemC(Order_subsystemC&& o) { set_jobject(o.get_jobject());  o.set_jobject(NULL); }
#endif

Order_subsystemC::~Order_subsystemC() { assign_(NULL); }





::zschimmer::javabridge::Class* Order_subsystemC::java_object_class_() const { return _class.get(); }

::zschimmer::javabridge::Class* Order_subsystemC::java_class_() { return Order_subsystemC__class::class_factory.clas(); }


void Order_subsystemC::Lazy_class::initialize() const {
    _value = Order_subsystemC__class::class_factory.clas();
}


}}}}}}}