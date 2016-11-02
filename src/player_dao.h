#ifndef _PLAYER_DAO_H_
#define _PLAYER_DAO_H_

#include <string>
#include "dao.h"
#include "player.h"

class PlayerDAO : public DAO {
public:
	virtual void init();
	void addPlayer(const Player player) const;
	Player getPlayer(const unsigned int telegramId) const;
};

#endif