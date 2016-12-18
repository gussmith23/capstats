#ifndef TEAM_DAO_H
#define TEAM_DAO_H

#include <map>
#include <memory>

#include "dao.h"
#include "player.h"

class TeamDAO : public DAO  {
public:
	TeamDAO(std::shared_ptr<otl_connect> db) : DAO(db) {}

	void init();

	/**
	 * Adds the teams for a single game into the database.
	 * \param gameid the id of the game corresponding to the given teams.
	 * \param teams a multimap which maps a team index in the game to a player on that team.
	 *		for example, some mappings might be 0 -> 1, 1 -> 3
	 *		for a 1v1 game between player 1 and player 3.
	 * \return true for success, false for failure
	 */
	bool addTeams(long gameid, std::multimap<int,long> teams);
	
	/**
	 * Gets the teams frpm a single game in the database.
	 * \param gameid the id of the game.
	 * \return a multimap which maps a team index in the game to a player on that team.
	 *		for example, some mappings might be 0 -> 1, 1 -> 3
	 *		for a 1v1 game between player 1 and player 3.
	 *		If no teams were found, the multimap will be empty.
	 */
	std::multimap<int, long> getTeams(long gameid);

	/**
	 * Updates teams in the database.
	 * \param gameid the id of the game corresponding to the given teams.
	 * \param teams a multimap which maps a team index in the game to a player on that team.
	 *		for example, some mappings might be 0 -> 1, 1 -> 3
	 *		for a 1v1 game between player 1 and player 3.
	 * \return true for success, false for failure
	 */
	bool updateTeams(long gameid, std::multimap<int, long> teams);

	bool deleteTeams(long gameid);
};

#endif // !TEAM_DAO_H
