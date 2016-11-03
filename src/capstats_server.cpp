#ifdef WIN32
#include <Windows.h>
#endif

#include <memory>
#include <cstdlib>
#include <restbed>

#include <sql.h>
#include <sqltypes.h>
#include <sqlext.h>

#include <string>

#include "include_otl.h"
#include "JsonBox.h"

#include "player_dao.h"

otl_connect db; // connect object

PlayerDAO playerDAO;

using namespace std;
using namespace restbed;
using namespace JsonBox;

void post_user_handler( const shared_ptr< Session > session )
{
    const auto request = session->get_request( );

    int content_length = stoi(request->get_header( "Content-Length", "0"));

    session->fetch( content_length, [ ]( const shared_ptr< Session > session, const Bytes & body )
    {
        fprintf( stdout, "%.*s\n", ( int ) body.size( ), body.data( ) );
        session->close( OK, "Hello, World!", { { "Content-Length", "13" } } );

		Value player;
		player.loadFromString(string(body.begin(), body.end()));

		playerDAO.addPlayer(Player(player["telegramId"].getInteger(), player["name"].getString()));
		
		session->close(OK, "", { { "Content-Length", "0" },{ "Content-Type", "application/json" } });

	} );
}

void get_user_handler(const shared_ptr<Session> session)
{
	const auto request = session->get_request();
	
	const unsigned int telegramId = stoi(request->get_query_parameter("telegramId"));

	Player player = playerDAO.getPlayer(telegramId);

	Object playerJson;
	playerJson["name"] = Value(player.getName());
	playerJson["telegramId"] = Value((int)player.getTelegramId());

	Object out;
	out["player"] = playerJson;

	stringstream stream;
	Value(out).writeToStream(stream);
	string response_body = stream.str();

	session->close(OK, response_body, { { "Content-Length", to_string(response_body.length()) },{ "Content-Type", "application/json" } });
}

void get_user_http(const shared_ptr<Session> session)
{
	const auto request = session->get_request();
	const unsigned int telegramId = stoi(request->get_query_parameter("telegramId"));

	Player player = playerDAO.getPlayer(telegramId);

	string out_html = "<h1>" + player.getName() + "</h1><p><b>Telegram ID:</b>" + "</p>";

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
	
	auto user = make_shared<Resource>();
	user->set_path("/player");
	user->set_method_handler("GET", get_user_handler);
	user->set_method_handler("POST", post_user_handler);


    auto settings = make_shared< Settings >( );
    settings->set_port( 23232 );
    settings->set_default_header( "Connection", "close" );

    Service service;
	service.publish(user);
    service.start( settings );

    return EXIT_SUCCESS;
}