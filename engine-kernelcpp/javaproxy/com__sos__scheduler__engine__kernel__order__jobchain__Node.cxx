// *** Generated by com.sos.scheduler.engine.cplusplus.generator ***

#include "_precompiled.h"

#include "com__sos__scheduler__engine__kernel__order__jobchain__Node.h"
#include "com__sos__scheduler__engine__cplusplus__runtime__Sister.h"
#include "java__lang__Object.h"
#include "java__lang__String.h"
#include "org__w3c__dom__Element.h"

namespace javaproxy { namespace com { namespace sos { namespace scheduler { namespace engine { namespace kernel { namespace order { namespace jobchain { 

struct Node__class : ::zschimmer::javabridge::Class
{
    Node__class(const string& class_name);
   ~Node__class();

    ::zschimmer::javabridge::Method const _processConfigurationDomElement__Lorg_w3c_dom_Element_2__method;

    static const ::zschimmer::javabridge::class_factory< Node__class > class_factory;
};

const ::zschimmer::javabridge::class_factory< Node__class > Node__class::class_factory ("com.sos.scheduler.engine.kernel.order.jobchain.Node");

Node__class::Node__class(const string& class_name) :
    ::zschimmer::javabridge::Class(class_name)
    ,_processConfigurationDomElement__Lorg_w3c_dom_Element_2__method(this, "processConfigurationDomElement", "(Lorg/w3c/dom/Element;)V"){}

Node__class::~Node__class() {}




Node::Node(jobject jo) { if (jo) assign_(jo); }

Node::Node(const Node& o) { assign_(o.get_jobject()); }

#ifdef Z_HAS_MOVE_CONSTRUCTOR
    Node::Node(Node&& o) { set_jobject(o.get_jobject());  o.set_jobject(NULL); }
#endif

Node::~Node() { assign_(NULL); }




void Node::processConfigurationDomElement(const ::zschimmer::javabridge::proxy_jobject< ::javaproxy::org::w3c::dom::Element >& p0) const {
    ::zschimmer::javabridge::raw_parameter_list<1> parameter_list;
    parameter_list._jvalues[0].l = p0.get_jobject();
    Node__class* cls = _class.get();
    cls->_processConfigurationDomElement__Lorg_w3c_dom_Element_2__method.call(get_jobject(), parameter_list);
}


::zschimmer::javabridge::Class* Node::java_object_class_() const { return _class.get(); }

::zschimmer::javabridge::Class* Node::java_class_() { return Node__class::class_factory.clas(); }


void Node::Lazy_class::initialize() const {
    _value = Node__class::class_factory.clas();
}


}}}}}}}}