#ifdef WIN32
#include <Windows.h>
#endif

#include <memory>
#include <cstdlib>
#include <restbed>

#include <sql.h>
#include <sqltypes.h>
#include <sqlext.h>

#include "include_otl.h"

#include "player_dao.h"

otl_connect db; // connect object

PlayerDAO playerDAO;

using namespace std;
using namespace restbed;

void post_method_handler( const shared_ptr< Session > session )
{
    const auto request = session->get_request( );

    int content_length = stoi(request->get_header( "Content-Length", "0"));

    session->fetch( content_length, [ ]( const shared_ptr< Session > session, const Bytes & body )
    {
        fprintf( stdout, "%.*s\n", ( int ) body.size( ), body.data( ) );
        session->close( OK, "Hello, World!", { { "Content-Length", "13" } } );
    } );
}

int main( const int, const char** )
{
	// Setup db.
	otl_connect::otl_initialize();
	try {

		db << "DRIVER=SQLite3 ODBC Driver;Database=test.db;"; 
		playerDAO.init();
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