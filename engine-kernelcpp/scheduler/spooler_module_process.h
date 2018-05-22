// $Id: spooler_module_process.h 13807 2009-11-18 08:59:21Z jz $        Joacim Zschimmer, Zschimmer GmbH, http://www.zschimmer.com

#ifndef __SPOOLER_MODULE_PROCESS_H
#define __SPOOLER_MODULE_PROCESS_H

namespace sos {
namespace scheduler {

//--------------------------------------------------------------------------Process_module_instance

struct Process_module_instance : Module_instance
{
#   ifndef Z_WINDOWS
        struct Process_event : Event
        {
                                        Process_event               ( const string& name )                  : Event(name) {}

            virtual void                close                       ();
            bool                        signaled                    ();
            virtual bool                wait                        ( double seconds = INT_MAX );
                                        operator bool               ()                                      { return _pid != 0; }

            int                        _pid;
            int                        _process_signaled;
            int                        _process_exit_code;
        };
#   endif


                                Process_module_instance     ( Module* );
                               ~Process_module_instance     ();

  //virtual void                attach_task                 ( Task*, Prefix_log* );

    void                        init                        ();
    bool                        load                        ();
    void                        close__end                  ();
    void                        close_process               ();
    Async_operation*            begin__start                ();
    bool                        begin__end                  ();
    void                        end__end                    ();
    Variant                     step__end                   ();
    Variant                     call                        ( const string& name );
    Variant                     call                        ( const string& name, const Variant& param, const Variant& );
    bool                        name_exists                 ( const string& name );
    bool                        loaded                      ()                                      { return _idispatch != NULL; }
    bool                        callable                    ()                                      { return _idispatch != NULL; }
    bool                        kill                        (int unix_signal);
    bool                        process_has_signaled        ();
    string                      get_first_line_as_state_text();
    void                        fill_process_environment_with_params();
    string                      step_result                 ();
    void                        transfer_back_order_params  ();
    void                        fetch_parameters_from_process( Com_variable_set* );
    File_path                   stdout_path                 ()                                      { return _stdout_path; }
    File_path                   stderr_path                 ()                                      { return _stderr_file.path(); }
    void                    set_stdout_path                 (const File_path& path)                 { _stdout_path = path; }
    bool                        try_delete_files            ( Has_log* );
    std::list<File_path>        undeleted_files             ();
    bool                        is_kill_thread_running      ()                                      { return _kill_thread  &&  _kill_thread->thread_is_running(); }

  private:
    void                        close_handle                ();
    string                      program_path                ();
    std::vector<string>         get_string_args             ();
    string                      get_parameter_name          (const string& suffix);


    Fill_zero                  _zero_;
    File_path                  _stdout_path;
    File                       _stdout_file;
    File                       _stderr_file;
    Mapped_file                _order_params_file;

    bool                       _is_killed;
    File                       _shell_file;

#   ifdef Z_WINDOWS
        Event                  _process_handle;
#    else
        Process_event          _process_handle;
        int                    _pid_to_unregister;
#   endif

    ptr<Thread>                _kill_thread;
    bool                       _spooler_process_before_called;
};

//-------------------------------------------------------------------------------------------------

} //namespace scheduler
} //namespace sos

#endif
