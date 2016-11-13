#ifndef TEAM_DAO_H
#define TEAM_DAO_H

#include <map>

#include "dao.h"
#include "player.h"

class TeamDAO : public DAO  {
public:
	void init();

	/**
	 * Adds the teams for a single game into the database.
	 * \param gameid the id of the game corresponding to the given teams.
	 * \param teams a multimap which maps a team index in the game to a player on that team.
	 *		for example, some mappings might be 0 -> 1, 1 -> 3
	 *		for a 1v1 game between player 1 and player 3.
	 * \return 0 for success, anything else is failure
	 */
	int addTeams(long gameid, std::multimap<int,long> teams);
};

#endif // !TEAM_DAO_H
