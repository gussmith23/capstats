#include "player_points_dao.hpp"

#include <mutex>

#include "include_otl.h"


using namespace std;

void PlayerPointsDAO::init() {

  *db << "create table if not exists playerpoints ("
          "game_id int,"
          "player_id int,"
          "points int,"
          "primary key (game_id, player_id),"
          "foreign key(game_id) references games(rowid),"
          "foreign key(player_id) references players(rowid))";
}

bool PlayerPointsDAO::addPoints(long gameid, multimap<int, int> points)
{
  try {
    otl_stream o(1,
        "insert into playerpoints (game_id, player_id, points) values (:game_id<long>, :player_id<int>, :points<int>)",
        *db);

    for (auto it = points.begin(); it != points.end(); ++it)
      o << gameid << it->first << it->second;

    o.flush();

    return true;
  }
  catch (otl_exception e)
  {
    return false;
  }
}

multimap<int, int> PlayerPointsDAO::getPoints(long gameid)
{
  try {
    multimap<int, int> out;
    otl_stream select(1,
        "select player_id, points from playerpoints where game_id=:game_id<long>",
        *db);
    select << gameid;
    select.flush();
    int playerid, points;
    while (!select.eof())
    {
      select >> playerid >> points;
      out.insert(pair<int, int>(playerid, points));
    }
    return out;
  }
  catch (otl_exception e)
  {
    return multimap<int, int>();
  }
}

bool PlayerPointsDAO::updatePoints(long gameId, std::multimap<int, int> points)
{
  if (!deletePoints(gameId)) return false;
  return addPoints(gameId, points);
}

bool PlayerPointsDAO::deletePoints(long gameId)
{
  try {
    otl_stream del(1,
        "delete from playerpoints where game_id=:gameId<long>",
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
