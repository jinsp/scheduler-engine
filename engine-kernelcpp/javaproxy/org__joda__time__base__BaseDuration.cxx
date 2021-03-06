// *** Generated by com.sos.scheduler.engine.cplusplus.generator ***

#include "_precompiled.h"

#include "org__joda__time__base__BaseDuration.h"
#include "java__lang__String.h"
#include "org__joda__time__ReadableInstant.h"
#include "org__joda__time__base__AbstractDuration.h"

namespace javaproxy { namespace org { namespace joda { namespace time { namespace base { 

struct BaseDuration__class : ::zschimmer::javabridge::Class
{
    BaseDuration__class(const string& class_name);
   ~BaseDuration__class();

    ::zschimmer::javabridge::Method const _getMillis____method;

    static const ::zschimmer::javabridge::class_factory< BaseDuration__class > class_factory;
};

const ::zschimmer::javabridge::class_factory< BaseDuration__class > BaseDuration__class::class_factory ("org.joda.time.base.BaseDuration");

BaseDuration__class::BaseDuration__class(const string& class_name) :
    ::zschimmer::javabridge::Class(class_name)
    ,_getMillis____method(this, "getMillis", "()J"){}

BaseDuration__class::~BaseDuration__class() {}




BaseDuration::BaseDuration(jobject jo) { if (jo) assign_(jo); }

BaseDuration::BaseDuration(const BaseDuration& o) { assign_(o.get_jobject()); }

#ifdef Z_HAS_MOVE_CONSTRUCTOR
    BaseDuration::BaseDuration(BaseDuration&& o) { set_jobject(o.get_jobject());  o.set_jobject(NULL); }
#endif

BaseDuration::~BaseDuration() { assign_(NULL); }




jlong BaseDuration::getMillis() const {
    ::zschimmer::javabridge::raw_parameter_list<0> parameter_list;
    BaseDuration__class* cls = _class.get();
    return cls->_getMillis____method.long_call(get_jobject(), parameter_list);
}


::zschimmer::javabridge::Class* BaseDuration::java_object_class_() const { return _class.get(); }

::zschimmer::javabridge::Class* BaseDuration::java_class_() { return BaseDuration__class::class_factory.clas(); }


void BaseDuration::Lazy_class::initialize() const {
    _value = BaseDuration__class::class_factory.clas();
}


}}}}}
