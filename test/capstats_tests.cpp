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