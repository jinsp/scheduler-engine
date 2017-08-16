// $Id: schedule.cxx 14164 2010-12-21 16:44:47Z rb $        Joacim Zschimmer, Zschimmer GmbH, http://www.zschimmer.com

#include "spooler.h"

namespace sos {
namespace scheduler {
namespace schedule {

//
//  Schedule_use könnte die Schedule-Logik von Job und Order übernehmen.
//
//  Schedule_use::_current_period: aktuelle oder, wenn es keine gibt, die nächste Periode
//  Schedule_use::_next_single_start: Nächster single_start
//  Zu beiden Angaben, aus welchem Schedule sie stammen
//  
//  Nächster Start wegen repeat oder absolute_repeat
//  

//-------------------------------------------------------------------------------Schedule_subsystem
    
struct Schedule_subsystem : Schedule_subsystem_interface
{
                                Schedule_subsystem          ( Scheduler* );

    // Subsystem:
    void                        close                       ();
    bool                        subsystem_initialize        ();
    bool                        subsystem_load              ();
    bool                        subsystem_activate          ();


    // File_based_subsystem

    string                      xml_element_name            () const                                { return "schedule"; }
    string                      xml_elements_name           () const                                { return "schedules"; }
    void                        assert_xml_element_name     ( const xml::Element_ptr& ) const;
    string                      object_type_name            () const                                { return "Schedule"; }
    string                      filename_extension          () const                                { return ".schedule.xml"; }
  //string                      normalized_name             ( const string& name ) const            { return lcase( name ); }
    ptr<Schedule>               new_file_based              (const string& source);
    xml::Element_ptr            new_file_baseds_dom_element ( const xml::Document_ptr& doc, const Show_what& ) { return doc.createElement( "schedules" ); }
    ptr<Schedule_folder>        new_schedule_folder         ( Folder* folder )                      { return Z_NEW( Schedule_folder( folder ) ); }


  //xml::Element_ptr            execute_xml                 ( Command_processor*, const xml::Element_ptr&, const Show_what& );
};

//-------------------------------------------------------------------------------------------------

Schedule    ::Class_descriptor  Schedule    ::class_descriptor  ( &typelib, "sos.spooler.Schedule", Schedule    ::_methods );
Schedule_use::Class_descriptor  Schedule_use::class_descriptor  ( &typelib, "sos.spooler.Run_time", Schedule_use::_methods );

const int                       max_include_nesting         = 10;
const int                       foresee_years               = 20;       // Längstens soviele Jahre voraussehen

const char* weekday_names[] = { "so"     , "mo"    , "di"      , "mi"       , "do"        , "fr"     , "sa"      ,
                                "sonntag", "montag", "dienstag", "mittwoch" , "donnerstag", "freitag", "samstag" ,
                                "sun"    , "mon"   , "tue"     , "wed"      , "thu"       , "fri"    , "sat"     ,
                                "sunday" , "monday", "tuesday" , "wednesday", "thursday"  , "friday" , "saturday",
                                NULL };

const char* month_names  [] = { "january", "february", "march", "april", "may", "june", "july", "august", "september", "october", "november", "december" };

//----------------------------------------------------------------------------weekday_of_day_number

inline int weekday_of_day_number( int day_number )
{
    return ( day_number + 4 ) % 7;
}

//-------------------------------------------------------------------------------get_weekday_number

static int get_weekday_number( const string& weekday_name )
{
    int    result     = -1;
    string day_string = lcase( weekday_name );
    
    for( const char** p = weekday_names; *p && result == -1; p++ )
        if( day_string == *p )  result = ( p - weekday_names ) % 7;

    if( result == -1 )
    {
        result = as_int( weekday_name );
        if( result == 7 )  result = 0;      // Sonntag darf auch 7 sein
    }

    if( result < 0  ||  result > 6 )  z::throw_xc( "SCHEDULER-445", weekday_name );

    return result;
}

//------------------------------------------------------------------------------get_weekday_numbers

static list<int> get_weekday_numbers( const string& weekday_names )
{
    list<int>      result;
    vector<string> names  = vector_split( " +", weekday_names );

    Z_FOR_EACH( vector<string>, names, it )  result.push_back( get_weekday_number( *it ) );

    return result;
}

//-------------------------------------------------------------------------string_from_when_holiday

string string_from_when_holiday( When_holiday when_holiday )
{
    string result;

    switch( when_holiday )
    {
        case wh_suppress:               result = "suppress";                break;
        case wh_ignore_holiday:         result = "ignore_holiday";          break;
        case wh_next_non_holiday:       result = "next_non_holiday";        break;
        case wh_previous_non_holiday:   result = "previous_non_holiday";    break;
        default:                        result = S() << "When_holiday(" << (int)when_holiday << ")";
    }

    return result;
}

//-------------------------------------------------------------------------when_holiday_from_string

When_holiday when_holiday_from_string( const string& w )
{
    When_holiday result;

    if( w == "suppress"             )  result = wh_suppress;
    else
    if( w == "ignore_holiday"       )  result = wh_ignore_holiday;
    else
    if( w == "next_non_holiday"     )  result = wh_next_non_holiday;
    else
    if( w == "previous_non_holiday" )  result = wh_previous_non_holiday;
    else
        z::throw_xc( Z_FUNCTION, w );

    return result;
}

//---------------------------------------------------------------------------new_schedule_subsystem

ptr<Schedule_subsystem_interface> new_schedule_subsystem( Scheduler* scheduler )
{
    ptr<Schedule_subsystem> result = Z_NEW( Schedule_subsystem( scheduler ) );
    return +result;
}

//----------------------------------------chedule_subsystem_interface::Schedule_subsystem_interface

Schedule_subsystem_interface::Schedule_subsystem_interface( Scheduler* scheduler, Type_code t )   
: 
    file_based_subsystem<Schedule>( scheduler, this, t )
{
}

//-----------------------------------------------------------Schedule_subsystem::Schedule_subsystem
    
Schedule_subsystem::Schedule_subsystem( Scheduler* scheduler )
:
    Schedule_subsystem_interface( scheduler, type_schedule_subsystem )
{
}

//------------------------------------------------------------------------Schedule_subsystem::close
    
void Schedule_subsystem::close()
{
    set_subsystem_state( subsys_stopped );
    file_based_subsystem<Schedule>::close();
}

//---------------------------------------------------------Schedule_subsystem::subsystem_initialize

bool Schedule_subsystem::subsystem_initialize()
{
    file_based_subsystem<Schedule>::subsystem_initialize();
    set_subsystem_state( subsys_initialized );
    return true;
}

//---------------------------------------------------------------Schedule_subsystem::subsystem_load

bool Schedule_subsystem::subsystem_load()
{
    file_based_subsystem<Schedule>::subsystem_load();
    set_subsystem_state( subsys_loaded );
    return true;
}

//-----------------------------------------------------------Schedule_subsystem::subsystem_activate

bool Schedule_subsystem::subsystem_activate()
{
    set_subsystem_state( subsys_active );
    file_based_subsystem<Schedule>::subsystem_activate();
    return true;
}

//------------------------------------------------------Schedule_subsystem::assert_xml_element_name

void Schedule_subsystem::assert_xml_element_name( const xml::Element_ptr& element ) const
{
    if( !element.nodeName_is( "run_time" ) )  File_based_subsystem::assert_xml_element_name( element );    // <run_time> und <schedule> sind ungefähr gleich
}

//-----------------------------------------------------Schedule_subsystem<Schedule>::new_file_based

ptr<Schedule> Schedule_subsystem::new_file_based(const string&)
{
    return Z_NEW( Schedule( this ) );
}

//------------------------------------------------------------------Schedule_subsystem::execute_xml

//xml::Element_ptr Schedule_subsystem::execute_xml( Command_processor* command_processor, const xml::Element_ptr& element, const Show_what& show_what )
//{
//    xml::Element_ptr result;
//
//    //if( element.nodeName_is( "schedule" ) )  spooler()->root_folder()->schedule_folder()->add_or_replace_file_based_xml( element );
//    //else
//    if( string_begins_with( element.nodeName(), "schedule." ) ) 
//    {
//        schedule( Absolute_path( root_path, element.getAttribute( "schedule" ) ) )->execute_xml( element, show_what );
//    }
//    else
//        z::throw_xc( "SCHEDULER-113", element.nodeName() );
//
//    return command_processor->_answer.createElement( "ok" );
//}

//-----------------------------------------------------------------Schedule_folder::Schedule_folder

Schedule_folder::Schedule_folder( Folder* folder )
:
    typed_folder<Schedule>( folder->spooler()->schedule_subsystem(), folder, type_schedule_folder )
{
}

//----------------------------------------------------------------Schedule_folder::~Schedule_folder
    
Schedule_folder::~Schedule_folder()
{
}

//---------------------------------------------------------------------------Schedule_use::_methods

const Com_method Schedule_use::_methods[] =
{
#ifdef COM_METHOD
    COM_PROPERTY_GET( Schedule_use,  1, Java_class_name, VT_BSTR    , 0 ),
    COM_PROPERTY_PUT( Schedule_use,  2, Xml            ,              0, VT_BSTR ),
    COM_PROPERTY_GET( Schedule_use,  3, Schedule       , VT_DISPATCH, 0 ),
#endif
    {}
};

//-----------------------------------------------------------------------Schedule_use::Schedule_use

Schedule_use::Schedule_use( File_based* using_object )
:
    Idispatch_implementation( &class_descriptor ),
    Abstract_scheduler_object( using_object->spooler(), static_cast<spooler_com::Irun_time*>( this ), type_schedule_use ),
    _zero_(this+1),
    _using_object(using_object),
    _scheduler_holidays_usage(with_scheduler_holidays),
    _time_zone_name(using_object->spooler()->_time_zone_name)
{
    _log = using_object->log();
    assert( _log );
}

//----------------------------------------------------------------------Schedule_use::~Schedule_use

Schedule_use::~Schedule_use()
{
    try
    {
        close();
    }
    catch( exception& x )
    {
        Z_LOG( Z_FUNCTION << " ERROR " <<  x.what() << "\n" );
    }

    set_schedule( NULL );
}

//---------------------------------------------------------------------Schedule_use::QueryInterface

STDMETHODIMP Schedule_use::QueryInterface( const IID& iid, void** result )
{
    Z_IMPLEMENT_QUERY_INTERFACE( this, iid, spooler_com::Irun_time           , result );
    Z_IMPLEMENT_QUERY_INTERFACE( this, iid, spooler_com::Ihas_java_class_name, result );
    Z_IMPLEMENT_QUERY_INTERFACE( this, iid, IDispatch                        , result );

    return Idispatch_implementation::QueryInterface( iid, result );
}

//------------------------------------------------------------------------------Schedule_use::close

void Schedule_use::close()
{
    remove_requisite( Requisite_path( spooler()->schedule_subsystem(), _schedule_path ) );

    _active_schedule_path.clear();
    _schedule_path       .clear();
    set_schedule( NULL );
}

//-------------------------------------------------------------------------Schedule_use::disconnect

void Schedule_use::disconnect()
{
    set_schedule(NULL);
}

//-----------------------------------------------------------------------Schedule_use::set_schedule

void Schedule_use::set_schedule( Schedule* schedule )
{
    if( schedule != _schedule )
    {
        if( schedule  &&  schedule->is_covering() )  z::throw_xc( "SCHEDULER-466", schedule->obj_name() );

        if( _schedule )  _schedule->remove_use( this );
        _schedule = schedule;
        if( _schedule )  _schedule->add_use( this );
    }
}

//---------------------------------------------------------------------------Schedule_use::schedule

Schedule* Schedule_use::schedule()
{
    assert( _schedule );
    if( !_schedule )  z::throw_xc( Z_FUNCTION );

    return _schedule;
}

//---------------------------------------------------------------------Schedule_use::set_xml_string

void Schedule_use::set_xml_string( File_based* source_file_based, const string& xml )
{
    xml::Document_ptr doc = xml::Document_ptr::from_xml_string(xml);
    if( _spooler->_validate_xml )  _spooler->_schema.validate( doc );

    set_dom( source_file_based, doc.documentElement() );
}

//----------------------------------------------------------------------------Schedule_use::set_dom

void Schedule_use::set_dom( File_based* source_file_based, const xml::Element_ptr& element )
{
    assert( element );
    //_using_object->assert_is_not_initialized();

    close();

    if( element.nodeName_is( "run_time" )  &&  element.hasAttribute( "schedule" ) )     // <run_time schedule="...">
    {                                                                                   // (Besser: <schedule.use schedule="...">)
        _schedule_path = Absolute_path::build( source_file_based, element.getAttribute( "schedule" ) );       
        add_requisite( Requisite_path( spooler()->schedule_subsystem(), _schedule_path ) );
        set_schedule( _spooler->schedule_subsystem()->schedule_or_null( _schedule_path ) );     // Verweise auf benanntes <schedule>  // SOS1219 
        if (!_schedule) log()->warn( message_string("SCHEDULER-161", "schedule", _schedule_path) );
    }
    else
    {
        set_schedule( Z_NEW( Schedule( _spooler->schedule_subsystem(), _scheduler_holidays_usage ) ) );     // Unbenanntes (privates) <schedule>
        _schedule->set_dom( source_file_based, element );
    }

    _active_schedule_path = _schedule_path.without_slash();
    _time_zone_name = element.getAttribute("time_zone", _time_zone_name);

    on_schedule_modified();
}

//-----------------------------------------------------------------------Schedule_use::dom_document

xml::Document_ptr Schedule_use::dom_document( const Show_what& show_what ) 
{
    xml::Document_ptr document;

    document.create();
    document.appendChild( dom_element( document, show_what ) );

    return document;
}

//------------------------------------------------------------------------Schedule_use::dom_element

xml::Element_ptr Schedule_use::dom_element( const xml::Document_ptr& document, const Show_what& show_what )
{
    xml::Element_ptr result;

    if( _schedule_path.empty() )
    {
        result = schedule()->dom_element( document, show_what );
    }
    else
    {
        result = document.createElement( "run_time" );          // <run_time schedule="...">  
        result.setAttribute( "schedule", _schedule_path );      // (Besser: <scheduler.use schedule="...">)
    }    

    //if( _schedule )
    //    if( Schedule* covering_schedule = _schedule->active_schedule_at( Time::now() ) )  
    //        result.setAttribute( "active_schedule", covering_schedule->path() );

    return result;
}

//---------------------------------------------------------------------------Schedule_use::try_load

bool Schedule_use::try_load()
{
    if (!_schedule)
    {
        if( !_schedule_path.empty() )
        {
            if( Schedule* schedule = _spooler->schedule_subsystem()->schedule_or_null( _schedule_path ) )
            {
                set_schedule( schedule );
            }
        }
    }

    return _schedule != NULL;
}

//----------------------------------------------------------------Schedule_use::on_requisite_loaded

bool Schedule_use::on_requisite_loaded( File_based* file_based )
{
    assert( file_based->subsystem()       == spooler()->schedule_subsystem() );
    assert( file_based->normalized_path() == spooler()->schedule_subsystem()->normalized_path( _schedule_path ) );
    assert( dynamic_cast<Schedule*>( file_based ) );

    try_load();
    assert( _schedule == dynamic_cast<Schedule*>( file_based ) );

    on_schedule_loaded();

    return true;
}

//---------------------------------------------------------Schedule_use::on_requisite_to_be_removed

bool Schedule_use::on_requisite_to_be_removed( File_based* )
{
    bool ok = on_schedule_to_be_removed();
    assert( ok );   // false nicht geprüft

    if( ok )                    // Stets true!
    {
        disconnect();
        // Das ist hier nicht ganz richtig: wenn ein on_requisite_to_be_removed() false liefert, 
        // sollte disconnect() nicht gerufen worden sein!
        // Das sollte nach on_requisite_removed()
    }

    return ok;
}

//--------------------------------------------------------Schedule_use::log_changed_active_schedule

void Schedule_use::log_changed_active_schedule( const Time& now )
{
    assert( is_defined() );

    Schedule* active_schedule = schedule()->active_schedule_at( now );

    if( _active_schedule_path != (string)active_schedule->path().without_slash() )      // Unbenannte <run_time> ist "", auch "/"
    {
        _using_object->log()->info( message_string( active_schedule == _schedule? "SCHEDULER-706" : "SCHEDULER-705", 
                                                    active_schedule->obj_name() ) );
        _active_schedule_path = active_schedule->path().without_slash();
    }
}

//------------------------------------------------------------------Schedule_use::next_single_start

Time Schedule_use::next_single_start( const Time& time )
{ 
    Period period = next_period( time, wss_next_single_start );

    return !period.absolute_repeat().is_eternal()? period.next_absolute_repeated(time)
                                                 : period.begin();
}

//---------------------------------------------------------------------Schedule_use::next_any_start

Time Schedule_use::next_any_start( const Time& time )
{ 
    Period period = next_period( time, wss_next_any_start );

    return !period.absolute_repeat().is_eternal()? period.next_repeated( time )
                                                 : period.begin();
}

//------------------------------------------------------------------------Schedule_use::next_period

Period Schedule_use::next_period( const Time& t, With_single_start single_start, const Time& before ) 
{ 
    Period result = schedule()->next_local_period( this, t.local_time(_time_zone_name), single_start, before.local_time(_time_zone_name) ); 
    result._begin = result._begin.utc_from_time_zone(_time_zone_name);
    result._end = result._end.utc_from_time_zone(_time_zone_name);
    result._absolute_repeat_begin = result._absolute_repeat_begin.utc_from_time_zone(_time_zone_name);
    return result;
}

//-----------------------------------------------------------------Schedule_use::next_allowed_start

Time Schedule_use::next_allowed_start( const Time& t )
{
    Time result = Time::never;

    if( _schedule )
    {
        Period period = next_period( t );
        result = max( t, period.begin() );
    }

    return result;
}

//-------------------------------------------------------Schedule_use::append_calendar_dom_elements

void Schedule_use::append_calendar_dom_elements( const xml::Element_ptr& element, Show_calendar_options* options )
{
    if( is_defined() )
    {
        Time t          = options->_from;
        Time last_begin;

        //bool check_for_once = _once;

        while( options->_count < options->_limit )
        {
            Period period = next_period( t, wss_next_any_start, options->_before );  
            if( period.begin() >= options->_before  ||  period.begin().is_never() )  break;

            //if( period._start_once  ||
            //    period._single_start  ||     
            //    _host_object->scheduler_type_code() == Scheduler_object::type_order ||
            //    check_for_once 
            {
                //check_for_once = false;

                //2008-04-26 if( period.begin() >= options->_from  &&  period.begin() != last_begin )
                if( period.end() > options->_from  &&  period.begin() != last_begin )
                {
                    element.appendChild( period.dom_element( element.ownerDocument() ) );
                    options->_count++;

                    last_begin = period.begin();
                }
            }

            Time new_t = period._single_start? period.begin() + Duration(1) : period.end();
            assert( t < new_t );
            if( new_t <= t )  break;
            t = new_t;
        }
    }
}

//---------------------------------------------------------------------------Schedule_use::obj_name

string Schedule_use::obj_name() const
{ 
    S result;

    result << "Schedule_use " << _using_object->obj_name();
    result << "-->";

    if( _schedule )  result << _schedule->obj_name();
               else  result << "(no schedule)";

    return result;
}

//----------------------------------------------------------------------------Schedule_use::put_Xml

STDMETHODIMP Schedule_use::put_Xml( BSTR xml )
{
    Z_COM_IMPLEMENT( set_xml_string( (File_based*)NULL, string_from_bstr( xml ) ) );
}

//-----------------------------------------------------------------------Schedule_use::get_Schedule

STDMETHODIMP Schedule_use::get_Schedule( spooler_com::Ischedule** result )
{
    *result = _schedule.copy();
    return S_OK;
}

//-------------------------------------------------------------------------------Schedule::_methods

const Com_method Schedule::_methods[] =
{
#ifdef COM_METHOD
    COM_PROPERTY_GET( Schedule,  1, Java_class_name, VT_BSTR    , 0 ),
    COM_PROPERTY_PUT( Schedule,  2, Xml            ,              0, VT_BSTR ),
    COM_PROPERTY_GET( Schedule,  2, Xml            , VT_BSTR    , 0 ),
#endif
    {}
};

//-------------------------------------------------------------------------------Schedule::Schedule

Schedule::Schedule( Schedule_subsystem_interface* schedule_subsystem_interface, Scheduler_holidays_usage scheduler_holidays_usage )
:
    Idispatch_implementation( &class_descriptor ),
    My_file_based( schedule_subsystem_interface, static_cast<spooler_com::Ischedule*>( this ), type_schedule ),
    _scheduler_holidays_usage(scheduler_holidays_usage),
    _zero_(this+1),
    javabridge::has_proxy<Schedule>(spooler())
{
    _inlay = Z_NEW( Inlay( this ) );
}

//------------------------------------------------------------------------------Schedule::~Schedule
    
Schedule::~Schedule()
{
    try
    {
        close();
    }
    catch( exception& x ) { Z_LOG2( "scheduler", Z_FUNCTION << ": " << x.what() ); }

    disconnect_covered_schedule();
    disconnect_covering_schedules();

    assert( _use_set.empty() );
    assert( _covering_schedules.empty() );
    assert( !_covered_schedule );
}

//-------------------------------------------------------------------------Schedule::QueryInterface

STDMETHODIMP Schedule::QueryInterface( const IID& iid, void** result )
{
    Z_IMPLEMENT_QUERY_INTERFACE( this, iid, spooler_com::Ischedule           , result );
    Z_IMPLEMENT_QUERY_INTERFACE( this, iid, spooler_com::Ihas_java_class_name, result );
    Z_IMPLEMENT_QUERY_INTERFACE( this, iid, IDispatch                        , result );

    return Idispatch_implementation::QueryInterface( iid, result );
}

//----------------------------------------------------------------------------------Schedule::close

void Schedule::close()
{
    disconnect_covering_schedules();
    assert( _covering_schedules.empty() );

    set_inlay( NULL );
    assert( _use_set.empty() );
}

//--------------------------------------------------------------------------Schedule::on_initialize

bool Schedule::on_initialize()
{
    initialize_inlay();
    return true;
}

//--------------------------------------------------------------------------------Schedule::on_load

bool Schedule::on_load()
{
    try_load_inlay();
    return true;
}

//----------------------------------------------------------------------------Schedule::on_activate

bool Schedule::on_activate()
{
    bool ok = try_load_inlay();
    if( !ok )  set_file_based_state( s_incomplete );
    return ok;
}

//---------------------------------------------------------------------Schedule::can_be_removed_now

bool Schedule::can_be_removed_now()
{
    assert( _use_set.empty() );

    return _use_set.empty(); 
}

//--------------------------------------------------------------------Schedule::can_be_replaced_now

bool Schedule::can_be_replaced_now()
{
    return replacement()  &&  
           replacement()->file_based_state() == File_based::s_initialized;
}

//---------------------------------------------------------------------Schedule::prepare_to_replace

void Schedule::prepare_to_replace()
{
}

//-------------------------------------------------------------------------Schedule::on_replace_now

Schedule* Schedule::on_replace_now()
{
    assert( can_be_replaced_now() );

    ptr<Schedule> replacement  = this->replacement();
    assert( replacement );

    set_inlay( replacement->_inlay );
    replacement->_inlay = NULL;

    set_replacement( NULL );        // Brauchen wir nicht mehr

    on_schedule_modified();

    return this;    // Der alte Schedule ist der neue Schedule
}

//-------------------------------------------------------------Schedule::on_requisite_to_be_removed

bool Schedule::on_requisite_to_be_removed( File_based* file_based )
{
    if( _covered_schedule )
    {
        assert( file_based == _covered_schedule );

        disconnect_covered_schedule();
    }

    assert( !_covered_schedule );
    return true;
}

//--------------------------------------------------------------------------------Schedule::add_use

void Schedule::add_use( Schedule_use* use )
{
    assert( _use_set.find( use ) == _use_set.end() );

    _use_set.insert( use );
}

//-----------------------------------------------------------------------------Schedule::remove_use

void Schedule::remove_use( Schedule_use* use )
{
    assert( _use_set.find( use ) != _use_set.end() );

    _use_set.erase( use );
}

//-------------------------------------------------------------------Schedule::on_schedule_modified

void Schedule::on_schedule_modified()
{
    Z_FOR_EACH( Use_set, _use_set, u )  (*u)->on_schedule_modified();
}

//------------------------------------------------------------------------------Schedule::set_inlay

void Schedule::set_inlay( Inlay* inlay )
{
    if( _inlay != inlay )
    {
        if( _inlay )
        {
            if( _inlay->_covered_schedule_path != "" )
            {
                remove_requisite( Requisite_path( subsystem(), _inlay->_covered_schedule_path ) );
                disconnect_covered_schedule();
            }
        }

        _inlay = inlay;

        if( _inlay )
        {
            if( file_based_state() >= s_initialized )
            {
                initialize_inlay();

                if( file_based_state() >= s_loaded )
                {
                    try_load_inlay();
                    on_schedule_modified();
                }
            }
        }
    }
}

//-----------------------------------------------------------------------Schedule::initialize_inlay

void Schedule::initialize_inlay()
{
    if( _inlay->_covered_schedule_path != "" )
    {
        add_requisite( Requisite_path( subsystem(), _inlay->_covered_schedule_path ) );
    }
}

//-------------------------------------------------------------------------Schedule::try_load_inlay

bool Schedule::try_load_inlay()
{
    bool result = true;

    if( _inlay->_covered_schedule_path != "" )
    {
        result = try_connect_covered_schedule();
    }

    return result;  // false: on_activate() setzt s_incomplete
}

//--------------------------------------------------------------------Schedule::cover_with_schedule

void Schedule::cover_with_schedule( Schedule* covering_schedule )
{
    if( is_covering() )  z::throw_xc( "SCHEDULER-463", obj_name(), covering_schedule->obj_name() );

    assert_no_overlapped_covering( covering_schedule );
    _covering_schedules[ covering_schedule->_inlay->_covered_schedule_begin.millis() ] = covering_schedule;

    on_schedule_modified();
}

//----------------------------------------------------------Schedule::assert_no_overlapped_covering

void Schedule::assert_no_overlapped_covering( Schedule* covering_schedule )
{
    Covering_schedules::iterator after = _covering_schedules.lower_bound( covering_schedule->_inlay->_covered_schedule_end.millis() );
    
    if( after != _covering_schedules.begin() )
    {
        Covering_schedules::iterator schedule_before = after;  schedule_before--;
        if( schedule_before->second->_inlay->_covered_schedule_end > covering_schedule->_inlay->_covered_schedule_begin )  
            z::throw_xc( "SCHEDULER-465", covering_schedule->obj_name(), schedule_before->second->obj_name() );
    }
}

//------------------------------------------------------------------Schedule::uncover_from_schedule

void Schedule::uncover_from_schedule( Schedule* covering_schedule )
{
    assert( _covering_schedules.find( covering_schedule->_inlay->_covered_schedule_begin.millis() ) != _covering_schedules.end() );

    _covering_schedules.erase( covering_schedule->_inlay->_covered_schedule_begin.millis() );

    on_schedule_modified();
}

//-----------------------------------------------------------Schedule::try_connect_covered_schedule

bool Schedule::try_connect_covered_schedule()
{
    if( !_covered_schedule &&  _inlay->_covered_schedule_path != "" )  
    {
        Schedule* covered_schedule = _spooler->schedule_subsystem()->schedule_or_null( _inlay->_covered_schedule_path );

        if( covered_schedule ) // &&  covered_schedule->file_based_state() >= s_loaded )
        {
            // Bei Überlappung gibt es eine Exception und der überdeckende Schedule wird nicht im überdeckten eingetragen.
            // Besser: Keine Exception, sondern return false, 
            //         überdeckender Scheduler wird im überdeckten unwirksam eingetragen,
            //         damit bei Änderung eines anderen überdeckenden Schedules erneut alle auf Überlappung geprüft werden können.
            //         Zustand is_overlapping.
            // Die SOS fordert nur "There can be more than one replacement schedule for a schedule, but the replacement schedules may not overlap."
            // Der Überlappende Schedule muss jetzt geändert (neu gespeichert) werden, damit er wirken kann.

            covered_schedule->cover_with_schedule( this );  // Löst Exception aus bei nicht eindeutiger Überdeckung
            _covered_schedule = covered_schedule;
        }
        else
        if( file_based_state() == s_active )
        {
            set_file_based_state( s_incomplete );
        }
    }

    return _covered_schedule != NULL;
}

//------------------------------------------------------------Schedule::disconnect_covered_schedule

void Schedule::disconnect_covered_schedule()
{
    if( _covered_schedule )
    {
        _covered_schedule->uncover_from_schedule( this );
        _covered_schedule = NULL;

        if( file_based_state() == s_active  &&
            requisite_is_registered( Requisite_path( _spooler->schedule_subsystem(), _inlay->_covered_schedule_path ) ) )
        {
            set_file_based_state( s_incomplete );
        }
    }
}

//----------------------------------------------------------Schedule::disconnect_covering_schedules

void Schedule::disconnect_covering_schedules()
{
    assert( _covering_schedules.empty() );   // Schedule::on_requisite_to_be_removed() sollte bereits die Verbindungen gelöst haben

    Covering_schedules covering_schedules = _covering_schedules;

    Z_FOR_EACH( Covering_schedules, covering_schedules, c )
    {
        c->second->disconnect_covered_schedule();
    }

    assert( _covering_schedules.empty() );
}

//----------------------------------------------------------------------Schedule::next_local_period

Period Schedule::next_local_period( Schedule_use* use, const Time& tim, With_single_start single_start, const Time& before ) 
{ 
    Period result;
    Time   interval_begin = Time(0);      // Standard-Schedule beginnt. Gilt, falls kein überdeckendes Schedule < t 
    Time   interval_end   = Time::never;  // Standard-Schedule endet. Gilt, falls kein überdeckendes Schedule >= t
    Time   t              = tim;

    // Überdeckende Schedule prüfen, <schedule substitute="...">

    for( Covering_schedules::iterator next_schedule = _covering_schedules.upper_bound(t.millis());; next_schedule++ )   // Liefert das erste Schedule nach t
    {
        assert( next_schedule == _covering_schedules.end()  ||  t < next_schedule->second->_inlay->_covered_schedule_begin );

        if( next_schedule == _covering_schedules.begin() )   // Kein überdeckendes Schedule mit _covered_schedule_begin < t?
        { 
            interval_begin = Time(0);
        }
        else
        {
            Covering_schedules::iterator schedule_before = next_schedule;  
            schedule_before--;

            Schedule* covering_schedule = schedule_before->second;

            if( covering_schedule->is_covering_at( t ) )   
            {
                assert( covering_schedule == covering_schedule_at( t ) );

                interval_begin = covering_schedule->_inlay->_covered_schedule_begin;
                interval_end   = covering_schedule->_inlay->_covered_schedule_end;
                assert( t >= interval_begin  &&  t < interval_end );

                Period period = covering_schedule->_inlay->next_local_period( use, t, single_start, before );
                if( period._begin < interval_end )  
                {
                    result = period;
                    result._schedule_path = covering_schedule->path();
                    break;     // Periode beginnt im überdeckenden Schedule? Gut!
                }

                t = interval_end;   // Das überdeckende Schedule hat keine Periode. Also weiter in unserem Standard-Schedule!
            }

            interval_begin = covering_schedule->_inlay->_covered_schedule_end;      // Beginn der Lücke nach dem letzten überdeckenden Schedule
        }

        interval_end = next_schedule != _covering_schedules.end()? next_schedule->second->_inlay->_covered_schedule_begin     // Ende der Lücke
                                                                 : Time::never;

        assert( t >= interval_begin  &&  t <= interval_end );

        if( interval_begin < interval_end )     // Die Lücke nach dem überdeckenden Schedule ist länger als 0?
        {
            assert( !covering_schedule_at( t ) );
            Period period = _inlay->next_local_period( use, t, single_start, before );       // Unser Standard-Schedule
            if( period.begin() < interval_end )  
            {
                result = period;
                result._schedule_path = path_or_empty();
                break;
            }
        }

        t = interval_end;
        if( t.is_never() )
        {
            assert( !covering_schedule_at( t ) );
            break;
        }
    }

    assert( t >= interval_begin  &&  t <= interval_end );

    if( !result.is_single_start() )
    {
        result._absolute_repeat_begin = result._begin;      // Damit absolute_repeat korrekt berechnet wird, auch wenn wir _begin verschieben
        if( result._begin < interval_begin )  result._begin = interval_begin;
        if( result._end   > interval_end   )  result._end   = interval_end;
    }

    return result;
}

//----------------------------------------------------------------------------Schedule::is_covering

bool Schedule::is_covering()
{ 
    return _inlay->_covered_schedule_path != ""; 
}

//-------------------------------------------------------------------------Schedule::is_covering_at

bool Schedule::is_covering_at( const Time& t )
{ 
    return is_covering()  && 
           t >= _inlay->_covered_schedule_begin &&  
           t <  _inlay->_covered_schedule_end; 
}

//-------------------------------------------------------------------Schedule::covering_schedule_at

Schedule* Schedule::covering_schedule_at( const Time& t )
{
    Schedule* result = NULL;

    Covering_schedules::iterator next_schedule = _covering_schedules.upper_bound( t.millis() );   // Liefert das erste Schedule nach t
    assert( next_schedule == _covering_schedules.end()  ||  t < next_schedule->second->_inlay->_covered_schedule_begin );

    if( next_schedule != _covering_schedules.begin() )   // Kein überdeckendes Schedule mit _covered_schedule_begin < t?
    {
        Covering_schedules::iterator schedule_before = next_schedule;  
        schedule_before--;
        assert( schedule_before->second->_inlay->_covered_schedule_begin <= t );

        Schedule* covering_schedule = schedule_before->second;

        if( covering_schedule->is_covering_at( t ) )
        {
            result = covering_schedule;
        }
    }
    
    return result;
}

//---------------------------------------------------------------------Schedule::active_schedule_at

Schedule* Schedule::active_schedule_at( const Time& t )
{
    if( Schedule* covering_schedule = covering_schedule_at( t ) )
    {
        return covering_schedule;
    }
    else
        return this;
}

//----------------------------------------------------------------Schedule::active_schedule_path_at

Absolute_path Schedule::active_schedule_path_at( const Time& t )
{
    if( Schedule* covering_schedule = covering_schedule_at( t ) )
    {
        return covering_schedule->path();
    }
    else
        return path();
}

//---------------------------------------------------------------------------Schedule::dom_document

xml::Document_ptr Schedule::dom_document( const Show_what& show_what ) 
{
    xml::Document_ptr document;

    document.create();
    document.appendChild( _inlay->dom_element( document, show_what ) );

    return document;
}

//----------------------------------------------------------------------------Schedule::dom_element

xml::Element_ptr Schedule::dom_element( const xml::Document_ptr& dom_document, const Show_what& show_what )           
{
    xml::Element_ptr result = dom_document.importNode(_inlay->dom_element( dom_document, show_what )); 

    if (!show_what.is_set( show_for_database_only)) {
        if( is_in_folder() )    // Benanntes <schedule>?
        {
            result.setAttribute_optional( "substitute", _inlay->_covered_schedule_path );   // Absoluten Pfad setzen

            if( file_based_state() == s_active )
            {
                Time now = Time::now();
                Schedule* covering_schedule = covering_schedule_at( now );
                result.setAttribute( "active", ( is_covering()? is_covering_at( now )
                                                              : !covering_schedule    )? "yes": "no" );  // Wird nicht überdeckt oder kann jetzt selbst überdecken?
                if( covering_schedule )  result.setAttribute( "now_covered_by_schedule", covering_schedule->path() );
            }

            fill_file_based_dom_element( result, show_what );   

            if( show_what.is_set( show_schedules )  &&  !_use_set.empty() )
            {
                xml::Element_ptr schedule_users_element = result.append_new_element( "schedule.users" );
                
                Z_FOR_EACH( Use_set, _use_set, u )
                {
                    xml::Element_ptr schedule_user_element = schedule_users_element.append_new_element( "schedule.user" );
                    (*u)->using_file_based()->set_identification_attributes( schedule_user_element );         // job="/.."  oder  job_chain="/.." order=".."
                }
            }
        }
    }


    return result;
}

//-------------------------------------------------------------------------Schedule::set_xml_string

void Schedule::set_xml_string( File_based* source_file_based, const string& xml_string )
{
    xml::Document_ptr doc = xml::Document_ptr::from_xml_string(xml_string);
    if( _spooler->_validate_xml )  _spooler->_schema.validate( doc );

    set_dom( source_file_based, doc.documentElement() );
}

//--------------------------------------------------------------------------------Schedule::set_dom

void Schedule::set_dom( const xml::Element_ptr& e )           
{ 
    set_dom( this, e ); 
}

//--------------------------------------------------------------------------------Schedule::set_dom

void Schedule::set_dom( File_based* source_file_based, const xml::Element_ptr& element )
{
    if( !element )  return;
    assert( element.nodeName_is( "run_time" )  ||  element.nodeName_is( "schedule" ) );
    //assert_is_not_initialized();

    ptr<Inlay> new_inlay = Z_NEW( Inlay( this ) );    // Alles ersetzen! Also erneuern wir das Inlay
    new_inlay->set_dom( source_file_based, element );
    set_inlay( new_inlay );
}

//--------------------------------------------------------------------------------Schedule::put_Xml

STDMETHODIMP Schedule::put_Xml( BSTR xml_bstr )
{
    Z_COM_IMPLEMENT( set_xml_string( (File_based*)NULL, string_from_bstr( xml_bstr ) ) );
}

//--------------------------------------------------------------------------------Schedule::get_Xml

STDMETHODIMP Schedule::get_Xml( BSTR* result )
{
    HRESULT hr = NOERROR;

    try
    {
        xml::Document_ptr dom_document;
        dom_document.create();

        hr = String_to_bstr(_inlay->dom_element(dom_document, Show_what()).xml_string(), result);
    }
    catch( const exception& x )  { hr = Set_excepinfo( x, Z_FUNCTION ); }

    return hr;
}

//---------------------------------------------------------------------------Schedule::Inlay::Inlay

Schedule::Inlay::Inlay( Schedule* schedule )
:
    _zero_(this+1),
    _schedule(schedule),
    _spooler(schedule->spooler()),
    _holidays(schedule->spooler()),
    _months(12)
{
    _dom.create();
    _dom.appendChild( _dom.createElement( "run_time" ) );       // <run_time/>, zur Kompatibilität für nicht gesetztes <run_time> in einem Job.
                                                                // Bei file_based_state() == s_undefined wird dann ein <run_time/> statt <schedule/> geliefert

    if( _schedule->_scheduler_holidays_usage == with_scheduler_holidays )      // Default. Nur bei Order nicht (siehe eMail von Andreas Liebert 2008-04-21).
    {
        _holidays = _spooler->holidays();                   // Kopie des Objekts (kein Zeiger)
    }
    else
        assert( _schedule->path().without_slash() == "" );
}

//--------------------------------------------------------------------------Schedule::Inlay::~Inlay
    
Schedule::Inlay::~Inlay()
{
}

//-------------------------------------------------------------------------Schedule::Inlay::set_dom
    
void Schedule::Inlay::set_dom( File_based* source_file_based, const xml::Element_ptr& element )
{
    if( !element )  return;
    assert( element.nodeName_is( "run_time" )  ||  element.nodeName_is( "schedule" ) );
    //_schedule->assert_is_not_initialized();


    Sos_optional_date_time  dt;
    Period                  default_period;
    Day                     default_day;
    bool                    period_seen    = false;


    _dom.create();
    _dom.importAndAppendChild(element);


    _title = element.getAttribute( "title ");

    _covered_schedule_path = Absolute_path::build( source_file_based, element.getAttribute( "substitute" ) );
    _covered_schedule_begin = Time::of_local_date_time(element.getAttribute( "valid_from"          ));
    _covered_schedule_end   = Time::of_local_date_time(element.getAttribute( "valid_to"  , "never" ));
    if( _covered_schedule_begin >= _covered_schedule_end )  z::throw_xc( "SCHEDULER-464", obj_name(), _covered_schedule_begin.utc_string(), _covered_schedule_end.utc_string() );

    if( _covered_schedule_path == "" )
    {
        if( !_covered_schedule_begin.is_zero() )  z::throw_xc( "SCHEDULER-467", "valid_from", "substitute" );
        if( !_covered_schedule_end.is_never()  )  z::throw_xc( "SCHEDULER-467", "valid_to"  , "substitute" );
    }

    _once = element.bool_getAttribute( "once", _once );

    if (element.hasAttribute("name")) {
        require_not_attribute(element, "time_zone");
    }


    default_period.set_dom( element );
    default_day = default_period;

    bool a_day_set = false;


    DOM_FOR_EACH_ELEMENT( element, e )
    {
        if( e.nodeName_is( "period" ) )
        {
            if( !period_seen )  period_seen = true, default_day = Day();
            default_day.add( Period( e, &default_period ) );
        }
        else
        if( e.nodeName_is( "at" ) )
        {
            a_day_set = true;
            string at = e.getAttribute( "at" );
            if( at == "now" )  _once = true;   // "now" wirkt nicht in _at_set, weil der Zeitpunkt gleich verstrichen sein wird
            Time at_time = Time::of_local_date_time( at );
            _at_set.add( at_time );
        }
        else
        if( e.nodeName_is( "date" ) )
        {
            a_day_set = true;
            dt.assign( e.getAttribute( "date" ) );
            if( !dt.time_is_zero() )  z::throw_xc( "SCHEDULER-208", e.getAttribute( "date" ) );
            Date date;
            date._day_nr = (int)( dt.as_time_t() / (24*60*60) );
            date._day.set_dom_periods( e, &default_day, &default_period );
            _date_set._date_set.insert( date );
        }
        else
        if( e.nodeName_is( "weekdays" ) )
        {
            a_day_set = true;
            _weekday_set.set_dom( e, &default_day, &default_period );
        }
        else
        if( e.nodeName_is( "monthdays" ) )
        {
            a_day_set = true;
            _monthday_set.set_dom( e, &default_day, &default_period );
        }
        else
        if( e.nodeName_is( "ultimos" ) )
        {
            a_day_set = true;
            _ultimo_set.set_dom( e, &default_day, &default_period );
        }
        else
        if( e.nodeName_is( "month" ) )
        {
            a_day_set = true;

            list<int>  months = month_indices_by_names( e.getAttribute( "month" ) );
            ptr<Month> month  = Z_NEW( Month() );
            month->set_dom( e );

            Z_FOR_EACH( list<int>, months, it )
            {
                if( _months[ *it ] )  z::throw_xc( "SCHEDULER-443", month_names[ *it ] );
                _months[ *it ] = month;
                _has_month = true;
            }
        }
        if( e.nodeName_is( "holidays" ) )
        {
            _holidays.clear();
            _holidays.set_dom( source_file_based, e );
        }
        else
        if( e.nodeName_is( "holiday" ) )
        {
            _holidays.set_dom( source_file_based, e );
        }
    }


    if( !a_day_set )  _weekday_set.fill_with_default( default_day );
}

//---------------------------------------------------------------------Schedule::Inlay::dom_element

xml::Element_ptr Schedule::Inlay::dom_element( const xml::Document_ptr& document, const Show_what& ) 
{
    return document.importNode( _dom.documentElement() );
}

//-----------------------------------------------------------------------Schedule::Inlay::is_filled

bool Schedule::Inlay::is_filled() const
{
    bool result = _at_set      .is_filled() 
               || _date_set    .is_filled()
               || _weekday_set .is_filled()
               || _monthday_set.is_filled()
               || _ultimo_set  .is_filled();

    if (!result && _has_month) {
        for (int i = 0; i < 12; i++) {
            result = _months[ i ]  &&  _months[ i ]->is_filled();
            if( result )  break;
        }
    }

    return result;
}

//---------------------------------------------------------------Schedule::Inlay::next_local_period

Period Schedule::Inlay::next_local_period( Schedule_use* use, const Time& beginning_time, With_single_start single_start, const Time& before )
{
    // Wenn mehrere Jahre vorausgesehen werden sollen (s. foresee_years), könnte der Algorithmus vielleicht beschleunigt werden.
    // single_start könnte geprüft werden: <run_time> ohne single_start usw. muss dann nicht durchsucht werden.
    // Oder nur <at> wird weit vorhergesehen, dann könnte man direkt zum Tag springen, dann rückwärts zum vorangehenden Nicht-Feiertag.
    // Ebenso, wenn nur <months> angegeben ist: Direkt zum nächsten angegebenen Monat springen.
    // Ebenso <weekdays> usw.


    Period result;

    if (!beginning_time.is_never() && is_filled())
    {
        Time limited_before = min( before, beginning_time + Duration(foresee_years*366*24*60*60));     // Längstens soviele Jahre ab beginning_time voraussehen

        for( Time t = beginning_time;  result.empty()  &&  t < limited_before;  t = t.midnight() + Duration::day)     
        {
            if( _holidays.is_included( t ) )  
            {
                result = next_period_of_same_day( t, single_start | wss_when_holiday_ignore_holiday );     // Nur when_holiday="ignore_holiday"
            }
            else
            {
                result = min( result, next_period_of_same_day( t, single_start ) );

                // Gestern war Feiertag? Periode mit when_holiday="next_non_holiday" suchen
                Duration foresee_duration = Duration(foresee_years*24*60*60);

                for (Duration before = -Duration::day;
                     before >= -foresee_duration  &&  _holidays.is_included(t + before); 
                     before -= Duration::day)
                {
                    Period p = next_period_of_same_day( t + before, single_start | wss_when_holiday_next_non_holiday );
                    if( !p.empty() )  result = min( result, p - before);   // Vergangene Periode auf heute verschieben
                }


                // Morgen ist Feiertag? Periode mit when_holiday="previous_non_holiday" suchen

                for (Duration after = Duration::day;
                     after <= foresee_duration  &&  !(t + after).is_never()  &&  _holidays.is_included(t + after); 
                     after += Duration::day)
                {
                    Period p = next_period_of_same_day(t + after, single_start | wss_when_holiday_previous_non_holiday);
                    if( !p.empty() )  result = min(result, p - after);    // Zukünftige Periode auf heute verschieben
                }
            }
        }
    }

    return result;
}

//---------------------------------------------------------Schedule::Inlay::next_period_of_same_day
    
Period Schedule::Inlay::next_period_of_same_day( const Time& t, With_single_start single_start )
{
    Period result;

    if( _at_set  .is_filled() )  result = min( result, _at_set  .next_period_of_same_day( t, single_start ) );
    if( _date_set.is_filled() )  result = min( result, _date_set.next_period_of_same_day( t, single_start ) );

    if (_has_month) {
        int m = t.month_nr() - 1;
        if (_months[m]) 
            return min(result, _months[m]->next_period_of_same_day(t, single_start));
    }
    
    if( _weekday_set .is_filled() )  result = min( result, _weekday_set .next_period_of_same_day( t, single_start ) );
    if( _monthday_set.is_filled() )  result = min( result, _monthday_set.next_period_of_same_day( t, single_start ) );
    if( _ultimo_set  .is_filled() )  result = min( result, _ultimo_set  .next_period_of_same_day( t, single_start ) );

    return result;
}

//-------------------------------------------------------------Schedule::Inlay::month_index_by_name

int Schedule::Inlay::month_index_by_name( const string& mm )
{
    string m = lcase( mm );

    for( int i = 0; i < 12; i++ )
    {
        if( m == month_names[ i ] )  return i;
    }

    int result = as_int( m );
    if( result >= 1  &&  result <= 12 )  return result - 1;

    z::throw_xc( "INVALID-MONTH", mm );      // Sollte wegen XML-Schemas nicht passieren
}

//----------------------------------------------------------Schedule::Inlay::month_indices_by_names

list<int> Schedule::Inlay::month_indices_by_names( const string& mm )
{
    list<int>      result;
    vector<string> names  = vector_split( " +", mm );

    Z_FOR_EACH( vector<string>, names, it )
    {
        result.push_back( month_index_by_name( *it ) );
    }

    return result;
}

//-------------------------------------------------------------------------------Period::operator==

bool Period::operator==( const Period& o ) const
{
    return _begin                 == o._begin                   &&
           _absolute_repeat_begin == o._absolute_repeat_begin   &&
           _end                   == o._end                     &&
           _repeat                == o._repeat                  &&
           _absolute_repeat       == o._absolute_repeat         &&
           _single_start          == o._single_start;
}

//-------------------------------------------------------------------------------Period::operator+=

Period& Period::operator+=(const Time& t)
{ 
    assert(!_begin.has_date() && !_end.has_date());
    Duration d = Duration(t.as_double());
    _begin += d; 
    _end += d; 
    return *this; 
}

//-------------------------------------------------------------------------Period::set_single_start

void Period::set_single_start( const Time& t )
{
    _begin           = t;
    _end             = t;
    _repeat          = Duration::eternal;
    _absolute_repeat = Duration::eternal;
    _single_start    = true;
    _let_run         = true;
}

//----------------------------------------------------------------------------------Period::set_dom

void Period::set_dom( const xml::Element_ptr& element, Period::With_or_without_date w, const Period* deflt )
{
    if( !element )  return;

    Sos_optional_date_time dt;

    if( deflt )  *this = *deflt;

    //if( _application == application_order  &&  element.getAttribute( "let_run" ) != "" )  z::throw_xc( "SCHEDULER-220", "let_run='yes'" );
    _let_run = element.bool_getAttribute( "let_run", _let_run );

    string single_start = element.getAttribute( "single_start" );
    if( !single_start.empty() )
    {
        dt.set_time( single_start );
        set_single_start( Time( dt ) );
    }
    else
    {
        string begin = element.getAttribute( "begin", "00:00:00" );        
        if( !begin.empty() )
        {
            if( w == with_date )  dt = begin;
                            else  dt.set_time( begin );
            _begin = Time(dt);
        }
        string repeat = element.getAttribute( "repeat" );
        if( !repeat.empty() )
        {
            if( repeat.find( ':' ) != string::npos )
            {
                Sos_optional_date_time dt;
                dt.set_time( repeat );
                _repeat = Duration(dt.time_as_double());
            }
            else
                _repeat = Duration(as_double(repeat));
        }

        if( _repeat.is_zero() )  _repeat = Duration::eternal;


        string absolute_repeat = element.getAttribute( "absolute_repeat" );
        if( !absolute_repeat.empty() )
        {
            if( absolute_repeat.find( ':' ) != string::npos )
            {
                Sos_optional_date_time dt;
                dt.set_time( absolute_repeat );
                _absolute_repeat = Duration(dt.time_as_double());
            }
            else
                _absolute_repeat = Duration(as_double(absolute_repeat));

            if( _absolute_repeat.is_zero() )  z::throw_xc( "SCHEDULER-441", element.nodeName(), "absolute_repeat", "0" );
            if( !_absolute_repeat.is_eternal() )  _repeat = Duration::eternal;
          //if( !_repeat.is_never()   )  z::throw_xc( "SCHEDULER-442", "absolute_repeat", "repeat" );
        }

        string end = element.getAttribute( "end" , "24:00:00" );
        if( !end.empty() )
        {
            if( w == with_date )  dt = end;
                            else  dt.set_time( end );
            _end = Time(dt);
        }
        _start_once = element.bool_getAttribute( "start_once", _start_once );   // Für Joacim Zschimmer
    }

    if( element.hasAttribute( "when_holiday" ) )  _when_holiday = when_holiday_from_string( element.getAttribute( "when_holiday" ) );

    check( w );
}

//------------------------------------------------------------------------------------Period::check

void Period::check( With_or_without_date w ) const
{
    if( _begin < Time(0))  goto FEHLER;
    if( _begin > _end   )  goto FEHLER;
    if( w == without_date )  if( _end > Time(24*60*60) )  goto FEHLER;
    return;

  FEHLER:
    z::throw_xc( "SCHEDULER-104", _begin.without_timezone_string(), _end.without_timezone_string() );
}

//-------------------------------------------------------------------------------Period::is_coming

bool Period::is_coming( const Time& time_of_day, With_single_start single_start ) const
{
    bool result;

    if( single_start & wss_when_holiday_ignore_holiday        &&  _when_holiday != wh_ignore_holiday       )  result = false;
    else
    if( single_start & wss_when_holiday_previous_non_holiday  &&  _when_holiday != wh_previous_non_holiday )  result = false;
    else
    if( single_start & wss_when_holiday_next_non_holiday      &&  _when_holiday != wh_next_non_holiday     )  result = false;
    else
    if( single_start & wss_next_period  &&  !_single_start  &&  time_of_day < _end )  result = true;
                                                                         // ^-- Falls time_of_day == previous_period.end(), sonst Schleife!
    else
    if( single_start & wss_next_begin  &&  !_single_start  &&  time_of_day <= _begin )  result = true;
                                                                        // ^ Falls _begin == 00:00 und time_of_day == 00:00 (Beginn des nächsten Tags)
    else
    if( single_start & wss_next_single_start  &&  _single_start  &&  time_of_day <= _begin )  result = true;
                                                                              // ^ Falls _begin == 00:00 und time_of_day == 00:00 (Beginn des nächsten Tags)
    else
  //if( single_start & wss_next_any_start  &&  ( ( _single_start || has_repeat_or_once() ) && time_of_day <= _begin ) )  result = true;
                                                                                                       // ^ Falls _begin == 00:00 und time_of_day == 00:00 (Beginn des nächsten Tags)
    if( single_start & wss_next_any_start  &&  ( _single_start         && time_of_day <= _begin ||
                                                  has_repeat_or_once() && time_of_day <  _end      ) )  result = true;
    else
    if( single_start & ( wss_next_any_start | wss_next_single_start )  &&  !_absolute_repeat.is_eternal()  &&  !next_absolute_repeated(time_of_day).is_never() )  result = true;
    else
        result = false;

    //Z_LOG2( "zschimmer", *this << ".is_coming(" << time_of_day << ',' << (int)single_start << ") ==> " << result << "\n" );

    return result;
}

//---------------------------------------------------------------------------------Period::next_try

Time Period::next_try( const Time& t ) const
{
    Time result = t;

    if( result >= end() )  result = Time::never;

    return result;
}

//----------------------------------------------------------------------------Period::next_repeated

Time Period::next_repeated( const Time& t ) const
{
    Time result = next_repeated_allow_after_end( t );

    return result < _end? result : Time::never;  
}

//----------------------------------------------------------------------------Period::next_repeated

Time Period::next_repeated_allow_after_end( const Time& t ) const
{
    Time result = Time::never;
    
    if( !_repeat.is_eternal() )
    {
        result = t < _begin? _begin 
                           : Time( t + _repeat );
    }
    else
    if( !_absolute_repeat.is_eternal() )
    {
        result = next_absolute_repeated(t); 
        // result ist never, wenn Ende der Periode erreicht ist. "allow_after_end" gilt nur für repeat, nicht für absolute_repeat.
    }

    return result;  
}

//-------------------------------------------------------------------Period::next_absolute_repeated

Time Period::next_absolute_repeated(const Time& tim) const
{
    assert( !_absolute_repeat.is_eternal() );

    Time t      = tim;
    Time result = Time::never;

    if( t < _begin )  t = _begin;

    int n = (int)( ( t - _absolute_repeat_begin ).as_double() / _absolute_repeat.as_double() );
    result = Time(_absolute_repeat_begin.as_double() + (n + 1) * _absolute_repeat.as_double());
    if( result == t + _absolute_repeat)  result = t;

    assert(result >= t);

    return result < _end? result : Time::never;  
}

//------------------------------------------------------------------------------------Period::print

void Period::print( ostream& s ) const
{
    s << "Period(" << _begin.utc_string() << ".." << _end.utc_string();
    if( _single_start )  s << " single_start";
    if( !_repeat.is_eternal() )  s << " repeat=" << _repeat;
    if( !_absolute_repeat.is_eternal() )  s << " absolute_repeat=" << _absolute_repeat;
    if( _let_run )  s << " let_run";
    s << ")";
}

//------------------------------------------------------------------------------Period::dom_element

xml::Element_ptr Period::dom_element( const xml::Document_ptr& dom_document ) const
{
    xml::Element_ptr result = dom_document.createElement( "period" );

    if( _single_start )
    {
        result.setAttribute( "single_start", _begin.xml_value( time::without_ms ) );
    }
    else
    {
        result =  dom_document.createElement( "period" );
        result.setAttribute( "begin", _begin.xml_value( time::without_ms ) );
        result.setAttribute( "end"  , _end  .xml_value( time::without_ms ) );
        if( _let_run )  result.setAttribute( "let_run", "yes" );

        if( !_repeat.is_eternal()          )  result.setAttribute( "repeat"         , _repeat.seconds() );
        if( !_absolute_repeat.is_eternal() )  result.setAttribute( "absolute_repeat", _absolute_repeat.seconds() );
        if( _when_holiday                  )  result.setAttribute( "when_holiday"   , string_from_when_holiday( _when_holiday ) );
    }

    result.setAttribute_optional( "schedule", _schedule_path );

    return result;
}

//-----------------------------------------------------------------------------------Period::to_xml

string Period::to_xml() const
{
    S result;

    result << "<period";
    result << " begin=\"" << _begin.without_timezone_string() << "\"";
    result << " end=\""   << _end  .without_timezone_string() << "\"";
    if( !_repeat         .is_eternal() )  result << " repeat=\""          << _repeat         .as_string() << "\"";
    if( !_absolute_repeat.is_eternal() )  result << " absolute_repeat=\"" << _absolute_repeat.as_string() << "\"";
    result << "/>";

    return result;
}

//---------------------------------------------------------------------------------Period::obj_name

string Period::obj_name() const
{
    ostrstream s;
    print( s );
    s << '\0';
    return s.str();
}

//-----------------------------------------------------------------------------Day::set_dom_periods

void Day::set_dom_periods( const xml::Element_ptr& element, const Day* default_day, const Period* default_period )
{
    if( !element )  return;

    if( default_day )  _period_set = default_day->_period_set;

    bool first = true;

    DOM_FOR_EACH_ELEMENT( element, e )
    {
        if( first )  first = false, _period_set.clear();
        _period_set.insert( Period( e, default_period ) );
    }
}

//---------------------------------------------------------------------------------------Day::merge

void Day::merge( const Day& other_day )
{
    _period_set.insert( other_day._period_set.begin(), other_day._period_set.end() );
}

//------------------------------------------------------------------------------------Day::has_time

bool Day::has_time( const Time& time_of_day )
{
    FOR_EACH( Period_set, _period_set, it )
    {
        if( it->begin() >= time_of_day || it->end() > time_of_day )  return true;
    }

    return false;
}

//--------------------------------------------------------------------------------Day::next_period_

Period Day::next_period_( const Time& time_of_day, With_single_start single_start ) const
{
    FOR_EACH_CONST( Period_set, _period_set, it )
    {
        if( it->is_coming( time_of_day, single_start ) )  return *it;
    }

    return Period();
}

//---------------------------------------------------------------------------------------Day::print

void Day::print( ostream& s ) const
{
    s << "Day(";

    FOR_EACH_CONST( Period_set, _period_set, it )
    {
        s << *it << ' ';
    }

    s << ")";
}

//---------------------------------------------------------------------------------Day_set::Day_set

Day_set::Day_set( int minimum, int maximum )
: 
    _zero_(this+1), 
    _minimum(minimum), 
    _maximum(maximum),
    _days(1+maximum)
{
} 

//---------------------------------------------------------------------------------Day_set::Day_set
    
Day_set::Day_set( int minimum, int maximum, const xml::Element_ptr& e )  
: 
    _zero_(this+1), 
    _minimum(minimum), 
    _maximum(maximum),
    _days(1+maximum)
{ 
    set_dom(e); 
}

//-----------------------------------------------------------------------------------Day_set::print

void Day_set::print( ostream& s ) const
{
    for( int i = 0; i < _days.size(); i++ )
    {
        if( _days[i] )  s << ' ' << i << "=" << *_days[i];
    }
}

//-------------------------------------------------------------------Weekday_set::fill_with_default

void Weekday_set::fill_with_default( const Day& default_day )
{ 
    ptr<Day> day = Z_NEW( Day( default_day ) );

    for( int i = 0; i < 7; i++ )  _days[i] = day;

    _is_day_set_filled = true;
}

//-------------------------------------------------------------Weekday_set::next_period_of_same_day

Period Weekday_set::next_period_of_same_day( const Time& tim, With_single_start single_start )
{
    Period result;

    if( _is_day_set_filled )
    {
        Time time_of_day = tim.time_of_day();
        int  day_nr      = tim.day_nr();
        int  weekday     = weekday_of_day_number( day_nr );

        if( const Day* day = _days[ weekday % 7 ] )
        {
            Period period = day->next_period( time_of_day, single_start );
            if( !period.empty() )  result = Time(day_nr*(24*60*60)) + period;
        }
    }

    return result;
}

//----------------------------------------------------------------------------Monthday_set::set_dom

void Monthday_set::set_dom( const xml::Element_ptr& monthdays_element, const Day* default_day, const Period* default_period )
{
    if( monthdays_element )
    {
        DOM_FOR_EACH_ELEMENT( monthdays_element, element )
        {
            if( element.nodeName_is( "day" ) )
            {
                Day       my_default_day ( element, default_day, default_period );
                list<int> day_numbers    = get_day_numbers( element.getAttribute( "day" ) );
                ptr<Day>  day            = Z_NEW( Day( element, &my_default_day, default_period ) );
                
                Z_FOR_EACH( list<int>, day_numbers, d )
                {
                    if( !_days[ *d ] )  _days[ *d ] = Z_NEW( Day( *day ) );
                                  else  _days[ *d ]->merge( *day );
                }

                _is_day_set_filled = true;
            }
            else
            if( element.nodeName_is( "weekday" ) )
            {
                Day       my_default_day ( element, default_day, default_period );
                list<int> weekdays       = get_weekday_numbers( element.getAttribute( "day" ) );
                int       which          = element.int_getAttribute( "which" );

                if( which == 0  ||  abs( which ) > max_weekdays_per_month )  z::throw_xc( Z_FUNCTION, S() << "Invalid value for which=" << which );    // XML-Schema hat schon geprüft

                Month_weekdays* m   = which > 0? &_month_weekdays : &_reverse_month_weekdays;
                ptr<Day>        day = Z_NEW( Day( element, &my_default_day, default_period ) );
                
                Z_FOR_EACH( list<int>, weekdays, w )
                {
                    if( m->day( abs(which), *w ) )  z::throw_xc( "SCHEDULER-446", *w, which );
                    m->day( abs(which), *w ) = day;
                }

                m->_is_filled = true;
            }
        }
    }
}

//------------------------------------------------------------Monthday_set::next_period_of_same_day

Period Monthday_set::next_period_of_same_day( const Time& tim, With_single_start single_start )
{
    Period result;

    if( is_filled() )
    {
        Time                    time_of_day = tim.time_of_day();
        int                     day_nr      = tim.day_nr();
        Sos_optional_date_time  date        = tim.as_time_t();
        int                     weekday     = weekday_of_day_number( day_nr );
        Period                  period;

        if( _is_day_set_filled )
        {
            if( const Day* day = _days[ date.day() ] )
            {
                period = day->next_period( time_of_day, single_start );
            }
        }

        if( _month_weekdays._is_filled )
        {
            int which =  1 + ( date.day() - 1 ) / 7;

            if( Day* day = _month_weekdays.day( which, weekday ) )
            {
                Period period2 = day->next_period( time_of_day, single_start );
                if( period > period2 )  period = period2;
            }
        }

        if( _reverse_month_weekdays._is_filled )
        {
            int reverse_which = 1 + ( last_day_of_month( date ) - date.day() ) / 7;

            if( Day* day = _reverse_month_weekdays.day( reverse_which, weekday ) )
            {
                Period period3 = day->next_period( time_of_day, single_start );
                if( period > period3 )  period = period3;
            }
        }

        if( !period.empty() )
        {
            result = Time(day_nr*(24*60*60)) + period;
        }
    }

    return result;
}

//----------------------------------------------------------------Monthday_set::Month_weekdays::day

ptr<Day>& Monthday_set::Month_weekdays::day( int which, int weekday_number ) 
{ 
    assert( which != 0  && which <= max_weekdays_per_month );
    assert( weekday_number >= 0  &&  weekday_number < 7 );

    int index = 7 * ( which - 1 ) + weekday_number;

    return _days[ index ]; 
}

//--------------------------------------------------------------Ultimo_set::next_period_of_same_day

Period Ultimo_set::next_period_of_same_day( const Time& tim, With_single_start single_start )
{
    Period result;

    if( _is_day_set_filled )
    {
        Time     time_of_day = tim.time_of_day();
        int      day_nr      = tim.day_nr();
        Sos_date date        = Sos_optional_date_time(tim.as_time_t());

        if( const Day* day = _days[ last_day_of_month( date ) - date.day() ] )
        {
            Period period = day->next_period( time_of_day, single_start );
            if( !period.empty() )  result = Time(day_nr*(24*60*60)) + period;
        }
    }

    return result;
}

//--------------------------------------------------------------------------------Holidays::set_dom

void Holidays::set_dom( File_based* source_file_based, const xml::Element_ptr& e, int include_nesting )
{
    if( e.nodeName_is( "holidays" ) )
    {
        DOM_FOR_EACH_ELEMENT( e, e2 )
        {
            if( e2.nodeName_is( "weekdays" ) )
            {
                DOM_FOR_EACH_ELEMENT( e2, e3 )
                {
                    if( e3.nodeName_is( "day" ) )
                    {
                        list<int> weekdays = get_weekday_numbers( e3.getAttribute( "day" ) );
                        Z_FOR_EACH( list<int>, weekdays, w )  _weekdays[*w] = true;
                    }
                }
            }
            else
            if( e2.nodeName_is( "holiday" ) )
            {
                Sos_optional_date_time dt;
                dt.assign( e2.getAttribute( "date" ) );
                include_day( dt.as_time_t() / (24*60*60) );
            }
            else
            if( e2.nodeName_is( "include" ) )
            {
                if( include_nesting >= max_include_nesting )  z::throw_xc( "SCHEDULER-390", max_include_nesting, "<holidays>" );

                Include_command include_command ( _spooler, source_file_based, e2, _spooler->_configuration_file_path.directory() );

                try
                {
                    string xml_bytes = include_command.register_include_and_read_content_bytes(source_file_based);
                    Z_LOG2( "scheduler", Z_FUNCTION << "  " << xml_bytes << "\n" );

                    xml::Document_ptr doc = xml::Document_ptr::from_xml_bytes(xml_bytes);
                    if( _spooler->_validate_xml )  _spooler->_schema.validate( doc );

                    if( !doc.documentElement() )  z::throw_xc( "SCHEDULER-239", "holidays" );

                    set_dom( source_file_based, doc.documentElement(), include_nesting+1 );
                }
                catch( exception& x )
                {
                    z::throw_xc( "SCHEDULER-399", include_command.obj_name(), x );
                }
            }
            else
                z::throw_xc( "SCHEDULER-319", e2.nodeName(), e.nodeName() );
        }
    }
    else
    if( e.nodeName_is( "holiday" ) )
    {   
        Sos_optional_date dt;
        dt.assign( e.getAttribute( "date" ) );
        include_day(dt.as_time_t() / (24*60*60));
    }
    else
        z::throw_xc( "SCHEDULER-319", e.nodeName(), Z_FUNCTION );
}

//----------------------------------------------------------------------------Holidays::is_included

bool Holidays::is_included( const Time& t )
{ 
    int day = t.day_nr();
    return _day_set.find(day) != _day_set.end()  ||  _weekdays[weekday_of_day_number(day)];
}

//--------------------------------------------------------------------------------------Date::print

void Date::print( ostream& s ) const
{
    Sos_optional_date dt;
    dt.set_time_t( _day_nr * (24*60*60) );

    s << "Date(" << dt << " " << _day << ')';
}

//----------------------------------------------------------------Date_set::next_period_of_same_day

Period Date_set::next_period_of_same_day( const Time& tim, With_single_start single_start )
{
    Period result;
    Time   time_of_day = tim.time_of_day();
    int    day_nr      = tim.day_nr();

    FOR_EACH( set<Date>, _date_set, it )
    {
        const Date& date = *it;

        if( date._day_nr >= day_nr )
        {
            if( date._day_nr == day_nr )
            {
                const Period& period = date._day.next_period( date._day_nr == day_nr? time_of_day : Time(0), single_start );
                if( !period.empty() )  result = Time(date._day_nr*(24*60*60)) + period;
            }

            break;
        }
    }

    return result;
}

//---------------------------------------------------------------------------------Date_set::print

void Date_set::print( ostream& s ) const
{
    s << "Date_set(";

    FOR_EACH_CONST( set<Date>, _date_set, it )
    {
        s << *it << " ";
    }

    s << ")";
}

//------------------------------------------------------------------At_set::next_period_of_same_day

Period At_set::next_period_of_same_day( const Time& tim, With_single_start single_start )
{
    Period result;
    
    if( single_start & (wss_next_any_start|wss_next_single_start))
    {
        FOR_EACH( set<Time>, _at_set, it )
        {
            const Time& time = *it;

            if( time >= tim )
            {
                if( time.day_nr() == tim.day_nr() )
                {
                    result._begin           = time;
                    result._end             = time + Duration::epsilon;     // JS-802, damit ein Auftrag nicht wegen _period.end() == at in eine Schleife geht
                    result._single_start    = true;
                    result._let_run         = true;
                    result._repeat          = Duration::eternal;
                    result._absolute_repeat = Duration::eternal;
                }

                break;
            }
        }
    }

    return result;
}

//---------------------------------------------------------------------------------Date_set::print

void At_set::print( ostream& s ) const
{
    s << "At_set(";

    FOR_EACH_CONST( set<Time>, _at_set, it )
    {
        s << it->utc_string() << " ";
    }

    s << ")";
}

//---------------------------------------------------------------------------------Day_set::set_dom

void Day_set::set_dom( const xml::Element_ptr& element, const Day* default_day, const Period* default_period )
{
    if( !element )  return;

    DOM_FOR_EACH_ELEMENT( element, e )
    {
        if( e.nodeName_is( "day" ) )
        {
            Day       my_default_day ( e, default_day, default_period );
            ptr<Day>  day            = Z_NEW( Day( e, &my_default_day, default_period ) );
            list<int> day_numbers    = get_day_numbers( e.getAttribute( "day" ) );

            Z_FOR_EACH( list<int>, day_numbers, d )
            {
                if( !_days[ *d ] )  _days[ *d ] = Z_NEW( Day( *day ) );
                              else  _days[ *d ]->merge( *day );
            }

            _is_day_set_filled = true;
        }
    }
}

//---------------------------------------------------------------------------------Day::set_dom_day

int Day_set::get_day_number( const string& day_string )
{
    int result;

    if( _minimum == 0  &&  _maximum == 6 )      // Es ist ein Wochentag
    {
        result = get_weekday_number( day_string );
    }
    else
    {
        result = as_int( day_string );
        if( result < _minimum  ||  result > _maximum )  z::throw_xc( "SCHEDULER-221", day_string, as_string( _minimum ), as_string( _maximum ) );
    }

    return result;
}

//-------------------------------------------------------------------------Day_set::get_day_numbers

list<int> Day_set::get_day_numbers( const string& days_string )
{
    list<int>      result;
    vector<string> names  = vector_split( " +", days_string );

    Z_FOR_EACH( vector<string>, names, it )  result.push_back( get_day_number( *it ) );

    return result;
}

//-----------------------------------------------------------------------------------Month::set_dom

void Month::set_dom( const xml::Element_ptr& element )
{
    Period  default_period;
    Day     default_day;
    bool    period_seen    = false;

    default_period.set_dom( element );
    default_day = default_period;

    bool a_day_is_set = false;

    DOM_FOR_EACH_ELEMENT( element, e )
    {
        if( e.nodeName_is( "period" ) )
        {
            if( !period_seen )  period_seen = true, default_day = Day();
            default_day.add( Period( e, &default_period ) );
        }
        else
        if( e.nodeName_is( "weekdays" ) )
        {
            a_day_is_set = true;
            _weekday_set.set_dom( e, &default_day, &default_period );
        }
        else
        if( e.nodeName_is( "monthdays" ) )
        {
            a_day_is_set = true;
            _monthday_set.set_dom( e, &default_day, &default_period );
        }
        else
        if( e.nodeName_is( "ultimos" ) )
        {
            a_day_is_set = true;
            _ultimo_set.set_dom( e, &default_day, &default_period );
        }
    }

    if( !a_day_is_set )  _weekday_set.fill_with_default( default_day );
}

//-------------------------------------------------------------------Month::next_period_of_same_day

Period Month::next_period_of_same_day( const Time& tim, With_single_start single_start )
{
    Period result;
    if( _weekday_set .is_filled() )  result = min( result, _weekday_set .next_period_of_same_day( tim, single_start ) );
    if( _monthday_set.is_filled() )  result = min( result, _monthday_set.next_period_of_same_day( tim, single_start ) );
    if( _ultimo_set  .is_filled() )  result = min( result, _ultimo_set  .next_period_of_same_day( tim, single_start ) );
    return result;
}

//-------------------------------------------------------------------------------------------------

} //namespace schedule
} //namespace scheduler
} //namespace sos
