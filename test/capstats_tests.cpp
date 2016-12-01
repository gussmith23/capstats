#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "include_otl.h"
#include "player_dao.h"
#include "player.h"
#include "game_dao.h"
#include "game.h"
#include "team_dao.h"
#include "capstats_exceptions.h"
#include <iostream>
#include <string>
#include <mutex>
#include <memory>
#include <map>

using namespace std;

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
	shared_ptr<GameDAO> gameDAO(new GameDAO(db, teamDAO));
	gameDAO->init();

	SECTION("Games added to and retrieved from database") {
		try {
			Game g; 
			g.setTime(23);
			long idBeforeAdd = g.getId();
			multimap<int, long> teams = { {1,2}, {3,4}, {3,5}, {6,7} };
			g.setTeams(teams);

			REQUIRE(gameDAO->addGame(g) == true);
			REQUIRE(g.getId() != idBeforeAdd);

			Game out = gameDAO->getGame(g.getId());
			REQUIRE(out.getTime() == 23);
			REQUIRE(out.getTeams() == teams);
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

}

TEST_CASE("Team DAO")
{
	otl_connect::otl_initialize();
	shared_ptr<otl_connect> db(new otl_connect);
	*db << "DRIVER=SQLite3 ODBC Driver;Database=:memory:;";

	shared_ptr<TeamDAO> teamDAO(new TeamDAO(db));
	teamDAO->init();

	SECTION("Add and get team.")
	{
		multimap<int, long> in = { {1,2}, {1, 23}, {0, 69}, {0, 420}, {4, 10} };
		teamDAO->addTeams(1, in);
		multimap<int, long> out = teamDAO->getTeams(1);
		REQUIRE(in == out);
	}
}