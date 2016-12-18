#include "points_dao.h"

#include <mutex>

#include "include_otl.h"


using namespace std;

void PointsDAO::init() {

	*db << "create table if not exists points ("
		"game_id int,"
		"team int,"
		"points int,"
		"primary key (game_id, team),"
		"foreign key(game_id) references games(rowid))";
}

bool PointsDAO::addPoints(long gameid, multimap<int, int> points)
{
	try {
		otl_stream o(50,
			"insert into points (game_id, team, points) values (:game_id<long>, :team<int>, :points<int>)",
			*db);

		for (auto it = points.begin(); it != points.end(); ++it)
		{
			o << gameid << it->first << it->second;
		}

		o.flush();

		return true;
	}
	catch (otl_exception e)
	{
		return false;
	}
}

multimap<int, int> PointsDAO::getPoints(long gameid)
{
	try {
		multimap<int, int> out;
		otl_stream select(50,
			"select team, points from points where game_id=:game_id<long>",
			*db);
		select << gameid;
		select.flush();
		int teamId, points;
		while (!select.eof())
		{
			select >> teamId >> points;
			out.insert(pair<int, int>(teamId, points));
		}
		return out;
	}
	catch (otl_exception e)
	{
		return multimap<int, int>();
	}
}

bool PointsDAO::updatePoints(long gameId, std::multimap<int, int> points)
{
	if (!deletePoints(gameId)) return false;
	return addPoints(gameId, points);
}

bool PointsDAO::deletePoints(long gameId)
{
	try {
		otl_stream del(1,
			"delete from points where game_id=:gameId<long>",
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
