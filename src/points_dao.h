#ifndef POINTS_DAO_H
#define POINTS_DAO_H

#include <map>
#include <memory>

#include "dao.h"
#include "player.h"

class PointsDAO : public DAO {
public:
	PointsDAO(std::shared_ptr<otl_connect> db) : DAO(db) {}

	void init();

	/**
	 * Adds the points for a single game into the database.
	 * \param gameid the id of the game corresponding to the given teams.
	 * \param teams a multimap which maps a team index in the game to the team's points.\
	 * \return true for success, false for failure
	 */
	bool addPoints(long gameid, std::multimap<int, int> points);

	/**
	 * Gets the points for a single game.
	 * \param gameid the id of the game.
	 * \return a multimap which maps a team index in the game to the points for the team.
	 *		If no teams were found, the multimap will be empty.
	 */
	std::multimap<int, int> getPoints(long gameid);

	/**
	* Updates points in the database.
	* \param gameid the id of the game corresponding to the given points.
	* \param teams a multimap with the new info
	* \return true for success, false for failure
	*/
	bool updatePoints(long gameid, std::multimap<int, int> points);

	bool deletePoints(long gameid);
};

#endif // !POINTS_DAO_H
