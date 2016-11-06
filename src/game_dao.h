#ifndef GAME_DAO_H
#define GAME_DAO_H

#include "dao.h"
#include "game.h"

class GameDAO : DAO {
public:
	void init();
	Game getGame(const long long id) const;
	int addGame(const Game &game) const;
};

#endif // !GAME_DAO_H
