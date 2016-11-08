#ifndef GAME_DAO_H
#define GAME_DAO_H

#include "dao.h"
#include "game.h"

class GameDAO : DAO {
public:
	void init();
	Game getGame(long id) const;
	long addGame(const Game &game) const;
};

#endif // !GAME_DAO_H
