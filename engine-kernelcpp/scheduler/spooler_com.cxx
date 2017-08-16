// $Id: spooler_com.cxx 14220 2011-04-29 14:12:18Z jz $
/*
    Hier sind implementiert

    Com_log                     COM-Hülle für Task_log
    Com_task                    COM-Hülle für Task
    Com_spooler                 COM-Hülle für Spooler
*/


#include "spooler.h"
//#include "../hostole/hostole.h"
#include "../zschimmer/z_com.h"
#include "../zschimmer/com_server.h"

#ifdef Z_WINDOWS
#   include <process.h>                    // getpid()
#endif

using namespace zschimmer::com;


namespace sos {
namespace scheduler {
namespace com_objects {

using namespace std;
using namespace spooler_com;

//-------------------------------------------------------------------------Typbibliothek (hostware)

Typelib_descr spooler_typelib ( LIBID_spooler_com, "Spooler", "1.0" );

DESCRIBE_CLASS( &spooler_typelib, Com_error          , error          , CLSID_Error          , "Spooler.Error"         , "1.0" )
DESCRIBE_CLASS( &spooler_typelib, Com_variable       , variable       , CLSID_Variable       , "Spooler.Variable"      , "1.0" )
DESCRIBE_CLASS( &spooler_typelib, Com_variable_set   , variable_set   , CLSID_Variable_set   , "Spooler.Variable_set"  , "1.0" )
DESCRIBE_CLASS( &spooler_typelib, Com_variable_set_enumerator, variable_set_enumerator, CLSID_Variable_set_enumerator, "Spooler.Com_variable_set_enumerator", "1.0" );
DESCRIBE_CLASS( &spooler_typelib, Com_log            , log            , CLSID_Log            , "Spooler.Log"           , "1.0" )
DESCRIBE_CLASS( &spooler_typelib, Com_job            , job            , CLSID_Job            , "Spooler.Job"           , "1.0" )
DESCRIBE_CLASS( &spooler_typelib, Com_task           , task           , CLSID_Task           , "Spooler.Task"          , "1.0" )
DESCRIBE_CLASS( &spooler_typelib, Com_spooler        , spooler        , CLSID_Spooler        , "Spooler.Spooler"       , "1.0" )
DESCRIBE_CLASS( &spooler_typelib, Com_spooler_context, spooler_context, CLSID_Spooler_context, "Spooler.Context"       , "1.0" )
DESCRIBE_CLASS( &spooler_typelib, Com_job_chain      , job_chain      , CLSID_Job_chain      , "Spooler.Job_chain"     , "1.0" )
DESCRIBE_CLASS( &spooler_typelib, Com_job_chain_node , job_chain_node , CLSID_Job_chain_node , "Spooler.Job_chain_node", "1.0" )
DESCRIBE_CLASS( &spooler_typelib, Com_order          , order          , CLSID_Order          , "Spooler.Order"         , "1.0" )
DESCRIBE_CLASS( &spooler_typelib, Com_order_queue    , order_queue    , CLSID_Order_queue    , "Spooler.Order_queue"   , "1.0" )

const int                           dispid_log_log          = 14;           // Siehe spooler.odl

//------------------------------------------------------------------------------------Typbibliothek

Typelib_ref                         typelib;  

Com_spooler_proxy::Class_descriptor Com_spooler_proxy::class_descriptor ( &typelib, "Spooler.Spooler_proxy", Com_spooler_proxy::_methods );
Com_task_proxy   ::Class_descriptor Com_task_proxy   ::class_descriptor ( &typelib, "Spooler.Task_proxy"   , Com_task_proxy   ::_methods );

//-----------------------------------------------------------------------------IID_Ihostware_dynobj

#ifdef Z_WINDOWS
    extern "C" const GUID IID_Ihostware_dynobj = { 0x9F716A02, 0xD1F0, 0x11CF, { 0x86, 0x9D, 0x44, 0x45, 0x53, 0x54, 0x00, 0x00 } };
#else
    DEFINE_GUID( IID_Ihostware_dynobj, 0x9F716A02, 0xD1F0, 0x11CF, 0x86, 0x9D, 0x44, 0x45, 0x53, 0x54, 0x00, 0x00 );
#endif

//----------------------------------------------------------------------------duration_from_variant

Duration duration_from_variant( const VARIANT& vt )
{
    return Duration::of( variant_as_string( vt ) );
}

//----------------------------------------------------------------------order_from_order_or_payload

static ptr<spooler_com::Iorder> order_from_order_or_payload( Spooler* spooler, const VARIANT& order_or_payload )
{
    ptr<spooler_com::Iorder> iorder;

    if( order_or_payload.vt == VT_DISPATCH  ||  order_or_payload.vt == VT_UNKNOWN )
    {
        if( V_UNKNOWN(&order_or_payload) )
        {
            iorder.Assign_qi( V_UNKNOWN( &order_or_payload ) );
        }
    }

    if( !iorder )  
    {
        //iorder = new Order( spooler, order_or_payload );
        ptr<Order> order = new Order( spooler->standing_order_subsystem() );
        order->set_payload( order_or_payload );

        iorder = +order;
    }

    return iorder;
}

//------------------------------------------------------------------------------Com_error::_methods
#ifdef Z_COM

const Com_method Com_error::_methods[] =
{ 
   // _flags              , _name             , _method                                        , _result_type, _types        , _default_arg_count
    { DISPATCH_PROPERTYGET, 1, "Java_class_name" , (Com_method_ptr)&Com_error::get_Java_class_name, VT_BSTR },
    { DISPATCH_PROPERTYGET, 0, "Is_error"        , (Com_method_ptr)&Com_error::get_Is_error       , VT_BOOL },
    { DISPATCH_PROPERTYGET, 3, "Code"            , (Com_method_ptr)&Com_error::get_Code           , VT_BSTR }, 
    { DISPATCH_PROPERTYGET, 4, "Text"            , (Com_method_ptr)&Com_error::get_Text           , VT_BSTR }, 
    {}
};

#endif
//-----------------------------------------------------------------------------Com_error::Com_error

Com_error::Com_error( const Xc_copy& x )
: 
    Sos_ole_object( error_class_ptr, (Ierror*)this ),
    _xc(x) 
{
}

//------------------------------------------------------------------------Com_error::QueryInterface

STDMETHODIMP Com_error::QueryInterface( const IID& iid, void** result )
{
    Z_IMPLEMENT_QUERY_INTERFACE( this, iid, Ihas_java_class_name, result );

    return Sos_ole_object::QueryInterface( iid, result );
}

//---------------------------------------------------------------------------------Com_error::close

void Com_error::close()
{ 
    _xc = NULL; 
}

//------------------------------------------------------------------------------Com_error::is_error

STDMETHODIMP Com_error::get_Is_error( VARIANT_BOOL* result )
{
    *result = _xc != NULL;
    return NOERROR;
}

//----------------------------------------------------------------------------------Com_error::code

STDMETHODIMP Com_error::get_Code( BSTR* code_bstr )
{
    HRESULT hr = NOERROR;

    try
    {
        if( !_xc )  *code_bstr = NULL;
              else  *code_bstr = SysAllocStringLen_char( _xc->code(), int_strlen( _xc->code() ) );
    }
    catch( const exception&  x )  { hr = _set_excepinfo( x, "Spooler.Error::code" ); }
    catch( const _com_error& x )  { hr = _set_excepinfo( x, "Spooler.Error::code" ); }

    return hr;
}

//----------------------------------------------------------------------------------Com_error::text

STDMETHODIMP Com_error::get_Text( BSTR* text_bstr )
{
    HRESULT hr = NOERROR;

    try
    {
        if( !_xc )  *text_bstr = NULL;
              else  *text_bstr = SysAllocString_string( _xc->what() );
    }
    catch( const exception&  x )  { hr = _set_excepinfo( x, "Spooler.Error::text" ); }
    catch( const _com_error& x )  { hr = _set_excepinfo( x, "Spooler.Error::text" ); }

    return hr;
}

//---------------------------------------------------------------------------Com_variable::_methods
#ifdef Z_COM

const Com_method Com_variable::_methods[] =
{ 
   // _flags         , dispid, _name                , _method                                           , _result_type  , _types        , _default_arg_count
    { DISPATCH_PROPERTYPUT, 0, "value"              , (Com_method_ptr)&Com_variable::put_Value          , VT_EMPTY      , { VT_BYREF|VT_VARIANT } },
    { DISPATCH_PROPERTYGET, 0, "value"              , (Com_method_ptr)&Com_variable::get_Value          , VT_VARIANT    },
    { DISPATCH_PROPERTYGET, 1, "java_class_name"    , (Com_method_ptr)&Com_variable::get_Java_class_name, VT_BSTR },
    { DISPATCH_PROPERTYGET, 2, "name"               , (Com_method_ptr)&Com_variable::get_Name           , VT_BSTR       },
    { DISPATCH_METHOD     , 3, "Clone"              , (Com_method_ptr)&Com_variable::Clone              , VT_DISPATCH   },
    {}
};

#endif

//----------------------------------------------------------------------get_variable_name_and_value

void get_variable_name_and_value( const xml::Element_ptr& element, string* name, string* value )
{
    *name  = element.getAttribute( "name" );
    *value = element.getAttribute( "value" );

    if( *value == "" )  *value = string_from_hex( element.getAttribute( "hex_value" ) );    // Jedes Hexbyte ein Unicode-Zeichen 00..FF
}                

//-----------------------------------------------------------------------Com_variable::Com_variable

Com_variable::Com_variable( const BSTR name, const VARIANT& value )
:
    Sos_ole_object( variable_class_ptr, (Ivariable*)this )
{
    if( SysStringLen( name ) == 0 )  z::throw_xc( "SCHEDULER-198" );

    {
        _name = name;
        _value = value;
    }
}

//-----------------------------------------------------------------------Com_variable::string_value
    
string Com_variable::string_value() const
{
    try
    {
        return string_from_variant( _value );
    }
    catch( exception& x )
    {
        z::throw_xc( "SCHEDULER-312", _name, vartype_name( _value.vt ), x );
    }
}

//---------------------------------------------------------------------Com_variable::QueryInterface

STDMETHODIMP Com_variable::QueryInterface( const IID& iid, void** result )
{
    Z_IMPLEMENT_QUERY_INTERFACE( this, iid, Ihas_java_class_name, result );

    return Sos_ole_object::QueryInterface( iid, result );
}

//------------------------------------------------------------------------------Com_variable::Clone

STDMETHODIMP Com_variable::Clone( Ivariable** result ) 
{ 
    HRESULT hr = NOERROR; 
    
    *result = new Com_variable(_name,_value); 
    (*result)->AddRef();

    return hr;
}

//------------------------------------------------------------------------Com_variable::dom_element

xml::Element_ptr Com_variable::dom_element( const xml::Document_ptr& doc, const string& subelement_name )
{
    xml::Element_ptr result = doc.createElement( subelement_name );
    Bstr             name;
    Variant          value;

    get_Name ( &name._bstr );
    get_Value( &value );

    result.setAttribute( "name" , string_from_bstr( name ) );

    string string_value = this->string_value();
    
    if( io::is_valid_latin1( string_value ) )  result.setAttribute( "value"    , string_value );
                                         else  result.setAttribute( "hex_value", lcase_hex_from_string( string_value ) );   // Jedes Hexbyte ein Unicode-Zeichen 00..FF

    VARTYPE vt = (VARTYPE)-1;

    if( value.vt == VT_EMPTY
     || value.vt == VT_NULL
     || value.vt == VT_I2
     || value.vt == VT_I4
     || value.vt == VT_R4       
     || value.vt == VT_R8       
     || value.vt == VT_CY       
     || value.vt == VT_DATE     
  // || value.vt == VT_BSTR          // VT_BSTR müssen wir nicht besonders kennzeichnen. Das ist Default.
  // || value.vt == VT_DISPATCH 
     || value.vt == VT_ERROR    
     || value.vt == VT_BOOL     
  // || value.vt == VT_VARIANT  
  // || value.vt == VT_UNKNOWN  
     || value.vt == VT_DECIMAL  
     || value.vt == VT_I1       
     || value.vt == VT_UI1      
     || value.vt == VT_UI2      
     || value.vt == VT_UI4      
     || value.vt == VT_I8       
     || value.vt == VT_UI8      
     || value.vt == VT_INT      
     || value.vt == VT_UINT     
  // || value.vt == VT_VOID     
     || value.vt == VT_HRESULT )
    {
        vt = value.vt; 
    }
/*
    else
    if( value.vt == VT_DISPATCH )
    {
        ptr<Variable_set> v;
        if( V_DISPATCH(value)
         && SUCCEEDED( V_DISPATCH(value)->QueryInterface( IID_Ivariable_set, v.pp() ) ) )  vt = value.vt;
    }
*/
    if( vt != (VARTYPE)-1 )  result.setAttribute( "vt", vt );
                       else  {} // Andere Typen sind nicht rückkonvertierbar. Die werden dann zum String.  


    return result;
}

//-----------------------------------------------------------------------Com_variable_set::_methods
#ifdef Z_COM

const Com_method Com_variable_set::_methods[] =
{ 
   // _flags         , dispid, _name                , _method                                           , _result_type  , _types        , _default_arg_count
    { DISPATCH_METHOD     , 1, "set_var"            , (Com_method_ptr)&Com_variable_set::Set_var        , VT_EMPTY      , { VT_BSTR, VT_BYREF|VT_VARIANT } },
    { DISPATCH_PROPERTYPUT, 0, "value"              , (Com_method_ptr)&Com_variable_set::put_Value      , VT_EMPTY      , { VT_BYREF|VT_VARIANT, VT_BYREF|VT_VARIANT }, 1 },
    { DISPATCH_PROPERTYGET, 0, "value"              , (Com_method_ptr)&Com_variable_set::get_Value      , VT_VARIANT    , { VT_BYREF|VT_VARIANT                      }, 1 },
    { DISPATCH_PROPERTYPUT, 8, "var"                , (Com_method_ptr)&Com_variable_set::put_Var        , VT_EMPTY      , { VT_BSTR, VT_BYREF|VT_VARIANT } },
    { DISPATCH_PROPERTYGET, 8, "var"                , (Com_method_ptr)&Com_variable_set::get_Var        , VT_VARIANT    , { VT_BSTR } },
    { DISPATCH_PROPERTYGET, 2, "count"              , (Com_method_ptr)&Com_variable_set::get_Count      , VT_I4         },
    { DISPATCH_PROPERTYGET, 3, "dom"                , (Com_method_ptr)&Com_variable_set::get_Dom        , VT_DISPATCH   },
    { DISPATCH_METHOD     , 4, "Clone"              , (Com_method_ptr)&Com_variable_set::Clone          , VT_DISPATCH   },
    { DISPATCH_METHOD     , 5, "merge"              , (Com_method_ptr)&Com_variable_set::Merge          , VT_EMPTY      , { VT_DISPATCH } },
    { DISPATCH_PROPERTYGET, DISPID_NEWENUM, "_NewEnum", (Com_method_ptr)&Com_variable_set::get__NewEnum , VT_DISPATCH   },
    { DISPATCH_PROPERTYPUT, 6, "xml"                , (Com_method_ptr)&Com_variable_set::put_Xml        , VT_EMPTY      , { VT_BSTR } },
    { DISPATCH_PROPERTYGET, 6, "xml"                , (Com_method_ptr)&Com_variable_set::get_Xml        , VT_BSTR       },
    { DISPATCH_PROPERTYGET, 7, "java_class_name"    , (Com_method_ptr)&Com_variable_set::get_Java_class_name, VT_BSTR   },
  //{ DISPATCH_PROPERTYGET, 9, "Names_array"        , (Com_method_ptr)&Com_variable_set::get_Names_array, VT_ARRAY      },
    { DISPATCH_PROPERTYGET, 9, "Names"              , (Com_method_ptr)&Com_variable_set::get_Names      , VT_BSTR       },
    { DISPATCH_METHOD     ,10, "Substitute"         , (Com_method_ptr)&Com_variable_set::Substitute     , VT_BSTR       , { VT_BSTR } },
    {}
};

#endif
//---------------------------------------------------------------Com_variable_set::Com_variable_set

Com_variable_set::Com_variable_set()
:
    Sos_ole_object( variable_set_class_ptr, (Ivariable_set*)this ),
    _ignore_case(true)
{
}

//---------------------------------------------------------------Com_variable_set::Com_variable_set

Com_variable_set::Com_variable_set( const xml::Element_ptr& element, const string& variable_element_name )
:
    Sos_ole_object( variable_set_class_ptr, (Ivariable_set*)this ),
    _ignore_case(true)
{
    set_dom( element, NULL, variable_element_name );
}

//---------------------------------------------------------------Com_variable_set::Com_variable_set

Com_variable_set::Com_variable_set( const Com_variable_set& o )
:
    Sos_ole_object( variable_set_class_ptr, (Ivariable_set*)this ),
    _ignore_case(o._ignore_case)
{
    for( Map::const_iterator it = o._map.begin(); it != o._map.end(); it++ )
    {
        Com_variable* v = it->second;
        if( v )
        {
            ptr<Ivariable> clone;

            v->Clone( clone.pp() );
            _map[ it->first ] = static_cast<Com_variable*>( +clone );
        }
    }
}

//-----------------------------------------------------------------Com_variable_set::QueryInterface

STDMETHODIMP Com_variable_set::QueryInterface( const IID& iid, void** result )
{
    Z_IMPLEMENT_QUERY_INTERFACE( this, iid, Ihas_java_class_name, result );

    return Sos_ole_object::QueryInterface( iid, result );
}


//------------------------------------------------------------------------Com_variable_set::set_dom

void Com_variable_set::set_dom( const xml::Element_ptr& params, Variable_set_map* variable_sets, 
                                const string& variable_element_name )
{
    register_include_and_set_dom( (Scheduler*)NULL, (File_based*)NULL, params, variable_sets, variable_element_name );
}
    
//---------------------------------------------------Com_variable_set::register_include_and_set_dom

void Com_variable_set::register_include_and_set_dom( Scheduler* scheduler, File_based* source_file_based,
                                                     const xml::Element_ptr& params, Variable_set_map* variable_sets, 
                                                     const string& variable_element_name )
{
    if( !params )  return;

    DOM_FOR_EACH_ELEMENT( params, e )
    {
        if( e.nodeName_is( variable_element_name ) ) 
        {
            set_variable( e, variable_sets );
        }
        else
        if( e.nodeName_is( "copy_params" ) )
        {
            string from = e.getAttribute( "from" );
            if( !variable_sets )  z::throw_xc( "SCHEDULER-329", from );
            Variable_set_map::iterator it = variable_sets->find( from );
            if( it == variable_sets->end() )  z::throw_xc( "SCHEDULER-329", from );

            if( it->second )  merge( it->second );
        }
        else
        if( e.nodeName_is( "include" )  &&  scheduler )
        {
            string xpath = e.getAttribute( "node" );
            if( xpath == "" )  xpath = "/params/*";

            Include_command include_command ( scheduler, source_file_based, e, scheduler->include_path() );

            try
            {
                    xml::Document_ptr included_doc   = xml::Document_ptr::from_xml_bytes(include_command.register_include_and_read_content_bytes(source_file_based));  // Registrierung nur wenn source_file_based != NULL
                xml::Node_list    nodes          = included_doc.select_nodes( xpath );

                for( int i = 0; i < nodes.count(); i++ )
                {
                    xml::Element_ptr ee = nodes[i];
                    if( !ee.nodeName_is( variable_element_name ) )  z::throw_xc( "SCHEDULER-409", variable_element_name, ee.nodeName() );
                    set_variable( ee, variable_sets );
                }
            }
            catch( exception& x )  { z::throw_xc( "SCHEDULER-399", include_command.obj_name(), x ); }
        }
        else
            z::throw_xc( "SCHEDULER-182", e.nodeName() );
    }
}

//-------------------------------------------------------------------Com_variable_set::set_variable

void Com_variable_set::set_variable( const xml::Element_ptr& element, Variable_set_map* variable_sets )
{
    string name;
    string value;

    get_variable_name_and_value( element, &name, &value );
    
    if( variable_sets )
    {
        Variable_set_map::iterator it = variable_sets->find( variable_set_name_for_substitution );
        if( it != variable_sets->end() )
        {
            value = subst_env( value, +it->second );
        }
    }

    Bstr    name_bstr = name;
    Variant value_vt = value;

    HRESULT hr = put_Var( name_bstr, &value_vt );                       
    if( FAILED(hr) )  throw_ole( hr, "Ivariable_set::put_var" );
}

//----------------------------------------------------------------------Com_variable_set::put_value

STDMETHODIMP Com_variable_set::put_Value( VARIANT* name, VARIANT* value )
{
    if( name->vt == VT_BSTR )
    {
        return put_Var( V_BSTR(name), value );
    }
    else
    if( name->vt == VT_ERROR )
    {
        if( value->vt != VT_BSTR )  return DISP_E_TYPEMISMATCH;
        
        return put_Xml( V_BSTR(value) );
    }
    else 
        return DISP_E_TYPEMISMATCH;
}

//----------------------------------------------------------------------Com_variable_set::get_value

STDMETHODIMP Com_variable_set::get_Value( VARIANT* name, VARIANT* value )
{
    if( name->vt == VT_BSTR )
    {
        return get_Var( V_BSTR(name), value );
    }
    else
    if( name->vt == VT_ERROR )
    {
        value->vt      = VT_BSTR;
        value->bstrVal = NULL;

        HRESULT hr = get_Xml( &V_BSTR(value) );
        //if( !FAILED(hr) )  Z_LOG2( "scheduler", "Com_variable_set::get_value => " << string_from_bstr(V_BSTR(value)) << "\n" );
        return hr;
    }
    else 
        return DISP_E_TYPEMISMATCH;
}

//------------------------------------------------------------------------Com_variable_set::set_var

void Com_variable_set::set_var( const string& name, const Variant& value )
{
    HRESULT hr = put_Var( Bstr( name ), (VARIANT*)&value );
    if( FAILED(hr) )  throw_ole( hr, "Ivariable_set::put_var" );
}

//------------------------------------------------------------------------Com_variable_set::set_var

void Com_variable_set::set_var( const string& name, const string& value )
{
    set_var( name, Variant( value ) );
}

//---------------------------------------------------------------------Com_variable_set::get_string

string Com_variable_set::get_string( const string& name ) const
{
    Variant result;
    get_var( Bstr(name), &result );
    return string_from_variant( result );
}

//------------------------------------------------------------------------Com_variable_set::get_int

int Com_variable_set::get_int(const string& name, int deflt) const {
    int result = deflt;
    string s = get_string(name);
    if (!s.empty()) 
        try { result = as_int(s); } 
        catch (z::Xc& x) { x.append_text("parameter " + name); throw; }
        catch (Xc& x) { x.insert("parameter " + name); throw; }
    return result;
}

//----------------------------------------------------------------------Com_variable_set::get_int64

int64 Com_variable_set::get_int64(const string& name, int64 deflt) const {
    int64 result = deflt;
    string s = get_string(name);
    if (!s.empty()) 
        try { result = as_int64(s.c_str()); } 
        catch (z::Xc& x) { x.append_text("parameter " + name); throw; }
        catch (Xc& x) { x.insert("parameter " + name); throw; }
    return result;
}

//-----------------------------------------------------------------------Com_variable_set::get_bool

bool Com_variable_set::get_bool(const string& name, bool deflt) const {
    bool result = deflt;
    string s = get_string(name);
    if (!s.empty()) 
        try { result = as_bool(s); } 
        catch (z::Xc& x) { x.append_text("parameter " + name); throw; }
        catch (Xc& x) { x.insert("parameter " + name); throw; }
    return result;
}

//-------------------------------------------------------------Com_variable_set::get_string_by_name

string Com_variable_set::get_string_by_name( const string& name, bool* name_found ) const
{
    Variant result;
    
    get_var( Bstr(name), &result );

    *name_found = !result.is_empty();

    return string_from_variant( result );
}

//---------------------------------------------------------------------Com_variable_set::java_names

ArrayListJ Com_variable_set::java_names() const {
    ArrayListJ result = ArrayListJ::new_instance(int_cast(_map.size()));
    Z_FOR_EACH_CONST(Map, _map, it)
        result.add(javaproxy::java::lang::String(it->second->_name));
    return result;
}

//--------------------------------------------------------------------------Com_variable_set::merge

void Com_variable_set::merge( const Ivariable_set* other )
{
    HRESULT hr = Merge( const_cast<Ivariable_set*>( other ) );
    if( FAILED(hr) )  throw_com( hr, "Com_variable_set::merge" );
}

//------------------------------------------------------------------------Com_variable_set::put_var

STDMETHODIMP Com_variable_set::put_Var( BSTR name, VARIANT* value )
{
    // Vorsicht mit _map.erase(): Ein Iterator auf das gelöschte Element wird ungültig. 
    // Com_variable_set_enumerator müsste dann ungültig werden. Aber wir benutzen erase() nicht.

    HRESULT hr = NOERROR;

    try
    {
        Bstr lname = name;

        if( _ignore_case )  lname.to_lower();

        Map::iterator it = _map.find( lname );
        if( it != _map.end()  &&  it->second )
        {
            it->second->put_Value( value );
        }
        else
        {
            ptr<Com_variable> v = new Com_variable( name, *value );
            _map[lname] = v;
        }
    }
    catch( const exception&  x )  { hr = _set_excepinfo( x, "Spooler.Variable_set::var" ); }
    catch( const _com_error& x )  { hr = _set_excepinfo( x, "Spooler.Variable_set::var" ); }

    return hr;
}

//---------------------------------------------------------------Com_variable_set::variable_or_null

Com_variable* Com_variable_set::variable_or_null( const string& name ) const
{
    return variable_or_null( Bstr( name ) );
}

//---------------------------------------------------------------Com_variable_set::variable_or_null

Com_variable* Com_variable_set::variable_or_null( const Bstr& name ) const
{
    Com_variable* result = NULL;

    Bstr lname = name;
    if( _ignore_case )  lname.to_lower();

    Map::const_iterator it = _map.find( lname );
    if( it != _map.end() )  result = it->second;

    return result;
}

//------------------------------------------------------------------------Com_variable_set::get_var

void Com_variable_set::get_var( BSTR name, VARIANT* value ) const
{
    VariantInit( value );

    if( Com_variable* v = variable_or_null( name  ) )
    {
        v->get_Value( value );
    }
}

//------------------------------------------------------------------------Com_variable_set::get_var

STDMETHODIMP Com_variable_set::get_Var( BSTR name, VARIANT* value )
{
    HRESULT hr = NOERROR;

    try
    {
        get_var( name, value );
    }
    catch( const exception&  x )  { hr = _set_excepinfo( x, "Spooler.Variable_set::var" ); }
    catch( const _com_error& x )  { hr = _set_excepinfo( x, "Spooler.Variable_set::var" ); }

    return hr;
}

//-----------------------------------------------------------Com_variable_set::estimated_byte_count

static const int memory_overhead = 8;
inline int aligned(int i) { return (i + 7) & ~7; }
int bstr_bytes(BSTR b) { return memory_overhead + aligned(4 + SysStringLen(b) + 1); }

int Com_variable_set::estimated_byte_count() const 
{
    int result = 0;

    Z_FOR_EACH_CONST (Map, _map, it) {
        result += bstr_bytes(it->first);
        if (const Com_variable* v = it->second) {
            result += bstr_bytes(v->_name);
            if (v->_value.vt == VT_BSTR)  result += bstr_bytes(V_BSTR(&v->_value)) + 1;
        }
    }

    return result;
}

//----------------------------------------------------------------------Com_variable_set::get_count

STDMETHODIMP Com_variable_set::get_Count( int* result )
{
    *result = count();
    return NOERROR;
}

//------------------------------------------------------------------------Com_variable_set::get_dom

STDMETHODIMP Com_variable_set::get_Dom( IXMLDOMDocument** doc )  
{ 
#   ifdef SPOOLER_HAS_MSXML
        
        HRESULT hr = NOERROR;

        try
        {
            *doc = dom()._ptr; 
            (*doc)->AddRef();
        }
        catch( const exception&  x )  { hr = _set_excepinfo( x, "Spooler.Variable_set::dom" ); }
        catch( const _com_error& x )  { hr = _set_excepinfo( x, "Spooler.Variable_set::dom" ); }

#    else

        __assume( doc );
        return E_NOTIMPL;

#   endif
}

//----------------------------------------------------------------------------Com_variable_set::dom

xml::Document_ptr Com_variable_set::dom( const string& element_name, const string& subelement_name )
{
    xml::Document_ptr doc;
    
    doc.create();
  //doc.appendChild( doc.createProcessingInstruction( "xml", "version=\"1.0\"" ) );
  //doc.appendChild( dom_element( doc, xml_element_name(), "variable" ) );
    doc.appendChild( dom_element( doc, element_name, subelement_name ) );

    return doc;
}

//--------------------------------------------------------------------Com_variable_set::dom_element

xml::Element_ptr Com_variable_set::dom_element( const xml::Document_ptr& doc, const string& element_name, const string& subelement_name )
{
    xml::Element_ptr result = doc.createElement( element_name );
    result.setAttribute("count",count());
    //result.setAttribute("estimated_byte_count",estimated_byte_count());

    for( Map::iterator it = _map.begin(); it != _map.end(); it++ )
    {
        Com_variable* v = it->second;
        if( v )
        {
            xml::Element_ptr variable_element = v->dom_element( doc, subelement_name );
            result.appendChild( variable_element );
        }
    }

    return result;
}

//----------------------------------------------------------Com_variable_set::to_environment_string

string Com_variable_set::to_environment_string() const
{
    size_t size = 0;

    Z_FOR_EACH_CONST( Map, _map, m )
        size += m->second->_name.length() + 1 + string_from_variant( m->second->_value ).length() + 1;      // Funktioniert nicht, wenn wir auf UTF-8 umstellen.

    string result;
    result.reserve( size );

    Z_FOR_EACH_CONST( Map, _map, m )
    {
        result += string_from_bstr( m->second->_name );
        result += '=';
        result += string_from_variant( m->second->_value );
        result += '\0';
    }

    assert( result.length() == size );
    return result;
}

//-------------------------------------------------------------Com_variable_set::to_xslt_parameters

void Com_variable_set::to_xslt_parameters( xml::Xslt_parameters* result, Has_log* warning_log )
{
    result->allocate( int_cast(_map.size()) );

    for( Map::iterator it = _map.begin(); it != _map.end(); it++ )
    {
        if( Com_variable* v = it->second )
        {
            string name  = string_from_bstr   ( v->_name );
            string value = string_from_variant( v->_value );

            try {
                result->set_string(name, value);
            }
            catch( exception& x )
            {
                if( !warning_log )  throw;
                warning_log->warn( x.what() );
            }
        }
    }
}

//--------------------------------------------------------------------------Com_variable_set::Clone

STDMETHODIMP Com_variable_set::Clone( Ivariable_set** result )
{
    HRESULT hr = NOERROR;

    try
    {
        *result = NULL;

        ptr<Com_variable_set> clone = new Com_variable_set( *this );

        *result = clone;
        (*result)->AddRef();
    }
    catch( const exception&  x )  { hr = _set_excepinfo( x, "Spooler.Variable_set::Clone" ); }
    catch( const _com_error& x )  { hr = _set_excepinfo( x, "Spooler.Variable_set::Clone" ); }

    return hr;
}

//--------------------------------------------------------------------------Com_variable_set::clone

ptr<Com_variable_set> Com_variable_set::clone()
{
    return new Com_variable_set( *this );
}

//--------------------------------------------------------------------------Com_variable_set::Merge

STDMETHODIMP Com_variable_set::Merge( Ivariable_set* other )
{
    HRESULT hr = NOERROR;

    try
    {
        Com_variable_set* o = dynamic_cast<Com_variable_set*>( other );
        if( !o )  return E_POINTER;

        for( Map::iterator it = o->_map.begin(); it != o->_map.end(); it++ )
        {
            if( it->second )
            {
                Com_variable*   v;
                ptr<Ivariable>  clone;

                hr = it->second->Clone( clone.pp() );

                v = static_cast<Com_variable*>( +clone );

                Bstr name = v->_name;
                if( _ignore_case )  name.to_lower();
                _map[ name ] = v;
            }
        }
    }
    catch( const exception&  x )  { hr = _set_excepinfo( x, "Spooler.Variable_set::merge" ); }
    catch( const _com_error& x )  { hr = _set_excepinfo( x, "Spooler.Variable_set::merge" ); }

    return hr;
}

//-------------------------------------------------------------------Com_variable_set::get__NewEnum

STDMETHODIMP Com_variable_set::get__NewEnum( IUnknown** iunknown )
{
    ptr<Com_variable_set_enumerator> e = new Com_variable_set_enumerator;
    e->initialize( this );

    *iunknown = e;
    (*iunknown)->AddRef();
    return NOERROR;                                            
}

//-----------------------------------------------------------------Com_variable_set::set_xml_string

void Com_variable_set::set_xml_string( const string& xml_text )  
{ 
    HRESULT hr = put_Xml( Bstr( xml_text ) );
    if( FAILED(hr) )  throw_ole( hr, "Variable_set::xml" );
}

//------------------------------------------------------------------------Com_variable_set::put_Xml

STDMETHODIMP Com_variable_set::put_Xml( BSTR xml_text )  
{ 
    HRESULT hr = NOERROR;

    try
    {
        xml::Document_ptr doc; // = dom( "params", "param" );
        doc.load_xml_string(xml_text);

        DOM_FOR_EACH_ELEMENT( doc.documentElement(), e )
        {
            if( e.nodeName_is( "variable" ) || e.nodeName_is( "param" ) )
            {
                string name;
                string value;

                get_variable_name_and_value( e, &name, &value );

                VARTYPE vt        = e.int_getAttribute( "vt", VT_BSTR );
                Bstr    name_bstr = name;
                Variant value_v   = value;

                value_v.change_type( vt );

                hr = put_Var( name_bstr, &value_v );
                if( FAILED( hr ) )  break;
            }
            else
                z::throw_xc( "SCHEDULER-409", "param", e.nodeName() );
        }

    }
    catch( const exception&  x )  { hr = _set_excepinfo( x, Z_FUNCTION ); }
    catch( const _com_error& x )  { hr = _set_excepinfo( x, Z_FUNCTION ); }

    return hr;
}

//------------------------------------------------------------------------Com_variable_set::get_xml

STDMETHODIMP Com_variable_set::get_Xml( BSTR* xml_doc  )  
{ 
    HRESULT hr = NOERROR;

    try
    {
        hr = String_to_bstr(dom(xml_element_name(), "variable").xml_string(), xml_doc);
    }
    catch( const exception&  x )  { hr = _set_excepinfo( x, Z_FUNCTION ); }
    catch( const _com_error& x )  { hr = _set_excepinfo( x, Z_FUNCTION ); }

    return hr;
}

//----------------------------------------------------------------------Com_variable_set::get_Names

STDMETHODIMP Com_variable_set::get_Names( BSTR* result )
{
    HRESULT hr = NOERROR;

    try
    {
        int length = int_cast(_map.size()) - 1;  // Anzahl der Semikolons
        
        if( length < 0 )
        {
            *result = NULL;
        }
        else
        {
            Z_FOR_EACH( Map, _map, m )  length += m->first.length();
            
            OLECHAR* p = *result = SysAllocStringLen( NULL, length );
            if( !*result )  return E_OUTOFMEMORY;

            Z_FOR_EACH( Map, _map, m )
            {
                //memcpy( p, m->first, m->first.length() * sizeof (OLECHAR) );
                if( m->second )
                {
                    memcpy( p, m->second->_name, m->second->_name.length() * sizeof (OLECHAR) );
                    p += m->first.length();
                    *p++ = ';';
                }
            }

            p[ -1 ] = '\0';      // Letztes Semikolon überschreiben, ist schon außerhalb des Strings
            assert( p == *result + length + 1 );
        }
    }
    catch( const exception&  x )  { hr = _set_excepinfo( x, Z_FUNCTION ); }
    catch( const _com_error& x )  { hr = _set_excepinfo( x, Z_FUNCTION ); }

    return hr;
}

//---------------------------------------------------------------------Com_variable_set::Substitute

STDMETHODIMP Com_variable_set::Substitute( BSTR bstr, BSTR* result )
{
    HRESULT hr = NOERROR;

    try
    {
        hr = String_to_bstr( subst_env( string_from_bstr( bstr ), this ), result );
    }
    catch( const exception&  x )  { hr = _set_excepinfo( x, Z_FUNCTION ); }

    return hr;
}

//---------------------------------------------------------------------------------------operator +

ptr<Com_variable_set> operator+( const ptr<Com_variable_set>& a, Com_variable_set* b )
{
    ptr<Com_variable_set> result = a->clone();
    result->merge( b );
    return result;
}

//------------------------------------------------------------Com_variable_set_enumerator::_methods
#ifdef Z_COM
/*
const Com_method Com_variable_set_enumerator::_methods[] =
{ 
   // _flags         , dispid, _name    , _method                                               , _result_type  , _types        , _default_arg_count
    { DISPATCH_METHOD     , 1, "Next"   , (Com_method_ptr)&Com_variable_set_enumerator::Next    , VT_LONG       , { VT_LONG, VT_BYREF|VT_VARIANT } },
    { DISPATCH_METHOD     , 2, "Skip"   , (Com_method_ptr)&Com_variable_set_enumerator::Skip    , VT_EMPTY      , { VT_LONG } },
    { DISPATCH_METHOD     , 3, "Reset"  , (Com_method_ptr)&Com_variable_set_enumerator::Reset   },
    { DISPATCH_METHOD     , 4, "Clone"  , (Com_method_ptr)&Com_variable_set_enumerator::Clone   , VT_DISPATCH  },
    {}
};
*/
#endif
//-----------------------------------------Com_variable_set_enumerator::Com_variable_set_enumerator

Com_variable_set_enumerator::Com_variable_set_enumerator()
:
    Sos_ole_object( variable_set_enumerator_class_ptr, this )
{
}

//------------------------------------------------------Com_variable_set_enumerator::QueryInterface

STDMETHODIMP Com_variable_set_enumerator::QueryInterface( REFIID iid, void** result )
{                                                                    
    Z_IMPLEMENT_QUERY_INTERFACE( this, iid, IEnumVARIANT            , result );
    Z_IMPLEMENT_QUERY_INTERFACE( this, iid, Ivariable_set_enumerator, result );

/*
    if( iid == IID_IEnumVARIANT )
    {
        *result = (IEnumVARIANT*)this;
        (*result)->AddRef();
        return S_OK;
    }

    if( iid == IID_Ivariable_set_enumerator )
    {
        *result = (Ivariable_set_enumerator)this;
        (*result)->AddRef();
        return S_OK;
    }
*/
    return Sos_ole_object::QueryInterface( iid, result );
}                                                                                                                                       

//----------------------------------------------------------Com_variable_set_enumerator::initialize

void Com_variable_set_enumerator::initialize( Com_variable_set* v )
{
    _variable_set = v;
    Reset();
}

//----------------------------------------------------------------Com_variable_set_enumerator::Next

STDMETHODIMP Com_variable_set_enumerator::Next( ulong32 celt, VARIANT* result, ulong32* pceltFetched )
{
    int i = 0;

    while( i < celt  &&  _iterator != _variable_set->_map.end() )
    {
        VariantInit( result );

        if( _iterator->second )
        {
            Com_variable* v = NULL;
            _iterator->second.copy_to( &v );
            result->vt = VT_DISPATCH;
            result->pdispVal = v;
        }

        _iterator++;
        result++;
        i++;
    }

    if( pceltFetched )  *pceltFetched = i;

    return i < celt? S_FALSE : S_OK;
}

//----------------------------------------------------------------Com_variable_set_enumerator::Skip

STDMETHODIMP Com_variable_set_enumerator::Skip( ulong32 celt )
{
    while( celt &&  _iterator != _variable_set->_map.end() )  _iterator++;

    return celt? S_FALSE : S_OK;
}

//---------------------------------------------------------------Com_variable_set_enumerator::Reset

STDMETHODIMP Com_variable_set_enumerator::Reset()
{
    _iterator = _variable_set->_map.begin();
    return NOERROR;
}

//---------------------------------------------------------------Com_variable_set_enumerator::Clone

STDMETHODIMP Com_variable_set_enumerator::Clone( IEnumVARIANT** )
{
    return ERROR;
}

//--------------------------------------------------------------------------------Com_log::_methods
#ifdef Z_COM

const Com_method Com_log::_methods[] =
{ 
   // _flags         , dispid, _name                   , _method                                           , _result_type  , _types        , _default_arg_count
    { DISPATCH_METHOD     ,  1, "debug9"               , (Com_method_ptr)&Com_log::Debug9                  , VT_EMPTY      , { VT_BSTR } },
    { DISPATCH_METHOD     ,  2, "debug8"               , (Com_method_ptr)&Com_log::Debug8                  , VT_EMPTY      , { VT_BSTR } },
    { DISPATCH_METHOD     ,  3, "debug7"               , (Com_method_ptr)&Com_log::Debug7                  , VT_EMPTY      , { VT_BSTR } },
    { DISPATCH_METHOD     ,  4, "debug6"               , (Com_method_ptr)&Com_log::Debug6                  , VT_EMPTY      , { VT_BSTR } },
    { DISPATCH_METHOD     ,  5, "debug5"               , (Com_method_ptr)&Com_log::Debug5                  , VT_EMPTY      , { VT_BSTR } },
    { DISPATCH_METHOD     ,  6, "debug4"               , (Com_method_ptr)&Com_log::Debug4                  , VT_EMPTY      , { VT_BSTR } },
    { DISPATCH_METHOD     ,  7, "debug3"               , (Com_method_ptr)&Com_log::Debug3                  , VT_EMPTY      , { VT_BSTR } },
    { DISPATCH_METHOD     ,  8, "debug2"               , (Com_method_ptr)&Com_log::Debug2                  , VT_EMPTY      , { VT_BSTR } },
    { DISPATCH_METHOD     ,  9, "debug1"               , (Com_method_ptr)&Com_log::Debug1                  , VT_EMPTY      , { VT_BSTR } },
    { DISPATCH_METHOD     , 10, "debug"                , (Com_method_ptr)&Com_log::Debug                   , VT_EMPTY      , { VT_BSTR } },
    { DISPATCH_METHOD     ,  0, "info"                 , (Com_method_ptr)&Com_log::Info                    , VT_EMPTY      , { VT_BSTR } },
    { DISPATCH_METHOD     , 11, "msg"                  , (Com_method_ptr)&Com_log::Msg                     , VT_EMPTY      , { VT_BSTR } },
    { DISPATCH_METHOD     , 12, "warn"                 , (Com_method_ptr)&Com_log::Warn                    , VT_EMPTY      , { VT_BSTR } },
    { DISPATCH_METHOD     , 13, "error"                , (Com_method_ptr)&Com_log::Error                   , VT_EMPTY      , { VT_BSTR } },
    { DISPATCH_METHOD     , 14, "log"                  , (Com_method_ptr)&Com_log::Log                     , VT_EMPTY      , { VT_I4, VT_BSTR } },
    { DISPATCH_PROPERTYGET, 15, "mail"                 , (Com_method_ptr)&Com_log::get_Mail                , VT_DISPATCH  },
    { DISPATCH_PROPERTYPUT, 16, "mail_on_error"        , (Com_method_ptr)&Com_log::put_Mail_on_error       , VT_EMPTY      , { VT_BOOL } },
    { DISPATCH_PROPERTYGET, 16, "mail_on_error"        , (Com_method_ptr)&Com_log::get_Mail_on_error       , VT_BOOL       },
    { DISPATCH_PROPERTYPUT, 17, "mail_on_success"      , (Com_method_ptr)&Com_log::put_Mail_on_success     , VT_EMPTY      , { VT_BOOL } },
    { DISPATCH_PROPERTYGET, 17, "mail_on_success"      , (Com_method_ptr)&Com_log::get_Mail_on_success     , VT_BOOL       },
    { DISPATCH_PROPERTYPUT, 18, "mail_on_process"      , (Com_method_ptr)&Com_log::put_Mail_on_process     , VT_EMPTY      , { VT_I4 } },
    { DISPATCH_PROPERTYGET, 18, "mail_on_process"      , (Com_method_ptr)&Com_log::get_Mail_on_process     , VT_I4         },
    { DISPATCH_PROPERTYPUT, 19, "level"                , (Com_method_ptr)&Com_log::put_Level               , VT_EMPTY      , { VT_I4 } },
    { DISPATCH_PROPERTYGET, 19, "level"                , (Com_method_ptr)&Com_log::get_Level               , VT_I4         },
    { DISPATCH_PROPERTYGET, 20, "filename"             , (Com_method_ptr)&Com_log::get_Filename            , VT_BSTR       },
    { DISPATCH_PROPERTYPUT, 21, "collect_within"       , (Com_method_ptr)&Com_log::put_Collect_within      , VT_EMPTY      , { VT_BYREF|VT_VARIANT } },
    { DISPATCH_PROPERTYGET, 21, "collect_within"       , (Com_method_ptr)&Com_log::get_Collect_within      , VT_R8         },
    { DISPATCH_PROPERTYPUT, 22, "collect_max"          , (Com_method_ptr)&Com_log::put_Collect_max         , VT_EMPTY      , { VT_BYREF|VT_VARIANT } },
    { DISPATCH_PROPERTYGET, 22, "collect_max"          , (Com_method_ptr)&Com_log::get_Collect_max         , VT_R8         },
    { DISPATCH_PROPERTYPUT, 23, "mail_it"              , (Com_method_ptr)&Com_log::put_Mail_it             , VT_EMPTY      , { VT_BOOL } },
    { DISPATCH_PROPERTYGET, 24, "java_class_name"      , (Com_method_ptr)&Com_log::get_Java_class_name     , VT_BSTR },
    { DISPATCH_PROPERTYGET, 25, "last_error_line"      , (Com_method_ptr)&Com_log::get_Last_error_line     , VT_BSTR },
    { DISPATCH_PROPERTYGET, 26, "last"                 , (Com_method_ptr)&Com_log::get_Last                , VT_BSTR       , { VT_BYREF|VT_VARIANT } },
    { DISPATCH_PROPERTYPUT, 27, "mail_on_warning"      , (Com_method_ptr)&Com_log::put_Mail_on_warning     , VT_EMPTY      , { VT_BOOL } },
    { DISPATCH_PROPERTYGET, 27, "mail_on_warning"      , (Com_method_ptr)&Com_log::get_Mail_on_warning     , VT_BOOL       },
    { DISPATCH_METHOD     , 28, "Start_new_file"       , (Com_method_ptr)&Com_log::Start_new_file          , VT_EMPTY      },
    { DISPATCH_METHOD     , 29, "Log_file"             , (Com_method_ptr)&Com_log::Log_file                , VT_EMPTY      , { VT_BSTR } },
    { DISPATCH_PROPERTYPUT, 30, "New_filename"         , (Com_method_ptr)&Com_log::put_New_filename        , VT_EMPTY      , { VT_BSTR } },
    { DISPATCH_PROPERTYGET, 30, "New_filename"         , (Com_method_ptr)&Com_log::get_New_filename        , VT_BSTR       },
    {}
};

#endif
//---------------------------------------------------------------------------------Com_log::Com_log

Com_log::Com_log( Prefix_log* log )
:
    Sos_ole_object( log_class_ptr, (Ilog*)this ),
    _zero_(this+1),
    _log(log)
{ 
}

//--------------------------------------------------------------------------Com_log::QueryInterface

STDMETHODIMP Com_log::QueryInterface( const IID& iid, void** result )
{
    Z_IMPLEMENT_QUERY_INTERFACE( this, iid, Ihas_java_class_name          , result );
    Z_IMPLEMENT_QUERY_INTERFACE( this, iid, Ihas_reference_with_properties, result );

    return Sos_ole_object::QueryInterface( iid, result );
}

//-----------------------------------------------------------Com_log::get_reference_with_properties

ptr<object_server::Reference_with_properties> Com_log::get_reference_with_properties()   //( const IID& iid )
{
    ptr<object_server::Reference_with_properties> result;

    //if( iid != IID_Ilog )  return E_NOINTERFACE;

    
    {
        if( !_log )  throw_com( E_POINTER, "Com_log::get_reference_with_properties" );

        //*result = _log->get_reference_with_properties();
        result = Z_NEW( object_server::Reference_with_properties( CLSID_Com_log_proxy, (Ilog*)this ) );  // IDispatch* wird von Com_log eingesetzt.

        result->set_property( "level", _log->log_level() );
    }

    return result;
}

//---------------------------------------------------------------------------------Com_log::set_log

void Com_log::set_log( Prefix_log* log )
{ 
    _log = log; 
}

//----------------------------------------------------------------------------------------Com_log::
    
STDMETHODIMP Com_log::Debug9( BSTR line )                       { return Log( spooler_com::log_debug9, line ); }
STDMETHODIMP Com_log::Debug8( BSTR line )                       { return Log( spooler_com::log_debug8, line ); }
STDMETHODIMP Com_log::Debug7( BSTR line )                       { return Log( spooler_com::log_debug7, line ); }
STDMETHODIMP Com_log::Debug6( BSTR line )                       { return Log( spooler_com::log_debug6, line ); }
STDMETHODIMP Com_log::Debug5( BSTR line )                       { return Log( spooler_com::log_debug5, line ); }
STDMETHODIMP Com_log::Debug4( BSTR line )                       { return Log( spooler_com::log_debug4, line ); }
STDMETHODIMP Com_log::Debug3( BSTR line )                       { return Log( spooler_com::log_debug3, line ); }
STDMETHODIMP Com_log::Debug2( BSTR line )                       { return Log( spooler_com::log_debug2, line ); }
STDMETHODIMP Com_log::Debug1( BSTR line )                       { return Log( spooler_com::log_debug1, line ); }
STDMETHODIMP Com_log::Debug ( BSTR line )                       { return Log( spooler_com::log_debug1, line ); }
STDMETHODIMP Com_log::Msg   ( BSTR line )                       { return Log( spooler_com::log_info  , line ); }
STDMETHODIMP Com_log::Info  ( BSTR line )                       { return Log( spooler_com::log_info  , line ); }
STDMETHODIMP Com_log::Warn  ( BSTR line )                       { return Log( spooler_com::log_warn  , line ); }
STDMETHODIMP Com_log::Error ( BSTR line )                       { return Log( spooler_com::log_error , line ); }

//-------------------------------------------------------------------------------------Com_log::log

STDMETHODIMP Com_log::Log( spooler_com::Log_level level, BSTR line )
{ 
    HRESULT hr = NOERROR;

    try 
    {
        if( !_log )  return E_POINTER;

        _log->log( (zschimmer::Log_level)level, bstr_as_string( line ) ); 
    }
    catch( const exception&  x )  { hr = _set_excepinfo( x, "Spooler.Log::log" ); }
    catch( const _com_error& x )  { hr = _set_excepinfo( x, "Spooler.Log::log" ); }

    return hr;
}

//--------------------------------------------------------------------------------Com_log::Log_file

STDMETHODIMP Com_log::Log_file( BSTR path )
{ 
    HRESULT hr = NOERROR;

    try 
    {
        if( !_log )  return E_POINTER;

        _log->log_file( bstr_as_string( path ) ); 
    }
    catch( const exception&  x )  { hr = _set_excepinfo( x, Z_FUNCTION ); }
    catch( const _com_error& x )  { hr = _set_excepinfo( x, Z_FUNCTION ); }

    return hr;
}

//--------------------------------------------------------------------------------Com_log::get_mail

STDMETHODIMP Com_log::get_Mail( Imail** mail )
{ 
    HRESULT hr = NOERROR;

    try 
    {
        if( !_log )  return E_POINTER;

        *mail = _log->imail();
        if( *mail )  (*mail)->AddRef();
    }
    catch( const exception&  x )  { hr = _set_excepinfo( x, "Spooler.Log::mail" ); }
    catch( const _com_error& x )  { hr = _set_excepinfo( x, "Spooler.Log::mail" ); }

    return hr;
}

//-----------------------------------------------------------------------Com_log::put_mail_on_error

STDMETHODIMP Com_log::put_Mail_on_error( VARIANT_BOOL b )
{
    HRESULT hr = NOERROR;

    try 
    {
        if( !_log )  return E_POINTER;

        _log->set_mail_on_error( b != 0 );
    }
    catch( const exception&  x )  { hr = _set_excepinfo( x, "Spooler.Log::mail_on_error" ); }
    catch( const _com_error& x )  { hr = _set_excepinfo( x, "Spooler.Log::mail_on_error" ); }

    return hr;
}

//-----------------------------------------------------------------------Com_log::get_mail_on_error

STDMETHODIMP Com_log::get_Mail_on_error( VARIANT_BOOL* b )
{
    HRESULT hr = NOERROR;

    try 
    {
        if( !_log )  return E_POINTER;

        *b = _log->mail_on_error();
    }
    catch( const exception&  x )  { hr = _set_excepinfo( x, "Spooler.Log::mail_on_error" ); }
    catch( const _com_error& x )  { hr = _set_excepinfo( x, "Spooler.Log::mail_on_error" ); }

    return hr;
}

//---------------------------------------------------------------------Com_log::put_Mail_on_warning

STDMETHODIMP Com_log::put_Mail_on_warning( VARIANT_BOOL b )
{
    HRESULT hr = NOERROR;

    try 
    {
        if( !_log )  return E_POINTER;

        _log->set_mail_on_warning( b != 0 );
    }
    catch( const exception&  x )  { hr = _set_excepinfo( x, Z_FUNCTION ); }
    catch( const _com_error& x )  { hr = _set_excepinfo( x, Z_FUNCTION ); }

    return hr;
}

//---------------------------------------------------------------------Com_log::get_Mail_on_warning

STDMETHODIMP Com_log::get_Mail_on_warning( VARIANT_BOOL* b )
{
    HRESULT hr = NOERROR;

    try 
    {
        if( !_log )  return E_POINTER;

        *b = _log->mail_on_warning();
    }
    catch( const exception&  x )  { hr = _set_excepinfo( x, Z_FUNCTION ); }
    catch( const _com_error& x )  { hr = _set_excepinfo( x, Z_FUNCTION ); }

    return hr;
}
    
//----------------------------------------------------------------------Com_log::put_mail_on_success

STDMETHODIMP Com_log::put_Mail_on_success( VARIANT_BOOL b )
{
    HRESULT hr = NOERROR;

    try 
    {
        if( !_log )  return E_POINTER;

        _log->set_mail_on_success( b != 0 );
    }
    catch( const exception&  x )  { hr = _set_excepinfo( x, "Spooler.Log::mail_on_success" ); }
    catch( const _com_error& x )  { hr = _set_excepinfo( x, "Spooler.Log::mail_on_success" ); }

    return hr;
}

//---------------------------------------------------------------------Com_log::get_mail_on_success

STDMETHODIMP Com_log::get_Mail_on_success( VARIANT_BOOL* b )
{
    HRESULT hr = NOERROR;

    try 
    {
        if( !_log )  return E_POINTER;

        *b = _log->mail_on_success();
    }
    catch( const exception&  x )  { hr = _set_excepinfo( x, "Spooler.Log::mail_on_success" ); }
    catch( const _com_error& x )  { hr = _set_excepinfo( x, "Spooler.Log::mail_on_success" ); }

    return hr;
}

//----------------------------------------------------------------------Com_log::put_mail_on_process

STDMETHODIMP Com_log::put_Mail_on_process( int level )
{
    HRESULT hr = NOERROR;

    try 
    {
        if( !_log )  return E_POINTER;

        _log->set_mail_on_process( level == -1? 1 : level );        // True (-1) -> 1
    }
    catch( const exception&  x )  { hr = _set_excepinfo( x, "Spooler.Log::mail_on_process" ); }
    catch( const _com_error& x )  { hr = _set_excepinfo( x, "Spooler.Log::mail_on_process" ); }

    return hr;
}

//---------------------------------------------------------------------Com_log::get_mail_on_process

STDMETHODIMP Com_log::get_Mail_on_process( int* result )
{
    HRESULT hr = NOERROR;

    try 
    {
        if( !_log )  return E_POINTER;

        *result = _log->mail_on_process();
    }
    catch( const exception&  x )  { hr = _set_excepinfo( x, "Spooler.Log::mail_on_process" ); }
    catch( const _com_error& x )  { hr = _set_excepinfo( x, "Spooler.Log::mail_on_process" ); }

    return hr;
}

//-----------------------------------------------------------------------------------Com_log::level

STDMETHODIMP Com_log::put_Level( int level )
{
    HRESULT hr = NOERROR;

    try 
    {
        if( !_log )  return E_POINTER;

        _log->set_log_level( (zschimmer::Log_level)level );
    }
    catch( const exception&  x )  { hr = _set_excepinfo( x, "Spooler.Log::log_level" ); }
    catch( const _com_error& x )  { hr = _set_excepinfo( x, "Spooler.Log::log_level" ); }

    return hr;
}

//-----------------------------------------------------------------------------------Com_log::level

STDMETHODIMP Com_log::get_Level( int* level )
{
    HRESULT hr = NOERROR;

    try 
    {
        if( !_log )  return E_POINTER;

        *level = _log->log_level();
    }
    catch( const exception&  x )  { hr = _set_excepinfo( x, "Spooler.Log::log_level" ); }
    catch( const _com_error& x )  { hr = _set_excepinfo( x, "Spooler.Log::log_level" ); }

    return hr;
}

//--------------------------------------------------------------------------------Com_log::filename

STDMETHODIMP Com_log::get_Filename( BSTR* filename_bstr )
{
    HRESULT hr = NOERROR;

    try 
    {
        if( !_log )  return E_POINTER;

        *filename_bstr = SysAllocString_string( _log->filename() );
    }
    catch( const exception&  x )  { hr = _set_excepinfo( x, "Spooler.Log::filename" ); }
    catch( const _com_error& x )  { hr = _set_excepinfo( x, "Spooler.Log::filename" ); }

    return hr;
}

//----------------------------------------------------------------------------Com_log::new_filename

STDMETHODIMP Com_log::put_New_filename( BSTR filename_bstr )
{
    HRESULT hr = NOERROR;

    try 
    {
        if( !_log )  return E_POINTER;
        _log->set_new_filename( bstr_as_string( filename_bstr ) );
    }
    catch( const exception&  x )  { hr = _set_excepinfo( x, "Spooler.Log::new_filename" ); }
    catch( const _com_error& x )  { hr = _set_excepinfo( x, "Spooler.Log::new_filename" ); }

    return hr;
}

//----------------------------------------------------------------------------Com_log::new_filename

STDMETHODIMP Com_log::get_New_filename( BSTR* filename_bstr )
{
    HRESULT hr = NOERROR;

    try 
    {
        if( !_log )  return E_POINTER;

        *filename_bstr = SysAllocString_string( _log->new_filename() );
    }
    catch( const exception&  x )  { hr = _set_excepinfo( x, "Spooler.Log::new_filename" ); }
    catch( const _com_error& x )  { hr = _set_excepinfo( x, "Spooler.Log::new_filename" ); }

    return hr;
}

//--------------------------------------------------------------------------Com_log::collect_within

STDMETHODIMP Com_log::put_Collect_within( VARIANT* time )
{
    HRESULT hr = NOERROR;

    try 
    {
        if( !_log )  return E_POINTER;

        _log->set_collect_within( duration_from_variant(*time) );
    }
    catch( const exception&  x )  { hr = _set_excepinfo( x, "Spooler.Log::collect_within" ); }
    catch( const _com_error& x )  { hr = _set_excepinfo( x, "Spooler.Log::collect_within" ); }

    return hr;
}

//--------------------------------------------------------------------------Com_log::collect_within

STDMETHODIMP Com_log::get_Collect_within( double* result )
{
    HRESULT hr = NOERROR;

    try 
    {
        if( !_log )  return E_POINTER;

        *result = _log->collect_within().as_double();
    }
    catch( const exception&  x )  { hr = _set_excepinfo( x, "Spooler.Log::collect_within" ); }
    catch( const _com_error& x )  { hr = _set_excepinfo( x, "Spooler.Log::collect_within" ); }

    return hr;
}

//-----------------------------------------------------------------------------Com_log::collect_max

STDMETHODIMP Com_log::put_Collect_max( VARIANT* time )
{
    HRESULT hr = NOERROR;

    try 
    {
        if( !_log )  return E_POINTER;

        _log->set_collect_max( duration_from_variant(*time) );
    }
    catch( const exception&  x )  { hr = _set_excepinfo( x, "Spooler.Log::collect_max" ); }
    catch( const _com_error& x )  { hr = _set_excepinfo( x, "Spooler.Log::collect_max" ); }

    return hr;
}

//-----------------------------------------------------------------------------Com_log::collect_max

STDMETHODIMP Com_log::get_Collect_max( double* result )
{
    HRESULT hr = NOERROR;

    try 
    {
        if( !_log )  return E_POINTER;

        *result = _log->collect_max().as_double();
    }
    catch( const exception&  x )  { hr = _set_excepinfo( x, "Spooler.Log::collect_max" ); }
    catch( const _com_error& x )  { hr = _set_excepinfo( x, "Spooler.Log::collect_max" ); }

    return hr;
}

//-----------------------------------------------------------------------------Com_log::put_mail_it

STDMETHODIMP Com_log::put_Mail_it( VARIANT_BOOL b )
{
    HRESULT hr = NOERROR;

    try 
    {
        if( !_log )  return E_POINTER;

        _log->set_mail_it( b != 0 );
    }
    catch( const exception&  x )  { hr = _set_excepinfo( x, "Spooler.Log::mail_it" ); }
    catch( const _com_error& x )  { hr = _set_excepinfo( x, "Spooler.Log::mail_it" ); }

    return hr;
}

//---------------------------------------------------------------------Com_log::get_last_error_line

STDMETHODIMP Com_log::get_Last_error_line( BSTR* result )
{
    HRESULT hr = NOERROR;

    try 
    {
        if( !_log )  return E_POINTER;

        hr = String_to_bstr( _log->last_error_line(), result );
    }
    catch( const exception&  x )  { hr = _set_excepinfo( x, "Spooler.Log::last_error_line" ); }
    catch( const _com_error& x )  { hr = _set_excepinfo( x, "Spooler.Log::last_error_line" ); }

    return hr;
}

//--------------------------------------------------------------------------------Com_log::get_Last

STDMETHODIMP Com_log::get_Last( VARIANT* level, BSTR* result )
{
    HRESULT hr = NOERROR;

    try 
    {
        if( !_log )  return E_POINTER;

        hr = String_to_bstr( _log->last( make_log_level( string_from_variant( *level ) ) ), result );
    }
    catch( const exception&  x )  { hr = _set_excepinfo( x, "Spooler.Log::last_error_line" ); }
    catch( const _com_error& x )  { hr = _set_excepinfo( x, "Spooler.Log::last_error_line" ); }

    return hr;
}

//--------------------------------------------------------------------------Com_log::Start_new_file

STDMETHODIMP Com_log::Start_new_file()
{
    HRESULT hr = NOERROR;

    try 
    {
        if( !_log )  return E_POINTER;

        _log->start_new_file();
    }
    catch( const exception&  x )  { hr = _set_excepinfo( x, Z_FUNCTION ); }
    catch( const _com_error& x )  { hr = _set_excepinfo( x, Z_FUNCTION ); }

    return hr;
}

//-------------------------------------------------------------------Com_log_proxy::Create_instance

HRESULT Com_log_proxy::Create_instance( zschimmer::com::object_server::Session*, ptr<Object>*, const IID& iid, ptr<IUnknown>* result )
{
    if( iid == object_server::IID_Iproxy )
    {
        ptr<Com_log_proxy> instance = Z_NEW( Com_log_proxy );
        *result = static_cast<IDispatch*>( +instance );
        return S_OK;
    }

    return E_NOINTERFACE;
}

//----------------------------------------------------------------------Com_log_proxy::set_property

void Com_log_proxy::set_property( const string& name, const Variant& value )
{
    if( name == "level" )  _level = value.as_int();
    else  
        z::throw_xc( "Com_log_proxy::set_property", name );
}

//---------------------------------------------------------------------Com_log_proxy::GetIDsOfNames
/*
STDMETHODIMP Com_log_proxy::GetIDsOfNames( const IID& iid, OLECHAR** rgszNames, UINT cNames, LCID lcid, DISPID* dispid )
{
    HRESULT hr;
    
    hr = com_get_dispid( _methods, iid, rszNames, cNames, lcid, dispid );       // Erst lokal versuchen

    if( hr == DISP_E_UNKNOWNNAME )
    {
        hr = Proxy::GetIDsOfNames( iid, rgszNames, cNames, lcid, dispid );      // Server aufrufen
    }

    return hr;
}
*/
//----------------------------------------------------------------------------Com_log_proxy::Invoke

STDMETHODIMP Com_log_proxy::Invoke( DISPID dispid, const IID& iid, LCID lcid, unsigned short flags, DISPPARAMS* dispparams, 
                                    VARIANT* result, EXCEPINFO* excepinfo, UINT* arg_nr )
{
    const Bstr& name = dispid == 0? "info" : name_from_dispid( dispid );

    if( name == "debug9" )  { if( _level > spooler_com::log_debug9 )  return S_FALSE; }
    else
    if( name == "debug8" )  { if( _level > spooler_com::log_debug8 )  return S_FALSE; }
    else
    if( name == "debug7" )  { if( _level > spooler_com::log_debug7 )  return S_FALSE; }
    else
    if( name == "debug6" )  { if( _level > spooler_com::log_debug6 )  return S_FALSE; }
    else
    if( name == "debug5" )  { if( _level > spooler_com::log_debug5 )  return S_FALSE; }
    else
    if( name == "debug4" )  { if( _level > spooler_com::log_debug4 )  return S_FALSE; }
    else
    if( name == "debug3" )  { if( _level > spooler_com::log_debug3 )  return S_FALSE; }
    else
    if( name == "debug2" )  { if( _level > spooler_com::log_debug2 )  return S_FALSE; }
    else
    if( name == "debug1" )  { if( _level > spooler_com::log_debug1 )  return S_FALSE; }
    else
    if( name == "debug"  )  { if( _level > spooler_com::log_debug  )  return S_FALSE; }
    else                                                                              
    if( name == "info"   )  { if( _level > spooler_com::log_info   )  return S_FALSE; }
    else
    if( name == "log"   )
    {
        if( dispparams->cArgs != 2 )  return DISP_E_BADPARAMCOUNT;
        if( int_from_variant( dispparams->rgvarg[ dispparams->cArgs - 1 ] ) < _level )  return S_FALSE;
    }
    else
    if( name == "level" )
    {
        if( flags & DISPATCH_PROPERTYPUT )
        {
            if( dispparams->cNamedArgs != 1 )  return DISP_E_BADPARAMCOUNT;
            if( dispparams->rgdispidNamedArgs[0] != DISPID_PROPERTYPUT )   return DISP_E_BADPARAMCOUNT;
            _level = int_from_variant( dispparams->rgvarg[0] );
        }
        else 
        {
            if( dispparams->cNamedArgs != 0 )  return DISP_E_BADPARAMCOUNT;

            if( result )  result->vt = VT_I4, V_I4(result) = _level;

            return S_OK;
        }
    }

    return Proxy::Invoke( dispid, iid, lcid, flags, dispparams, result, excepinfo, arg_nr );      // Server aufrufen
}

//-----------------------------------------------------------------------------Com_log_proxy::level
/*
STDMETHODIMP Com_log_proxy::get_level( int* level )
{
    HRESULT hr = NOERROR;

    *level = _level;

    return hr;
}
*/
//-----------------------------------------------------------------------------Com_log_proxy::level
/*
STDMETHODIMP Com_log_proxy::put_level( int level )
{
    HRESULT hr = NOERROR;

    _level = level;

    return hr;
}
*/
//-------------------------------------------------------------------------------Com_log_proxy::log
/*
STDMETHODIMP Com_log_proxy::log( spooler_com::Log_level level, BSTR line )
{ 
    HRESULT hr = NOERROR;

    if( level < _level )  return hr;

    if( !_dispid_valid )
    {
        _object->GetIDsOfName()
        _disp_id_valid = true;
    }

    hr = _object->Invoke();
}
*/
//------------------------------------------------------------------------------Com_log_proxy::log2

void Com_log_proxy::log2( zschimmer::Log_level log_level, const string&, const string& line, Has_log* )
{
    //assert( !prefix_log );
    //assert( prefix == "" );

    if( log_level >= _level )
    {
        UINT       arg_nr   = 0;
        Dispparams parameters;
        Excepinfo  excepinfo;
        Variant    result;

        parameters.set_arg_count( 2 );
        parameters[ 0 ] = (int)log_level;
        parameters[ 1 ] = line;

        HRESULT hr = Proxy::Invoke_from_any_thread( dispid_log_log, IID_NULL, STANDARD_LCID, DISPATCH_METHOD, &parameters, &result, &excepinfo, &arg_nr );

        if( FAILED( hr ) )  throw_com_excepinfo( hr, &excepinfo, Z_FUNCTION, "" );
    }
}

//--------------------------------------------------------------------------------Com_job::_methods
#ifdef Z_COM

const Com_method Com_job::_methods[] =
{ 
   // _flags         , dispid, _name                            , _method                                               , _result_type  , _types        , _default_arg_count
    { DISPATCH_METHOD     ,  1, "start_when_directory_changed"  , (Com_method_ptr)&Com_job::Start_when_directory_changed, VT_EMPTY      , { VT_BSTR, VT_BSTR }, 1 },
    { DISPATCH_METHOD     ,  2, "clear_when_directory_changed"  , (Com_method_ptr)&Com_job::Clear_when_directory_changed },
    { DISPATCH_METHOD     ,  3, "start"                         , (Com_method_ptr)&Com_job::Start                       , VT_DISPATCH   , { VT_BYREF|VT_VARIANT }, 1 },
    { DISPATCH_PROPERTYGET,  5, "include_path"                  , (Com_method_ptr)&Com_job::get_Include_path            , VT_BSTR       },
    { DISPATCH_PROPERTYGET,  6, "name"                          , (Com_method_ptr)&Com_job::get_Name                    , VT_BSTR       },
    { DISPATCH_METHOD     ,  7, "wake"                          , (Com_method_ptr)&Com_job::Wake                        },
    { DISPATCH_PROPERTYPUT,  8, "state_text"                    , (Com_method_ptr)&Com_job::put_State_text              , VT_EMPTY      , { VT_BSTR } },
    { DISPATCH_PROPERTYGET,  9, "title"                         , (Com_method_ptr)&Com_job::get_Title                   , VT_BSTR       },
    { DISPATCH_PROPERTYPUT, 10, "delay_after_error"             , (Com_method_ptr)&Com_job::put_Delay_after_error       , VT_EMPTY      , { VT_I4, VT_BYREF|VT_VARIANT } },
    { DISPATCH_PROPERTYGET, 11, "order_queue"                   , (Com_method_ptr)&Com_job::get_Order_queue             , VT_DISPATCH   },
    { DISPATCH_PROPERTYGET, 12, "java_class_name"               , (Com_method_ptr)&Com_job::get_Java_class_name         , VT_BSTR },
    { DISPATCH_PROPERTYPUT, 13, "delay_order_after_setback"     , (Com_method_ptr)&Com_job::put_Delay_order_after_setback,VT_EMPTY      , { VT_I4, VT_BYREF|VT_VARIANT } },
    { DISPATCH_PROPERTYPUT, 14, "max_order_setbacks"            , (Com_method_ptr)&Com_job::put_Max_order_setbacks      , VT_EMPTY      , { VT_I4 } },
    { DISPATCH_PROPERTYGET, 14, "max_order_setbacks"            , (Com_method_ptr)&Com_job::get_Max_order_setbacks      , VT_INT        },
    { DISPATCH_METHOD     , 15, "clear_delay_after_error"       , (Com_method_ptr)&Com_job::Clear_delay_after_error     , VT_EMPTY      },
    { DISPATCH_METHOD     , 16, "Remove"                        , (Com_method_ptr)&Com_job::Remove                      , VT_EMPTY      },
    { DISPATCH_METHOD     , 17, "Execute_command"               , (Com_method_ptr)&Com_job::Execute_command             , VT_EMPTY      , { VT_BSTR } },
    { DISPATCH_PROPERTYGET, 19, "Process_class"                 , (Com_method_ptr)&Com_job::get_Process_class           , VT_DISPATCH   },
    { DISPATCH_PROPERTYGET, 20, "Folder_path"                   , (Com_method_ptr)&Com_job::get_Folder_path             , VT_BSTR       },
    { DISPATCH_PROPERTYGET, 21, "Configuration_directory"       , (Com_method_ptr)&Com_job::get_Configuration_directory , VT_BSTR       },
    { DISPATCH_PROPERTYGET, 22, "Setback_max"                   , (Com_method_ptr)&Com_job::get_Setback_max             , VT_INT        },
    { DISPATCH_PROPERTYGET, 23, "Script_code"                   , (Com_method_ptr)&Com_job::get_Script_code             , VT_BSTR       },
    {}
};

#endif
//---------------------------------------------------------------------------------Com_job::Com_job

Com_job::Com_job( Standard_job* job )
:
    Sos_ole_object( job_class_ptr, (Ijob*)this ),
    _job(job)
{
}

//--------------------------------------------------------------------------Com_job::QueryInterface

STDMETHODIMP Com_job::QueryInterface( const IID& iid, void** result )
{
    Z_IMPLEMENT_QUERY_INTERFACE( this, iid, Ihas_java_class_name, result );

    return Sos_ole_object::QueryInterface( iid, result );
}

//------------------------------------------------------------Com_job::start_when_directory_changed 

STDMETHODIMP Com_job::Start_when_directory_changed( BSTR directory_name, BSTR filename_pattern )
{
    HRESULT hr = NOERROR;

    try
    {
        if( !_job )  return E_POINTER;

        _job->start_when_directory_changed( bstr_as_string( directory_name ), bstr_as_string( filename_pattern ) );
    }
    catch( const exception&  x )  { hr = _set_excepinfo( x, "Spooler.Job::start_when_directory_changed" ); }
    catch( const _com_error& x )  { hr = _set_excepinfo( x, "Spooler.Job::start_when_directory_changed" ); }

    return hr;
}

//------------------------------------------------------------Com_job::clear_when_directory_changed 

STDMETHODIMP Com_job::Clear_when_directory_changed()
{
    HRESULT hr = NOERROR;

    try
    {
        if( !_job )  return E_POINTER;
        _job->clear_when_directory_changed();
    }
    catch( const exception&  x )  { hr = _set_excepinfo( x, "Spooler.Job::clear_when_directory_changed" ); }
    catch( const _com_error& x )  { hr = _set_excepinfo( x, "Spooler.Job::clear_when_directory_changed" ); }

    return hr;
}

//-----------------------------------------------------------------------------------Com_job::start

STDMETHODIMP Com_job::Start( VARIANT* params, Itask** itask )
{
    HRESULT hr = NOERROR;

    try
    {
        if( !_job )  return E_POINTER;

        ptr<Task>           task;

        ptr<Ivariable_set>  pars;
        Time                start_at = Time(0); 

        if( params  &&  params->vt != VT_EMPTY  &&  params->vt != VT_NULL  &&  params->vt != VT_ERROR )
        {
            if( params->vt != VT_DISPATCH && params->vt != VT_UNKNOWN )  return DISP_E_TYPEMISMATCH;
            hr = pars.Assign_qi( params->punkVal );
            if( FAILED(hr) )  return hr;
        }

        Variant task_name_vt;
        if( pars )  pars->get_Var( Bstr("spooler_task_name"), &task_name_vt );
        hr = task_name_vt.ChangeType( VT_BSTR );    if( FAILED(hr) )  throw_ole( hr, "ChangeType", "spooler_task_name" );

        Variant start_after_vt;
        if( pars )  pars->get_Var( Bstr("spooler_start_after"), &start_after_vt );
        if( start_after_vt.vt != VT_EMPTY )
        {
            hr = start_after_vt.ChangeType( VT_R8 );    if( FAILED(hr) )  throw_ole( hr, "ChangeType", "spooler_start_after" );
            start_at = Time::now() + Duration(start_after_vt.dblVal);
        }

        string name = bstr_as_string( task_name_vt.bstrVal );
        Com_variable_set* variable_set = dynamic_cast<Com_variable_set*>(+pars);
        if (pars && !variable_set) z::throw_xc("Ivariable_set is not a Com_variable_set");
        task = _job->start_task( variable_set, name, start_at );

        *itask = new Com_task( task );
        (*itask)->AddRef();
    }
    catch( const exception&  x )  { hr = _set_excepinfo( x, "Spooler.Job::start" ); }
    catch( const _com_error& x )  { hr = _set_excepinfo( x, "Spooler.Job::start" ); }

    return hr;
}

//------------------------------------------------------------------------------------Com_job::wake

STDMETHODIMP Com_job::Wake()
{
    HRESULT hr = NOERROR;

    try
    {
        if( !_job )  return E_POINTER;
        _job->set_state_cmd( Job::sc_wake );
    }
    catch( const exception&  x )  { hr = _set_excepinfo( x, "Spooler.Job::start_when_directory_changed" ); }
    catch( const _com_error& x )  { hr = _set_excepinfo( x, "Spooler.Job::start_when_directory_changed" ); }

    return hr;
}

//------------------------------------------------------------------------Com_job::get_include_path

STDMETHODIMP Com_job::get_Include_path( BSTR* result )
{
    if( !_job )  return E_POINTER;
    //if( current_thread_id() != _job->thread()->thread_id() )  return E_ACCESSDENIED;

    *result = SysAllocString_string( _job->_spooler->include_path() );

    return NOERROR;
}

//--------------------------------------------------------------------------------Com_job::get_Name

STDMETHODIMP Com_job::get_Name( BSTR* result )
{
    if( !_job )  return E_POINTER;
    //if( current_thread_id() != _job->thread()->thread_id() )  return E_ACCESSDENIED;

    *result = SysAllocString_string( _job->path().without_slash() );

    return NOERROR;
}

//--------------------------------------------------------------------------Com_job::put_state_text

STDMETHODIMP Com_job::put_State_text( BSTR text )
{
    HRESULT hr = NOERROR;

    try
    {
        if( !_job )  z::throw_xc( "SCHEDULER-122" );
      //if( current_thread_id() != _job->thread()->thread_id() )  return E_ACCESSDENIED;

        _job->set_state_text( bstr_as_string( text ) );
    }
    catch( const exception&  x )  { hr = _set_excepinfo( x, "Spooler.Job.state_text" ); }
    catch( const _com_error& x )  { hr = _set_excepinfo( x, "Spooler.Job.state_text" ); }

    return hr;
}

//-------------------------------------------------------------------------------Com_job::get_title

STDMETHODIMP Com_job::get_Title( BSTR* title )
{
    HRESULT hr = NOERROR;

    try
    {
        if( !_job )  z::throw_xc( "SCHEDULER-122" );

        *title = SysAllocString_string( _job->title() );
    }
    catch( const exception&  x )  { hr = _set_excepinfo( x, "Spooler.Job.title" ); }
    catch( const _com_error& x )  { hr = _set_excepinfo( x, "Spooler.Job.title" ); }

    return hr;
}

//-------------------------------------------------------------------Com_job::put_delay_after_error

STDMETHODIMP Com_job::put_Delay_after_error( int error_steps, VARIANT* time )
{
    HRESULT hr = NOERROR;

    try
    {
        if( !_job )  z::throw_xc( "SCHEDULER-122" );

        _job->set_delay_after_error( error_steps, string_from_variant( *time ) );
    }
    catch( const exception&  x )  { hr = _set_excepinfo( x, "Spooler.Job.delay_after_error" ); }
    catch( const _com_error& x )  { hr = _set_excepinfo( x, "Spooler.Job.delay_after_error" ); }

    return hr;
}

//-----------------------------------------------------------------Com_job::clear_delay_after_error

STDMETHODIMP Com_job::Clear_delay_after_error()
{
    HRESULT hr = NOERROR;

    try
    {
        if( !_job )  z::throw_xc( "SCHEDULER-122" );

        _job->clear_delay_after_error();
    }
    catch( const exception&  x )  { hr = _set_excepinfo( x, "Spooler.Job.clear_delay_after_error" ); }
    catch( const _com_error& x )  { hr = _set_excepinfo( x, "Spooler.Job.clear_delay_after_error" ); }

    return hr;
}

//----------------------------------------------------------------------------------Com_job::Remove

STDMETHODIMP Com_job::Remove()
{
    HRESULT hr = NOERROR;

    try
    {
        if( !_job )  z::throw_xc( "SCHEDULER-122" );

        _job->remove( File_based::rm_base_file_too );
    }
    catch( const exception&  x )  { hr = _set_excepinfo( x, Z_FUNCTION ); }
    catch( const _com_error& x )  { hr = _set_excepinfo( x, Z_FUNCTION ); }

    return hr;
}

//-------------------------------------------------------------------------Com_job::Execute_command

STDMETHODIMP Com_job::Execute_command( BSTR command_bstr )
{
    HRESULT hr = NOERROR;

    try
    {
        if( !_job )  z::throw_xc( "SCHEDULER-122" );

        _job->set_state_cmd( Job::as_state_cmd( string_from_bstr( command_bstr ) ) );
    }
    catch( const exception&  x )  { hr = _set_excepinfo( x, Z_FUNCTION ); }
    catch( const _com_error& x )  { hr = _set_excepinfo( x, Z_FUNCTION ); }

    return hr;
}

//-------------------------------------------------------------------------Com_job::get_order_queue

STDMETHODIMP Com_job::get_Order_queue( Iorder_queue** result )
{
    HRESULT hr = NOERROR;

    try
    {
        if( !_job )  z::throw_xc( "SCHEDULER-122" );

        *result = _job->any_order_queue();
        if( *result )  (*result)->AddRef();
    }
    catch( const exception&  x )  { hr = _set_excepinfo( x, "Spooler.Job.order_queue" ); }
    catch( const _com_error& x )  { hr = _set_excepinfo( x, "Spooler.Job.order_queue" ); }

    return hr;
}

//-----------------------------------------------------------Com_job::put_delay_order_after_setback

STDMETHODIMP Com_job::put_Delay_order_after_setback( int setback_number, VARIANT* time )
{
    HRESULT hr = NOERROR;

    try
    {
        if( !_job )  z::throw_xc( "SCHEDULER-122" );

        _job->set_delay_order_after_setback( setback_number, string_from_variant(*time) );
    }
    catch( const exception&  x )  { hr = _set_excepinfo( x, "Spooler.Job.delay_order_after_setback" ); }
    catch( const _com_error& x )  { hr = _set_excepinfo( x, "Spooler.Job.delay_order_after_setback" ); }

    return hr;
}

//------------------------------------------------------------------Com_job::put_max_order_setbacks

STDMETHODIMP Com_job::put_Max_order_setbacks( int count )
{
    HRESULT hr = NOERROR;

    try
    {
        if( !_job )  z::throw_xc( "SCHEDULER-122" );

        _job->set_max_order_setbacks( count );
    }
    catch( const exception&  x )  { hr = _set_excepinfo( x, "Spooler.Job.max_order_setbacks" ); }
    catch( const _com_error& x )  { hr = _set_excepinfo( x, "Spooler.Job.max_order_setbacks" ); }

    return hr;
}

//-------------------------------------------------------------------Com_job::put_Machine_resumable

STDMETHODIMP Com_job::put_Machine_resumable( VARIANT_BOOL machine_resumable )
{
    HRESULT hr = NOERROR;

    try
    {
        if( !_job )  z::throw_xc( "SCHEDULER-122" );
        if( !_job->_spooler->_zschimmer_mode )  return FALSE;  // E_ACCESSDENIED

        _job->set_machine_resumable( machine_resumable != 0 );
    }
    catch( const exception&  x )  { hr = _set_excepinfo( x, Z_FUNCTION ); }
    catch( const _com_error& x )  { hr = _set_excepinfo( x, Z_FUNCTION ); }

    return hr;
}

//-----------------------------------------------------------------------Com_job::get_Process_class

STDMETHODIMP Com_job::get_Process_class( spooler_com::Iprocess_class** result )
{
    HRESULT hr = NOERROR;

    try
    {
        if( !_job )  z::throw_xc( "SCHEDULER-122" );

        *result = _job->default_process_class_or_null();
        if( *result )  (*result)->AddRef();
    }
    catch( const exception&  x )  { hr = _set_excepinfo( x, Z_FUNCTION ); }

    return hr;
}

//-------------------------------------------------------------------------Com_job::get_Folder_path

STDMETHODIMP Com_job::get_Folder_path( BSTR* result )

// eMail von Püschel, 2008-02-24 17:12
// Rückgabewert: liefert den Pfad des Jobs relativ zum Live-Directory. Der Pfad beginnt mit  einem "/", alle Bestandteile eines Pfads sind durch "/" getrennt
// Beispiele: 
// - für einen Job c:\scheduler\config\live\somewhere\excel\sample.job.xml wird "/somewhere/excel" zurückgeliefert 
// - für einen Job c:\scheduler\config\live\sample.xml wird "/"  zurückgeliefert 
// - für einen Job außerhalb des Live-Verzeichnisses wird "" (Leerstring) zurückgeliefert
// - die Methode liefert nicht null

{
    HRESULT hr = NOERROR;

    try
    {
        if( !_job )  z::throw_xc( "SCHEDULER-122" );

        hr = String_to_bstr( _job->has_base_file() && 
                             _job->configuration_origin() == confdir_local?  // Siehe Püschels eMail vom 2008-02-24 17:12
                                _job->folder_path() 
                              : Absolute_path(), result );

    }
    catch( const exception&  x )  { hr = _set_excepinfo( x, Z_FUNCTION ); }

    return hr;
}

//-------------------------------------------------------------Com_job::get_Configuration_directory

STDMETHODIMP Com_job::get_Configuration_directory( BSTR* result )
{
    HRESULT hr = NOERROR;

    try
    {
        if( !_job )  z::throw_xc( "SCHEDULER-122" );

        hr = String_to_bstr( _job->has_base_file()? _job->base_file_info()._path.directory() : File_path(), result );
    }
    catch( const exception&  x )  { hr = _set_excepinfo( x, Z_FUNCTION ); }

    return hr;
}

//-------------------------------------------------------------------------Com_job::get_Setback_max
// deprecated

STDMETHODIMP Com_job::get_Setback_max( int* result )
{
    return get_Max_order_setbacks(result);
}

//-------------------------------------------------------------------Com_job::get_Max_order_setbacks

STDMETHODIMP Com_job::get_Max_order_setbacks(int* result)
{
    HRESULT hr = NOERROR;

    try
    {
        if( !_job )  return E_POINTER;

        *result = _job->max_order_setbacks();
    }
    catch( const exception&  x )  { hr = _set_excepinfo( x, Z_FUNCTION ); }
    catch( const _com_error& x )  { hr = _set_excepinfo( x, Z_FUNCTION ); }

    return hr;
}

//-------------------------------------------------------------Com_job::get_Script_code

STDMETHODIMP Com_job::get_Script_code( BSTR* result )
{
    HRESULT hr = NOERROR;

    try
    {
        if( !_job )  z::throw_xc( "SCHEDULER-122" );

        hr = String_to_bstr( trim(_job->module()->_text_with_includes.read_plain_or_xml_string()), result );
    }
    catch( const exception&  x )  { hr = _set_excepinfo( x, Z_FUNCTION ); }

    return hr;
}

//-------------------------------------------------------------------------------Com_task::_methods
#ifdef Z_COM  // JS-421 Com unter Unix

const Com_method Com_task::_methods[] =
{ 
   // _flags         , dispid, _name                        , _method                                           , _result_type  , _types        , _default_arg_count
    { DISPATCH_PROPERTYPUT,  2, "error"                     , (Com_method_ptr)&Com_task::put_Error              , VT_EMPTY      , { VT_BYREF|VT_VARIANT } },
    { DISPATCH_PROPERTYGET,  2, "error"                     , (Com_method_ptr)&Com_task::get_Error              , VT_DISPATCH  },
    { DISPATCH_PROPERTYGET,  3, "job"                       , (Com_method_ptr)&Com_task::get_Job                , VT_DISPATCH  },
    { DISPATCH_PROPERTYGET,  4, "params"                    , (Com_method_ptr)&Com_task::get_Params             , VT_DISPATCH  },
    { DISPATCH_PROPERTYPUT,  5, "result"                    , (Com_method_ptr)&Com_task::put_Result             , VT_EMPTY      , { VT_BYREF|VT_DISPATCH } },
    { DISPATCH_PROPERTYGET,  5, "result"                    , (Com_method_ptr)&Com_task::get_Result             , VT_DISPATCH  },
    { DISPATCH_METHOD     ,  6, "wait_until_terminated"     , (Com_method_ptr)&Com_task::Wait_until_terminated  , VT_BOOL       , { VT_R8|VT_BYREF } },
    { DISPATCH_PROPERTYPUT,  7, "repeat"                    , (Com_method_ptr)&Com_task::put_Repeat             , VT_EMPTY      , { VT_R8|VT_BYREF } },
    { DISPATCH_METHOD     ,  8, "end"                       , (Com_method_ptr)&Com_task::End                    },
    { DISPATCH_PROPERTYPUT,  9, "history_field"             , (Com_method_ptr)&Com_task::put_History_field      , VT_EMPTY      , { VT_BSTR, VT_BYREF|VT_VARIANT } },
    { DISPATCH_PROPERTYGET, 10, "id"                        , (Com_method_ptr)&Com_task::get_Id                 , VT_I4         },
    { DISPATCH_PROPERTYPUT, 11, "delay_spooler_process"     , (Com_method_ptr)&Com_task::put_Delay_spooler_process, VT_EMPTY    , { VT_BYREF|VT_VARIANT } },
    { DISPATCH_PROPERTYGET, 13, "order"                     , (Com_method_ptr)&Com_task::get_Order              , VT_DISPATCH   },
    { DISPATCH_PROPERTYGET, 14, "java_class_name"           , (Com_method_ptr)&Com_task::get_Java_class_name    , VT_BSTR       },
    { DISPATCH_PROPERTYGET, 15, "changed_directories"       , (Com_method_ptr)&Com_task::get_Changed_directories, VT_BSTR       },
    { DISPATCH_METHOD     , 16, "add_pid"                   , (Com_method_ptr)&Com_task::Add_pid                , VT_EMPTY      , { VT_INT, VT_BYREF|VT_VARIANT }, 1 },
    { DISPATCH_METHOD     , 17, "remove_pid"                , (Com_method_ptr)&Com_task::Remove_pid             , VT_EMPTY      , { VT_INT } },
    { DISPATCH_PROPERTYGET, 18, "stderr_text"               , (Com_method_ptr)&Com_task::get_Stderr_text        , VT_BSTR       },
    { DISPATCH_PROPERTYGET, 19, "stdout_text"               , (Com_method_ptr)&Com_task::get_Stdout_text        , VT_BSTR       },
    { DISPATCH_METHOD     , 20, "Create_subprocess"         , (Com_method_ptr)&Com_task::Create_subprocess      , VT_DISPATCH   , { VT_BYREF|VT_VARIANT }, 1 },
    { DISPATCH_METHOD     , 21, "Add_subprocess"            , (Com_method_ptr)&Com_task::Add_subprocess         , VT_EMPTY      , { VT_INT, VT_R8|VT_BYREF, VT_BOOL, VT_BOOL, VT_BOOL, VT_BSTR }, 1 },
    { DISPATCH_PROPERTYPUT, 22, "Priority_class"            , (Com_method_ptr)&Com_task::put_Priority_class     , VT_EMPTY      , { VT_BSTR } },
    { DISPATCH_PROPERTYGET, 22, "Priority_class"            , (Com_method_ptr)&Com_task::get_Priority_class     , VT_BSTR       },
    { DISPATCH_PROPERTYGET, 23, "Step_count"                , (Com_method_ptr)&Com_task::get_Step_count         , VT_INT        },
    { DISPATCH_PROPERTYGET, 24, "Stderr_path"               , (Com_method_ptr)&Com_task::get_Stderr_path        , VT_BSTR       },
    { DISPATCH_PROPERTYGET, 25, "Stdout_path"               , (Com_method_ptr)&Com_task::get_Stdout_path        , VT_BSTR       },
    { DISPATCH_METHOD     , 26, "Set_error_code_and_text"   , (Com_method_ptr)&Com_task::Set_error_code_and_text, VT_EMPTY      , { VT_BSTR, VT_BSTR } },
    { DISPATCH_PROPERTYGET, 27, "Web_service"               , (Com_method_ptr)&Com_task::get_Web_service        , VT_DISPATCH   },
    { DISPATCH_PROPERTYGET, 28, "Web_service_or_null"       , (Com_method_ptr)&Com_task::get_Web_service_or_null, VT_DISPATCH   },
    { DISPATCH_PROPERTYPUT, 29, "Priority"                  , (Com_method_ptr)&Com_task::put_Priority           , VT_EMPTY      , { VT_INT } },
    { DISPATCH_PROPERTYGET, 29, "Priority"                  , (Com_method_ptr)&Com_task::get_Priority           , VT_INT        },
    { DISPATCH_PROPERTYPUT, 30, "Exit_code"                 , (Com_method_ptr)&Com_task::put_Exit_code          , VT_EMPTY      , { VT_INT } },
    { DISPATCH_PROPERTYGET, 30, "Exit_code"                 , (Com_method_ptr)&Com_task::get_Exit_code          , VT_INT        },
    { DISPATCH_PROPERTYGET, 31, "Trigger_files"             , (Com_method_ptr)&Com_task::get_Trigger_files      , VT_BSTR       },
    { DISPATCH_METHOD     , 32, "Try_hold_lock"                            , (Com_method_ptr)&Com_task::Try_hold_lock                            , VT_BOOL, { VT_BSTR } },
    { DISPATCH_METHOD     , 33, "Try_hold_lock_non_exclusive"              , (Com_method_ptr)&Com_task::Try_hold_lock_non_exclusive              , VT_BOOL, { VT_BSTR } },
    { DISPATCH_METHOD     , 34, "Call_me_again_when_locks_available"       , (Com_method_ptr)&Com_task::Call_me_again_when_locks_available       , VT_EMPTY },
    { DISPATCH_PROPERTYGET, 35, "Params_xml"                , (Com_method_ptr)&Com_task::get_Params_xml         , VT_BSTR       },
    { DISPATCH_PROPERTYPUT, 35, "Params_xml"                , (Com_method_ptr)&Com_task::put_Params_xml         , VT_EMPTY      , { VT_BSTR } }, /* JS-1026 get => put */
    { DISPATCH_PROPERTYGET, 36, "Order_params_xml"          , (Com_method_ptr)&Com_task::get_Order_params_xml   , VT_BSTR       },
    { DISPATCH_PROPERTYPUT, 36, "Order_params_xml"          , (Com_method_ptr)&Com_task::put_Order_params_xml   , VT_EMPTY      , { VT_BSTR } }, /* JS-421 get => put */
    { DISPATCH_PROPERTYGET, 37, "Web_service_access_token"  , (Com_method_ptr)&Com_task::get_Web_service_access_token, VT_BSTR  },
    //{ DISPATCH_METHOD     , 35, "Try_lock_else_call_me_again"              , (Com_method_ptr)&Com_task::Try_lock_else_call_me_again              , VT_BOOL, { VT_BSTR } },
    //{ DISPATCH_METHOD     , 36, "Try_lock_non_exclusive_else_call_me_again", (Com_method_ptr)&Com_task::Try_lock_non_exclusive_else_call_me_again, VT_BOOL, { VT_BSTR } },
    {}
};

#endif
//-------------------------------------------------------------------------------Com_task::Com_task

Com_task::Com_task( Task* task )
:
    Sos_ole_object( task_class_ptr, (Itask*)this ),
    _task(task)
{
}

//-------------------------------------------------------------------------Com_task::QueryInterface

STDMETHODIMP Com_task::QueryInterface( const IID& iid, void** result )
{
    Z_IMPLEMENT_QUERY_INTERFACE( this, iid, Ihas_java_class_name          , result );
    Z_IMPLEMENT_QUERY_INTERFACE( this, iid, Ihas_reference_with_properties, result );

    return Sos_ole_object::QueryInterface( iid, result );
}

//-------------------------------------------------------Com_spooler::get_reference_with_properties

ptr<object_server::Reference_with_properties> Com_spooler::get_reference_with_properties()
{
    ptr<object_server::Reference_with_properties> result;

    {
        if( !_spooler )  throw_com( E_POINTER, "Com_spooler::get_reference_with_properties" );

        result = Z_NEW( object_server::Reference_with_properties( CLSID_Spooler_proxy, static_cast<Ispooler*>( this ) ) );
    }

    return result;
}

//----------------------------------------------------------Com_task::get_reference_with_properties

ptr<object_server::Reference_with_properties> Com_task::get_reference_with_properties()
{
    ptr<object_server::Reference_with_properties> result;

    {
        if( !_task )  throw_com( E_POINTER, "Com_task::get_reference_with_properties" );

        result = Z_NEW( object_server::Reference_with_properties( CLSID_Task_proxy, static_cast<Itask*>( this ) ) );
        result->set_property( "subprocess_own_process_group_default", _task->_spooler->_subprocess_own_process_group_default );

        //if( Remote_module_instance_proxy* r = dynamic_cast<Remote_module_instance_proxy*>( +_task->_module_instance ) )
        //{
        //    if( r->_process )
        //    {
        //        result->set_property( "stdout_path", r->_process->stdout_path() );
        //        result->set_property( "stderr_path", r->_process->stderr_path() );
        //    }
        //}
    }

    return result;
}

//-------------------------------------------------------------------------------Com_task::set_task

void Com_task::set_task( Task* task )
{ 
    _task = task; 
}

//----------------------------------------------------------------------------------Com_task::error

STDMETHODIMP Com_task::put_Error( VARIANT* error_par )
{
    HRESULT hr = NOERROR;

    try
    {
        if( !_task )  z::throw_xc( "SCHEDULER-122" );
        if( current_thread_id() != _task->_spooler->thread_id() )  return E_ACCESSDENIED;

        Variant error_vt = *error_par;
        hr = error_vt.ChangeType( VT_BSTR );        if( FAILED(hr) )  return hr;

        string error_text = bstr_as_string( error_vt.bstrVal );
        _task->set_error( Xc( "SCHEDULER-120", error_text.c_str() ) );
    }
    catch( const exception&  x )  { hr = _set_excepinfo( x, "Spooler.Task::error" ); }
    catch( const _com_error& x )  { hr = _set_excepinfo( x, "Spooler.Task::error" ); }

    return hr;
}

//----------------------------------------------------------------------------------Com_task::error

STDMETHODIMP Com_task::get_Error( Ierror** result )
{
    HRESULT hr = NOERROR;

    try
    {
        if( !_task )  z::throw_xc( "SCHEDULER-122" );

        *result = new Com_error( _task->error() );
        (*result)->AddRef();
    }
    catch( const exception&  x )  { hr = _set_excepinfo( x, "Spooler.Task.Error" ); }
    catch( const _com_error& x )  { hr = _set_excepinfo( x, "Spooler.Task.Error" ); }

    return hr;
}

//--------------------------------------------------------------------------------Com_task::get_job

STDMETHODIMP Com_task::get_Job( Ijob** result )
{
    HRESULT hr = NOERROR;

    try
    {
        if( !_task )  z::throw_xc( "SCHEDULER-122" );

        *result = _task->_job->com_job();
        if( *result )  (*result)->AddRef();
    }
    catch( const exception&  x )  { hr = _set_excepinfo( x, "Spooler.Task.job" ); }
    catch( const _com_error& x )  { hr = _set_excepinfo( x, "Spooler.Task.job" ); }

    return hr;
}

//-----------------------------------------------------------------------------Com_task::get_params

STDMETHODIMP Com_task::get_Params( Ivariable_set** result )
{
    HRESULT hr = NOERROR;

    try
    {
        if( !_task )  z::throw_xc( "SCHEDULER-122" );

        *result = _task->params();
        if( *result )  
        {
            (*result)->AddRef();
        }
    }
    catch( const exception&  x )  { hr = _set_excepinfo( x, "Spooler.Task.params" ); }
    catch( const _com_error& x )  { hr = _set_excepinfo( x, "Spooler.Task.params" ); }

    return hr;
}

//------------------------------------------------------------------Com_task::wait_until_terminated

STDMETHODIMP Com_task::Wait_until_terminated( double* wait_time, VARIANT_BOOL* ok )
{
    HRESULT hr = NOERROR;

    try
    {
        if( _task )  *ok = _task->wait_until_terminated( *wait_time );
               else  *ok = true;
    }
    catch( const exception&  x )  { hr = _set_excepinfo( x, "Spooler.Task.wait_until_terminated" ); }
    catch( const _com_error& x )  { hr = _set_excepinfo( x, "Spooler.Task.wait_until_terminated" ); }

    return hr;
}

//------------------------------------------------------------------------------------Com_task::end

STDMETHODIMP Com_task::End()
{
    HRESULT hr = NOERROR;

    try
    {
        if( _task )  _task->cmd_end();
    }
    catch( const exception&  x )  { hr = _set_excepinfo( x, "Spooler.Task.end" ); }
    catch( const _com_error& x )  { hr = _set_excepinfo( x, "Spooler.Task.end" ); }

    return hr;
}

//-----------------------------------------------------------------------------Com_task::put_result

STDMETHODIMP Com_task::put_Result( VARIANT* value )
{
    HRESULT hr = NOERROR;

    try
    {
        if( !_task )  z::throw_xc( "SCHEDULER-122" );
        if( current_thread_id() != _task->_spooler->thread_id() )  return E_ACCESSDENIED;
      //if( !_task->_job->its_current_task(_task) )  z::throw_xc( "SCHEDULER-138" );

        hr = _task->_result.Copy( value );
    }
    catch( const exception&  x )  { hr = _set_excepinfo( x, "Spooler.Task.result" ); }
    catch( const _com_error& x )  { hr = _set_excepinfo( x, "Spooler.Task.result" ); }

    return hr;
}

//-----------------------------------------------------------------------------Com_task::get_result

STDMETHODIMP Com_task::get_Result( VARIANT* value )
{
    HRESULT hr = NOERROR;

    try
    {
        if( !_task )  z::throw_xc( "SCHEDULER-122" );

        VariantInit( value ); 
        hr = VariantCopy( value, &_task->_result );
    }
    catch( const exception&  x )  { hr = _set_excepinfo( x, "Spooler.Task.result" ); }
    catch( const _com_error& x )  { hr = _set_excepinfo( x, "Spooler.Task.result" ); }

    return hr;
}

//-----------------------------------------------------------------------------Com_task::put_repeat

STDMETHODIMP Com_task::put_Repeat( double* seconds )
{
    HRESULT hr = NOERROR;

    try
    {
        if( !_task )  z::throw_xc( "SCHEDULER-122" );
        if( current_thread_id() != _task->_spooler->thread_id() )  return E_ACCESSDENIED;

        _task->_job->set_repeat(Duration(*seconds));
    }
    catch( const exception&  x )  { hr = _set_excepinfo( x, "Spooler.Task.repeat" ); }
    catch( const _com_error& x )  { hr = _set_excepinfo( x, "Spooler.Task.repeat" ); }

    return hr;
}

//----------------------------------------------------------------------Com_task::put_history_field

STDMETHODIMP Com_task::put_History_field( BSTR name, VARIANT* value )
{
    HRESULT hr = NOERROR;

    try
    {
        if( !_task )  z::throw_xc( "SCHEDULER-122" );
        if( current_thread_id() != _task->_spooler->thread_id() )  return E_ACCESSDENIED;

        _task->set_history_field( bstr_as_string(name), *value );
    }
    catch( const exception&  x )  { hr = _set_excepinfo( x, "Spooler.Task.history_field" ); }
    catch( const _com_error& x )  { hr = _set_excepinfo( x, "Spooler.Task.history_field" ); }

    return hr;
}

//---------------------------------------------------------------------------------Com_task::get_id

STDMETHODIMP Com_task::get_Id( int* result )
{
    HRESULT hr = NOERROR;

    try
    {
        if( !_task )  z::throw_xc( "SCHEDULER-122" );

        *result = _task->id();
    }
    catch( const exception&  x )  { hr = _set_excepinfo( x, "Spooler.Task.id" ); }
    catch( const _com_error& x )  { hr = _set_excepinfo( x, "Spooler.Task.id" ); }

    return hr;
}

//--------------------------------------------------------------Com_task::put_delay_spooler_process

STDMETHODIMP Com_task::put_Delay_spooler_process( VARIANT* time )
{
    HRESULT hr = NOERROR;

    try
    {
        if( !_task )  z::throw_xc( "SCHEDULER-122" );

        _task->set_delay_spooler_process( duration_from_variant( *time ) );
    }
    catch( const exception&  x )  { hr = _set_excepinfo( x, "Spooler.Task.delay_spooler_process" ); }
    catch( const _com_error& x )  { hr = _set_excepinfo( x, "Spooler.Task.delay_spooler_process" ); }

    return hr;
}

//------------------------------------------------------------------------------Com_task::get_order

STDMETHODIMP Com_task::get_Order( Iorder** result )
{
    HRESULT hr = NOERROR;

    try
    {
        if( !_task )  z::throw_xc( "SCHEDULER-122" );
        if( current_thread_id() != _task->_spooler->thread_id() )  return E_ACCESSDENIED;

        *result = _task->order();
        if( *result )  (*result)->AddRef();
    }
    catch( const exception&  x )  { hr = _set_excepinfo( x, "Spooler.Task.result" ); }
    catch( const _com_error& x )  { hr = _set_excepinfo( x, "Spooler.Task.result" ); }

    return hr;
}

//----------------------------------------------------------------Com_task::get_changed_directories

STDMETHODIMP Com_task::get_Changed_directories( BSTR* result )
{
    HRESULT hr = S_OK;

    try
    {
        if( !_task )  z::throw_xc( "SCHEDULER-122" );
        if( current_thread_id() != _task->_spooler->thread_id() )  return E_ACCESSDENIED;

        hr = String_to_bstr( _task->_changed_directories, result );
    }
    catch( const exception&  x )  { hr = _set_excepinfo( x, Z_FUNCTION ); }
    catch( const _com_error& x )  { hr = _set_excepinfo( x, Z_FUNCTION ); }

    return hr;
}

//----------------------------------------------------------------------Com_task::get_Trigger_files

STDMETHODIMP Com_task::get_Trigger_files( BSTR* result )
{
    HRESULT hr = S_OK;

    try
    {
        if( !_task )  z::throw_xc( "SCHEDULER-122" );
        if( current_thread_id() != _task->_spooler->thread_id() )  return E_ACCESSDENIED;

        hr = String_to_bstr( _task->_trigger_files, result );
    }
    catch( const exception&  x )  { hr = _set_excepinfo( x, Z_FUNCTION ); }
    catch( const _com_error& x )  { hr = _set_excepinfo( x, Z_FUNCTION ); }

    return hr;
}

//--------------------------------------------------------------------------Com_task::Try_hold_lock

STDMETHODIMP Com_task::Try_hold_lock( BSTR lock_path_bstr, VARIANT_BOOL* result )
{
    Z_LOGI( Z_FUNCTION << '\n' );

    HRESULT hr = S_OK;

    try
    {
        if( !_task )  z::throw_xc( "SCHEDULER-122" );
        if( current_thread_id() != _task->_spooler->thread_id() )  return E_ACCESSDENIED;

        *result = _task->try_hold_lock( string_from_bstr( lock_path_bstr ) )? VARIANT_TRUE 
                                                                            : VARIANT_FALSE;
    }
    catch( const exception&  x )  { hr = Set_excepinfo( x, Z_FUNCTION ); }
    catch( const _com_error& x )  { hr = Set_excepinfo( x, Z_FUNCTION ); }

    return hr;
}

//------------------------------------------------------------Com_task::Try_hold_lock_non_exclusive

STDMETHODIMP Com_task::Try_hold_lock_non_exclusive( BSTR lock_path_bstr, VARIANT_BOOL* result )
{
    Z_LOGI( Z_FUNCTION << '\n' );
    
    HRESULT hr = S_OK;
    
    try
    {
        if( !_task )  z::throw_xc( "SCHEDULER-122" );
        if( current_thread_id() != _task->_spooler->thread_id() )  return E_ACCESSDENIED;

        *result = _task->try_hold_lock( string_from_bstr( lock_path_bstr ), lock::Lock::lk_non_exclusive )? VARIANT_TRUE 
                                                                                                          : VARIANT_FALSE;
    }
    catch( const exception&  x )  { hr = Set_excepinfo( x, Z_FUNCTION ); }
    catch( const _com_error& x )  { hr = Set_excepinfo( x, Z_FUNCTION ); }
    
    return hr;
}

//-----------------------------------------------------Com_task::Call_me_again_when_locks_available

STDMETHODIMP Com_task::Call_me_again_when_locks_available()
{
    Z_LOGI( Z_FUNCTION << '\n' );
    
    HRESULT hr = S_OK;
    
    try
    {
        if( !_task )  z::throw_xc( "SCHEDULER-122" );
        if( current_thread_id() != _task->_spooler->thread_id() )  return E_ACCESSDENIED;

        _task->delay_until_locks_available();
    }
    catch( const exception&  x )  { hr = Set_excepinfo( x, Z_FUNCTION ); }
    catch( const _com_error& x )  { hr = Set_excepinfo( x, Z_FUNCTION ); }
    
    return hr;
}

//------------------------------------------------------------Com_task::Try_lock_else_call_me_again

//STDMETHODIMP Com_task::Try_lock_else_call_me_again( BSTR lock_path_bstr, VARIANT_BOOL* result )
//{
//    Z_LOGI( Z_FUNCTION << '\n' );
//    
//    HRESULT hr = S_OK;
//    
//    try
//    {
//        if( !_task )  z::throw_xc( "SCHEDULER-122" );
//        if( current_thread_id() != _task->_spooler->thread_id() )  return E_ACCESSDENIED;
//
//        *result = _task->try_hold_lock( string_from_bstr( lock_path_bstr ) )? VARIANT_TRUE 
//                                                                            : VARIANT_FALSE;
//        if( !*result )  _task->delay_until_locks_available();
//    }
//    catch( const exception&  x )  { hr = Set_excepinfo( x, Z_FUNCTION ); }
//    catch( const _com_error& x )  { hr = Set_excepinfo( x, Z_FUNCTION ); }
//    
//    return hr;
//}
//
////--------------------------------------Com_task::Try_lock_non_exclusive_else_call_me_again
//
//STDMETHODIMP Com_task::Try_lock_non_exclusive_else_call_me_again( BSTR lock_path_bstr, VARIANT_BOOL* result )
//{
//    Z_LOGI( Z_FUNCTION << '\n' );
//
//    HRESULT hr = S_OK;
//
//    try
//    {
//        if( !_task )  z::throw_xc( "SCHEDULER-122" );
//        if( current_thread_id() != _task->_spooler->thread_id() )  return E_ACCESSDENIED;
//
//        *result = _task->try_hold_lock( string_from_bstr( lock_path_bstr ), lock::Lock::lk_non_exclusive )? VARIANT_TRUE 
//                                                                                                          : VARIANT_FALSE;
//        if( !*result )  _task->delay_until_locks_available();
//    }
//    catch( const exception&  x )  { hr = Set_excepinfo( x, Z_FUNCTION ); }
//    catch( const _com_error& x )  { hr = Set_excepinfo( x, Z_FUNCTION ); }
//
//    return hr;
//}

//--------------------------------------------------------------------------------Com_task::add_pid

STDMETHODIMP Com_task::Add_pid( int pid, VARIANT* timeout )
{
    Z_LOG2( "scheduler", Z_FUNCTION << "(" << pid << "," << debug_string_from_variant( *timeout ) << ")\n" );
    
    HRESULT hr = S_OK;
    
    try
    {
        if( !_task )  z::throw_xc( "SCHEDULER-122" );
        if( current_thread_id() != _task->_spooler->thread_id() )  return E_ACCESSDENIED;

        Duration t  = timeout->vt == VT_EMPTY || com::variant_is_missing( *timeout )? Duration::eternal
                                                                                    : duration_from_variant( *timeout );
        _task->add_pid( pid, t );
    }
    catch( const exception&  x )  { hr = _set_excepinfo( x, Z_FUNCTION ); }
    catch( const _com_error& x )  { hr = _set_excepinfo( x, Z_FUNCTION ); }
    
    return hr;
}

//-----------------------------------------------------------------------------Com_task::remove_pid

STDMETHODIMP Com_task::Remove_pid( int pid )
{
    Z_LOG2( "scheduler", Z_FUNCTION << "(" << pid << ")\n" );
    
    HRESULT hr = S_OK;
    
    try
    {
        if( !_task )  z::throw_xc( "SCHEDULER-122" );
        if( current_thread_id() != _task->_spooler->thread_id() )  return E_ACCESSDENIED;

        _task->remove_pid( pid );
    }
    catch( const exception&  x )  { hr = _set_excepinfo( x, Z_FUNCTION ); }
    catch( const _com_error& x )  { hr = _set_excepinfo( x, Z_FUNCTION ); }
    
    return hr;
}

//------------------------------------------------------Com_task::get_Stderr_or_stdout_text_or_path

STDMETHODIMP Com_task::get_Stderr_or_stdout_text_or_path( BSTR* result, bool get_stderr, bool get_text )
{
    HRESULT hr = S_OK;
    
    try
    {
        *result = NULL;

        if( !_task )  z::throw_xc( "SCHEDULER-122" );
        if( current_thread_id() != _task->_spooler->thread_id() )  return E_ACCESSDENIED;
        if( !_task->_module_instance )  return S_FALSE;

        string filename = get_stderr? _task->_module_instance->stderr_path() 
                                    : _task->_module_instance->stdout_path();
        if( filename == "" )  return S_FALSE;

        if( get_text )  hr = String_to_bstr( string_from_file( filename ), result );
                  else  hr = String_to_bstr( filename, result );
    }
    catch( const exception&  x )  { hr = _set_excepinfo( x, Z_FUNCTION ); }
    catch( const _com_error& x )  { hr = _set_excepinfo( x, Z_FUNCTION ); }
    
    return hr;
}

//------------------------------------------------------------------------Com_task::get_Stderr_text

STDMETHODIMP Com_task::get_Stderr_text( BSTR* result )
{
    Z_LOG2( "scheduler", Z_FUNCTION << "()\n" );

    return get_Stderr_or_stdout_text_or_path( result, true, true );
}

//------------------------------------------------------------------------Com_task::get_Stderr_path

STDMETHODIMP Com_task::get_Stderr_path( BSTR* result )
{
    Z_LOG2( "scheduler", Z_FUNCTION << "()\n" );

    return get_Stderr_or_stdout_text_or_path( result, true, false );
}

//------------------------------------------------------------------------Com_task::get_Stdout_text

STDMETHODIMP Com_task::get_Stdout_text( BSTR* result )
{
    Z_LOG2( "scheduler", Z_FUNCTION << "()\n" );
    
    return get_Stderr_or_stdout_text_or_path( result, false, true );
}

//------------------------------------------------------------------------Com_task::get_Stdout_path

STDMETHODIMP Com_task::get_Stdout_path( BSTR* result )
{
    Z_LOG2( "scheduler", Z_FUNCTION << "()\n" );
    
    return get_Stderr_or_stdout_text_or_path( result, false, false );
}

//----------------------------------------------------------------------Com_task::Create_subprocess

STDMETHODIMP Com_task::Create_subprocess( VARIANT* program_and_parameters, Isubprocess** result )
{
    Z_LOG2( "scheduler", Z_FUNCTION << "(" << debug_string_from_variant( *program_and_parameters ) << "\n" );
    if( !_task )  return E_POINTER;

    return _task->_subprocess_register.Create_subprocess( program_and_parameters, result, this );
}

//--------------------------------------------------------------------Com_task::Register_subprocess
// Wird aufgerufen von Com_task_proxy

STDMETHODIMP Com_task::Add_subprocess( int pid, double* timeout, VARIANT_BOOL ignore_error, VARIANT_BOOL ignore_signal, VARIANT_BOOL is_process_group, BSTR title )
{
    Z_LOG2( "scheduler", Z_FUNCTION << "(" << pid << ',' << timeout << ',' << ignore_error << ',' << ignore_signal << ',' << is_process_group << ',' << string_from_bstr(title) << ")\n" );
    HRESULT hr = S_OK;
    
    try
    {
        if( !_task )  z::throw_xc( "SCHEDULER-122" );

        _task->add_subprocess( pid, 
                               Duration(*timeout), 
                               ignore_error? true : false, 
                               ignore_signal? true : false, 
                               is_process_group != 0,
                               string_from_bstr( title ) );
    }
    catch( const exception&  x )  { hr = _set_excepinfo( x, Z_FUNCTION ); }
    catch( const _com_error& x )  { hr = _set_excepinfo( x, Z_FUNCTION ); }
    
    return hr;
}

//---------------------------------------------------------------------Com_task::put_Priority_class

STDMETHODIMP Com_task::put_Priority_class( BSTR )
{
    // Siehe Com_task_proxy::put_Priority_class()
    //Z_COM_IMPLEMENT( hr = zschimmer::Process( getpid() ).put_Priority_class( priority_class_bstr ) );
    return FALSE;
}

//---------------------------------------------------------------------Com_task::get_Priority_class

STDMETHODIMP Com_task::get_Priority_class( BSTR* result )
{
    // Siehe Com_task_proxy::get_Priority_class()
    Z_COM_IMPLEMENT( hr = zschimmer::Process( getpid() ).get_Priority_class( result ) );
}

//---------------------------------------------------------------------------Com_task::put_Priority

STDMETHODIMP Com_task::put_Priority( int )
{
    // Siehe Com_task_proxy::put_Priority_class()
    return FALSE;
}

//---------------------------------------------------------------------------Com_task::get_Priority

STDMETHODIMP Com_task::get_Priority( int* result )
{
    // Siehe Com_task_proxy::get_Priority_class()
    Z_COM_IMPLEMENT( hr = zschimmer::Process( getpid() ).get_Priority( result ) );
}

//--------------------------------------------------------------------------Com_task::put_Exit_code

STDMETHODIMP Com_task::put_Exit_code( int exit_code )
{
    HRESULT hr = NOERROR;

    try
    {
        if( !_task )  return E_POINTER;

        _task->set_exit_code( exit_code );
    }
    catch( const exception&  x )  { hr = _set_excepinfo( x, Z_FUNCTION ); }

    return hr;
}

//--------------------------------------------------------------------------Com_task::get_Exit_code

STDMETHODIMP Com_task::get_Exit_code( int* result )
{
    HRESULT hr = NOERROR;

    try
    {
        if( !_task )  return E_POINTER;

        *result = _task->exit_code();
    }
    catch( const exception&  x )  { hr = _set_excepinfo( x, Z_FUNCTION ); }

    return hr;
}

//-------------------------------------------------------------------------Com_task::get_Step_count

STDMETHODIMP Com_task::get_Step_count( int* result )
{
    HRESULT hr = S_OK;
    
    try
    {
        if( !_task )  z::throw_xc( "SCHEDULER-122" );

        *result = _task->_step_count;
    }
    catch( const exception&  x )  { hr = _set_excepinfo( x, Z_FUNCTION ); }
    catch( const _com_error& x )  { hr = _set_excepinfo( x, Z_FUNCTION ); }
    
    return hr;
}

//------------------------------------------------------------------------------Com_task::Set_error_code_and_text

STDMETHODIMP Com_task::Set_error_code_and_text( BSTR error_code, BSTR error_text )
{
    HRESULT hr = S_OK;

    // Für <monitor>: Wenn spooler_process() eine Exception liefert, fängt der Scheduler sie zunächst ab,
    // meldet sie mit dieser Methode und ruft dann spooler_process_after().
    // Ohne <monitor> wird die Exception wie üblich geliefert.
    
    try
    {
        if( !_task )  z::throw_xc( "SCHEDULER-122" );

        if( SysStringLen( error_code ) > 0 )
        {
            Xc x ( string_from_bstr( error_code ).c_str() );
            x.set_what( string_from_bstr( error_text ) );
            _task->set_error_xc( x );
        }
        else
        {
            Xc x ( "SOS-2000" );   // Dummy
            x.set_what( string_from_bstr( error_text ) );
            _task->set_error( *(const exception*)&x );
        }
    }
    catch( const exception&  x )  { hr = _set_excepinfo( x, Z_FUNCTION ); }
    catch( const _com_error& x )  { hr = _set_excepinfo( x, Z_FUNCTION ); }
    
    return hr;
}

//------------------------------------------------------------------------Com_task::get_Web_service

STDMETHODIMP Com_task::get_Web_service( Iweb_service** result )
{
    HRESULT hr = NOERROR;

    try
    {
        if( !_task )  return E_POINTER;

        *result = _task->web_service();
        (*result)->AddRef();
    }
    catch( const exception&  x )  { hr = _set_excepinfo( x, Z_FUNCTION ); }

    return hr;
}

//----------------------------------------------------------------Com_task::get_Web_service_or_null

STDMETHODIMP Com_task::get_Web_service_or_null( Iweb_service** result )
{
    HRESULT hr = NOERROR;

    try
    {
        if( !_task )  return E_POINTER;

        *result = _task->web_service_or_null();
        (*result)->AddRef();
    }
    catch( const exception&  x )  { hr = _set_excepinfo( x, Z_FUNCTION ); }

    return hr;
}

//-------------------------------------------------------------------------Com_task::get_Params_xml
// Für Process_module_instance::fill_process_environment_with_params()

STDMETHODIMP Com_task::get_Params_xml( BSTR* result )
{
    HRESULT hr = NOERROR;

    try
    {
        if( !_task )  return E_POINTER;

        hr = _task->params()->get_Xml( result );
    }
    catch( const exception&  x )  { hr = _set_excepinfo( x, Z_FUNCTION ); }

    return hr;
}

//-------------------------------------------------------------------------Com_task::put_Params_xml
// Für Process_module_instance::fill_process_environment_with_params()

STDMETHODIMP Com_task::put_Params_xml( BSTR xml_bstr )
{
    HRESULT hr = NOERROR;

    try
    {
        if( !_task )  return E_POINTER;

        hr = _task->params()->put_Xml( xml_bstr );
    }
    catch( const exception&  x )  { hr = _set_excepinfo( x, Z_FUNCTION ); }

    return hr;
}

//-------------------------------------------------------------------Com_task::get_Order_params_xml
// Für Process_module_instance::fill_process_environment_with_params()

STDMETHODIMP Com_task::get_Order_params_xml( BSTR* result )
{
    HRESULT hr = NOERROR;

    try
    {
        if( !_task )  return E_POINTER;
        *result = NULL;
        if( Order* order = _task->order() )  order->params()->get_Xml( result );
    }
    catch( const exception&  x )  { hr = _set_excepinfo( x, Z_FUNCTION ); }

    return hr;
}

STDMETHODIMP Com_task::get_Web_service_access_token(BSTR* result) {
    if (!_task) return E_POINTER;
    try {
        return String_to_bstr(_task->web_service_access_token(), result);
    } catch (const exception&  x)  {
        return _set_excepinfo(x, Z_FUNCTION);
    }
}

//-------------------------------------------------------------------Com_task::put_Order_params_xml
// Für Process_module_instance::fill_process_environment_with_params()

STDMETHODIMP Com_task::put_Order_params_xml( BSTR xml_bstr )
{
    HRESULT hr = NOERROR;

    try
    {
        if( !_task )  return E_POINTER;

        if( Order* order = _task->order() )  order->params()->put_Xml( xml_bstr );
    }
    catch( const exception&  x )  { hr = _set_excepinfo( x, Z_FUNCTION ); }

    return hr;
}

//-------------------------------------------------------------------------Com_task_proxy::_methods
// Dispid wie bei Com_task!

const Com_method Com_task_proxy::_methods[] =
{ 
#if defined COM_METHOD
    COM_METHOD      ( Com_task_proxy, 20, Create_subprocess     , VT_DISPATCH , 1, VT_BYREF|VT_VARIANT ),
    COM_PROPERTY_PUT( Com_task_proxy, 22, Priority_class        ,               0, VT_BSTR ),
    COM_PROPERTY_GET( Com_task_proxy, 22, Priority_class        , VT_BSTR     , 0 ),
    COM_PROPERTY_PUT( Com_task_proxy, 29, Priority              ,               0, VT_INT ),
    COM_PROPERTY_GET( Com_task_proxy, 29, Priority              , VT_INT      , 0 ),
    COM_PROPERTY_GET( Com_task_proxy, 18, Stderr_text           , VT_BSTR     , 0 ),
    COM_PROPERTY_GET( Com_task_proxy, 19, Stdout_text           , VT_BSTR     , 0 ),
    COM_PROPERTY_GET( Com_task_proxy, 24, Stderr_path           , VT_BSTR     , 0 ),
    COM_PROPERTY_GET( Com_task_proxy, 25, Stdout_path           , VT_BSTR     , 0 ),
#endif
    {}
};

//------------------------------------------------------------------Com_task_proxy::Create_instance

HRESULT Com_task_proxy::Create_instance( zschimmer::com::object_server::Session*, ptr<Object>*, const IID& iid, ptr<IUnknown>* result )
{
    if( iid == object_server::IID_Iproxy )
    {
        ptr<Com_task_proxy> instance = Z_NEW( Com_task_proxy );
        *result = static_cast<IDispatch*>( static_cast<Proxy*>( +instance ) );
        return S_OK;
    }

    return E_NOINTERFACE;
}

//-------------------------------------------------------------------Com_task_proxy::Com_task_proxy

Com_task_proxy::Com_task_proxy()
: 
    Proxy_with_local_methods( &class_descriptor ),
    _subprocess_register( Z_NEW( Subprocess_register ) )
{
}

//---------------------------------------------------------------------Com_task_proxy::set_property
    
void Com_task_proxy::set_property( const string& name, const Variant& value )
{
    if( name == "subprocess_own_process_group_default" )  _subprocess_own_process_group_default = value.as_bool();
    //else
    //if( name == "stdout_path" )  _stdout_path = value.as_string();
    //else
    //if( name == "stderr_path" )  _stderr_path = value.as_string();
    else  
        z::throw_xc( Z_FUNCTION, name );
}

//----------------------------------------------------------------Com_task_proxy::Create_subprocess

STDMETHODIMP Com_task_proxy::Create_subprocess( VARIANT* program_and_parameters, Isubprocess** result )
{
    return _subprocess_register->Create_subprocess( program_and_parameters, result, static_cast<Idispatch_implementation*>( this ) );
}

//---------------------------------------------------------------Com_task_proxy::put_Priority_class

STDMETHODIMP Com_task_proxy::put_Priority_class( BSTR priority_class_bstr )
{
    Z_COM_IMPLEMENT( hr = zschimmer::Process( getpid() ).put_Priority_class( priority_class_bstr ) );
}

//---------------------------------------------------------------Com_task_proxy::get_Priority_class

STDMETHODIMP Com_task_proxy::get_Priority_class( BSTR* result )
{
    Z_COM_IMPLEMENT( hr = zschimmer::Process( getpid() ).get_Priority_class( result ) );
}

//---------------------------------------------------------------------Com_task_proxy::put_Priority

STDMETHODIMP Com_task_proxy::put_Priority( int priority )
{
    Z_COM_IMPLEMENT( hr = zschimmer::Process( getpid() ).put_Priority( priority ) );
}

//---------------------------------------------------------------------Com_task_proxy::get_Priority

STDMETHODIMP Com_task_proxy::get_Priority( int* result )
{
    Z_COM_IMPLEMENT( hr = zschimmer::Process( getpid() ).get_Priority( result ) );
}

//------------------------------------------------------------------Com_task_proxy::get_Stderr_text

STDMETHODIMP Com_task_proxy::get_Stderr_text( BSTR* result )
{
    Z_LOG2( "scheduler", Z_FUNCTION << "()\n" );

    HRESULT hr = NOERROR;

    try
    {
        *result = NULL;

        if (_stderr_path != "")
            hr = String_to_bstr(string_from_file(_stderr_path), result);
        else
        if( xml::Element_ptr task_process_element = this->task_process_element() )
            hr = String_to_bstr( string_from_file( task_process_element.getAttribute( "stderr_path" ) ), result );
    }
    catch( const exception&  x )  { hr = Set_excepinfo( x, Z_FUNCTION ); }

    return hr;
}

//------------------------------------------------------------------Com_task_proxy::get_Stderr_path

STDMETHODIMP Com_task_proxy::get_Stderr_path( BSTR* result )
{
    Z_LOG2( "scheduler", Z_FUNCTION << "()\n" );

    HRESULT hr = NOERROR;

    try
    {
        *result = NULL;

        if (_stderr_path != "")
            hr = String_to_bstr(_stderr_path, result);
        else
        if( xml::Element_ptr task_process_element = this->task_process_element() )
            hr = String_to_bstr( task_process_element.getAttribute( "stderr_path" ), result );
    }
    catch( const exception&  x )  { hr = Set_excepinfo( x, Z_FUNCTION ); }

    return hr;
}

//------------------------------------------------------------------Com_task_proxy::get_Stdout_text

STDMETHODIMP Com_task_proxy::get_Stdout_text( BSTR* result )
{
    Z_LOG2( "scheduler", Z_FUNCTION << "()\n" );

    HRESULT hr = NOERROR;

    try {
        *result = NULL;
        hr = String_to_bstr(string_from_file(_stdout_path), result);
    }
    catch( const exception&  x )  { hr = Set_excepinfo( x, Z_FUNCTION ); }

    return hr;
}

//------------------------------------------------------------------Com_task_proxy::get_Stdout_path

STDMETHODIMP Com_task_proxy::get_Stdout_path( BSTR* result )
{
    Z_LOG2( "scheduler", Z_FUNCTION << "()\n" );

    HRESULT hr = NOERROR;

    try {
        *result = NULL;
        hr = String_to_bstr(_stdout_path, result);
    }
    catch( const exception&  x )  { hr = Set_excepinfo( x, Z_FUNCTION ); }

    return hr;
}

//-----------------------------------------Com_task_proxy::remote_module_instance_server_class_data

xml::Element_ptr Com_task_proxy::task_process_element()
{
    xml::Element_ptr result;

    Object* o = session()->server()->get_class_object_or_null( spooler_com::CLSID_Remote_module_instance_server );
    if( Com_remote_module_instance_server::Class_data* c = dynamic_cast<Com_remote_module_instance_server::Class_data*>( o ) )
    {
        result = c->_task_process_element;
    }

    return result;
}

//-----------------------------------------Com_task_proxy::remote_module_instance_server_class_data

//Com_remote_module_instance_server::Class_data* Com_task_proxy::remote_module_instance_server_class_data()
//{
//    Com_remote_module_instance_server::Class_data* result = NULL;
//
//    Object* o = session()->server()->get_class_object( spooler_com::CLSID_Remote_module_instance_server );
//    result = dynamic_cast<Com_remote_module_instance_server::Class_data*>( o ) );
//
//    return result;
//}

//------------------------------------------------------------Com_task_proxy::wait_for_subprocesses

void Com_task_proxy::wait_for_subprocesses()
{
    _subprocess_register->wait();
}

//----------------------------------------------------------------------------Com_spooler::_methods
#ifdef Z_COM

const Com_method Com_spooler::_methods[] =
{ 
   // _flags         , dispid, _name                        , _method                                           , _result_type  , _types        , _default_arg_count
    { DISPATCH_PROPERTYGET,  1, "log"                       , (Com_method_ptr)&Com_spooler::get_Log             , VT_DISPATCH  },
    { DISPATCH_PROPERTYGET,  2, "id"                        , (Com_method_ptr)&Com_spooler::get_Id              , VT_BSTR      },
    { DISPATCH_PROPERTYGET,  3, "param"                     , (Com_method_ptr)&Com_spooler::get_Param           , VT_BSTR      },
    { DISPATCH_PROPERTYGET,  4, "script"                    , (Com_method_ptr)&Com_spooler::get_Script          , VT_DISPATCH  },
    { DISPATCH_PROPERTYGET,  5, "job"                       , (Com_method_ptr)&Com_spooler::get_Job             , VT_DISPATCH  , { VT_BSTR } },
    { DISPATCH_METHOD     ,  6, "create_variable_set"       , (Com_method_ptr)&Com_spooler::Create_variable_set , VT_DISPATCH  },
    { DISPATCH_PROPERTYGET,  7, "include_path"              , (Com_method_ptr)&Com_spooler::get_Include_path    , VT_BSTR      },
    { DISPATCH_PROPERTYGET,  8, "log_dir"                   , (Com_method_ptr)&Com_spooler::get_Log_dir         , VT_BSTR      },
    { DISPATCH_METHOD     ,  9, "let_run_terminate_and_restart", (Com_method_ptr)&Com_spooler::Let_run_terminate_and_restart },
    { DISPATCH_PROPERTYGET, 10, "variables"                 , (Com_method_ptr)&Com_spooler::get_Variables       , VT_DISPATCH  },
    { DISPATCH_PROPERTYPUT,  0, "var"                       , (Com_method_ptr)&Com_spooler::put_Var             , VT_EMPTY     , { VT_BSTR, VT_BYREF|VT_VARIANT } },
    { DISPATCH_PROPERTYGET,  0, "var"                       , (Com_method_ptr)&Com_spooler::get_Var             , VT_VARIANT   , { VT_BSTR } },
    { DISPATCH_PROPERTYGET, 12, "db_name"                   , (Com_method_ptr)&Com_spooler::get_Db_name         , VT_BSTR      },
    { DISPATCH_METHOD     , 13, "create_job_chain"          , (Com_method_ptr)&Com_spooler::Create_job_chain    , VT_DISPATCH  },
    { DISPATCH_METHOD     , 14, "add_job_chain"             , (Com_method_ptr)&Com_spooler::Add_job_chain       , VT_EMPTY     , { VT_DISPATCH } },
    { DISPATCH_PROPERTYGET, 15, "job_chain"                 , (Com_method_ptr)&Com_spooler::get_Job_chain       , VT_DISPATCH  , { VT_BSTR } },
    { DISPATCH_METHOD     , 16, "create_order"              , (Com_method_ptr)&Com_spooler::Create_order        , VT_DISPATCH  },
    { DISPATCH_PROPERTYGET, 17, "is_service"                , (Com_method_ptr)&Com_spooler::get_Is_service      , VT_BOOL      },
    { DISPATCH_PROPERTYGET, 18, "java_class_name"           , (Com_method_ptr)&Com_spooler::get_Java_class_name , VT_BSTR      },
    { DISPATCH_PROPERTYGET, 19, "directory"                 , (Com_method_ptr)&Com_spooler::get_Directory       , VT_BSTR      },
    { DISPATCH_METHOD     , 20, "job_chain_exists"          , (Com_method_ptr)&Com_spooler::Job_chain_exists    , VT_BOOL       , { VT_BSTR } },
    { DISPATCH_PROPERTYGET, 21, "hostname"                  , (Com_method_ptr)&Com_spooler::get_Hostname        , VT_BSTR      },
    { DISPATCH_METHOD     , 22, "abort_immediately"         , (Com_method_ptr)&Com_spooler::Abort_immediately   , VT_EMPTY     },
    { DISPATCH_METHOD     , 23, "abort_immediately_and_restart", (Com_method_ptr)&Com_spooler::Abort_immediately_and_restart, VT_EMPTY },
    { DISPATCH_PROPERTYGET, 24, "Db_variables_table_name"   , (Com_method_ptr)&Com_spooler::get_Db_variables_table_name, VT_BSTR      },
    { DISPATCH_PROPERTYGET, 25, "Db_tasks_table_name"       , (Com_method_ptr)&Com_spooler::get_Db_tasks_table_name    , VT_BSTR      },
    { DISPATCH_PROPERTYGET, 26, "Db_orders_table_name"      , (Com_method_ptr)&Com_spooler::get_Db_orders_table_name   , VT_BSTR      },
    { DISPATCH_PROPERTYGET, 27, "Db_history_table_name"     , (Com_method_ptr)&Com_spooler::get_Db_history_table_name  , VT_BSTR      },
    { DISPATCH_PROPERTYGET, 28, "Db_order_history_table_name", (Com_method_ptr)&Com_spooler::get_Db_order_history_table_name, VT_BSTR      },
    { DISPATCH_PROPERTYGET, 29, "Ini_path"                  , (Com_method_ptr)&Com_spooler::get_Ini_path         , VT_BSTR      },
    { DISPATCH_METHOD     , 30, "Execute_xml"               , (Com_method_ptr)&Com_spooler::Execute_xml          , VT_BSTR      , { VT_BSTR } },
    { DISPATCH_PROPERTYGET, 31, "Tcp_port"                  , (Com_method_ptr)&Com_spooler::get_Tcp_port         , VT_INT       },
    { DISPATCH_PROPERTYGET, 32, "Udp_port"                  , (Com_method_ptr)&Com_spooler::get_Udp_port         , VT_INT       },
    { DISPATCH_METHOD     , 33, "Create_xslt_stylesheet"    , (Com_method_ptr)&Com_spooler::Create_xslt_stylesheet, VT_DISPATCH },
    { DISPATCH_METHOD     , 34, "Terminate"                 , (Com_method_ptr)&Com_spooler::Terminate            , VT_EMPTY     , { VT_BYREF|VT_VARIANT, VT_BYREF|VT_VARIANT, VT_BYREF|VT_VARIANT, VT_BYREF|VT_VARIANT }, 4 },
    { DISPATCH_METHOD     , 35, "Terminate_and_restart"     , (Com_method_ptr)&Com_spooler::Terminate_and_restart, VT_EMPTY     , { VT_BYREF|VT_VARIANT }, 1 },
    { DISPATCH_PROPERTYGET, 37, "Locks"                     , (Com_method_ptr)&Com_spooler::get_Locks            , VT_DISPATCH  },
    { DISPATCH_PROPERTYGET, 38, "Process_classes"           , (Com_method_ptr)&Com_spooler::get_Process_classes  , VT_DISPATCH  },
    { DISPATCH_PROPERTYGET, 39, "Supervisor_client"         , (Com_method_ptr)&Com_spooler::get_Supervisor_client, VT_DISPATCH  },
    { DISPATCH_PROPERTYGET, 40, "Configuration_directory"   , (Com_method_ptr)&Com_spooler::get_Configuration_directory, VT_BSTR },
    { DISPATCH_PROPERTYGET, 41, "Schedule"                  , (Com_method_ptr)&Com_spooler::get_Schedule         , VT_DISPATCH  , { VT_BSTR } },
    { DISPATCH_PROPERTYGET, 42, "Uri"                       , (Com_method_ptr)&Com_spooler::get_Uri       , VT_BSTR      },
    {}
};

#endif
//-------------------------------------------------------------------------Com_spooler::Com_spooler

Com_spooler::Com_spooler( Spooler* spooler )
:
    Sos_ole_object( spooler_class_ptr, (Ispooler*)this ),
    _spooler(spooler)
{
}

//----------------------------------------------------------------------Com_spooler::QueryInterface

STDMETHODIMP Com_spooler::QueryInterface( const IID& iid, void** result )
{
    Z_IMPLEMENT_QUERY_INTERFACE( this, iid, Ihas_java_class_name          , result );
    Z_IMPLEMENT_QUERY_INTERFACE( this, iid, Ihas_reference_with_properties, result );

    return Sos_ole_object::QueryInterface( iid, result );
}

//-----------------------------------------------------------------------------Com_spooler::get_Log

STDMETHODIMP Com_spooler::get_Log( Ilog** com_log )
{
    {
        if( !_spooler )  return E_POINTER;

        *com_log = _spooler->_com_log;
        if( *com_log )  (*com_log)->AddRef();
    }

    return NOERROR;
}

//----------------------------------------------------------------------------------Com_spooler::id

STDMETHODIMP Com_spooler::get_Id( BSTR* id_bstr )
{
    {
        if( !_spooler )  return E_POINTER;

        *id_bstr = SysAllocString_string( _spooler->id() );
    }

    return NOERROR;
}

//-------------------------------------------------------------------------------Com_spooler::param

STDMETHODIMP Com_spooler::get_Param( BSTR* param_bstr )
{
    {
        if( !_spooler )  return E_POINTER;

        *param_bstr = SysAllocString_string( _spooler->_spooler_param );
    }

    return NOERROR;
}

//---------------------------------------------------------------------------Com_spooler::get_script

STDMETHODIMP Com_spooler::get_Script( IDispatch** script_object )
{
    {
        if( !_spooler )  return E_POINTER;
        if( !_spooler->scheduler_script_subsystem()->default_scheduler_script_or_null() )  return E_ACCESSDENIED;

        *script_object = _spooler->scheduler_script_subsystem()->default_scheduler_script()->module_instance()->dispatch();
        if( *script_object )  (*script_object)->AddRef();
    }

    return NOERROR;
}

//-----------------------------------------------------------------------------Com_spooler::get_job

STDMETHODIMP Com_spooler::get_Job( BSTR job_name, Ijob** com_job )
{
    HRESULT hr = NOERROR;

    try
    {
        if( !_spooler )  return E_POINTER;
        *com_job = _spooler->job_subsystem()->job( Absolute_path( root_path, bstr_as_string( job_name ) ) )->com_job();
        (*com_job)->AddRef();
    }
    catch( const exception&  x )  { hr = _set_excepinfo( x, "Spooler.Spooler.job" ); }
    catch( const _com_error& x )  { hr = _set_excepinfo( x, "Spooler.Spooler.job" ); }

    return hr;
}

//-----------------------------------------------------------------Com_spooler::create_variable_set

STDMETHODIMP Com_spooler::Create_variable_set( Ivariable_set** result )
{
    {
        if( !_spooler )  return E_POINTER;

        *result = new Com_variable_set;
        (*result)->AddRef();
    }

    return NOERROR;
}

//--------------------------------------------------------------------Com_spooler::get_include_path

STDMETHODIMP Com_spooler::get_Include_path( BSTR* result )
{
    {
        if( !_spooler )  return E_POINTER;
        *result = SysAllocString_string( _spooler->_include_path );
    }

    return NOERROR;
}

//-------------------------------------------------------------------------Com_spooler::get_log_dir

STDMETHODIMP Com_spooler::get_Log_dir( BSTR* result )
{
    {
        if( !_spooler )  return E_POINTER;
        *result = SysAllocString_string( _spooler->_log_directory );
    }

    return NOERROR;
}

//-------------------------------------------------------Com_spooler::let_run_terminate_and_restart

STDMETHODIMP Com_spooler::Let_run_terminate_and_restart()
{
    {
        if( !_spooler )  return E_POINTER;

        _spooler->cmd_let_run_terminate_and_restart();
    }

    return NOERROR;
}

//-----------------------------------------------------------------------Com_spooler::get_variables

STDMETHODIMP Com_spooler::get_Variables( Ivariable_set** result )
{
    {
        if( !_spooler )  return E_POINTER;

        *result = _spooler->_variables;
        if( *result )  (*result)->AddRef();
    }

    return NOERROR;
}

//-----------------------------------------------------------------------------Com_spooler::put_var

STDMETHODIMP Com_spooler::put_Var( BSTR name, VARIANT* value )
{
    HRESULT  hr;

#   ifdef _DEBUG
        const char* crash_string = "*CRASH SCHEDULER*";
        static int  dummy;

        if( ( name == NULL || SysStringLen(name) == 0 )  &&  string_from_variant(*value) == crash_string )
        {
            _spooler->log()->error( "spooler.var(\"\")=\"" + string(crash_string) + "\" WILL LET CRASH JOBSCHEDULER NOW!" );
            dummy = *(int*)NULL;
        }
#   endif

    ptr<Ivariable_set> variables;

    hr = get_Variables( variables.pp() );  if( FAILED(hr) )  return hr;

    return variables->put_Var( name, value );
}

//-----------------------------------------------------------------------------Com_spooler::get_var

STDMETHODIMP Com_spooler::get_Var( BSTR name, VARIANT* value )
{
    HRESULT hr;
    ptr<Ivariable_set> variables;

    hr = get_Variables( variables.pp() );  if( FAILED(hr) )  return hr;

    return variables->get_Var( name, value );
}

//-------------------------------------------------------------------------Com_spooler::get_db_name

STDMETHODIMP Com_spooler::get_Db_name( BSTR* result )
{
    {
        if( !_spooler )  return E_POINTER;
        *result = SysAllocString_string( _spooler->settings()->_db_name );
    }

    return NOERROR;
}

//--------------------------------------------------------------------Com_spooler::create_job_chain

STDMETHODIMP Com_spooler::Create_job_chain( spooler_com::Ijob_chain** result )
{
    ptr<Job_chain> job_chain = new Job_chain( _spooler );

    job_chain->set_folder_path( root_path );

    *result = job_chain;
    (*result)->AddRef();

    return S_OK;
}

//-----------------------------------------------------------------------Com_spooler::add_job_chain

STDMETHODIMP Com_spooler::Add_job_chain( spooler_com::Ijob_chain* ijob_chain )
{
    HRESULT hr = NOERROR;

    try
    {
        if( !_spooler )  return E_POINTER;

        Job_chain* job_chain = dynamic_cast<Job_chain*>( ijob_chain );
        if( !job_chain)  return E_POINTER;

        //job_chain->set_defined();
        Folder* folder = _spooler->root_folder();
        job_chain->set_folder_path( folder->path() );
        job_chain->initialize();
        folder->job_chain_folder()->add_job_chain( job_chain );
        job_chain->activate();
    }
    catch( const exception&  x )  { hr = _set_excepinfo( x, "Spooler.add_job_chain" ); }
    catch( const _com_error& x )  { hr = _set_excepinfo( x, "Spooler.add_job_chain" ); }

    return hr;
}

//-----------------------------------------------------------------------Com_spooler::get_job_chain

STDMETHODIMP Com_spooler::get_Job_chain( BSTR name, spooler_com::Ijob_chain** result )
{
    HRESULT hr = NOERROR;

    try
    {
        if( !_spooler )  return E_POINTER;

        *result = _spooler->order_subsystem()->job_chain( Absolute_path( root_path, string_from_bstr(name) ) ); //->com_job_chain();
        (*result)->AddRef();
    }
    catch( const exception&  x )  { hr = _set_excepinfo( x, "Spooler.get_job_chain" ); }
    catch( const _com_error& x )  { hr = _set_excepinfo( x, "Spooler.get_job_chain" ); }

    return hr;
}

//----------------------------------------------------------------------------Spooler::create_order

STDMETHODIMP Com_spooler::Create_order( Iorder** result )
{
    HRESULT hr = NOERROR;

    try
    {
        if( !_spooler )  return E_POINTER;

        *result = _spooler->standing_order_subsystem()->new_order().copy();
    }
    catch( const exception&  x )  { hr = _set_excepinfo( x, "Spooler.get_job_chain" ); }
    catch( const _com_error& x )  { hr = _set_excepinfo( x, "Spooler.get_job_chain" ); }

    return hr;
}

//--------------------------------------------------------------------------scheduler::get_is_service

STDMETHODIMP Com_spooler::get_Is_service( VARIANT_BOOL* result )
{
    HRESULT hr = NOERROR;

    {
        if( !_spooler )  return E_POINTER;

        *result = _spooler->is_service();
    }

    return hr;
}

//---------------------------------------------------------------------------scheduler::get_directory

STDMETHODIMP Com_spooler::get_Directory( BSTR* result )
{
    HRESULT hr = NOERROR;

    {
        if( !_spooler )  return E_POINTER;

        hr = String_to_bstr( _spooler->directory(), result );
    }

    return hr;
}

//-----------------------------------------------------------------------Com_spooler::get_hostname

STDMETHODIMP Com_spooler::get_Hostname( BSTR* result )
{
    HRESULT hr = NOERROR;

    try
    {
        if( !_spooler )  return E_POINTER;

        hr = String_to_bstr( _spooler->_short_hostname, result );
    }
    catch( const exception&  x )  { hr = _set_excepinfo( x, "Spooler.get_hostname" ); }
    catch( const _com_error& x )  { hr = _set_excepinfo( x, "Spooler.get_hostname" ); }

    return hr;
}

//--------------------------------------------------------------------Com_spooler::job_chain_exists

STDMETHODIMP Com_spooler::Job_chain_exists( BSTR name, VARIANT_BOOL* result )
{
    HRESULT hr = NOERROR;

    try
    {
        if( !_spooler )  return E_POINTER;

        *result = _spooler->order_subsystem()->job_chain_or_null( Absolute_path( root_path, name ) ) != NULL;
    }
    catch( const exception&  x )  { hr = _set_excepinfo( x, "Spooler.job_chain_exists" ); }
    catch( const _com_error& x )  { hr = _set_excepinfo( x, "Spooler.job_chain_exists" ); }

    return hr;
}

//--------------------------------------------------------------------------------My_variant_to_int
// Liefert min(v,INT_MAX)

static HRESULT My_variant_to_int( const VARIANT& v, int* result )
{
    if( !variant_is_missing( v ) )
    {
        if( !variant_is_int64( v ) )  return DISP_E_BADVARTYPE;
        *result = (int)min( int64_from_variant( v ), (int64)INT_MAX );
    }

    return S_OK;
}

//---------------------------------------------------------------------------Com_spooler::Terminate

STDMETHODIMP Com_spooler::Terminate( VARIANT* timeout_v, VARIANT* restart_v, VARIANT* all_schedulers_v, VARIANT* continue_exclusive_operation_v )
{
    HRESULT hr = S_OK;

    if( !_spooler )  return E_POINTER;

    try
    {
        int    timeout                      = INT_MAX;
        bool   restart                      = false;
        string continue_exclusive_operation;
        bool   all_schedulers               = false;

                           hr = My_variant_to_int ( *timeout_v       , &timeout        );
        if( !FAILED(hr) )  hr =    Variant_to_bool( *restart_v       , &restart        );
        
        if( !FAILED(hr) )
        {
            bool b = false;     // Default
            hr = Variant_to_bool( *continue_exclusive_operation_v, &b );
            if( !FAILED(hr) )  
            {
                continue_exclusive_operation = b? cluster::continue_exclusive_any 
                                                : cluster::continue_exclusive_non_backup;
            }
            else
            {
                hr = S_OK;
                continue_exclusive_operation = string_from_variant( *continue_exclusive_operation_v );
            }
        }

        if( !FAILED(hr) )  hr = Variant_to_bool( *all_schedulers_v, &all_schedulers );

        if( !FAILED(hr) )  _spooler->cmd_terminate( restart, timeout, continue_exclusive_operation, all_schedulers );
    }
    catch( const exception&  x )  { hr = _set_excepinfo( x, Z_FUNCTION ); }
    catch( const _com_error& x )  { hr = _set_excepinfo( x, Z_FUNCTION ); }

    return hr;
}

//---------------------------------------------------------------Com_spooler::Terminate_and_restart

STDMETHODIMP Com_spooler::Terminate_and_restart( VARIANT* timeout_vt )
{
    HRESULT hr = S_OK;

    if( !_spooler )  return E_POINTER;

    try
    {
        int timeout = INT_MAX;
        hr = My_variant_to_int( *timeout_vt, &timeout );
        if( FAILED(hr) )  return hr;

        _spooler->cmd_terminate_and_restart( timeout );
    }
    catch( const exception&  x )  { hr = _set_excepinfo( x, Z_FUNCTION ); }
    catch( const _com_error& x )  { hr = _set_excepinfo( x, Z_FUNCTION ); }

    return hr;
}

//-------------------------------------------------------------------Com_spooler::abort_immediately

STDMETHODIMP Com_spooler::Abort_immediately()
{
    HRESULT hr = S_OK;

    if( !_spooler )  return E_POINTER;

    try
    {
        _spooler->abort_immediately( false );
    }
    catch( const exception&  x )  { hr = _set_excepinfo( x, Z_FUNCTION ); }
    catch( const _com_error& x )  { hr = _set_excepinfo( x, Z_FUNCTION ); }

    return hr;
}

//-------------------------------------------------------Com_spooler::abort_immediately_and_restart

STDMETHODIMP Com_spooler::Abort_immediately_and_restart()
{
    HRESULT hr = S_OK;

    if( !_spooler )  return E_POINTER;

    try
    {
        _spooler->abort_immediately( true );
    }
    catch( const exception&  x )  { hr = _set_excepinfo( x, Z_FUNCTION ); }
    catch( const _com_error& x )  { hr = _set_excepinfo( x, Z_FUNCTION ); }

    return hr;
}

//---------------------------------------------------------Com_spooler::get_Db_variables_table_name

STDMETHODIMP Com_spooler::get_Db_variables_table_name( BSTR* result )                    
{ 
    if( !_spooler )  return E_POINTER;
    return String_to_bstr( _spooler->db()->_variables_tablename, result ); 
}

//-------------------------------------------------------------Com_spooler::get_Db_tasks_table_name

STDMETHODIMP Com_spooler::get_Db_tasks_table_name( BSTR* result )
{ 
    if( !_spooler )  return E_POINTER;
    return String_to_bstr( _spooler->db()->_tasks_tablename, result ); 
}

//------------------------------------------------------------Com_spooler::get_Db_orders_table_name

STDMETHODIMP Com_spooler::get_Db_orders_table_name( BSTR* result )                    
{ 
    if( !_spooler )  return E_POINTER;
    return String_to_bstr( _spooler->db()->_orders_tablename, result ); 
}

//-----------------------------------------------------------Com_spooler::get_Db_history_table_name

STDMETHODIMP Com_spooler::get_Db_history_table_name( BSTR* result )                    
{ 
    if( !_spooler )  return E_POINTER;
    return String_to_bstr( _spooler->db()->_job_history_tablename, result ); 
}

//-----------------------------------------------------Com_spooler::get_Db_order_history_table_name

STDMETHODIMP Com_spooler::get_Db_order_history_table_name( BSTR* result )                    
{ 
    if( !_spooler )  return E_POINTER;
    return String_to_bstr( _spooler->db()->_order_history_tablename, result ); 
}

//------------------------------------------------------------------------Com_spooler::get_Ini_path

STDMETHODIMP Com_spooler::get_Ini_path( BSTR* result )                    
{ 
    if( !_spooler )  return E_POINTER;
    return String_to_bstr( _spooler->_factory_ini, result ); 
}

//-------------------------------------------------------------------------Com_spooler::Execute_xml

STDMETHODIMP Com_spooler::Execute_xml( BSTR xml, BSTR* result )                    
{ 
    HRESULT hr = S_OK;

    if( !_spooler )  return E_POINTER;

    try
    {
        //TODO Spooler::execute_xml() nutzen!
        Command_processor cp ( _spooler, Security::seclev_all );
        hr = String_to_bstr( cp.execute_xml_string( string_from_bstr( xml ) ), result );
    }
    catch( const exception&  x )  { hr = _set_excepinfo( x, Z_FUNCTION ); }
    catch( const _com_error& x )  { hr = _set_excepinfo( x, Z_FUNCTION ); }

    return hr;
}

//------------------------------------------------------------------------Com_spooler::get_Tcp_port

STDMETHODIMP Com_spooler::get_Tcp_port( int* result )
{
    if( !_spooler )  return E_POINTER;
    *result = _spooler->_tcp_port;

    return S_OK;
}

//------------------------------------------------------------------------Com_spooler::get_Udp_port

STDMETHODIMP Com_spooler::get_Udp_port( int* result )
{
    if( !_spooler )  return E_POINTER;
    *result = _spooler->_udp_port;

    return S_OK;
}

//--------------------------------------------------------------Com_spooler::Create_xslt_stylesheet

HRESULT Com_spooler::Create_xslt_stylesheet( Ixslt_stylesheet** result )
{
    HRESULT hr = S_OK;

    if( !_spooler )  return E_POINTER;

    try
    {
        ptr<Xslt_stylesheet>  stylesheet = Z_NEW( Xslt_stylesheet );
        *result = stylesheet.take();
    }
    catch( const exception&  x )  { hr = _set_excepinfo( x, Z_FUNCTION ); }

    return hr;
}

//------------------------------------------------------------Com_spooler::put_Suspend_after_resume

HRESULT Com_spooler::put_Suspend_after_resume( VARIANT_BOOL b )
{
    HRESULT hr = S_OK;

    if( !_spooler )  return E_POINTER;

    try
    {
        if( _spooler->_zschimmer_mode )  _spooler->_suspend_after_resume = b != 0;
    }
    catch( const exception&  x )  { hr = _set_excepinfo( x, Z_FUNCTION ); }

    return hr;
}

//---------------------------------------------------------------------------Com_spooler::Terminate

STDMETHODIMP Com_spooler::get_Locks( Ilocks** result )
{
    HRESULT hr = S_OK;

    if( !_spooler )  return E_POINTER;

    try
    {
        *result = _spooler->lock_subsystem();
        if( *result )  (*result)->AddRef();
    }
    catch( const exception&  x )  { hr = _set_excepinfo( x, Z_FUNCTION ); }

    return hr;
}

//-----------------------------------------------------------------Com_spooler::get_Process_classes

STDMETHODIMP Com_spooler::get_Process_classes( Iprocess_classes** result )
{
    HRESULT hr = S_OK;

    if( !_spooler )  return E_POINTER;

    try
    {
        *result = _spooler->process_class_subsystem();
        if( *result )  (*result)->AddRef();
    }
    catch( const exception&  x )  { hr = _set_excepinfo( x, Z_FUNCTION ); }

    return hr;
}

//---------------------------------------------------------------Com_spooler::get_Supervisor_client

STDMETHODIMP Com_spooler::get_Supervisor_client( Isupervisor_client** result )
{
    HRESULT hr = S_OK;

    if( !_spooler )  return E_POINTER;

    try
    {
        *result = _spooler->supervisor_client();
        if( *result )  (*result)->AddRef();
    }
    catch( const exception&  x )  { hr = _set_excepinfo( x, Z_FUNCTION ); }

    return hr;
}

//---------------------------------------------------------Com_spooler::get_Configuration_directory

STDMETHODIMP Com_spooler::get_Configuration_directory( BSTR* result )
{
    HRESULT hr = NOERROR;

    if( !_spooler )  return E_POINTER;

    try
    {
        string dir = _spooler->_configuration_directories[ confdir_local ];
        
        if( string_ends_with( dir, "/" ) ||
            string_ends_with( dir, Z_DIR_SEPARATOR ) )  dir.erase( dir.length() - 1 );

        hr = String_to_bstr( dir, result );
    }
    catch( const exception& x )  { hr = _set_excepinfo( x, Z_FUNCTION ); }

    return hr;
}

//------------------------------------------------------------------------Com_spooler::get_Schedule

STDMETHODIMP Com_spooler::get_Schedule( BSTR path_bstr, Ischedule** result )
{
    if( !_spooler )  return E_POINTER;

    HRESULT hr = S_OK;

    try
    {
        *result = _spooler->schedule_subsystem()->schedule_or_null( Absolute_path( root_path, string_from_bstr( path_bstr ) ) );
        if( *result )  (*result)->AddRef();
    }
    catch( const exception&  x )  { hr = Set_excepinfo( x, Z_FUNCTION ); }
    catch( const _com_error& x )  { hr = Set_excepinfo( x, Z_FUNCTION ); }

    return hr;
}

STDMETHODIMP Com_spooler::get_Uri(BSTR* result) {
    if (!_spooler)  return E_POINTER;
    try {
        return String_to_bstr(_spooler->schedulerJ().uri(), result);
    } catch (const exception&  x)  {
        return Set_excepinfo(x, Z_FUNCTION);
    }
}

//----------------------------------------------------------------------Com_spooler_proxy::_methods

// dispids wie bei Com_spooler::_methods!

const Com_method Com_spooler_proxy::_methods[] =
{ 
#ifdef Z_COM
    COM_PROPERTY_GET( Com_spooler_proxy,  7, Include_path          , VT_BSTR     , 0 ),
    COM_PROPERTY_GET( Com_spooler_proxy,  8, Log_dir               , VT_BSTR     , 0 ),
    COM_PROPERTY_GET( Com_spooler_proxy, 19, Directory             , VT_BSTR     , 0 ),
    COM_PROPERTY_GET( Com_spooler_proxy, 29, Ini_path              , VT_BSTR     , 0 ),
    COM_METHOD      ( Com_spooler_proxy, 33, Create_xslt_stylesheet, VT_DISPATCH , 0 ),
#endif
    {}
};

//---------------------------------------------------------------Com_spooler_proxy::Create_instance

HRESULT Com_spooler_proxy::Create_instance( zschimmer::com::object_server::Session*, ptr<Object>*, const IID& iid, ptr<IUnknown>* result )
{
    if( iid == object_server::IID_Iproxy )
    {
        ptr<Com_spooler_proxy> instance = Z_NEW( Com_spooler_proxy );
        *result = static_cast<IDispatch*>( static_cast<Proxy*>( +instance ) );
        return S_OK;
    }

    return E_NOINTERFACE;
}

//-------------------------------------------------------------Com_spooler_proxy::Com_spooler_proxy

Com_spooler_proxy::Com_spooler_proxy()
: 
    Proxy_with_local_methods( &class_descriptor )
{
}

//--------------------------------------------------------------Com_spooler_proxy::get_Include_path
    
STDMETHODIMP Com_spooler_proxy::get_Include_path( BSTR* result )                    
{ 
    HRESULT hr = E_FAIL;

    Object* o = session()->server()->get_class_object_or_null( spooler_com::CLSID_Remote_module_instance_server );
    if( Com_remote_module_instance_server::Class_data* c = dynamic_cast<Com_remote_module_instance_server::Class_data*>( o ) )
    {
        hr = String_to_bstr( c->_task_process_element.getAttribute( "scheduler.include_path" ), result ); 
    }

    return hr;
}

//-------------------------------------------------------------------Com_spooler_proxy::get_Log_dir

STDMETHODIMP Com_spooler_proxy::get_Log_dir( BSTR* result )                    
{ 
    HRESULT hr = E_FAIL;

    Object* o = session()->server()->get_class_object_or_null( spooler_com::CLSID_Remote_module_instance_server );
    if( Com_remote_module_instance_server::Class_data* c = dynamic_cast<Com_remote_module_instance_server::Class_data*>( o ) )
    {
        hr = String_to_bstr( c->_task_process_element.getAttribute( "scheduler.log_dir" ), result ); 
    }

    return hr;
}

//-----------------------------------------------------------------Com_spooler_proxy::get_Directory

STDMETHODIMP Com_spooler_proxy::get_Directory( BSTR* result )                    
{ 
    HRESULT hr = E_FAIL;

    Object* o = session()->server()->get_class_object_or_null( spooler_com::CLSID_Remote_module_instance_server );
    if( Com_remote_module_instance_server::Class_data* c = dynamic_cast<Com_remote_module_instance_server::Class_data*>( o ) )
    {
        hr = String_to_bstr( c->_task_process_element.getAttribute( "scheduler.directory" ), result ); 
    }

    return hr;
}

//------------------------------------------------------------------Com_spooler_proxy::get_Ini_path
    
STDMETHODIMP Com_spooler_proxy::get_Ini_path( BSTR* result )                    
{ 
    HRESULT hr = E_FAIL;

    Object* o = session()->server()->get_class_object_or_null( spooler_com::CLSID_Remote_module_instance_server );
    if( Com_remote_module_instance_server::Class_data* c = dynamic_cast<Com_remote_module_instance_server::Class_data*>( o ) )
    {
        hr = String_to_bstr( c->_task_process_element.getAttribute( "scheduler.ini_path" ), result ); 
    }

    return hr;
}

//----------------------------------------------------------------Com_task_proxy::Create_subprocess

STDMETHODIMP Com_spooler_proxy::Create_xslt_stylesheet( spooler_com::Ixslt_stylesheet** result )
{
    HRESULT hr = S_OK;

    try
    {
        ptr<Xslt_stylesheet>  stylesheet = Z_NEW( Xslt_stylesheet );
        *result = stylesheet.take();
    }
    catch( const exception&  x )  { hr = Set_excepinfo( x, Z_FUNCTION ); }

    return hr;
}

//----------------------------------------------------------------------------Com_context::_methods
#ifdef Z_COM

const Com_method Com_context::_methods[] =
{ 
   // _flags         , dispid, _name                        , _method                                           , _result_type  , _types        , _default_arg_count
    { DISPATCH_PROPERTYGET,  1, "log"                       , (Com_method_ptr)&Com_context::get_Log             , VT_DISPATCH  },
    { DISPATCH_PROPERTYGET,  2, "spooler"                   , (Com_method_ptr)&Com_context::get_Spooler         , VT_DISPATCH  },
  //{ DISPATCH_PROPERTYGET,  3, "thread"                    , (Com_method_ptr)&Com_context::get_Thread          , VT_DISPATCH  },
    { DISPATCH_PROPERTYGET,  4, "job"                       , (Com_method_ptr)&Com_context::get_Job             , VT_DISPATCH  },
    { DISPATCH_PROPERTYGET,  5, "task"                      , (Com_method_ptr)&Com_context::get_Task            , VT_DISPATCH  },
    {}
};

#endif
//-------------------------------------------------------------------------Com_context::Com_context

Com_context::Com_context()
: 
    Sos_ole_object( spooler_context_class_ptr, this )
{
}

//-------------------------------------------------------------------------------Com_context::close

void Com_context::close()
{ 
    {
        _log     = NULL;
        _spooler = NULL; 
      //_thread  = NULL; 
        _job     = NULL; 
      //if( _task )  _task->set_task(NULL), _task = NULL; 
        _task = NULL;
    }
}

//--------------------------------------------------------------------------Com_job_chain::_methods
#ifdef COM_METHOD

const Com_method Com_job_chain::_methods[] =
{ 
   // _flags         , dispid, _name                        , _method                                            , _result_type  , _types        , _default_arg_count
    { DISPATCH_PROPERTYPUT,  1, "name"                      , (Com_method_ptr)&Com_job_chain::put_Name           , VT_EMPTY      , { VT_BSTR } },
    { DISPATCH_PROPERTYGET,  1, "name"                      , (Com_method_ptr)&Com_job_chain::get_Name           , VT_BSTR       },
    { DISPATCH_PROPERTYGET,  2, "order_count"               , (Com_method_ptr)&Com_job_chain::get_Order_count    , VT_INT        },
    { DISPATCH_METHOD     ,  3, "add_job"                   , (Com_method_ptr)&Com_job_chain::Add_job            , VT_EMPTY      , { VT_VARIANT|VT_BYREF, VT_VARIANT|VT_BYREF, VT_VARIANT|VT_BYREF, VT_VARIANT|VT_BYREF }, 3 },
    { DISPATCH_METHOD     ,  4, "add_end_state"             , (Com_method_ptr)&Com_job_chain::Add_end_state      , VT_EMPTY      , { VT_VARIANT|VT_BYREF } },
    { DISPATCH_METHOD     ,  5, "add_order"                 , (Com_method_ptr)&Com_job_chain::Add_order          , VT_DISPATCH   , { VT_VARIANT|VT_BYREF } },
    { DISPATCH_PROPERTYGET,  6, "node"                      , (Com_method_ptr)&Com_job_chain::get_Node           , VT_DISPATCH   , { VT_VARIANT|VT_BYREF } },
    { DISPATCH_PROPERTYGET,  7, "order_queue"               , (Com_method_ptr)&Com_job_chain::get_Order_queue    , VT_DISPATCH   , { VT_VARIANT|VT_BYREF } },
    { DISPATCH_PROPERTYGET,  8, "java_class_name"           , (Com_method_ptr)&Com_job_chain::get_Java_class_name, VT_BSTR },
    { DISPATCH_PROPERTYPUT,  9, "Orders_recoverable"        , (Com_method_ptr)&Com_job_chain::put_Orders_recoverable, VT_EMPTY, { VT_BOOL } },
    { DISPATCH_PROPERTYGET,  9, "Orders_recoverable"        , (Com_method_ptr)&Com_job_chain::get_Orders_recoverable, VT_BOOL },
    { DISPATCH_METHOD     , 10, "remove_all_pending_orders" , (Com_method_ptr)&Com_job_chain::Remove_all_pending_orders   , VT_INT  },
    { DISPATCH_METHOD     , 11, "Try_add_order"             , (Com_method_ptr)&Com_job_chain::Try_add_order      , VT_BOOL       , { VT_DISPATCH } },
    { DISPATCH_METHOD     , 12, "Add_or_replace_order"      , (Com_method_ptr)&Com_job_chain::Add_or_replace_order, VT_EMPTY     , { VT_DISPATCH } },
    { DISPATCH_METHOD     , 13, "Remove"                    , (Com_method_ptr)&Com_job_chain::Remove             , VT_EMPTY      },
    { DISPATCH_PROPERTYPUT, 14, "Title"                     , (Com_method_ptr)&Com_job_chain::put_Title          , VT_EMPTY      , { VT_BSTR } },
    { DISPATCH_PROPERTYGET, 14, "Title"                     , (Com_method_ptr)&Com_job_chain::get_Title          , VT_BSTR },
    { DISPATCH_PROPERTYGET, 15, "Path"                      , (Com_method_ptr)&Com_job_chain::get_Path           , VT_BSTR },
    { DISPATCH_PROPERTYGET, 16, "States"                    , (Com_method_ptr)&Com_job_chain::get_States         , (VARENUM)(VT_BSTR|VT_ARRAY) },
    {}
};

#endif
//---------------------------------------------------------------------Com_job_chain::Com_job_chain

Com_job_chain::Com_job_chain( Job_chain* job_chain )
:
    Sos_ole_object( job_chain_class_ptr, (Ijob_chain*)this ),
    _job_chain(job_chain)
{
}

//--------------------------------------------------------------------Com_job_chain::QueryInterface

STDMETHODIMP Com_job_chain::QueryInterface( const IID& iid, void** result )
{
    Z_IMPLEMENT_QUERY_INTERFACE( this, iid, Ihas_java_class_name, result );

    return Sos_ole_object::QueryInterface( iid, result );
}

//--------------------------------------------------------------------------Com_job_chain::put_name

STDMETHODIMP Com_job_chain::put_Name( BSTR name_bstr )
{
    HRESULT hr = NOERROR;

    try
    {
        if( !_job_chain )  return E_POINTER;

        _job_chain->set_name( string_from_bstr( name_bstr ) );
    }
    catch( const exception&  x )  { hr = _set_excepinfo( x, "Spooler.Job_chain.name" ); }
    catch( const _com_error& x )  { hr = _set_excepinfo( x, "Spooler.Job_chain.name" ); }

    return hr;
}

//--------------------------------------------------------------------------Com_job_chain::get_name

STDMETHODIMP Com_job_chain::get_Name( BSTR* result )
{
    HRESULT hr = NOERROR;

    try
    {
        if( !_job_chain )  return E_POINTER;

        *result = bstr_from_string( _job_chain->name() );
    }
    catch( const exception&  x )  { hr = _set_excepinfo( x, "Spooler.Job_chain.name" ); }
    catch( const _com_error& x )  { hr = _set_excepinfo( x, "Spooler.Job_chain.name" ); }

    return hr;
}

//-------------------------------------------------------------------Com_job_chain::get_order_count

STDMETHODIMP Com_job_chain::get_Order_count( int* result )
{
    HRESULT hr = NOERROR;

    try
    {
        if( !_job_chain )  return E_POINTER;

        for( Retry_transaction ta ( _job_chain->db() ); ta.enter_loop(); ta++ ) try
        {
            *result = _job_chain->order_count( &ta );
        }
        catch( exception& x ) { ta.reopen_database_after_error( x, Z_FUNCTION ); }
    }
    catch( const exception&  x )  { hr = _set_excepinfo( x, Z_FUNCTION ); }
    catch( const _com_error& x )  { hr = _set_excepinfo( x, Z_FUNCTION ); }

    return hr;
}

//---------------------------------------------------------------------------Com_job_chain::add_job

STDMETHODIMP Com_job_chain::Add_job( VARIANT* job_or_jobname, VARIANT* begin_state, VARIANT* end_state, VARIANT* error_state )
{
    HRESULT hr = NOERROR;

    try
    {
        if( !_job_chain )  return E_POINTER;

        Job* job = NULL;

        switch( job_or_jobname->vt )
        {
            case VT_BSTR:
            {
                string jobname = string_from_variant(*job_or_jobname);
                
                job = jobname == ""  ||  stricmp( jobname.c_str(), "*end" ) == 0? NULL
                                                                                : _job_chain->_spooler->job_subsystem()->job( Absolute_path( root_path, jobname ) );
                break;
            }

          //case VT_UNKNOWN:
          //case VT_DISPATCH:
          //{
          //    break;
          //}

            default: return DISP_E_TYPEMISMATCH;
        }

        _job_chain->add_job_node( job->path(), *begin_state, *end_state, *error_state );
    }
    catch( const exception&  x )  { hr = _set_excepinfo( x, "Spooler.Job_chain.add_job" ); }
    catch( const _com_error& x )  { hr = _set_excepinfo( x, "Spooler.Job_chain.add_job" ); }

    return hr;
}

//---------------------------------------------------------------------Com_job_chain::add_end_state

STDMETHODIMP Com_job_chain::Add_end_state( VARIANT* state )
{
    HRESULT hr = NOERROR;

    try
    {
        if( !_job_chain )  return E_POINTER;

        VARIANT error; VariantInit( &error );  error.vt = VT_ERROR;  error.scode = DISP_E_PARAMNOTFOUND;
        _job_chain->add_end_node( *state );
    }
    catch( const exception&  x )  { hr = _set_excepinfo( x, "Spooler.Job_chain.add_end_state" ); }
    catch( const _com_error& x )  { hr = _set_excepinfo( x, "Spooler.Job_chain.add_end_state" ); }

    return hr;
}

//-------------------------------------------------------------------------Com_job_chain::add_order

STDMETHODIMP Com_job_chain::Add_order( VARIANT* order_or_payload, spooler_com::Iorder** result )
{
    HRESULT hr = NOERROR;

    //Z_LOGI2( "scheduler", "Job_chain.add_order\n" );

    try
    {
        if( !_job_chain )  return E_POINTER;
        if (!_job_chain->is_loaded_or_active()) z::throw_xc("SCHEDULER-151");

        ptr<spooler_com::Iorder> iorder = order_from_order_or_payload( _job_chain->_spooler, *order_or_payload );
        if( !iorder )  return E_POINTER;

        Order* order = dynamic_cast<Order*>( &*iorder );
        if( !order )  return E_INVALIDARG;

        // Einstieg nur über Order, damit Semaphoren stets in derselben Reihenfolge gesperrt werden.
        order->place_in_job_chain( dynamic_cast<Job_chain*>( this ) );  
        //dynamic_cast<Job_chain*>( this )->add_order( order );
        

        *result = iorder;
        (*result)->AddRef();
    }
    catch( const exception&  x )  { hr = _set_excepinfo( x, "Spooler.Job_chain.add_order" ); }
    catch( const _com_error& x )  { hr = _set_excepinfo( x, "Spooler.Job_chain.add_order" ); }

    //Z_LOG2( "scheduler", "Job_chain.add_order  hr=" << (void*)hr << "\n" );

    return hr;
}

//--------------------------------------------------------------Com_job_chain::Add_or_replace_order

STDMETHODIMP Com_job_chain::Add_or_replace_order( spooler_com::Iorder* iorder )
{
    HRESULT hr = NOERROR;

    try
    {
        if( !_job_chain )  return E_POINTER;
        if (!_job_chain->is_loaded_or_active()) z::throw_xc("SCHEDULER-151");

        Order* order = dynamic_cast<Order*>( &*iorder );
        if( !order )  return E_INVALIDARG;

        // Einstieg nur über Order, damit Semaphoren stets in derselben Reihenfolge gesperrt werden.
        order->place_or_replace_in_job_chain( dynamic_cast<Job_chain*>( this ) );  
    }
    catch( const exception&  x )  { hr = _set_excepinfo( x, Z_FUNCTION ); }
    catch( const _com_error& x )  { hr = _set_excepinfo( x, Z_FUNCTION ); }

    //Z_LOG2( "scheduler", "Job_chain.add_order  hr=" << (void*)hr << "\n" );

    return hr;
}

//---------------------------------------------------------------------Com_job_chain::Try_add_order

STDMETHODIMP Com_job_chain::Try_add_order( Iorder* iorder, VARIANT_BOOL* result )
{
    HRESULT hr = NOERROR;

    try
    {
        if( !_job_chain )  return E_POINTER;
        if (!_job_chain->is_loaded_or_active()) z::throw_xc("SCHEDULER-151");
        if( !iorder )  return E_POINTER;

        Order* order = dynamic_cast<Order*>( &*iorder );
        if( !order )  return E_INVALIDARG;

        // Einstieg nur über Order, damit Semaphoren stets in derselben Reihenfolge gesperrt werden.
        *result = order->try_place_in_job_chain( dynamic_cast<Job_chain*>( this ) )? VARIANT_FALSE : VARIANT_TRUE;  
    }
    catch( const exception&  x )  { hr = _set_excepinfo( x, Z_FUNCTION ); }
    catch( const _com_error& x )  { hr = _set_excepinfo( x, Z_FUNCTION ); }

    //Z_LOG2( "scheduler", "Job_chain.add_order  hr=" << (void*)hr << "\n" );

    return hr;
}

//-------------------------------------------------------------------Com_job_chain::get_order_queue

STDMETHODIMP Com_job_chain::get_Order_queue( VARIANT* state, Iorder_queue** result )
{
    HRESULT hr = NOERROR;

    try
    {
        if( !_job_chain )  return E_POINTER;
        if (!_job_chain->is_loaded_or_active()) z::throw_xc("SCHEDULER-151");

        if( job_chain::Order_queue_node* node = job_chain::Order_queue_node::try_cast( _job_chain->node_from_state( *state ) ) )
        {
            *result = node->order_queue();
        }

        if( *result )  (*result)->AddRef();
    }
    catch( const exception&  x )  { hr = _set_excepinfo( x, "Spooler.Job_chain.order_queue" ); }
    catch( const _com_error& x )  { hr = _set_excepinfo( x, "Spooler.Job_chain.order_queue" ); }

    return hr;
}

//-----------------------------------------------------------------------------Com_job_chain::node

STDMETHODIMP Com_job_chain::get_Node( VARIANT* state, Ijob_chain_node** result )
{
    HRESULT hr = NOERROR;

    try
    {
        if( !_job_chain )  return E_POINTER;
        if( _job_chain->state() < Job_chain::jc_initialized )  z::throw_xc( "SCHEDULER-151" );

        *result = _job_chain->node_from_state( *state );
        if( *result )  (*result)->AddRef();
    }
    catch( const exception&  x )  { hr = _set_excepinfo( x, "Spooler.Job_chain.node" ); }
    catch( const _com_error& x )  { hr = _set_excepinfo( x, "Spooler.Job_chain.node" ); }

    return hr;
}

//------------------------------------------------------------Com_job_chain::put_Orders_recoverable

STDMETHODIMP Com_job_chain::put_Orders_recoverable( VARIANT_BOOL b )
{
    HRESULT hr = NOERROR;

    try
    {
        if( !_job_chain )  return E_POINTER;

        _job_chain->set_orders_are_recoverable( b != VARIANT_FALSE );
    }
    catch( const exception&  x )  { hr = _set_excepinfo( x, "Spooler.Job_chain.node" ); }
    catch( const _com_error& x )  { hr = _set_excepinfo( x, "Spooler.Job_chain.node" ); }

    return hr;
}

//------------------------------------------------------------Com_job_chain::get_Orders_recoverable

STDMETHODIMP Com_job_chain::get_Orders_recoverable( VARIANT_BOOL* result )
{
    if( !_job_chain )  return E_POINTER;

    *result = _job_chain->orders_are_recoverable()? VARIANT_TRUE : VARIANT_FALSE;

    return S_OK;
}

//---------------------------------------------------------Com_job_chain::Remove_all_pending_orders

STDMETHODIMP Com_job_chain::Remove_all_pending_orders( int* result )
{
    HRESULT hr = NOERROR;

    try
    {
        if( !_job_chain )  return E_POINTER;

        *result = _job_chain->remove_all_pending_orders();
    }
    catch( const exception&  x )  { hr = _set_excepinfo( x, Z_FUNCTION ); }
    catch( const _com_error& x )  { hr = _set_excepinfo( x, Z_FUNCTION ); }

    return hr;
}

//----------------------------------------------------------------------------Com_job_chain::Remove

STDMETHODIMP Com_job_chain::Remove()
{
    HRESULT hr = NOERROR;

    try
    {
        if( !_job_chain )  return E_POINTER;

        _job_chain->remove( File_based::rm_base_file_too );
    }
    catch( const exception&  x )  { hr = _set_excepinfo( x, Z_FUNCTION ); }
    catch( const _com_error& x )  { hr = _set_excepinfo( x, Z_FUNCTION ); }

    return hr;
}

//-------------------------------------------------------------------------Com_job_chain::put_Title

STDMETHODIMP Com_job_chain::put_Title( BSTR title_bstr )
{
    HRESULT hr = NOERROR;

    try
    {
        if( !_job_chain )  return E_POINTER;

        _job_chain->set_title( string_from_bstr( title_bstr ) );
    }
    catch( const exception&  x )  { hr = Set_excepinfo( x, Z_FUNCTION ); }
    catch( const _com_error& x )  { hr = Set_excepinfo( x, Z_FUNCTION ); }

    return hr;
}

//-------------------------------------------------------------------------Com_job_chain::get_Title

STDMETHODIMP Com_job_chain::get_Title( BSTR* result )
{
    HRESULT hr = NOERROR;

    try
    {
        if( !_job_chain )  return E_POINTER;

        hr = String_to_bstr( _job_chain->title(), result );
    }
    catch( const exception&  x )  { hr = Set_excepinfo( x, Z_FUNCTION ); }
    catch( const _com_error& x )  { hr = Set_excepinfo( x, Z_FUNCTION ); }

    return hr;
}

//--------------------------------------------------------------------------Com_job_chain::get_Path

STDMETHODIMP Com_job_chain::get_Path(BSTR* result)
{
    HRESULT hr = NOERROR;

    try
    {
        if (!_job_chain)  return E_POINTER;

        hr = String_to_bstr(_job_chain->path().to_string(), result);
    }
    catch (const exception&  x)  { hr = Set_excepinfo(x, Z_FUNCTION); }
    catch (const _com_error& x)  { hr = Set_excepinfo(x, Z_FUNCTION); }

    return hr;
}

//------------------------------------------------------------------------Com_job_chain::get_States

STDMETHODIMP Com_job_chain::get_States(SAFEARRAY** result)
{
    HRESULT hr = NOERROR;

    try
    {
        if (!_job_chain)  return E_POINTER;

        Locked_safearray<BSTR> safearray (_job_chain->_node_list.size());
        int i = 0;
        Z_FOR_EACH(Job_chain::Node_list, _job_chain->_node_list, it)
        {
            safearray[i++] = bstr_from_string((*it)->string_order_state());
        }

        *result = safearray.take_safearray();
    }
    catch (const exception&  x)  { hr = Set_excepinfo(x, Z_FUNCTION); }
    catch (const _com_error& x)  { hr = Set_excepinfo(x, Z_FUNCTION); }

    return hr;
}


//---------------------------------------------------------------------Com_job_chain_node::_methods
#ifdef Z_COM

const Com_method Com_job_chain_node::_methods[] =
{ 
   // _flags         , dispid, _name                        , _method                                               , _result_type  , _types        , _default_arg_count
    { DISPATCH_PROPERTYGET,  1, "state"                     , (Com_method_ptr)&Com_job_chain_node::get_State        , VT_VARIANT   },
    { DISPATCH_PROPERTYGET,  2, "next_node"                 , (Com_method_ptr)&Com_job_chain_node::get_Next_node    , VT_DISPATCH  },
    { DISPATCH_PROPERTYGET,  3, "error_node"                , (Com_method_ptr)&Com_job_chain_node::get_Error_node   , VT_DISPATCH  },
    { DISPATCH_PROPERTYGET,  4, "job"                       , (Com_method_ptr)&Com_job_chain_node::get_Job          , VT_DISPATCH  },
    { DISPATCH_PROPERTYGET,  5, "next_state"                , (Com_method_ptr)&Com_job_chain_node::get_Next_state   , VT_VARIANT   },
    { DISPATCH_PROPERTYGET,  6, "error_state"               , (Com_method_ptr)&Com_job_chain_node::get_Error_state  , VT_VARIANT   },
    { DISPATCH_PROPERTYGET,  7, "java_class_name"           , (Com_method_ptr)&Com_job_chain_node::get_Java_class_name, VT_BSTR    },
    { DISPATCH_PROPERTYPUT,  8, "Action"                    , (Com_method_ptr)&Com_job_chain_node::put_Action       , VT_EMPTY     , { VT_BSTR } },
    { DISPATCH_PROPERTYGET,  8, "Action"                    , (Com_method_ptr)&Com_job_chain_node::get_Action       , VT_BSTR      },
    {}
};

#endif
//-----------------------------------------------------------Com_job_chain_node::Com_job_chain_node

Com_job_chain_node::Com_job_chain_node()
:
    Sos_ole_object( job_chain_node_class_ptr, (Ijob_chain_node*)this )
{
}

//---------------------------------------------------------------Com_job_chain_node::QueryInterface

STDMETHODIMP Com_job_chain_node::QueryInterface( const IID& iid, void** result )
{
    Z_IMPLEMENT_QUERY_INTERFACE( this, iid, Ihas_java_class_name, result );

    return Sos_ole_object::QueryInterface( iid, result );
}

//--------------------------------------------------------------------Com_job_chain_node::get_state

STDMETHODIMP Com_job_chain_node::get_State( VARIANT* result ) 
{ 
    job_chain::Node* node = static_cast<job_chain::Node*>( this );
    return node->order_state().CopyTo( result );
    //return VariantCopy( result, &((job_chain::Node*)(this))->order_state() );
}

//----------------------------------------------------------------Com_job_chain_node::get_next_node

STDMETHODIMP Com_job_chain_node::get_Next_node( Ijob_chain_node** result )
{ 
    job_chain::Node* node = static_cast<job_chain::Node*>( this );
    *result = node->next_node();
    if( *result )  (*result)->AddRef();
    return S_OK;
}

//---------------------------------------------------------------Com_job_chain_node::get_error_node

STDMETHODIMP Com_job_chain_node::get_Error_node( Ijob_chain_node** result )   
{ 
    job_chain::Node* node = static_cast<job_chain::Node*>( this );
    *result = node->error_node(); 
    if( *result )  (*result)->AddRef();
    return S_OK;
}

//---------------------------------------------------------------Com_job_chain_node::get_next_state

STDMETHODIMP Com_job_chain_node::get_Next_state( VARIANT* result )
{ 
    job_chain::Node* node = static_cast<job_chain::Node*>( this );
    return node->next_state().CopyTo( result );
    //return VariantCopy( result, &((job_chain::Node*)(this))->next_state() );
}

//--------------------------------------------------------------Com_job_chain_node::get_error_state

STDMETHODIMP Com_job_chain_node::get_Error_state( VARIANT* result )   
{ 
    job_chain::Node* node = static_cast<job_chain::Node*>( this );
    return node->error_state().CopyTo( result );
    //return VariantCopy( result, &((job_chain::Node*)(this))->error_state() );
}

//----------------------------------------------------------------------Com_job_chain_node::get_job

STDMETHODIMP Com_job_chain_node::get_Job( Ijob** result )              
{ 
    if( job_chain::Job_node* job_node = job_chain::Job_node::try_cast( (job_chain::Node*)this ) )
    {
        *result = job_node->job()->com_job(); 
    }

    if( *result )  (*result)->AddRef();
    return S_OK;
}

//-------------------------------------------------------------------Com_job_chain_node::put_Action

STDMETHODIMP Com_job_chain_node::put_Action( BSTR action_bstr )
{
    HRESULT hr = NOERROR;

    try
    {
        set_action_complete(string_from_bstr(action_bstr));
    }
    catch( const exception&  x )  { hr = _set_excepinfo( x, Z_FUNCTION ); }

    return hr;
}

//-------------------------------------------------------------------Com_job_chain_node::get_Action

STDMETHODIMP Com_job_chain_node::get_Action( BSTR* result )
{
    HRESULT hr = NOERROR;

    try
    {
        hr = String_to_bstr( string_action(), result );
    }
    catch( const exception&  x )  { hr = _set_excepinfo( x, Z_FUNCTION ); }

    return hr;
}

//------------------------------------------------------------------------------Com_order::_methods
#ifdef Z_COM

const Com_method Com_order::_methods[] =
{ 
   // _flags         , dispid, _name                        , _method                                           , _result_type  , _types        , _default_arg_count
    { DISPATCH_PROPERTYPUT,  1, "id"                        , (Com_method_ptr)&Com_order::put_Id                , VT_EMPTY      , { VT_VARIANT|VT_BYREF  } },
    { DISPATCH_PROPERTYGET,  1, "id"                        , (Com_method_ptr)&Com_order::get_Id                , VT_VARIANT    },
    { DISPATCH_PROPERTYPUT,  2, "title"                     , (Com_method_ptr)&Com_order::put_Title             , VT_EMPTY      , { VT_BSTR     } },
    { DISPATCH_PROPERTYGET,  2, "title"                     , (Com_method_ptr)&Com_order::get_Title             , VT_BSTR       },
    { DISPATCH_PROPERTYPUT,  3, "priority"                  , (Com_method_ptr)&Com_order::put_Priority          , VT_EMPTY      , { VT_INT      } },
    { DISPATCH_PROPERTYGET,  3, "priority"                  , (Com_method_ptr)&Com_order::get_Priority          , VT_INT        },
    { DISPATCH_PROPERTYGET,  4, "job_chain"                 , (Com_method_ptr)&Com_order::get_Job_chain         , VT_DISPATCH   },
    { DISPATCH_PROPERTYGET,  5, "job_chain_node"            , (Com_method_ptr)&Com_order::get_Job_chain_node    , VT_DISPATCH   },
  //{ DISPATCH_PROPERTYPUTREF,6,"job"                       , (Com_method_ptr)&Com_order::putref_Job            , VT_EMPTY      , { VT_DISPATCH } },
  //{ DISPATCH_PROPERTYPUT,  6, "job"                       , (Com_method_ptr)&Com_order::put_Job               , VT_EMPTY      , { VT_VARIANT|VT_BYREF  } },
  //{ DISPATCH_PROPERTYGET,  6, "job"                       , (Com_method_ptr)&Com_order::get_Job               , VT_DISPATCH   },
    { DISPATCH_PROPERTYPUT,  7, "state"                     , (Com_method_ptr)&Com_order::put_State             , VT_EMPTY      , { VT_VARIANT|VT_BYREF  } },
    { DISPATCH_PROPERTYGET,  7, "state"                     , (Com_method_ptr)&Com_order::get_State             , VT_VARIANT    },
    { DISPATCH_PROPERTYPUT,  8, "state_text"                , (Com_method_ptr)&Com_order::put_State_text        , VT_EMPTY      , { VT_BSTR     } },
    { DISPATCH_PROPERTYGET,  8, "state_text"                , (Com_method_ptr)&Com_order::get_State_text        , VT_BSTR       },
    { DISPATCH_PROPERTYGET,  9, "error"                     , (Com_method_ptr)&Com_order::get_Error             , VT_DISPATCH   },
    { DISPATCH_PROPERTYPUTREF,10,"payload"                  , (Com_method_ptr)&Com_order::putref_Payload        , VT_EMPTY      , { VT_DISPATCH } },
    { DISPATCH_PROPERTYPUT, 10, "payload"                   , (Com_method_ptr)&Com_order::put_Payload           , VT_EMPTY      , { VT_VARIANT|VT_BYREF  } },
    { DISPATCH_PROPERTYGET, 10, "payload"                   , (Com_method_ptr)&Com_order::get_Payload           , VT_VARIANT    },
    { DISPATCH_METHOD     , 11, "payload_is_type"           , (Com_method_ptr)&Com_order::Payload_is_type       , VT_BOOL       , { VT_BSTR } },
    { DISPATCH_PROPERTYGET, 12, "java_class_name"           , (Com_method_ptr)&Com_order::get_Java_class_name   , VT_BSTR },
    { DISPATCH_METHOD     , 13, "setback"                   , (Com_method_ptr)&Com_order::Setback               , VT_EMPTY      },
    { DISPATCH_PROPERTYPUT, 14, "at"                        , (Com_method_ptr)&Com_order::put_At                , VT_EMPTY      , { VT_VARIANT|VT_BYREF } },
    { DISPATCH_PROPERTYGET, 14, "at"                        , (Com_method_ptr)&Com_order::get_At                , VT_DATE       },
    { DISPATCH_PROPERTYGET, 15, "Run_time"                  , (Com_method_ptr)&Com_order::get_Run_time          , VT_DISPATCH   },
    { DISPATCH_METHOD     , 16, "remove_from_job_chain"     , (Com_method_ptr)&Com_order::Remove_from_job_chain , VT_EMPTY      },
    { DISPATCH_PROPERTYGET, 17, "String_next_start_time"    , (Com_method_ptr)&Com_order::get_String_next_start_time, VT_BSTR   },
  //{ DISPATCH_PROPERTYGET, 18, "Log"                       , (Com_method_ptr)&Com_order::get_Log               , VT_DISPATCH   },
    { DISPATCH_PROPERTYGET, 18, "Xml"                       , (Com_method_ptr)&Com_order::get_Xml               , VT_BSTR       , { VT_BSTR }, 1 },
    { DISPATCH_PROPERTYGET, 19, "Web_service"               , (Com_method_ptr)&Com_order::get_Web_service       , VT_DISPATCH   },
    { DISPATCH_PROPERTYGET, 20, "Web_service_or_null"       , (Com_method_ptr)&Com_order::get_Web_service_or_null,VT_DISPATCH   },
    { DISPATCH_PROPERTYGET, 21, "Web_service_operation"        , (Com_method_ptr)&Com_order::get_Web_service_operation        , VT_DISPATCH   },
    { DISPATCH_PROPERTYGET, 22, "Web_service_operation_or_null", (Com_method_ptr)&Com_order::get_Web_service_operation_or_null, VT_DISPATCH   },
    { DISPATCH_PROPERTYPUT, 23, "Xml_payload"               , (Com_method_ptr)&Com_order::put_Xml_payload       , VT_EMPTY      , { VT_BSTR } },
    { DISPATCH_PROPERTYGET, 23, "Xml_payload"               , (Com_method_ptr)&Com_order::get_Xml_payload       , VT_BSTR       },
    { DISPATCH_PROPERTYPUT, 24, "Params"                    , (Com_method_ptr)&Com_order::put_Params            , VT_EMPTY      , { VT_DISPATCH } },
    { DISPATCH_PROPERTYGET, 24, "Params"                    , (Com_method_ptr)&Com_order::get_Params            , VT_DISPATCH   },
    { DISPATCH_PROPERTYPUT, 25, "Suspended"                 , (Com_method_ptr)&Com_order::put_Suspended         , VT_EMPTY      , { VT_BOOL } },
    { DISPATCH_PROPERTYGET, 25, "Suspended"                 , (Com_method_ptr)&Com_order::get_Suspended         , VT_BOOL       },
    { DISPATCH_PROPERTYGET, 26, "Log"                       , (Com_method_ptr)&Com_order::get_Log               , VT_DISPATCH   },
    { DISPATCH_PROPERTYPUT, 27, "End_state"                 , (Com_method_ptr)&Com_order::put_End_state         , VT_EMPTY      , { VT_VARIANT|VT_BYREF  } },
    { DISPATCH_PROPERTYGET, 27, "End_state"                 , (Com_method_ptr)&Com_order::get_End_state         , VT_VARIANT    },
    { DISPATCH_PROPERTYGET, 28, "Setback_count"             , (Com_method_ptr)&Com_order::get_Setback_count     , VT_INT        },
  //{ DISPATCH_METHOD     , 26, "Start_now"                 , (Com_method_ptr)&Com_order::Start_now             , VT_EMPTY      },
    { DISPATCH_PROPERTYPUT, 29, "Ignore_max_orders"         , (Com_method_ptr)&Com_order::put_Ignore_max_orders , VT_EMPTY      , { VT_BOOL } },
    { DISPATCH_PROPERTYGET, 29, "Ignore_max_orders"         , (Com_method_ptr)&Com_order::get_Ignore_max_orders , VT_BOOL       },
    { DISPATCH_PROPERTYGET, 30, "History_id"                , (Com_method_ptr)&Com_order::get_History_id        , VT_BSTR       },
    { DISPATCH_PROPERTYGET, 31, "Last_error"                , (Com_method_ptr)&Com_order::get_Last_error        , VT_BSTR       },
    {}
};

#endif
//-----------------------------------------------------------------------------Com_order::Com_order

Com_order::Com_order( Order* order )
:
    Sos_ole_object( order_class_ptr, (Iorder*)this ),
    _zero_(this+1),
    _order(order)
{
}

//------------------------------------------------------------------------Com_order::QueryInterface

STDMETHODIMP Com_order::QueryInterface( const IID& iid, void** result )
{
    Z_IMPLEMENT_QUERY_INTERFACE( this, iid, Ihas_java_class_name, result );

    return Sos_ole_object::QueryInterface( iid, result );
}

//--------------------------------------------------------------------------------Com_order::put_id

STDMETHODIMP Com_order::put_Id( VARIANT* id )
{
    HRESULT hr = NOERROR;

    try
    {
        if( !_order )  return E_POINTER;

        _order->set_id( *id );
    }
    catch( const exception&  x )  { hr = _set_excepinfo( x, "Spooler.Order.id" ); }
    catch( const _com_error& x )  { hr = _set_excepinfo( x, "Spooler.Order.id" ); }

    return hr;
}

//--------------------------------------------------------------------------------Com_order::get_id

STDMETHODIMP Com_order::get_Id( VARIANT* result )
{
    HRESULT hr = NOERROR;

    try
    {
        if( !_order )  return E_POINTER;

        return _order->id().CopyTo( result );
    }
    catch( const exception&  x )  { hr = _set_excepinfo( x, "Spooler.Order.id" ); }
    catch( const _com_error& x )  { hr = _set_excepinfo( x, "Spooler.Order.id" ); }

    return hr;
}

//-----------------------------------------------------------------------------Com_order::put_title

STDMETHODIMP Com_order::put_Title( BSTR title_bstr )
{
    HRESULT hr = NOERROR;

    try
    {
        if( !_order )  return E_POINTER;

        _order->set_title( string_from_bstr(title_bstr) );
    }
    catch( const exception&  x )  { hr = _set_excepinfo( x, "Spooler.Order.title" ); }
    catch( const _com_error& x )  { hr = _set_excepinfo( x, "Spooler.Order.title" ); }

    return hr;
}

//-----------------------------------------------------------------------------Com_order::get_title

STDMETHODIMP Com_order::get_Title( BSTR* result )
{
    HRESULT hr = NOERROR;

    try
    {
        if( !_order )  return E_POINTER;

        *result = bstr_from_string( _order->title() );
    }
    catch( const exception&  x )  { hr = _set_excepinfo( x, "Spooler.Order.title" ); }
    catch( const _com_error& x )  { hr = _set_excepinfo( x, "Spooler.Order.title" ); }

    return hr;
}
    
//--------------------------------------------------------------------------Com_order::put_priority

STDMETHODIMP Com_order::put_Priority( int priority )
{
    HRESULT hr = NOERROR;

    try
    {
        if( !_order )  return E_POINTER;

        _order->set_priority( priority );
    }
    catch( const exception&  x )  { hr = _set_excepinfo( x, "Spooler.Order.priority" ); }
    catch( const _com_error& x )  { hr = _set_excepinfo( x, "Spooler.Order.priority" ); }

    return hr;
}

//--------------------------------------------------------------------------Com_order::get_priority

STDMETHODIMP Com_order::get_Priority( int* result )
{
    HRESULT hr = NOERROR;

    try
    {
        if( !_order )  return E_POINTER;

        *result = _order->priority();
    }
    catch( const exception&  x )  { hr = _set_excepinfo( x, "Spooler.Order.priority" ); }
    catch( const _com_error& x )  { hr = _set_excepinfo( x, "Spooler.Order.priority" ); }

    return hr;
}
    
//-------------------------------------------------------------------------Com_order::get_job_chain

STDMETHODIMP Com_order::get_Job_chain( Ijob_chain** result )
{
    HRESULT hr = NOERROR;

    *result = NULL;

    try
    {
        if( !_order )  return E_POINTER;

        Job_chain* job_chain = _order->job_chain_for_api();
        if( job_chain )  
        {
            ptr<Ijob_chain> ijob_chain = job_chain; //->com_job_chain();
            ijob_chain.CopyTo( result );
        }
    }
    catch( const exception&  x )  { hr = _set_excepinfo( x, "Spooler.Order.job_chain" ); }
    catch( const _com_error& x )  { hr = _set_excepinfo( x, "Spooler.Order.job_chain" ); }

    return hr;
}

//--------------------------------------------------------------------Com_order::get_job_chain_node

STDMETHODIMP Com_order::get_Job_chain_node( Ijob_chain_node** result )
{
    HRESULT hr = NOERROR;

    {
        if( !_order )  return E_POINTER;

        *result = _order->job_chain_node();
        if( *result )  (*result)->AddRef();
    }

    return hr;
}

//-----------------------------------------------------------------------------Com_order::put_state

STDMETHODIMP Com_order::put_State( VARIANT* state )
{
    HRESULT hr = NOERROR;

    try
    {
        if( !_order )  return E_POINTER;

        _order->set_state(*state);
    }
    catch( const exception&  x )  { hr = _set_excepinfo( x, "Spooler.Order.state" ); }
    catch( const _com_error& x )  { hr = _set_excepinfo( x, "Spooler.Order.state" ); }

    return hr;
}

//-----------------------------------------------------------------------------Com_order::get_state

STDMETHODIMP Com_order::get_State( VARIANT* result )
{
    HRESULT hr = NOERROR;

    try
    {
        if( !_order )  return E_POINTER;

        return _order->state().CopyTo( result );
    }
    catch( const exception&  x )  { hr = _set_excepinfo( x, "Spooler.Order.state" ); }
    catch( const _com_error& x )  { hr = _set_excepinfo( x, "Spooler.Order.state" ); }

    return hr;
}

//------------------------------------------------------------------------Com_order::put_state_text

STDMETHODIMP Com_order::put_State_text( BSTR state_text_bstr )
{
    HRESULT hr = NOERROR;

    try
    {
        if( !_order )  return E_POINTER;

        _order->set_state_text( string_from_bstr(state_text_bstr) );
    }
    catch( const exception&  x )  { hr = _set_excepinfo( x, "Spooler.Order.state_text" ); }
    catch( const _com_error& x )  { hr = _set_excepinfo( x, "Spooler.Order.state_text" ); }

    return hr;
}

//------------------------------------------------------------------------Com_order::get_state_text

STDMETHODIMP Com_order::get_State_text( BSTR* result )
{
    HRESULT hr = NOERROR;

    try
    {
        if( !_order )  return E_POINTER;

        *result = bstr_from_string( _order->state_text() );
    }
    catch( const exception&  x )  { hr = _set_excepinfo( x, "Spooler.Order.state_text" ); }
    catch( const _com_error& x )  { hr = _set_excepinfo( x, "Spooler.Order.state_text" ); }

    return hr;
}

//-----------------------------------------------------------------------------Com_order::get_error

STDMETHODIMP Com_order::get_Error( Ierror** )
{
    HRESULT hr = NOERROR;

    try
    {
        if( !_order )  return E_POINTER;

        return E_FAIL;//...;
    }
    catch( const exception&  x )  { hr = _set_excepinfo( x, "Spooler.Order.error" ); }
    catch( const _com_error& x )  { hr = _set_excepinfo( x, "Spooler.Order.error" ); }

    return hr;
}

//---------------------------------------------------------------------------Com_order::put_payload

STDMETHODIMP Com_order::put_Payload( VARIANT* payload )
{
    HRESULT hr = NOERROR;

    try
    {
        if( !_order )  return E_POINTER;

        _order->set_payload( *payload );
    }
    catch( const exception&  x )  { hr = _set_excepinfo( x, "Spooler.Order.payload" ); }
    catch( const _com_error& x )  { hr = _set_excepinfo( x, "Spooler.Order.payload" ); }

    return hr;
}

//------------------------------------------------------------------------Com_order::putref_payload

STDMETHODIMP Com_order::putref_Payload( IUnknown* payload )
{
    HRESULT hr = NOERROR;

    try
    {
        if( !_order )  return E_POINTER;

        Variant payload_vt = payload;

        if( payload )
        {
            ptr<IDispatch> idispatch;
            hr = idispatch.Assign_qi( payload );
            if( SUCCEEDED(hr) )  payload_vt = idispatch;
        }

        _order->set_payload( payload_vt );
    }
    catch( const exception&  x )  { hr = _set_excepinfo( x, "Spooler.Order.payload" ); }
    catch( const _com_error& x )  { hr = _set_excepinfo( x, "Spooler.Order.payload" ); }

    return hr;
}

//---------------------------------------------------------------------------Com_order::get_payload

STDMETHODIMP Com_order::get_Payload( VARIANT* result )
{
    HRESULT hr = NOERROR;

    try
    {
        if( !_order )  return E_POINTER;

        hr = _order->payload().CopyTo( result );
    }
    catch( const exception&  x )  { hr = _set_excepinfo( x, "Spooler.Order.payload" ); }
    catch( const _com_error& x )  { hr = _set_excepinfo( x, "Spooler.Order.payload" ); }

    return hr;
}

//-----------------------------------------------------------------------Com_order::payload_is_type

STDMETHODIMP Com_order::Payload_is_type( BSTR typname_bstr, VARIANT_BOOL* result )
{
    HRESULT hr = NOERROR;

    *result = false;

    try
    {
        if( !_order )  return E_POINTER;

        string typname = lcase( string_from_bstr( typname_bstr ) );

        Variant payload = _order->payload();
        
        switch( payload.vt )
        {
            case VT_UNKNOWN:
            case VT_DISPATCH:
            {
                if( typname == "spooler.variable_set" )
                {
                    ptr<Ivariable_set> variable_set;
                    hr = variable_set.Assign_qi( V_UNKNOWN( &payload ) );
                    if( SUCCEEDED(hr)  )  { *result = true;  return hr; }
                }

                if( typname == "hostware.dyn_obj" 
                 || typname == "hostware.record" )
                {
                    ptr<IUnknown> hostware_dynobj;
                    hr = hostware_dynobj.Assign_qi( V_UNKNOWN( &payload ), IID_Ihostware_dynobj );
                    if( SUCCEEDED(hr) )  { *result = true;  return hr; }
                }

                hr = S_FALSE;
                break;
            }
            
            default: ;
        }
    }
    catch( const exception&  x )  { hr = _set_excepinfo( x, "Spooler.Order.payload_is_type" ); }
    catch( const _com_error& x )  { hr = _set_excepinfo( x, "Spooler.Order.payload_is_type" ); }

    return hr;
}

//-------------------------------------------------------------------------------Com_order::setback

STDMETHODIMP Com_order::Setback()
{
    HRESULT hr = NOERROR;

    try
    {
        if( !_order )  return E_POINTER;

        _order->setback();
    }
    catch( const exception&  x )  { hr = _set_excepinfo( x, "Spooler.Order.setback" ); }
    catch( const _com_error& x )  { hr = _set_excepinfo( x, "Spooler.Order.setback" ); }

    return hr;
}

//--------------------------------------------------------------------------------Com_order::put_At

STDMETHODIMP Com_order::put_At( VARIANT* datetime )
{
    HRESULT hr = NOERROR;

    try
    {
        if( !_order )  return E_POINTER;
        if( !datetime )  return E_POINTER;

        Order* o = dynamic_cast<Order*>(this);
        if (!o) return E_FAIL;
        _order->set_at( Time::of_date_time_with_now( string_from_variant( *datetime ), o->spooler()->_time_zone_name));
    }
    catch( const exception&  x )  { hr = _set_excepinfo( x, Z_FUNCTION ); }
    catch( const _com_error& x )  { hr = _set_excepinfo( x, Z_FUNCTION ); }
                
    return hr;
}

//--------------------------------------------------------------------------------Com_order::get_At

STDMETHODIMP Com_order::get_At( DATE* result )
{
    HRESULT hr = NOERROR;

    try
    {
        if( !_order )  return E_POINTER;

        *result = _order->at().as_local_com_date();
    }
    catch( const exception&  x )  { hr = _set_excepinfo( x, Z_FUNCTION ); }
    catch( const _com_error& x )  { hr = _set_excepinfo( x, Z_FUNCTION ); }

    return hr;
}

//--------------------------------------------------------------------------Com_order::get_Run_time

STDMETHODIMP Com_order::get_Run_time( Irun_time** result )
{
    HRESULT hr = NOERROR;

    try
    {
        if( !_order )  return E_POINTER;

        *result = +_order->schedule_use();  // Siehe Lieberts eMail vom 21. April 2008: nicht benanntes <schedule> liefern!
        if( *result )  (*result)->AddRef();
    }
    catch( const exception&  x )  { hr = _set_excepinfo( x, Z_FUNCTION ); }
    catch( const _com_error& x )  { hr = _set_excepinfo( x, Z_FUNCTION ); }

    return hr;
}

//-----------------------------------------------------------------Com_order::Remove_from_job_chain

STDMETHODIMP Com_order::Remove_from_job_chain()
{
    HRESULT hr = NOERROR;

    try
    {
        if( !_order )  return E_POINTER;

        _order->remove( File_based::rm_base_file_too );  
    }
    catch( const exception&  x )  { hr = _set_excepinfo( x, Z_FUNCTION ); }

    return hr;
}

//------------------------------------------------------------Com_order::get_String_next_start_time

STDMETHODIMP Com_order::get_String_next_start_time( BSTR* result )
{
    HRESULT hr = NOERROR;

    try
    {
        if( !_order )  return E_POINTER;

        hr = String_to_bstr( _order->at().without_timezone_string(), result );
    }
    catch( const exception&  x )  { hr = _set_excepinfo( x, Z_FUNCTION ); }

    return hr;
}

//-------------------------------------------------------------------------------Com_order::get_Xml

STDMETHODIMP Com_order::get_Xml( BSTR, BSTR* result )
{
    HRESULT hr = NOERROR;

    try
    {
        if( !_order )  return E_POINTER;

        hr = String_to_bstr(_order->dom(Show_what(show_payload | show_schedule | show_log)).xml_string(), result);
    }
    catch( const exception&  x )  { hr = _set_excepinfo( x, Z_FUNCTION ); }

    return hr;
}

//-----------------------------------------------------------------------Com_order::get_Web_service

STDMETHODIMP Com_order::get_Web_service( Iweb_service** result )
{
    HRESULT hr = NOERROR;

    try
    {
        if( !_order )  return E_POINTER;

        *result = _order->web_service();
        (*result)->AddRef();
    }
    catch( const exception&  x )  { hr = _set_excepinfo( x, Z_FUNCTION ); }

    return hr;
}

//---------------------------------------------------------------Com_order::get_Web_service_or_null

STDMETHODIMP Com_order::get_Web_service_or_null( Iweb_service** result )
{
    HRESULT hr = NOERROR;

    try
    {
        if( !_order )  return E_POINTER;

        *result = _order->web_service_or_null();
        (*result)->AddRef();
    }
    catch( const exception&  x )  { hr = _set_excepinfo( x, Z_FUNCTION ); }

    return hr;
}

//-------------------------------------------------------------Com_order::get_Web_service_operation

STDMETHODIMP Com_order::get_Web_service_operation( Iweb_service_operation** result )
{
    HRESULT hr = NOERROR;

    try
    {
        if( !_order )  return E_POINTER;

        *result = _order->web_service_operation();
        (*result)->AddRef();
    }
    catch( const exception&  x )  { hr = _set_excepinfo( x, Z_FUNCTION ); }

    return hr;
}

//-----------------------------------------------------Com_order::get_Web_service_operation_or_null

STDMETHODIMP Com_order::get_Web_service_operation_or_null( Iweb_service_operation** result )
{
    HRESULT hr = NOERROR;

    try
    {
        if( !_order )  return E_POINTER;

        *result = _order->web_service_operation_or_null();
        if( *result )  (*result)->AddRef();
    }
    catch( const exception&  x )  { hr = _set_excepinfo( x, Z_FUNCTION ); }

    return hr;
}

//-----------------------------------------------------------------------Com_order::put_Xml_payload

STDMETHODIMP Com_order::put_Xml_payload( BSTR xml_payload )
{
    HRESULT hr = NOERROR;

    try
    {
        if( !_order )  return E_POINTER;

        _order->set_payload_xml_string( string_from_bstr( xml_payload ) );
    }
    catch( const exception&  x )  { hr = _set_excepinfo( x, Z_FUNCTION ); }
    catch( const _com_error& x )  { hr = _set_excepinfo( x, Z_FUNCTION ); }

    return hr;
}

//-----------------------------------------------------------------------Com_order::get_Xml_payload

STDMETHODIMP Com_order::get_Xml_payload( BSTR* result )
{
    HRESULT hr = NOERROR;

    try
    {
        if( !_order )  return E_POINTER;

        hr = String_to_bstr( _order->xml_payload_string(), result );
    }
    catch( const exception&  x )  { hr = _set_excepinfo( x, Z_FUNCTION ); }
    catch( const _com_error& x )  { hr = _set_excepinfo( x, Z_FUNCTION ); }

    return hr;
}

//----------------------------------------------------------------------------Com_order::put_Params

STDMETHODIMP Com_order::put_Params( Ivariable_set* variable_set )
{
    HRESULT hr = NOERROR;

    try
    {
        if( !_order )  return E_POINTER;

        Variant v = variable_set;
        _order->set_payload( v );
    }
    catch( const exception&  x )  { hr = _set_excepinfo( x, Z_FUNCTION ); }
    catch( const _com_error& x )  { hr = _set_excepinfo( x, Z_FUNCTION ); }

    return hr;
}

//-----------------------------------------------------------------------Com_order::get_Xml_payload

STDMETHODIMP Com_order::get_Params( Ivariable_set** result )
{
    HRESULT hr = NOERROR;

    *result = NULL;

    try
    {
        if( !_order )  return E_POINTER;

        *result = _order->params().copy();      // Legt automatisch ein Variable_set an. Exception, wenn payload etwas anderes ist
    }
    catch( const exception&  x )  { hr = _set_excepinfo( x, Z_FUNCTION ); }
    catch( const _com_error& x )  { hr = _set_excepinfo( x, Z_FUNCTION ); }

    return hr;
}

//-------------------------------------------------------------------------Com_order::put_Suspended

STDMETHODIMP Com_order::put_Suspended( VARIANT_BOOL suspended )
{
    HRESULT hr = NOERROR;

    try
    {
        if( !_order )  return E_POINTER;

        _order->set_suspended( suspended != 0 );
    }
    catch( const exception& x )  { hr = _set_excepinfo( x, Z_FUNCTION ); }

    return hr;
}

//-------------------------------------------------------------------------Com_order::get_Suspended

STDMETHODIMP Com_order::get_Suspended( VARIANT_BOOL* result )
{
    HRESULT hr = NOERROR;

    *result = VARIANT_FALSE;

    try
    {
        if( !_order )  return E_POINTER;

        *result = _order->suspended()? VARIANT_TRUE : VARIANT_FALSE;
    }
    catch( const exception&  x )  { hr = _set_excepinfo( x, Z_FUNCTION ); }

    return hr;
}

//-------------------------------------------------------------------------Com_order::get_Suspended

STDMETHODIMP Com_order::get_Log( Ilog** result )
{
    HRESULT hr = NOERROR;

    try
    {
        if( !_order )  return E_POINTER;

        *result = _order->com_log();
        if( *result )  (*result)->AddRef();
    }
    catch( const exception&  x )  { hr = _set_excepinfo( x, Z_FUNCTION ); }

    return hr;
}

//-------------------------------------------------------------------------Com_order::put_End_state

STDMETHODIMP Com_order::put_End_state( VARIANT* state )
{
    HRESULT hr = NOERROR;

    try
    {
        if( !_order )  return E_POINTER;

        _order->set_end_state(*state, /*with_original=*/!_order->is_touched());
    }
    catch( const exception&  x )  { hr = _set_excepinfo( x, Z_FUNCTION ); }
    catch( const _com_error& x )  { hr = _set_excepinfo( x, Z_FUNCTION ); }

    return hr;
}

//-------------------------------------------------------------------------Com_order::get_End_state

STDMETHODIMP Com_order::get_End_state( VARIANT* result )
{
    HRESULT hr = NOERROR;

    try
    {
        if( !_order )  return E_POINTER;

        return _order->end_state().CopyTo( result );
    }
    catch( const exception&  x )  { hr = _set_excepinfo( x, Z_FUNCTION ); }
    catch( const _com_error& x )  { hr = _set_excepinfo( x, Z_FUNCTION ); }

    return hr;
}

//---------------------------------------------------------------------Com_order::get_Setback_count

STDMETHODIMP Com_order::get_Setback_count( int* result )
{
    HRESULT hr = NOERROR;

    try
    {
        if( !_order )  return E_POINTER;

        *result = _order->setback_count();
    }
    catch( const exception&  x )  { hr = _set_excepinfo( x, Z_FUNCTION ); }
    catch( const _com_error& x )  { hr = _set_excepinfo( x, Z_FUNCTION ); }

    return hr;
}


STDMETHODIMP Com_order::put_Ignore_max_orders( VARIANT_BOOL b )
{
    try {
        if (!_order) return E_POINTER;
        _order->set_ignore_max_orders(b != 0);
        return NOERROR;
    }
    catch (const exception& x)  { return _set_excepinfo(x, Z_FUNCTION); }
}


STDMETHODIMP Com_order::get_Ignore_max_orders(VARIANT_BOOL* result)
{
    try {
        *result = VARIANT_FALSE;
        if (!_order) return E_POINTER;
        *result = _order->is_ignore_max_orders() ? VARIANT_TRUE : VARIANT_FALSE;
        return NOERROR;
    }
    catch (const exception& x)  { return _set_excepinfo(x, Z_FUNCTION); }
}

STDMETHODIMP Com_order::get_History_id(BSTR* result) {
    try {
        if (!_order) return E_POINTER;
        return String_to_bstr(as_string(_order->history_id()), result);
    }
    catch (const exception& x)  { return _set_excepinfo(x, Z_FUNCTION); }
}

STDMETHODIMP Com_order::get_Last_error(BSTR* result) {
    try {
        if (!_order) return E_POINTER;
        return String_to_bstr(_order->last_error(), result);
    }
    catch (const exception& x)  { return _set_excepinfo(x, Z_FUNCTION); }
}

//------------------------------------------------------------------------Com_order_queue::_methods
#ifdef Z_COM

const Com_method Com_order_queue::_methods[] =
{ 
   // _flags          , dispid, _name                       , _method                                           , _result_type  , _types        , _default_arg_count
    { DISPATCH_PROPERTYGET,  1, "length"                    , (Com_method_ptr)&Com_order_queue::get_Length      , VT_INT        },
  //{ DISPATCH_METHOD     ,  2, "add_order"                 , (Com_method_ptr)&Com_order_queue::Add_order       , VT_DISPATCH   , { VT_VARIANT|VT_BYREF } },
    { DISPATCH_PROPERTYGET,  3, "java_class_name"           , (Com_method_ptr)&Com_order_queue::get_Java_class_name, VT_BSTR },
    {}
};

#endif
//-----------------------------------------------------------------Com_order_queue::Com_order_queue

Com_order_queue::Com_order_queue()
:
    Sos_ole_object( order_queue_class_ptr, (Iorder_queue*)this ),
    _zero_(this+1)
{
}

//------------------------------------------------------------------Com_order_queue::QueryInterface

STDMETHODIMP Com_order_queue::QueryInterface( const IID& iid, void** result )
{
    Z_IMPLEMENT_QUERY_INTERFACE( this, iid, Ihas_java_class_name, result );

    return Sos_ole_object::QueryInterface( iid, result );
}

//----------------------------------------------------------------------Com_order_queue::get_length

STDMETHODIMP Com_order_queue::get_Length( int* result )
{
    {
        Order_queue* order_queue = dynamic_cast<Order_queue*>( this );

        for( Retry_transaction ta ( order_queue->db() ); ta.enter_loop(); ta++ ) try
        {
            *result = order_queue->order_count( &ta );
        }
        catch( exception& x ) { ta.reopen_database_after_error( x, Z_FUNCTION ); }
    }

    return S_OK;
}

//-------------------------------------------------------------------------------------------------

} //namespace com_objects
} //namespace scheduler
} //namespace sos
