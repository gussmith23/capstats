#include <ctime>
#include <map>
#include "game_dao.h"
#include "game.h"
#include "include_otl.h"
#include "capstats_exceptions.h"
#include "team_dao.h"
#include "points_dao.h"

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
	multimap<int, int> points = pointsDAO->getPoints(gameId);

	Game out; 
	out.setId(gameId);
	out.setTime(timestamp);
	out.setTeams(teams);
	out.setPoints(points);

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

	// and then the points...
	if (pointsDAO->addPoints(game.getId(), game.getPoints()) == false) return false;

	return true;
}

bool GameDAO::updateGame(const Game & game) const
{
	if (!teamDAO->updateTeams(game.getId(), game.getTeams())) return false;
	if (!pointsDAO->updatePoints(game.getId(), game.getPoints())) return false;
	
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

std::vector<Game> GameDAO::findGames(long id) const
{
  stringstream selectStringBase;
  selectStringBase << "select rowid ";
  selectStringBase << "from games ";

  vector<string> whereClauses;
  if (id >=0) whereClauses.push_back("rowid=:rowid<long>");

  stringstream whereClauseString;
  if (whereClauses.size() > 0) 
  {
    whereClauseString << "where ";
    std::copy(whereClauses.begin(), whereClauses.end() - 1, ostream_iterator<string>(whereClauseString, " AND "));
    whereClauseString << whereClauses.back();
  }

  string selectString = selectStringBase.str() + whereClauseString.str();
  otl_stream select(50,
      selectString.c_str(),
      *db);

  if (id >= 0) select << id;

  vector<Game> out;
  while (!select.eof()) 
  {
    long id; 
    select >> id;
    out.push_back(getGame(id));
  }

  return out;

}
