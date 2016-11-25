#include <ctime>
#include <mutex>
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

	Game out; 
	out.setId(gameId);
	out.setTime(timestamp);

	return out;
}

bool GameDAO::addGame(Game &game) const
{
	if (teamDAO->addTeams(game.getId(), game.getTeams()) == false) return false;

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

	return true;
}