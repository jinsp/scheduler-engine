// *** Generated by com.sos.scheduler.engine.cplusplus.generator ***

#ifndef _JAVAPROXY_COM_SOS_SCHEDULER_ENGINE_KERNEL_ORDER_JOBCHAIN_NESTEDJOBCHAINNODE_H_
#define _JAVAPROXY_COM_SOS_SCHEDULER_ENGINE_KERNEL_ORDER_JOBCHAIN_NESTEDJOBCHAINNODE_H_

#include "../zschimmer/zschimmer.h"
#include "../zschimmer/java.h"
#include "../zschimmer/Has_proxy.h"
#include "../zschimmer/javaproxy.h"
#include "../zschimmer/lazy.h"
#include "com__sos__scheduler__engine__kernel__order__jobchain__Node.h"
#include "java__lang__Object.h"

namespace javaproxy { namespace com { namespace sos { namespace scheduler { namespace engine { namespace kernel { namespace order { namespace jobchain { struct Node; }}}}}}}}
namespace javaproxy { namespace java { namespace lang { struct String; }}}


namespace javaproxy { namespace com { namespace sos { namespace scheduler { namespace engine { namespace kernel { namespace order { namespace jobchain { 


struct NestedJobChainNode__class;

struct NestedJobChainNode : ::zschimmer::javabridge::proxy_jobject< NestedJobChainNode >, ::javaproxy::com::sos::scheduler::engine::kernel::order::jobchain::Node {
  private:
    static NestedJobChainNode new_instance();  // Not implemented
  public:

    NestedJobChainNode(jobject = NULL);

    NestedJobChainNode(const NestedJobChainNode&);

    #ifdef Z_HAS_MOVE_CONSTRUCTOR
        NestedJobChainNode(NestedJobChainNode&&);
    #endif

    ~NestedJobChainNode();

    NestedJobChainNode& operator=(jobject jo) { assign_(jo); return *this; }
    NestedJobChainNode& operator=(const NestedJobChainNode& o) { assign_(o.get_jobject()); return *this; }
    #ifdef Z_HAS_MOVE_CONSTRUCTOR
        NestedJobChainNode& operator=(NestedJobChainNode&& o) { set_jobject(o.get_jobject()); o.set_jobject(NULL); return *this; }
    #endif

    jobject get_jobject() const { return ::zschimmer::javabridge::proxy_jobject< NestedJobChainNode >::get_jobject(); }

  protected:
    void set_jobject(jobject jo) {
        ::zschimmer::javabridge::proxy_jobject< NestedJobChainNode >::set_jobject(jo);
        ::javaproxy::com::sos::scheduler::engine::kernel::order::jobchain::Node::set_jobject(jo);
    }
  public:


    ::zschimmer::javabridge::Class* java_object_class_() const;

    static ::zschimmer::javabridge::Class* java_class_();


  private:
    struct Lazy_class : ::zschimmer::abstract_lazy<NestedJobChainNode__class*> {
        void initialize() const;
    };

    Lazy_class _class;
};


}}}}}}}}

#endif