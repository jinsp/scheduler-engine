// *** Generated by com.sos.scheduler.engine.cplusplus.generator ***

package com.sos.scheduler.engine.kernel.cppproxy;

@javax.annotation.Generated("C++/Java-Generator - SOS GmbH Berlin")
@SuppressWarnings({"unchecked", "rawtypes"})
final class Prefix_logCImpl
extends com.sos.scheduler.engine.cplusplus.runtime.CppProxyImpl<com.sos.scheduler.engine.kernel.log.PrefixLog>
implements com.sos.scheduler.engine.kernel.cppproxy.Prefix_logC {

    // <editor-fold defaultstate="collapsed" desc="Generated code - DO NOT EDIT">

    private Prefix_logCImpl(com.sos.scheduler.engine.cplusplus.runtime.Sister context) { // Nur für JNI zugänglich
        setSister(sisterType.sister(this, context));
    }

    @Override public java.lang.String java_last(java.lang.String p0) {
        com.sos.scheduler.engine.cplusplus.runtime.CppProxy.threadLock.lock();
        try {
            java.lang.String result = java_last__native(cppReference(), p0);
            checkIsNotReleased(java.lang.String.class, result);
            return result;
        }
        catch (Exception x) { throw com.sos.scheduler.engine.cplusplus.runtime.CppProxies.propagateCppException(x, this); }
        finally {
            com.sos.scheduler.engine.cplusplus.runtime.CppProxy.threadLock.unlock();
        }
    }

    private static native java.lang.String java_last__native(long cppReference, java.lang.String p0);


    @Override public void java_log(int p0, java.lang.String p1) {
        com.sos.scheduler.engine.cplusplus.runtime.CppProxy.threadLock.lock();
        try {
            java_log__native(cppReference(), p0, p1);
        }
        catch (Exception x) { throw com.sos.scheduler.engine.cplusplus.runtime.CppProxies.propagateCppException(x, this); }
        finally {
            com.sos.scheduler.engine.cplusplus.runtime.CppProxy.threadLock.unlock();
        }
    }

    private static native void java_log__native(long cppReference, int p0, java.lang.String p1);


    @Override public boolean started() {
        com.sos.scheduler.engine.cplusplus.runtime.CppProxy.threadLock.lock();
        try {
            return started__native(cppReference());
        }
        catch (Exception x) { throw com.sos.scheduler.engine.cplusplus.runtime.CppProxies.propagateCppException(x, this); }
        finally {
            com.sos.scheduler.engine.cplusplus.runtime.CppProxy.threadLock.unlock();
        }
    }

    private static native boolean started__native(long cppReference);


    @Override public java.lang.String this_filename() {
        com.sos.scheduler.engine.cplusplus.runtime.CppProxy.threadLock.lock();
        try {
            java.lang.String result = this_filename__native(cppReference());
            checkIsNotReleased(java.lang.String.class, result);
            return result;
        }
        catch (Exception x) { throw com.sos.scheduler.engine.cplusplus.runtime.CppProxies.propagateCppException(x, this); }
        finally {
            com.sos.scheduler.engine.cplusplus.runtime.CppProxy.threadLock.unlock();
        }
    }

    private static native java.lang.String this_filename__native(long cppReference);


    // </editor-fold>
}