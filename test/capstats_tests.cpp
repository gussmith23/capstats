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

using namespace std;

otl_connect db;
mutex dbMutex;

TEST_CASE("Player DAO") {
	
	if (db.connected) {
		db.logoff();
	}

	otl_connect::otl_initialize();
	db << "DRIVER=SQLite3 ODBC Driver;Database=:memory:;";

	PlayerDAO player_dao;
	player_dao.init();

	SECTION("Players added to and retrieved from database") {
		try {
			Player in;
			in.setTelegramId(23);
			in.setName("gus");

			long idBeforeAdd = in.getId();

			REQUIRE(player_dao.addPlayer(in) == true);
			REQUIRE(in.getId() != idBeforeAdd);

			Player out = player_dao.getPlayer(in.getId());
			REQUIRE(out.getName() == "gus");
			REQUIRE(out.getTelegramId() == 23);
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

	if (db.connected) {
		db.logoff();
	}

	otl_connect::otl_initialize();
	db << "DRIVER=SQLite3 ODBC Driver;Database=:memory:;";

	GameDAO game_dao;
	game_dao.init();

	TeamDAO teamDAO;
	teamDAO.init();


	SECTION("Games added to and retrieved from database") {
		try {
			Game g; 
			g.setTime(23);
			long idBeforeAdd = g.getId();

			REQUIRE(game_dao.addGame(g) == true);
			REQUIRE(g.getId() != idBeforeAdd);

			Game out = game_dao.getGame(g.getId());
			REQUIRE(out.getTime() == 23);
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
			Game out = game_dao.getGame(2323);
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

