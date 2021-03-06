// *** Generated by com.sos.scheduler.engine.cplusplus.generator ***

#include "_precompiled.h"

#include "java__util__AbstractList.h"
#include "java__lang__Object.h"
#include "java__lang__String.h"
#include "java__util__AbstractCollection.h"
#include "java__util__List.h"

namespace javaproxy { namespace java { namespace util { 

struct AbstractList__class : ::zschimmer::javabridge::Class
{
    AbstractList__class(const string& class_name);
   ~AbstractList__class();

    ::zschimmer::javabridge::Method const _add__ILjava_lang_Object_2__method;
    ::zschimmer::javabridge::Method const _add__Ljava_lang_Object_2__method;
    ::zschimmer::javabridge::Method const _clear____method;
    ::zschimmer::javabridge::Method const _equals__Ljava_lang_Object_2__method;
    ::zschimmer::javabridge::Method const _get__I__method;
    ::zschimmer::javabridge::Method const _hashCode____method;
    ::zschimmer::javabridge::Method const _indexOf__Ljava_lang_Object_2__method;
    ::zschimmer::javabridge::Method const _lastIndexOf__Ljava_lang_Object_2__method;
    ::zschimmer::javabridge::Method const _remove__I__method;
    ::zschimmer::javabridge::Method const _set__ILjava_lang_Object_2__method;
    ::zschimmer::javabridge::Method const _subList__II__method;

    static const ::zschimmer::javabridge::class_factory< AbstractList__class > class_factory;
};

const ::zschimmer::javabridge::class_factory< AbstractList__class > AbstractList__class::class_factory ("java.util.AbstractList");

AbstractList__class::AbstractList__class(const string& class_name) :
    ::zschimmer::javabridge::Class(class_name)
    ,_add__ILjava_lang_Object_2__method(this, "add", "(ILjava/lang/Object;)V")
    ,_add__Ljava_lang_Object_2__method(this, "add", "(Ljava/lang/Object;)Z")
    ,_clear____method(this, "clear", "()V")
    ,_equals__Ljava_lang_Object_2__method(this, "equals", "(Ljava/lang/Object;)Z")
    ,_get__I__method(this, "get", "(I)Ljava/lang/Object;")
    ,_hashCode____method(this, "hashCode", "()I")
    ,_indexOf__Ljava_lang_Object_2__method(this, "indexOf", "(Ljava/lang/Object;)I")
    ,_lastIndexOf__Ljava_lang_Object_2__method(this, "lastIndexOf", "(Ljava/lang/Object;)I")
    ,_remove__I__method(this, "remove", "(I)Ljava/lang/Object;")
    ,_set__ILjava_lang_Object_2__method(this, "set", "(ILjava/lang/Object;)Ljava/lang/Object;")
    ,_subList__II__method(this, "subList", "(II)Ljava/util/List;"){}

AbstractList__class::~AbstractList__class() {}




AbstractList::AbstractList(jobject jo) { if (jo) assign_(jo); }

AbstractList::AbstractList(const AbstractList& o) { assign_(o.get_jobject()); }

#ifdef Z_HAS_MOVE_CONSTRUCTOR
    AbstractList::AbstractList(AbstractList&& o) { set_jobject(o.get_jobject());  o.set_jobject(NULL); }
#endif

AbstractList::~AbstractList() { assign_(NULL); }




void AbstractList::add(jint p0, const ::zschimmer::javabridge::proxy_jobject< ::javaproxy::java::lang::Object >& p1) const {
    ::zschimmer::javabridge::raw_parameter_list<2> parameter_list;
    parameter_list._jvalues[0].i = p0;
    parameter_list._jvalues[1].l = p1.get_jobject();
    AbstractList__class* cls = _class.get();
    cls->_add__ILjava_lang_Object_2__method.call(get_jobject(), parameter_list);
}

bool AbstractList::add(const ::zschimmer::javabridge::proxy_jobject< ::javaproxy::java::lang::Object >& p0) const {
    ::zschimmer::javabridge::raw_parameter_list<1> parameter_list;
    parameter_list._jvalues[0].l = p0.get_jobject();
    AbstractList__class* cls = _class.get();
    return 0 != cls->_add__Ljava_lang_Object_2__method.bool_call(get_jobject(), parameter_list);
}

void AbstractList::clear() const {
    ::zschimmer::javabridge::raw_parameter_list<0> parameter_list;
    AbstractList__class* cls = _class.get();
    cls->_clear____method.call(get_jobject(), parameter_list);
}

bool AbstractList::equals(const ::zschimmer::javabridge::proxy_jobject< ::javaproxy::java::lang::Object >& p0) const {
    ::zschimmer::javabridge::raw_parameter_list<1> parameter_list;
    parameter_list._jvalues[0].l = p0.get_jobject();
    AbstractList__class* cls = _class.get();
    return 0 != cls->_equals__Ljava_lang_Object_2__method.bool_call(get_jobject(), parameter_list);
}

::javaproxy::java::lang::Object AbstractList::get(jint p0) const {
    ::zschimmer::javabridge::raw_parameter_list<1> parameter_list;
    parameter_list._jvalues[0].i = p0;
    AbstractList__class* cls = _class.get();
    ::javaproxy::java::lang::Object result;
    result.steal_local_ref(cls->_get__I__method.jobject_call(get_jobject(), parameter_list));
    return result;
}

jint AbstractList::hashCode() const {
    ::zschimmer::javabridge::raw_parameter_list<0> parameter_list;
    AbstractList__class* cls = _class.get();
    return cls->_hashCode____method.int_call(get_jobject(), parameter_list);
}

jint AbstractList::indexOf(const ::zschimmer::javabridge::proxy_jobject< ::javaproxy::java::lang::Object >& p0) const {
    ::zschimmer::javabridge::raw_parameter_list<1> parameter_list;
    parameter_list._jvalues[0].l = p0.get_jobject();
    AbstractList__class* cls = _class.get();
    return cls->_indexOf__Ljava_lang_Object_2__method.int_call(get_jobject(), parameter_list);
}

jint AbstractList::lastIndexOf(const ::zschimmer::javabridge::proxy_jobject< ::javaproxy::java::lang::Object >& p0) const {
    ::zschimmer::javabridge::raw_parameter_list<1> parameter_list;
    parameter_list._jvalues[0].l = p0.get_jobject();
    AbstractList__class* cls = _class.get();
    return cls->_lastIndexOf__Ljava_lang_Object_2__method.int_call(get_jobject(), parameter_list);
}

::javaproxy::java::lang::Object AbstractList::remove(jint p0) const {
    ::zschimmer::javabridge::raw_parameter_list<1> parameter_list;
    parameter_list._jvalues[0].i = p0;
    AbstractList__class* cls = _class.get();
    ::javaproxy::java::lang::Object result;
    result.steal_local_ref(cls->_remove__I__method.jobject_call(get_jobject(), parameter_list));
    return result;
}

::javaproxy::java::lang::Object AbstractList::set(jint p0, const ::zschimmer::javabridge::proxy_jobject< ::javaproxy::java::lang::Object >& p1) const {
    ::zschimmer::javabridge::raw_parameter_list<2> parameter_list;
    parameter_list._jvalues[0].i = p0;
    parameter_list._jvalues[1].l = p1.get_jobject();
    AbstractList__class* cls = _class.get();
    ::javaproxy::java::lang::Object result;
    result.steal_local_ref(cls->_set__ILjava_lang_Object_2__method.jobject_call(get_jobject(), parameter_list));
    return result;
}

::javaproxy::java::util::List AbstractList::subList(jint p0, jint p1) const {
    ::zschimmer::javabridge::raw_parameter_list<2> parameter_list;
    parameter_list._jvalues[0].i = p0;
    parameter_list._jvalues[1].i = p1;
    AbstractList__class* cls = _class.get();
    ::javaproxy::java::util::List result;
    result.steal_local_ref(cls->_subList__II__method.jobject_call(get_jobject(), parameter_list));
    return result;
}


::zschimmer::javabridge::Class* AbstractList::java_object_class_() const { return _class.get(); }

::zschimmer::javabridge::Class* AbstractList::java_class_() { return AbstractList__class::class_factory.clas(); }


void AbstractList::Lazy_class::initialize() const {
    _value = AbstractList__class::class_factory.clas();
}


}}}
