#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "include_otl.h"
#include "player_dao.h"
#include "player.h"
#include "game_dao.h"
#include "game.h"
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
			long id = player_dao.addPlayer(in);
			Player out = player_dao.getPlayer(id);
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

	SECTION("Invalid player requested throws exception") {
		try {
			Player out = player_dao.getPlayer(2323);
			FAIL();
		}
		catch (PlayerNotFoundException) {
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


	SECTION("Games added to and retrieved from database") {
		try {
			int id = game_dao.addGame(Game(23));
			REQUIRE(id != 0);
			Game out = game_dao.getGame(id);
			REQUIRE(out.getDate() == 23);
		}
		catch (otl_exception e) {
			cout << e.msg << endl;
			FAIL();
		}
		catch (...) {
			FAIL();
		}
	}

	SECTION("Invalid game requested throws exception") {
		try {
			Game out = game_dao.getGame(2323);
			FAIL();
		}
		catch (GameNotFoundException) {
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