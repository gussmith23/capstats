#include "team_dao.h"

#include <mutex>

#include "include_otl.h"


using namespace std;

void TeamDAO::init() {

	/*
	game_id: rowid from game table - a game
	player_id: rowid from player table - a player
	team: the team which the player was on in the given game
	*/
	*db << "create table if not exists playergame ("
		"game_id int,"
		"player_id int,"
		"team int,"
		"primary key (game_id, player_id, team),"
		"foreign key(game_id) references games(rowid),"
		"foreign key(player_id) references players(rowid))";
}

bool TeamDAO::addTeams(long gameid, multimap<int, long> teams)
{
	try {
		otl_stream o(50,
			"insert into playergame (game_id, player_id, team) values (:game_id<long>, :player_id<long>, :team<int>)",
			*db);

		for (auto it = teams.begin(); it != teams.end(); ++it)
		{
			o << gameid << it->second << it->first;
		}

		o.flush();

		return true;
	} 
	catch (otl_exception)
	{
		return false;
	}
}

multimap<int, long> TeamDAO::getTeams(long gameid)
{
	try {
		multimap<int, long> out;
		otl_stream select(50,
			"select player_id, team from playergame where game_id=:game_id<long>",
			*db);
		select << gameid;
		select.flush();
		long playerId; int teamId;
		while (!select.eof())
		{
			select >> playerId >> teamId;
			out.insert(pair<int, long>(teamId,playerId));
		}
		return out;
	}
	catch (otl_exception)
	{
		return multimap<int, long>();
	}
}

bool TeamDAO::updateTeams(long gameId, std::multimap<int, long> teams)
{
	if (!deleteTeams(gameId)) return false;
	return addTeams(gameId, teams);
}

bool TeamDAO::deleteTeams(long gameId)
{
	try {
		multimap<int, long> out;
		otl_stream del(1,
			"delete from playergame where game_id=:gameId<long>",
			*db);
		del << gameId;
		del.flush();
		return true;
	}
	catch (otl_exception)
	{
		return false;
	}
}
