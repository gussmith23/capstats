#include "team_dao.h"

#include <mutex>
#include <sstream>
                
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
	  otl_stream o(1,
			"insert into playergame (game_id, player_id, team) values (:game_id<long>, :player_id<long>, :team<int>)",
			*db);

		for (auto it = teams.begin(); it != teams.end(); ++it)
		{
			o << gameid << it->second << it->first;
		}

		// o.flush();

		return true;
	} 
	catch (otl_exception e)
	{
	  cerr << e.stm_text << endl << e.var_info << endl;
		return false;
	}
}

multimap<int, long> TeamDAO::getTeams(long gameid)
{
	try {
		multimap<int, long> out;
		otl_stream select(1,
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
	catch (otl_exception e)
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

set<long> TeamDAO::getGameIdsOfGamesWithPlayers(vector<long> playerIds)
{
  if (playerIds.size() <= 0) return set<long>(); 
  
  try {
    stringstream selectStringBase;
    selectStringBase << "select * from (";

    int i = 0;
    vector<string> selectStatements;
    for (long id : playerIds)
    {
      stringstream selectStatement;
      selectStatement << "(select game_id from playergame "
          << "where player_id = :player" << i << "id<long>) "
          << "t" << i << " ";
      if (i != 0)
        selectStatement << "on t" << i-1 << ".game_id = t" << i << ".game_id ";
      selectStatements.push_back(selectStatement.str());
      i++;
    }

    copy(selectStatements.begin(), selectStatements.end() - 1, ostream_iterator<string>(selectStringBase, " inner join "));
    selectStringBase << selectStatements.back();
    selectStringBase << ")";

    otl_stream select(50,
        selectStringBase.str().c_str(),
        *db);

    for (long id : playerIds) select << id;

    set<long> out;
    while (!select.eof()) {
      long id;
      select >> id;
      out.insert(id);
    }

    return out;
  } 
  catch (otl_exception e)
  {
    return set<long>();
  }
}
