#ifdef WIN32
#include <Windows.h>
#endif

#include <memory>
#include <cstdlib>

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

  int content_length = request->get_header( "Content-Length", 0);

  session->fetch( content_length, [this]( const shared_ptr< Session > session, const Bytes & body )
      {
      try {
      Value player;
      player.loadFromString(string(body.begin(), body.end()));

      Player in = jsonToPlayer(player);
      bool playerHasContent = in.getTelegramId() != -1 || in.getName() != "" 
          || in.getTelegramUsername() != "";
        
      if (!playerHasContent) 
      {
        session->close(FORBIDDEN);
        return;
      }

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

  int content_length = request->get_header("Content-Length", 0);

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
  int id = static_cast<int>(player.getId());
  if (id >= 0) out["id"] = id;
  else out["id"] = Value();

  string name = player.getName();
  if (name.size() > 0) out["name"] = name;
  else out["name"] = Value();

  int telegramId = static_cast<int>(player.getTelegramId());
  if (telegramId >= 0) out["telegramId"] = telegramId;
  else out["telegramId"] = Value();

  string telegramUsername = player.getTelegramUsername();
  if (telegramUsername.size() > 0) out["telegramUsername"] = telegramUsername;
  else out["telegramUsername"] = Value();

  return out;
}

Player CapstatsServer::jsonToPlayer(const JsonBox::Value & json)
{
  Player out;
  Object obj = json.getObject();
  out.setId(obj["id"].tryGetInteger(-1));
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

    // SQLite foreign keys
    // TODO foreign keys not working it seems.
    *db << "PRAGMA foreign_keys = ON";

    playerDAO->init();
    gameDAO->init();
    teamDAO->init();
    pointsDAO->init();
  }

  catch (otl_exception& p) { // intercept OTL exceptions
    cerr << p.msg << endl; // print out error message
    cerr << p.stm_text << endl; // print out SQL that caused the error
    cerr << p.var_info << endl; // print out the variable that caused the error
  }

  // Add default UUIDs.
  for (DefaultUUID defaultUUID : defaultUUIDs) {
    apiKeyDAO->addKey(defaultUUID.uuid, defaultUUID.description);
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
  gameWithId->set_method_handler("PUT", bind1st(mem_fun(&CapstatsServer::game_put_json), this));
  auto game = make_shared<Resource>();
  game->set_path("/game");
  game->set_method_handler("POST", bind1st(mem_fun(&CapstatsServer::game_post_json), this));
  game->set_method_handler("GET", bind1st(mem_fun(&CapstatsServer::gameWithoutId_get_json), this));



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
  out["id"] = static_cast<int>(game.getId());
  out["time"] = (int) game.getTime();

  map<int, Array> teams;
  for (const auto& pair : game.getTeams())
  {
    if (teams.find(pair.first) == teams.end())
      teams[pair.first] = Array();
    teams[pair.first].push_back(Value(static_cast<int>(pair.second)));
  }
  for (const auto& pair : teams)
  {
    out["teams"][to_string(pair.first)] = pair.second;
  }

  for (const auto& pair : game.getPoints())
    out["points"][::to_string(pair.first)] = pair.second;

  return out;
}

JsonBox::Value CapstatsServer::gamesToJson(const std::vector<Game>& games)
{
  Array out;
  for (Game game : games) out.push_back(gameToJson(game));
  return Value(out);
}

Game CapstatsServer::jsonToGame(const JsonBox::Value & json)
{
  Game out;

  Object obj = json.getObject();
  out.setId(obj["id"].tryGetInteger(-1));
  out.setTime(obj["time"].tryGetInteger(0));

  multimap<int, long> teams;
  for (auto i : obj["teams"].getObject())
    for (auto j : i.second.getArray())
      teams.insert(pair<int, long>(::stoi(i.first), j.getInteger()));
  out.setTeams(teams);

  multimap<int, int> points;
  for (auto i : obj["points"].getObject())
    points.insert(pair<int, int>(::stoi(i.first), i.second.getInteger()));
  out.setPoints(points);

  return out;
}

void CapstatsServer::game_post_json(const std::shared_ptr<restbed::Session> session)
{
  const auto request = session->get_request();

  int content_length = request->get_header("Content-Length", 0);

  session->fetch(content_length, [this](const shared_ptr< Session > session, const Bytes & body)
      {
      Value game;
      game.loadFromString(string(body.begin(), body.end()));

      Game out = jsonToGame(game);

      gameDAO->addGame(out);

      game["id"] = static_cast<int>(out.getId());

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

  if (game.getId() < 0)
  {
    session->close(NOT_FOUND);
    return;
  }

  Value gameJson = gameToJson(game);

  stringstream ss; ss << gameJson;

  session->close(OK, ss.str(), { { "Content-Length", ::to_string(ss.str().size()) } });

}

void CapstatsServer::gameWithoutId_get_json(const std::shared_ptr<restbed::Session> session)
{
  try
  {
    const auto request = session->get_request();

    /* get query parameters. 
    string telegramUsername = request->get_query_parameter("telegramUsername");
    string name = request->get_query_parameter("name");
    long telegramId = -1;
    string telegramId_string = request->get_query_parameter("telegramId");
    if (telegramId_string != "") telegramId = ::stol(telegramId_string);
    */

    string players = request->get_query_parameter("players");
    istringstream iss(players);
    string token;
    vector<long> playerIds;
    while (getline(iss,token, ',')) {
      try {
        long id = stol(token);
        playerIds.push_back(id);
      }
      catch (invalid_argument) {}
    }

    vector<Game> games = gameDAO->findGames(-1, playerIds);

    Value gamesJson = gamesToJson(games);

    stringstream ss;
    gamesJson.writeToStream(ss);
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
void CapstatsServer::game_put_json(const std::shared_ptr<restbed::Session> session)
{
  const auto request = session->get_request();

  int content_length = request->get_header("Content-Length", 0);

  long id = ::stol(request->get_path_parameter("id"));

  session->fetch(content_length, [this, id](const shared_ptr< Session > session, const Bytes & body)
      {
      try {
      Value game;
      game.loadFromString(string(body.begin(), body.end()));

      Game in = jsonToGame(game);
      in.setId(id);
      if (!gameDAO->updateGame(in)) {
      session->close(INTERNAL_SERVER_ERROR);
      return;
      }

      Value out = gameToJson(in);

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
