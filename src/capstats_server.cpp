#include <memory>
#include <cstdlib>
#include <restbed>

#define OTL_ODBC // Compile OTL 4.0/ODBC
#define ODBCVER 0x0250 // ODBC Version # needs to be downgraded
// to 2.5 because the SQLite ODBC driver seems
// to run slower when ODBC 3.x functions
// used (performance is not as good as with
// ODBC 2.5 function calls)
#include <otlv4.h> // include the OTL 4.0 header file

otl_connect db; // connect object

using namespace std;
using namespace restbed;



void post_method_handler( const shared_ptr< Session > session )
{
    const auto request = session->get_request( );

    int content_length = stoi(request->get_header( "Content-Length", "0" ));

    session->fetch( content_length, [ ]( const shared_ptr< Session > session, const Bytes & body )
    {
        fprintf( stdout, "%.*s\n", ( int ) body.size( ), body.data( ) );
        session->close( OK, "Hello, World!", { { "Content-Length", "13" } } );
    } );
}

int main( const int, const char** )
{
	otl_connect::otl_initialize();
	try {
		db << ":memory:";
		try { db << "drop table test_tab"; }
		catch (otl_exception&) {}
		db << "create table test_tab(f1 int, f2 varchar(30))";

	}

	catch (otl_exception& p) { // intercept OTL exceptions
		cerr << p.msg << endl; // print out error message
		cerr << p.stm_text << endl; // print out SQL that caused the error
		cerr << p.var_info << endl; // print out the variable that caused the error
	}

    auto resource = make_shared< Resource >( );
    resource->set_path( "/resource" );
    resource->set_method_handler( "POST", post_method_handler );

    auto settings = make_shared< Settings >( );
    settings->set_port( 23232 );
    settings->set_default_header( "Connection", "close" );

    Service service;
    service.publish( resource );
    service.start( settings );

    return EXIT_SUCCESS;
}