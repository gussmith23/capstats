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

int TeamDAO::addTeams(long gameid, std::multimap<int, long> teams)
{
	try {
		otl_stream o(50,
			"insert into playergame (game_id, player_id, team) values (:game_id<long>, :player_id<long>, :team<int>)",
			db);

		for (auto it = teams.begin(); it != teams.end(); ++it)
		{
			o << gameid << it->second << it->first;
		}

		o.flush();

		return 0;
	} 
	catch (otl_exception)
	{
		return 1;
	}
}