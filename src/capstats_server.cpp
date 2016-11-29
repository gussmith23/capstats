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
#include <mutex>
#include <functional>
#include <map>
#include <sstream>

#include "JsonBox.h"

#include "player_dao.h"
#include "team_dao.h"

#include "capstats_exceptions.h"

#include "capstats_server.h"

using namespace std;
using namespace restbed;
using namespace JsonBox;

void CapstatsServer::post_user_handler( const shared_ptr< Session > session )
{
    const auto request = session->get_request( );

    int content_length = stoi(request->get_header( "Content-Length", "0"));

    session->fetch( content_length, [this]( const shared_ptr< Session > session, const Bytes & body )
    {
        fprintf( stdout, "%.*s\n", ( int ) body.size( ), body.data( ) );
        session->close( OK, "Hello, World!", { { "Content-Length", "13" } } );

		Value player;
		player.loadFromString(string(body.begin(), body.end()));

		Player out;
		out.setName(player["name"].getString());
		out.setTelegramId(player["telegramId"].getInteger());
		playerDAO->addPlayer(out);

		session->close(OK, "", { { "Content-Length", "0" },{ "Content-Type", "application/json" } });

	} );
}

void CapstatsServer::get_user_handler(const shared_ptr<Session> session)
{
    try
    {
        const auto request = session->get_request();
        const unsigned int telegramId = stoi(request->get_query_parameter("telegramId", "0"));

        Player player = playerDAO->getPlayer(telegramId);

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
    catch (const PlayerNotFoundException& e)
    {
        string body(e.what());
        session->close(NOT_FOUND, body, { {"Content-Length", to_string(body.size())}, {"Content-type", "text/html"}});
    }
    catch (const otl_exception& e)
    {
        cerr << e.msg << endl;
        cerr << e.stm_text << endl;
        cerr << e.var_info << endl;
    }
    catch (const exception& e)
    {
        string body;
        body = "Unexpected exception: \"";
        body += e.what();
        body += "\"";

        cerr << body << endl;

        session->close(INTERNAL_SERVER_ERROR, body, { {"Content-Length", to_string(body.size())}, {"Content-type", "text/html"} });
    }
}

void CapstatsServer::get_user_html(const shared_ptr<Session> session)
{
	const auto request = session->get_request();
	const unsigned int telegramId = stoi(request->get_query_parameter("telegramId"));

	Player player = playerDAO->getPlayer(telegramId);

	string out_html = "<h1>" + player.getName() + "</h1><p><b>Telegram ID:</b>"  + to_string(player.getTelegramId()) + "</p>";

	out_html += "<form action='player' method='post'><input type='text' name='name'><input type ='text' name='telegramId'><input type='submit'></form>";
	session->close(OK, out_html, { { "Content-Length", to_string(out_html.length()) },{ "Content-Type", "text/html" } });
}



void CapstatsServer::init() {

	// Setup db.
	otl_connect::otl_initialize();
	try {
		string connectionString = "DRIVER=SQLite3 ODBC Driver;Database=" + databasePath + ";";
		*db << connectionString.c_str();
		playerDAO->init();
		gameDAO->init();
		teamDAO->init();
	}

	catch (otl_exception& p) { // intercept OTL exceptions
		cerr << p.msg << endl; // print out error message
		cerr << p.stm_text << endl; // print out SQL that caused the error
		cerr << p.var_info << endl; // print out the variable that caused the error
	}
}

int CapstatsServer::run() {


	auto user = make_shared<Resource>();
	user->set_path("/player");
	user->set_method_handler("GET", bind1st(mem_fun(&CapstatsServer::get_user_handler), this));
	user->set_method_handler("POST", bind1st(mem_fun(&CapstatsServer::post_user_handler), this));

	auto game = make_shared<Resource>();
	game->set_paths({ "/game", "/game/{id: [0-9]+}" });
	game->set_method_handler("GET", { { "Accepts", "application/json" } }, bind1st(mem_fun(&CapstatsServer::game_get_json), this));
	game->set_method_handler("POST", { {"Content-Type", "application/json"} }, bind1st(mem_fun(&CapstatsServer::game_post_json), this));

    auto settings = make_shared< Settings >();
    settings->set_port(port);
    settings->set_default_header("Connection", "close");

    Service service;
	service.publish(user);
	service.publish(game);
    service.start( settings );

    return EXIT_SUCCESS;
}

// TODO: getting errors when i try to pass via constant reference.
bool CapstatsServer::addPlayerJson(Object playerJson) {
	Player out;
	if (playerJson.count("name")) out.setName(playerJson["name"].tryGetString(""));
	if (playerJson.count("telegramId")) out.setTelegramId(playerJson["telegramId"].tryGetInteger(-1));

	return playerDAO->addPlayer(out);
}

Value CapstatsServer::getPlayerJson(long id) {
	Object playerJson;
	try {
		Player player = playerDAO->getPlayer(id);

		playerJson["name"] = Value(player.getName());
		playerJson["telegramId"] = Value((int)player.getTelegramId());
	}
	catch (PlayerNotFoundException) {
		
	}

	Object out;
	out["player"] = playerJson;
	
	return out;
}

JsonBox::Value CapstatsServer::gameToJson(const Game & game)
{
	Value out;
	out["id"] = game.getId();
	out["time"] = (int) game.getTime();
	
	map<int, Array> teams;
	for (const auto& pair : game.getTeams())
	{
		if (teams.find(pair.first) == teams.end())
			teams[pair.first] = Array();
		teams[pair.first].push_back(Value(pair.second));
	}
	for (const auto& pair : teams)
	{
		out["teams"][to_string(pair.first)] = pair.second;
	}

	return out;
}

void CapstatsServer::game_post_json(const std::shared_ptr<restbed::Session> session)
{
	const auto request = session->get_request();

	int content_length = stoi(request->get_header("Content-Length", "0"));

	session->fetch(content_length, [this](const shared_ptr< Session > session, const Bytes & body)
	{
		Value game;
		game.loadFromString(string(body.begin(), body.end()));
		
		multimap<int, long> teams;
		for (const auto& x : game["teams"].getObject())
		{
			int team = stoi(x.first);
			for (const auto& playerId : x.second.getArray())
				teams.insert(pair<int, long>(team, playerId.getInteger()));
		}

		map<int, int> scores;
		for (const auto& scorePair : game["score"].getObject())
		{
			int team = stoi(scorePair.first);
			scores.insert(pair<int, int>(team, scorePair.second.getInteger()));
		}

		time_t time = game["time"].getInteger();

		Game out;
		out.setTeams(teams);
		out.setTime(time);

		gameDAO->addGame(out);

		game["id"] = out.getId();

		stringstream ss;
		game.writeToStream(ss);
		string jsonString = ss.str();
		session->close(OK, jsonString, { { "Content-Length", to_string(jsonString.length()) },{ "Content-Type", "application/json" } });

	});

}

void CapstatsServer::game_get_json(const std::shared_ptr<restbed::Session> session)
{
	const auto& request = session->get_request();

	const long id = stol(request->get_path_parameter("id"));

	Game game = gameDAO->getGame(id);

	Value gameJson = gameToJson(game);
	
	stringstream ss; ss << gameJson;

	session->close(OK, ss.str(), { { "Content-Length", ::to_string(ss.str().size()) } });

}
