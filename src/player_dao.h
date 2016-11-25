#ifndef _PLAYER_DAO_H_
#define _PLAYER_DAO_H_

#include <string>
#include "dao.h"
#include "player.h"
#include <memory>

class PlayerDAO : public DAO {
public:
	PlayerDAO(std::shared_ptr<otl_connect> db) : DAO(db) {}

	virtual void init();

	/**
	* Adds a player into the database. Upon successful insertion, this function
	* overwrites the "id" field in the Player object to the database rowid of
	* the newly added Player.
	* \param player the Player object to add.
	* \return true for success, false for failure
	*/
	bool addPlayer(Player &player) const;

	/**
	* Gets a player from the database by rowid.
	* \param id the rowid of the player
	* \return a Player object representing the player. If the Player's id is
	*	set to -1, then the player was not found.
	*/
	Player getPlayer(long id) const;
};

#endif