#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "include_otl.h"
#include "player_dao.h"
#include "player.h"
#include <iostream>
#include <string>

using namespace std;

otl_connect db;

TEST_CASE("Players added to and retrieved from database") {
	otl_connect::otl_initialize();
	db << "DRIVER=SQLite3 ODBC Driver;Database=:memory:;";
	PlayerDAO player_dao;
	player_dao.init();
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