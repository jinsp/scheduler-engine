// $Id: spooler_mail.h 13751 2009-03-01 17:14:58Z jz $

#ifndef __SPOOLER_MAIL_H
#define __SPOOLER_MAIL_H

#include <map>

//#include "../kram/olestd.h"
#include "../zschimmer/com.h"
#include "../kram/sysxcept.h"
#include "../kram/sosscrpt.h"
#include "../kram/com.h"
#include "../kram/com_server.h"
#include "../kram/sos_mail.h"


namespace sos {
namespace scheduler {


//------------------------------------------------------------------------------------Mail_defaults
    
struct Mail_defaults
{
                                Mail_defaults               ( Spooler* );

    bool                        has_value                   ( const string& name ) const;
    string                      operator[]                  ( const string& name ) const;
    string                      value                       ( const string& name ) const;               // Liefert nicht "-", sondern ""
    void                        set                         ( const string& name, const string& value )  { _map[ name ] = value; }
    ListJ to_java() const;

    typedef map<string,string>  Map;
    Map                        _map;
};

//-----------------------------------------------------------------------------------------Com_mail

struct Com_mail : spooler_com::Imail, 
                  spooler_com::Ihas_java_class_name, 
                  Sos_ole_object               
{
    typedef stdext::hash_map<string,string>  Defaults_map;

    struct File
    {
        string                 _real_filename;
        string                 _mail_filename;
        string                 _content_type;
        string                 _encoding;
    };


    void*                       operator new                ( size_t size )                         { return sos_alloc( size, "spooler.Mail" ); }
    void                        operator delete             ( void* ptr )                           { sos_free( ptr ); }


    Z_GNU_ONLY(                 Com_mail                    ();  )                                  // Für gcc 3.2. Nicht implementiert
                                Com_mail                    ( Spooler* );
                               ~Com_mail                    ();
                                
    void                        init                        ();

    STDMETHODIMP                QueryInterface              ( REFIID, void** );

    USE_SOS_OLE_OBJECT_WITHOUT_QI

    STDMETHODIMP            get_Java_class_name             ( BSTR* result )                        { return String_to_bstr( const_java_class_name(), result ); }
    STDMETHODIMP_(char*)  const_java_class_name             ()                                      { return (char*)"sos.spooler.Mail"; }

    STDMETHODIMP            put_To                          ( BSTR );
    STDMETHODIMP            get_To                          ( BSTR* result )                        { return String_to_bstr( to(), result ); }

    STDMETHODIMP            put_Cc                          ( BSTR );
    STDMETHODIMP            get_Cc                          ( BSTR* result )                        { return String_to_bstr( cc(), result ); }

    STDMETHODIMP            put_Bcc                         ( BSTR );
    STDMETHODIMP            get_Bcc                         ( BSTR* result )                        { return String_to_bstr( bcc(), result ); }

    STDMETHODIMP            put_From                        ( BSTR );
    STDMETHODIMP            get_From                        ( BSTR* );
                                                                                                    
    STDMETHODIMP            put_Subject                     ( BSTR );
    STDMETHODIMP            get_Subject                     ( BSTR* );

    STDMETHODIMP            put_Body                        ( BSTR );
    STDMETHODIMP            get_Body                        ( BSTR* );

    STDMETHODIMP                Add_file                    ( BSTR real_filename, BSTR mail_filename, BSTR content_type, BSTR encoding );
  //STDMETHODIMP                Add_attachment              ( BSTR filename, BSTR data );

    STDMETHODIMP            put_Smtp                        ( BSTR );
    STDMETHODIMP            get_Smtp                        ( BSTR* );

    STDMETHODIMP            put_Queue_dir                   ( BSTR );
    STDMETHODIMP            get_Queue_dir                   ( BSTR* );

    STDMETHODIMP                Add_header_field            ( BSTR field_name, BSTR value );

    STDMETHODIMP                Dequeue                     ( int* count );

    STDMETHODIMP            get_Dequeue_log                 ( BSTR* result )                        { return String_to_bstr( _msg->dequeue_log(), result ); }

    STDMETHODIMP            put_Xslt_stylesheet_path        ( BSTR path )                           { _xslt_stylesheet.release(); return Bstr_to_string( path, &_xslt_stylesheet_path ); }
    STDMETHODIMP            get_Xslt_stylesheet_path        ( BSTR* result )                        { return String_to_bstr( _xslt_stylesheet_path, result ); }
    STDMETHODIMP            get_Xslt_stylesheet             ( spooler_com::Ixslt_stylesheet** );

    xml::Element_ptr            dom_element                 ( const xml::Document_ptr& dom );
    void                    set_dom                         ( const xml::Element_ptr& );

    void                    set_mail_defaults               ( const Mail_defaults& defaults          ) { _defaults = defaults; }

    void                    set_subject                     ( const string& );
    string                      subject                     ();

    void                    set_from                        ( const string& );
    void                    set_from_name                   ( const string& );
    string                      from                        ();

    void                    set_to                          ( const string& );
    string                      to                          ();

    void                    set_cc                          ( const string& );
    string                      cc                          ();

    void                    set_bcc                         ( const string& );
    string                      bcc                         ();

    void                    set_body                        ( const string& );
    string                      body                        ();

    void                    set_smtp                        ( const string& );
    string                      smtp                        ();

    void                    set_queue_dir                   ( const string& );
    string                      queue_dir                   ();

    void                        add_header_field            ( const string& name, const string& value );
    void                        add_file                    ( const string& real_filename, const string& mail_filename, const string& content_type, const string& encoding );
    void                        add_attachment              ( const string& data         , const string& mail_filename, const string& content_type, const string& encoding );


    int                         auto_dequeue                ()                                      { return _msg->auto_dequeue(); }
    void                    use_defaults                    ( const Mail_defaults& );
    void                    use_queue_defaults              ( const Mail_defaults& );
    void                    use_smtp_default                ( const Mail_defaults& );
    int                         send                        ( const Mail_defaults& );
    ptr<Xslt_stylesheet>        xslt_stylesheet             ();

  private:
    string                      value_with_default          ( const string& value, const string& default_name );

    Fill_zero                  _zero_;
    Spooler*                   _spooler;

    Sos_ptr<mail::Message>     _msg;

    string                     _smtp;
    string                     _subject;
    string                     _from;
    string                     _to;
    string                     _cc;
    bool                       _cc_set;
    string                     _bcc;
    bool                       _bcc_set;
    string                     _body;
    list<File>                 _files;

    typedef list< pair<string,string> >  Header_fields;
    Header_fields              _header_fields;

    Mail_defaults              _defaults;

    string                     _xslt_stylesheet_path;
    ptr<Xslt_stylesheet>       _xslt_stylesheet;
};

} //namespace scheduler
} //namespace sos


#endif
