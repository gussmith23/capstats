#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "include_otl.h"
#include "api_key_dao.h"
#include "capstats_config.h"
#include "player_dao.h"
#include "player.h"
#include "game_dao.h"
#include "game.h"
#include "team_dao.h"
#include "points_dao.h"
#include "player_points_dao.hpp"
#include "capstats_exceptions.h"
#include <iostream>
#include <string>
#include <memory>
#include <map>
#include <set>

using namespace std;

// TODO: When porting to linux from windows, there was a bug where, if the
// buffer of the otl stream objects was >1, things would be put in the
// database in weird ways (i.e. insert streams that inserted multimple
// rows would have rows all messed up, columns would be switched, etc).
// i fixed it by making the buffer 1 for all of them, but it's not a satisfying
// fix.

/* TODO(gus): remove completely when comfortable
// from http://stackoverflow.com/questions/440133/how-do-i-create-a-random-alpha-numeric-string-in-c
void gen_random(char *s, const int len) {
  static const char alphanum[] =
    "0123456789"
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "abcdefghijklmnopqrstuvwxyz";

  for (int i = 0; i < len; ++i) {
    s[i] = alphanum[rand() % (sizeof(alphanum) - 1)];
  }

  s[len] = 0;
}*/

TEST_CASE("Player DAO") {
  otl_connect::otl_initialize();
  shared_ptr<otl_connect> db = shared_ptr<otl_connect>(new otl_connect);
  *db << "DRIVER=SQLite3 ODBC Driver;Database=:memory:;";

  PlayerDAO player_dao(db);
  player_dao.init();

  SECTION("Players added to and retrieved from database") {
    try {
      Player in;
      in.setTelegramId(23);
      in.setName("gus");
      in.setTelegramUsername("testUsername");

      long idBeforeAdd = in.getId();

      REQUIRE(player_dao.addPlayer(in) == true);
      REQUIRE(in.getId() != idBeforeAdd);

      Player out = player_dao.getPlayer(in.getId());
      REQUIRE(out.getName() == "gus");
      REQUIRE(out.getTelegramId() == 23);
      REQUIRE(out.getTelegramUsername() == "testUsername");
    }
    catch (otl_exception e) {
      cout << e.msg << endl;
      FAIL();
    }
    catch (...) {
      FAIL();
    }
  }

  SECTION("Invalid player requested returns error code") {
    try {
      Player out = player_dao.getPlayer(2323);
      REQUIRE(out.getId() == -1);
    }
    catch (otl_exception e) {
      cout << e.msg << endl;
      FAIL();
    }
    catch (...) {
      FAIL();
    }
  }

  SECTION("Find player by username - player exists") {
    try
    {
      Player in;
      in.setTelegramUsername("testUsername");
      player_dao.addPlayer(in);
      Player out = player_dao.findPlayerByTelegramUsername("testUsername");
      REQUIRE(out.getTelegramUsername() == "testUsername");
      REQUIRE(out.getId() >= 0);
    }
    catch (...)
    {
      FAIL();
    }
  }

  SECTION("Find player by username - player does not exist") {
    try
    {
      Player out = player_dao.findPlayerByTelegramUsername("testUsername");
      REQUIRE(out.getId() == -1);
    }
    catch (...)
    {
      FAIL();
    }
  }

  SECTION("Update player") {
    try {
      Player in; in.setName("gus");
      player_dao.addPlayer(in);
      in.setName("henry");
      player_dao.updatePlayer(in);
      Player out = player_dao.getPlayer(in.getId());
      REQUIRE(out.getName() == "henry");
    }
    catch (const otl_exception& e) {
      cout << e.msg << endl << e.stm_text << endl << e.var_info << endl;
      FAIL();
    }
    catch (...) {
      FAIL();
    }
  }

  SECTION("Find players") {
    try {
      map<int, Player> players;

      Player in;
      in.setName("gus"); in.setTelegramUsername("justgus"); in.setTelegramId(1);
      player_dao.addPlayer(in);

      Player in2;
      in2.setName("henry"); in2.setTelegramUsername("justhenry"); in2.setTelegramId(2);
      player_dao.addPlayer(in2);

      Player in3;
      in3.setName("henry"); in3.setTelegramUsername("justhenryy"); in3.setTelegramId(3);
      player_dao.addPlayer(in3);

      //players.insert(pair<int,Player>(in.getId(), in));

      vector<Player> out = player_dao.findPlayers(1);
      REQUIRE(out.size() == 1);
      REQUIRE(out[0].getName() == "gus");

      out = player_dao.findPlayers(-1, "henry");
      REQUIRE(out.size() == 2);

      out = player_dao.findPlayers(100, "in 1844, meridian time ", 101, "personnel met in");
      REQUIRE(out.size() == 0);

      out = player_dao.findPlayers();
      REQUIRE(out.size() == 3);

    }
    catch (const otl_exception& e) {
      cout << e.msg << endl << e.stm_text << endl << e.var_info << endl;
      FAIL();
    }
    catch (...) {
      FAIL();
    }
  }
}

TEST_CASE("Game DAO") {
  otl_connect::otl_initialize();
  shared_ptr<otl_connect> db(new otl_connect);
  *db << "DRIVER=SQLite3 ODBC Driver;Database=:memory:;";

  shared_ptr<TeamDAO> teamDAO(new TeamDAO(db));
  teamDAO->init();
  shared_ptr<PointsDAO> pointsDAO(new PointsDAO(db));
  pointsDAO->init();
  shared_ptr<PlayerPointsDAO> playerPointsDAO(new PlayerPointsDAO(db));
  playerPointsDAO->init();
  shared_ptr<GameDAO> gameDAO(new GameDAO(db, teamDAO, pointsDAO, playerPointsDAO));
  gameDAO->init();

  SECTION("Games added to and retrieved from database") {
    try {
      Game g; 
      g.setTime(23);
      long idBeforeAdd = g.getId();
      multimap<int, long> teams = { {1,2}, {3,4}, {3,5}, {6,7} };
      g.setTeams(teams);
      multimap<int, int> points = { {1,9}, {3, 11}, {200, 20000} };
      g.setPoints(points);
      multimap<int, int> playerPoints = { {2,1}, {4,2}, {5,3}, {7,4} };
      g.setPlayerPoints(playerPoints);

      REQUIRE(gameDAO->addGame(g) == true);
      REQUIRE(g.getId() != idBeforeAdd);

      Game out = gameDAO->getGame(g.getId());
      REQUIRE(out.getTime() == 23);
      REQUIRE(out.getTeams() == teams);
      REQUIRE(out.getPoints() == points);
      REQUIRE(out.getPlayerPoints() == playerPoints);
    }
    catch (otl_exception e) {
      cout << e.msg << endl;
      FAIL();
    }
    catch (...) {
      FAIL();
    }
  }
  SECTION("Invalid game requested returns error code") {
    try {
      Game out = gameDAO->getGame(2323);
      REQUIRE(out.getId() == -1);
    }
    catch (otl_exception e) {
      cout << e.msg << endl;
      FAIL();
    }
    catch (...) {
      FAIL();
    }
  }
  SECTION("update game") {
    Game g;
    g.setTime(23);
    long idBeforeAdd = g.getId();
    multimap<int, long> teams = { { 1,2 },{ 3,4 },{ 3,5 },{ 6,7 } };
    g.setTeams(teams);
    multimap<int, int> points = { { 1,9 },{ 3, 11 },{ 200, 20000 } };
    g.setPoints(points);
    multimap<int, int> playerPoints = { {2,1}, {4,2}, {5,3}, {7,4} };
    g.setPlayerPoints(playerPoints);
    gameDAO->addGame(g);

    teams.insert(pair<int, long>(8, 9));
    g.setTeams(teams);
    g.setTime(24);
    points.insert(pair<int, int>(10, 11));

    REQUIRE(gameDAO->updateGame(g));
    Game out = gameDAO->getGame(g.getId());
    REQUIRE(out.getId() == g.getId());
    REQUIRE(out.getTime() == g.getTime());
    REQUIRE(out.getTeams() == g.getTeams());
    REQUIRE(out.getPoints() == g.getPoints());
    REQUIRE(out.getPlayerPoints() == g.getPlayerPoints());
  }

  SECTION("find games, games present") {
    Game g;
    g.setTime(23);
    multimap<int, long> teams = { { 1,2 },{ 3,4 },{ 3,5 },{ 6,7 } };
    g.setTeams(teams);
    multimap<int, int> points = { { 1,9 },{ 3, 11 },{ 200, 20000 } };
    g.setPoints(points);
    multimap<int, int> playerPoints = { {2,1}, {4,2}, {5,3}, {7,4} };
    g.setPlayerPoints(playerPoints);
    gameDAO->addGame(g);
    g.setTime(24);
    g.setId(-1);
    gameDAO->addGame(g);

    REQUIRE(gameDAO->findGames().size() == 2);
  }


  SECTION("find games using id") {
    Game g;
    g.setTime(23);
    multimap<int, long> teams = { { 1,2 },{ 3,4 },{ 3,5 },{ 6,7 } };
    g.setTeams(teams);
    multimap<int, int> points = { { 1,9 },{ 3, 11 },{ 200, 20000 } };
    g.setPoints(points);
    multimap<int, int> playerPoints = { {2,1}, {4,2}, {5,3}, {7,4} };
    g.setPlayerPoints(playerPoints);
    gameDAO->addGame(g);
    long id = g.getId();
    g.setId(-1);
    gameDAO->addGame(g);

    REQUIRE(gameDAO->findGames(id).size() == 1);
  }

  SECTION("find games, no games present") {
    REQUIRE(gameDAO->findGames().size() == 0);
  }

}

TEST_CASE("Team DAO")
{
  otl_connect::otl_initialize();
  shared_ptr<otl_connect> db(new otl_connect);
  *db << "DRIVER=SQLite3 ODBC Driver;Database=:memory:";

  shared_ptr<TeamDAO> teamDAO(new TeamDAO(db));
  teamDAO->init();

  SECTION("Add and get team.")
  {
    multimap<int, long> in = { { 1,2 },{ 1, 23 },{ 0, 69 },{ 0, 420 },{ 4, 10 } };
    teamDAO->addTeams(1, in);
    multimap<int, long> out = teamDAO->getTeams(1);
    REQUIRE(in == out);
  }

  SECTION("Update player")
  {
    multimap<int, long> in = { { 1,2 },{ 1, 23 },{ 0, 69 },{ 0, 420 },{ 4, 10 } };
    teamDAO->addTeams(1, in);
    in.insert(pair<int, long>(5, 6));
    REQUIRE(teamDAO->updateTeams(1, in));
    multimap<int, long> out = teamDAO->getTeams(1);
    REQUIRE(in == out);
  }

  SECTION("Delete team")
  {
    multimap<int, long> in = { {1,2} };
    REQUIRE(teamDAO->getTeams(1).size() == 0);
    teamDAO->addTeams(1, in);
    REQUIRE(teamDAO->deleteTeams(1));
    REQUIRE(teamDAO->getTeams(1).size() == 0);
  }

  SECTION("Get gameids by players.")
  {
    multimap<int, long> in = { { 1,2 },{ 1, 23 },{ 0, 69 },{ 0, 420 },{ 4, 10 } };
    teamDAO->addTeams(1, in);
    in = { { 1,69 },{ 1, 23 },{ 0, 10 } };
    teamDAO->addTeams(2, in);
    in = { { 1,420 },{ 1, 23 },{ 0, 10 } };
    teamDAO->addTeams(3, in);

    set<long> gameIds = teamDAO->getGameIdsOfGamesWithPlayers({69, 420});
    REQUIRE(gameIds.size() == 1);
  }
}

TEST_CASE("Points DAO")
{
  try {
    otl_connect::otl_initialize();
    shared_ptr<otl_connect> db(new otl_connect);
    *db << "DRIVER=SQLite3 ODBC Driver;Database=:memory:;";

    shared_ptr<PointsDAO> pointsDAO(new PointsDAO(db));
    pointsDAO->init();

    SECTION("Add and get points.")
    {
      multimap<int, int> in = { { 1,2 }, { 0, 69 }, { 4, 10 } };
      pointsDAO->addPoints(1, in);
      multimap<int, int> out = pointsDAO->getPoints(1);
      REQUIRE(in == out);
    }

    SECTION("Update points")
    {
      multimap<int, int> in = { { 1,2 }, { 0, 69 }, { 4, 10 } };
      pointsDAO->addPoints(1, in);
      in.insert(pair<int, long>(5, 6));
      REQUIRE(pointsDAO->updatePoints(1, in));
      multimap<int, int> out = pointsDAO->getPoints(1);
      REQUIRE(in == out);
    }
  }
  catch (otl_exception e) {
    cerr << e.msg << endl << e.stm_text << endl << e.var_info << endl;
    FAIL();
  }
}

TEST_CASE("Player Points DAO")
{
  try {
    otl_connect::otl_initialize();
    shared_ptr<otl_connect> db(new otl_connect);
    *db << "DRIVER=SQLite3 ODBC Driver;Database=:memory:;";

    // TODO(gus): when foreign keys work, we'll need to instantiate more DAOs.
    shared_ptr<PlayerPointsDAO> playerPointsDAO(new PlayerPointsDAO(db));
    playerPointsDAO->init();

    SECTION("Add and get points.")
    {
      multimap<int, int> in = { { 1,2 }, { 2,3 } };
      playerPointsDAO->addPoints(1, in);
      multimap<int, int> out = playerPointsDAO->getPoints(1);
      REQUIRE(in == out);
    }

    SECTION("Update points")
    {
      multimap<int, int> in = { { 1,2 }, { 0, 69 }, { 4, 10 } };
      playerPointsDAO->addPoints(1, in);
      in.insert(pair<int, long>(5, 6));
      REQUIRE(playerPointsDAO->updatePoints(1, in));
      multimap<int, int> out = playerPointsDAO->getPoints(1);
      REQUIRE(in == out);
    }
  }
  catch (otl_exception e) {
    cerr << e.msg << endl << e.stm_text << endl << e.var_info << endl;
    FAIL();
  }
}

TEST_CASE("API Key DAO")
{
  try {
    otl_connect::otl_initialize();
    shared_ptr<otl_connect> db(new otl_connect);
    *db << "DRIVER=SQLite3 ODBC Driver;Database=:memory:;";

    shared_ptr<APIKeyDAO> apiKeyDAO(new APIKeyDAO(db));
    apiKeyDAO->init();

    const std::string key = "key";

    SECTION("Add, check, and remove key")
    {
      apiKeyDAO->addKey(key);
      REQUIRE(apiKeyDAO->checkKey(key) == true);
      apiKeyDAO->removeKey(key);
      REQUIRE(apiKeyDAO->checkKey(key) == false);
    }

    SECTION("Add key with description")
    {
      apiKeyDAO->addKey("testkey2", "my second test key");
      REQUIRE(apiKeyDAO->checkKey("testkey2") == true);
      apiKeyDAO->removeKey("testkey2");
      REQUIRE(apiKeyDAO->checkKey("testkey2") == false);
    }

    SECTION("Check nonexistent key")
    {
      REQUIRE(apiKeyDAO->checkKey("") == false);
      REQUIRE(apiKeyDAO->checkKey("not a key") == false);
    }

    // TODO(gus): Very split on what to do here. On one hand, we can say that
    // we shouldn't care whether or not the data is actually in the database,
    // in which case a nonexistent key shouldn't matter. On the other, at some
    // point that fact should be made available to the end user - so if not
    // here, where should it happen?
    SECTION("Remove nonexistent key")
    {
      try {
        apiKeyDAO->removeKey("not a key");
        //FAIL();
      } catch (...) {
        // TODO(gus): add exception
      }
    }
  }
  catch (otl_exception e) {
    cerr << e.msg << endl << e.stm_text << endl << e.var_info << endl;
    FAIL();
  }
}
