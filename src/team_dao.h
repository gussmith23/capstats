#ifndef TEAM_DAO_H
#define TEAM_DAO_H

#include <map>

#include "dao.h"
#include "player.h"

class TeamDAO : public DAO  {
public:
	void init();

	void addTeams(std::multimap<int,Player>);
};

#endif // !TEAM_DAO_H
