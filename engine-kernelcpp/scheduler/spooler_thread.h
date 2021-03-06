// $Id: spooler_thread.h 14037 2010-09-28 07:46:07Z rb $

#ifndef __SPOOLER_THREAD_H
#define __SPOOLER_THREAD_H

namespace sos {
namespace scheduler {

//-----------------------------------------------------------------------------------Task_subsystem

struct Task_subsystem : Object, Subsystem, javabridge::has_proxy<Task_subsystem>
{
    Fill_zero                  _zero_;

    Z_GNU_ONLY(                 Task_subsystem              (); )                                   // Für gcc 3.2.1
                                Task_subsystem              ( Spooler* );
                               ~Task_subsystem              ();


    virtual void                close                       ();

    bool                        do_something                ();
    string                      name                        () const                                { return "task"; }
    xml::Element_ptr            dom_element                 ( const xml::Document_ptr&, const Show_what& );
    
    xml::Element_ptr            state_task_statistic_element( const xml::Document_ptr& dom_document, Task::State state ) const;
    xml::Element_ptr            task_statistic_element      ( const xml::Document_ptr& dom_document, const string& attribute_name, const string& attribute_value, int count ) const;
    int                         count_tasks_with_state      ( Task::State state ) const;
    int                         count_tasks_exist           () const;
    xml::Element_ptr            exist_task_statistic_element( const xml::Document_ptr& dom_document ) const;
    
    bool                        has_tasks                   ()                                      { return !_task_set.empty(); }

    void                        add_task                    ( Task* task )                          { _task_set.insert(task);  signal( task->obj_name() ); }

    string                      task_log                    (int task_id) const;
    ptr<Task>                   get_task_or_null            ( int task_id ) const;
    Task*                       get_next_task               ();

    void                        increment_running_tasks     ();
    void                        decrement_running_tasks     ();
    void                        count_started_tasks         ();
    void                        count_finished_tasks        ();
    int                         finished_tasks_count        () const                                { return _finished_tasks_count; }

    void                        count_task                  ()                                      { InterlockedIncrement( &_task_count ); }
    void                        count_step                  ()                                      { InterlockedIncrement( &_step_count ); }

    bool                        try_to_free_process         (Process_class_requestor*, Process_class*);
    void                        end_all_tasks               (Task_end_mode = task_end_normal);

    // Für andere Threads:
    bool                        is_ready_for_termination    ();
    void                        signal                      ( const string& signal_name )           { if( _event )  _event->signal(signal_name); }

    virtual string             _obj_name                    () const                                { return "Task_subsystem"; }



    Task*                       get_next_task_to_run        ();
    void                        remove_task                 (Task*);
    void                        build_prioritized_order_job_array();




    Event*                     _event;

    Task_set                   _task_set;
    bool                       _task_closed;

    vector<Job*>               _prioritized_order_job_array;            // Jobs am Ende einer Jobkette priorisieren
    int                        _prioritized_order_job_array_job_chain_version;    // Wenn verschieden von _spooler->_job_chain_map, _prioritized_order_job_array neu aufbauen!
    int                        _prioritized_order_job_array_job_version;

    long32                     _running_tasks_count;        // Wenn 0, dann warten
                                                            // Statistik
    long32                     _step_count;                 // Seit Spooler-Start ausgeführte Schritte
    long32                     _task_count;                 // Seit Spooler-Start gestartetet Tasks

    int                        _started_tasks_count;
    int                        _finished_tasks_count;
};

//-------------------------------------------------------------------------------------------------

} //namespace scheduler
} //namespace sos

#endif
