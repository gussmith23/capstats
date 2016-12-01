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

void CapstatsServer::player_post_json( const shared_ptr< Session > session )
{
    const auto request = session->get_request( );

    int content_length = stoi(request->get_header( "Content-Length", "0"));

    session->fetch( content_length, [this]( const shared_ptr< Session > session, const Bytes & body )
    {
		try {
			Value player;
			player.loadFromString(string(body.begin(), body.end()));

			Player in = jsonToPlayer(player);
			if (!playerDAO->addPlayer(in)) {
				session->close(INTERNAL_SERVER_ERROR);
				return;
			}

			Value out = playerToJson(in);

			stringstream ss;
			out.writeToStream(ss);

			session->close(CREATED, ss.str(), 
					{ { "Content-Length", ::to_string(ss.str().length()) },
					{ "Content-Type", "application/json" },
					{ "Location", "/" + ::to_string(in.getId())} });
		}
		catch (const otl_exception& e) {
			cerr << e.msg << endl;
			cerr << e.stm_text << endl;
			cerr << e.var_info << endl;
			session->close(INTERNAL_SERVER_ERROR);
		}
		catch (const exception& e)
		{
			string body;
			body = "Unexpected exception: \"";
			body += e.what();
			body += "\"";
			cerr << body << endl;
			session->close(INTERNAL_SERVER_ERROR, body, { { "Content-Length", to_string(body.size()) },{ "Content-type", "text/html" } });
		}
	} );
}

void CapstatsServer::player_get_json(const shared_ptr<Session> session)
{
    try
    {
        const auto request = session->get_request();
		string idPathParameterString = request->get_path_parameter("id");
		unsigned long id;
		if (idPathParameterString == "") id = -1;
		else id = stoi(idPathParameterString);

        Player player = playerDAO->getPlayer(id);

		if (player.getId() < 0) {
			session->close(NOT_FOUND);
			return;
		}

		Value playerJson = playerToJson(player);

        stringstream ss;
        playerJson.writeToStream(ss);
        string response_body = ss.str();

        session->close(OK, response_body, 
				{ { "Content-Length", to_string(response_body.length()) },
				{ "Content-Type", "application/json" },
				{ "Location", "/" + ::to_string(player.getId())} });
    }
    catch (const otl_exception& e)
    {
        cerr << e.msg << endl;
        cerr << e.stm_text << endl;
        cerr << e.var_info << endl;
		session->close(INTERNAL_SERVER_ERROR);
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

void CapstatsServer::playerWithoutId_get_json(const std::shared_ptr<restbed::Session> session)
{
	try
	{
		const auto request = session->get_request();

		string telegramUsername = request->get_query_parameter("telegramUsername");
		string name = request->get_query_parameter("name");
		long telegramId = -1;
		string telegramId_string = request->get_query_parameter("telegramId");
		if (telegramId_string != "") telegramId = ::stol(telegramId_string);

		vector<Player> players = playerDAO->findPlayers(-1, name, telegramId, telegramUsername);

		Value playersJson = playersToJson(players);

		stringstream ss;
		playersJson.writeToStream(ss);
		string response_body = ss.str();

		session->close(OK, response_body,
			{ { "Content-Length", to_string(response_body.length()) },
			{ "Content-Type", "application/json" } });
	}
	catch (const otl_exception& e)
	{
		cerr << e.msg << endl;
		cerr << e.stm_text << endl;
		cerr << e.var_info << endl;
		session->close(INTERNAL_SERVER_ERROR);
	}
	catch (const exception& e)
	{
		string body;
		body = "Unexpected exception: \"";
		body += e.what();
		body += "\"";
		cerr << body << endl;
		session->close(INTERNAL_SERVER_ERROR, body, { { "Content-Length", to_string(body.size()) },{ "Content-type", "text/html" } });
	}
	session->close(NOT_IMPLEMENTED);
}

void CapstatsServer::player_put_json(const std::shared_ptr<restbed::Session> session)
{
	const auto request = session->get_request();

	int content_length = stoi(request->get_header("Content-Length", "0"));

	long id = ::stol(request->get_path_parameter("id"));

	session->fetch(content_length, [this, id](const shared_ptr< Session > session, const Bytes & body)
	{
		try {
			Value player;
			player.loadFromString(string(body.begin(), body.end()));

			Player in = jsonToPlayer(player);
			in.setId(id);
			if (!playerDAO->updatePlayer(in)) {
				session->close(INTERNAL_SERVER_ERROR);
				return;
			}

			Value out = playerToJson(in);

			stringstream ss;
			out.writeToStream(ss);

			session->close(OK, ss.str(), { { "Content-Length", ::to_string(ss.str().length()) },{ "Content-Type", "application/json" } });
		}
		catch (const otl_exception& e) {
			cerr << e.msg << endl;
			cerr << e.stm_text << endl;
			cerr << e.var_info << endl;
			session->close(INTERNAL_SERVER_ERROR);
		}
		catch (const exception& e)
		{
			string body;
			body = "Unexpected exception: \"";
			body += e.what();
			body += "\"";
			cerr << body << endl;
			session->close(INTERNAL_SERVER_ERROR, body, { { "Content-Length", to_string(body.size()) },{ "Content-type", "text/html" } });
		}
	});
}

JsonBox::Value CapstatsServer::playerToJson(const Player & player)
{
	JsonBox::Value out;
	//out["id"] = player.getId();
	out["name"] = player.getName();
	out["telegramId"] = player.getTelegramId();
	out["telegramUsername"] = player.getTelegramUsername();
	return out;
}

Player CapstatsServer::jsonToPlayer(const JsonBox::Value & json)
{
	Player out;
	Object obj = json.getObject();
	//out.setId(obj["id"].tryGetInteger(-1));
	out.setName(obj["name"].tryGetString(""));
	out.setTelegramId(obj["telegramId"].tryGetInteger(-1));
	out.setTelegramUsername(obj["telegramUsername"].tryGetString(""));
	return out;
}

JsonBox::Value CapstatsServer::playersToJson(const std::vector<Player>& players)
{
	Array out;
	for (Player player : players) out.push_back(playerToJson(player));
	return Value(out);
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


	auto playerWithId = make_shared<Resource>();
	playerWithId->set_path("/player/{id: [0-9]+}");
	playerWithId->set_method_handler("GET", bind1st(mem_fun(&CapstatsServer::player_get_json), this));
	playerWithId->set_method_handler("PUT", bind1st(mem_fun(&CapstatsServer::player_put_json), this));
	auto player = make_shared<Resource>();
	player->set_path("/player");
	player->set_method_handler("POST", bind1st(mem_fun(&CapstatsServer::player_post_json), this));
	player->set_method_handler("GET", bind1st(mem_fun(&CapstatsServer::playerWithoutId_get_json), this));

	auto gameWithId = make_shared<Resource>();
	gameWithId->set_path("/game/{id: [0-9]+}");
	gameWithId->set_method_handler("GET", bind1st(mem_fun(&CapstatsServer::game_get_json), this));
	auto game = make_shared<Resource>();
	game->set_path("/game");
	game->set_method_handler("POST", bind1st(mem_fun(&CapstatsServer::game_post_json), this));



    auto settings = make_shared< Settings >();
    settings->set_port(port);
    settings->set_default_header("Connection", "close");	

    Service service;
	service.publish(player);
	service.publish(playerWithId);
	service.publish(game);
	service.publish(gameWithId);
    service.start( settings );

    return EXIT_SUCCESS;
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
