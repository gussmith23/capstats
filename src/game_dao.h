#ifndef GAME_DAO_H
#define GAME_DAO_H

#include "dao.h"
#include "game.h"
#include "team_dao.h"
#include <memory>

class GameDAO : DAO {
public:
	GameDAO(std::shared_ptr<otl_connect> db, std::shared_ptr<TeamDAO> teamDAO) : DAO(db), teamDAO(teamDAO) { };

	void init();

	/**
	* Gets a game from the database by rowid.
	* \param id the rowid of the game
	* \return a Game object representing the game. If the Game's id is
	*	set to -1, then the game was not found.
	*/
	Game getGame(long id) const;

	/**
	* Adds a game into the database. Upon successful insertion, this function 
	* overwrites the "id" field in the Game object to the database rowid of 
	* the newly added Game.
	* \param game the game object to add.
	* \return true for success, false for failure
	*/
	bool addGame(Game &game) const;

private:
	std::shared_ptr<TeamDAO> teamDAO;
};

#endif // !GAME_DAO_H
