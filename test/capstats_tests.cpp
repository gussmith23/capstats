#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "include_otl.h"
#include "player_dao.h"
#include "player.h"
#include "game_dao.h"
#include "game.h"
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
			player_dao.addPlayer(Player(1, "gus"));
			Player out = player_dao.getPlayer(1);
			REQUIRE(out.getName() == "gus");
			REQUIRE(out.getTelegramId() == 1);
		}
		catch (otl_exception e) {
			cout << e.msg << endl;
			FAIL();
		}
	}

	SECTION("Invalid player requested") {
		try {
			Player out = player_dao.getPlayer(2323);
			FAIL();
		}
		catch (otl_exception e) {
			cout << e.msg << endl;
			FAIL();
		}
		catch (const string &e) {
			REQUIRE(e == "Player not found");
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
	}

	SECTION("Invalid game gotten") {
		try {
			Game out = game_dao.getGame(2323);
			FAIL();
		}
		catch (otl_exception e) {
			cout << e.msg << endl;
			FAIL();
		}
		catch (const string &e) {
			REQUIRE(e == "Game not found");
		}
	}

}