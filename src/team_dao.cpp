#include "team_dao.h"

#include <mutex>

#include "include_otl.h"


using namespace std;

extern otl_connect db;
extern mutex dbMutex;

void TeamDAO::init() {

	/*
	game_id: rowid from game table - a game
	player_id: rowid from player table - a player
	team: the team which the player was on in the given game
	*/
	db << "create table if not exists playergame ("
		"game_id int,"
		"player_id int,"
		"team int,"
		"primary key (game_id, player_id, team),"
		"foreign key(game_id) references games(rowid),"
		"foreign key(player_id) references players(rowid))";
}