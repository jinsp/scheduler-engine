// *** Generated by com.sos.scheduler.engine.cplusplus.generator ***

#ifndef _JAVAPROXY_COM_SOS_SCHEDULER_ENGINE_KERNEL_ORDER_ORDER_H_
#define _JAVAPROXY_COM_SOS_SCHEDULER_ENGINE_KERNEL_ORDER_ORDER_H_

#include "../zschimmer/zschimmer.h"
#include "../zschimmer/java.h"
#include "../zschimmer/Has_proxy.h"
#include "../zschimmer/javaproxy.h"
#include "../zschimmer/lazy.h"
#include "java__lang__Object.h"

namespace javaproxy { namespace com { namespace sos { namespace scheduler { namespace engine { namespace kernel { namespace async { struct CppCall; }}}}}}}
namespace javaproxy { namespace com { namespace sos { namespace scheduler { namespace engine { namespace kernel { namespace filebased { struct FileBased; }}}}}}}
namespace javaproxy { namespace com { namespace sos { namespace scheduler { namespace engine { namespace kernel { namespace order { struct UnmodifiableOrder; }}}}}}}
namespace javaproxy { namespace java { namespace lang { struct Object; }}}
namespace javaproxy { namespace java { namespace lang { struct String; }}}


namespace javaproxy { namespace com { namespace sos { namespace scheduler { namespace engine { namespace kernel { namespace order { 


struct Order__class;

struct Order : ::zschimmer::javabridge::proxy_jobject< Order >, ::javaproxy::java::lang::Object {
  private:
    static Order new_instance();  // Not implemented
  public:

    Order(jobject = NULL);

    Order(const Order&);

    #ifdef Z_HAS_MOVE_CONSTRUCTOR
        Order(Order&&);
    #endif

    ~Order();

    Order& operator=(jobject jo) { assign_(jo); return *this; }
    Order& operator=(const Order& o) { assign_(o.get_jobject()); return *this; }
    #ifdef Z_HAS_MOVE_CONSTRUCTOR
        Order& operator=(Order&& o) { set_jobject(o.get_jobject()); o.set_jobject(NULL); return *this; }
    #endif

    jobject get_jobject() const { return ::zschimmer::javabridge::proxy_jobject< Order >::get_jobject(); }

  protected:
    void set_jobject(jobject jo) {
        ::zschimmer::javabridge::proxy_jobject< Order >::set_jobject(jo);
        ::javaproxy::java::lang::Object::set_jobject(jo);
    }
  public:

    void agentFileExists(const ::zschimmer::javabridge::proxy_jobject< ::javaproxy::com::sos::scheduler::engine::kernel::async::CppCall >& p0) const;
    void persistState() const;

    ::zschimmer::javabridge::Class* java_object_class_() const;

    static ::zschimmer::javabridge::Class* java_class_();


  private:
    struct Lazy_class : ::zschimmer::abstract_lazy<Order__class*> {
        void initialize() const;
    };

    Lazy_class _class;
};


}}}}}}}

#endif