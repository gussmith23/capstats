#include <ctime>
#include <map>
#include "game_dao.h"
#include "game.h"
#include "include_otl.h"
#include "capstats_exceptions.h"
#include "team_dao.h"

using namespace std;

void GameDAO::init() 
{
	*db << "create table if not exists games (timestamp int)";
}

Game GameDAO::getGame(long gameId) const 
{
	otl_stream o(50, 
		"select timestamp from games where rowid=:rowid<long>", 
		*db);
	o << gameId;
	long timestamp;
	if (!o.eof())
		o >> timestamp;
	else return Game(-1);

	multimap<int, long> teams = teamDAO->getTeams(gameId);

	Game out; 
	out.setId(gameId);
	out.setTime(timestamp);
	out.setTeams(teams);

	return out;
}

bool GameDAO::addGame(Game &game) const
{
	otl_stream insertStream(
		1,
		"insert into games (timestamp) values (:timestamp<long>)",
		*db);
	insertStream << static_cast<long>(game.getTime());
	insertStream.flush();
	otl_stream lastRowidStream(
		1,
		"select last_insert_rowid()",
		*db);
	lastRowidStream.flush();
	string rowid;
	lastRowidStream >> rowid;

	game.setId(stol(rowid));

	// Now that we've got the gameid, add the teams...
	if (teamDAO->addTeams(game.getId(), game.getTeams()) == false) return false;

	return true;
}

bool GameDAO::updateGame(const Game & game) const
{
	if (!teamDAO->updateTeams(game.getId(), game.getTeams())) return false;
	
	try {
		otl_stream update(1,
			"update games "
			"set timestamp=:timestamp<long> "
			"where rowid=:rowid<long>",
			*db);
		update << static_cast<long>(game.getTime())
			<< game.getId();
		update.flush();
		return true;
	}
	catch (otl_exception) {
		return false;
	}
}
