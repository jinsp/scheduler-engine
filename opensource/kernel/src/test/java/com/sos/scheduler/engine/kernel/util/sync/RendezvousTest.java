package com.sos.scheduler.engine.kernel.util.sync;

import com.sos.scheduler.engine.common.sync.Rendezvous;
import com.sos.scheduler.engine.common.time.Time;
import org.junit.After;
import org.junit.Before;
import org.junit.Test;

import static org.junit.Assert.fail;

public final class RendezvousTest {
    private final Rendezvous<Arg,Result> rendezvous = new Rendezvous<Arg,Result>();
    private final MyThread thread = new MyThread();
    private final Time timeout = Time.of(3);

    @Before public void before() {
        thread.start();
    }

    @After public void after() throws Exception {
        thread.join(timeout.getMillis());
        if (thread.isAlive()) {
            fail("Thread does not terminate");
            thread.interrupt();
        }
    }

    @Test public void test1() throws Exception {
        Result r = rendezvous.call(new Arg(1));
        assert(r.value == 2);
        rendezvous.call(new TerminateArg());
    }


    @Test public void testException() throws Exception {
        try {
            Result r = rendezvous.call(new Arg(-1));
            fail("Exception expected");
        }
        catch (MyException x) {} // Okay

        rendezvous.call(new TerminateArg());
    }


    private static class Arg {
        private final int value;
        private Arg(int o) { value = o; }
    }


    private class TerminateArg extends Arg {
        private TerminateArg() { super(0); }
    }

    
    private static class Result {
        private final int value;
        private Result(int o) { value = o; }
    }


    private class MyThread extends Thread {
        @Override public void run() {
            try {
                rendezvous.beginServing();
                while(true) {
                    boolean ok = serve();
                    if (!ok) break;
                }
            }
            finally {
                rendezvous.closeServing();
            }
        }

        private boolean serve() {
            Arg a = rendezvous.enter(timeout);
            Result r = null;
            try {
                r = f(a);
            }
            catch(RuntimeException x) {
                rendezvous.leaveException(x);
            }
            finally {
                if (rendezvous.isInRendezvous())    // Wenn keine Exception
                    rendezvous.leave(r);
            }
            return a.value != 0;
        }

        private Result f(Arg a) {
            if (a.value == -1)  throw new MyException();
            return new Result(a.value + 1);
        }
    }
        
    private static class MyException extends RuntimeException {}
}