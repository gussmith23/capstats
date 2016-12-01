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

	/**
	 * Find a player in the database by telegram username.
	 * \param telegramUsername the username to find.
	 * \return a Player object representing the player. If the Player's id is
	 *	set to -1, then the player was not found.
     */
	Player findPlayerByTelegramUsername(const std::string& telegramUsername);

	/**
	 * Find a player with the given fields.
	 */
	std::vector<Player> findPlayers(long id = -1, const std::string& name = "",
		long telegramId = -1, const std::string& telegramUsername = "");

	/**
	 * Updates player in the database. Note that the update will happen based
	 * on the id of the Player object, so it must be a valid id.
	 * \param player the player to update, containing the new data to put in
	 *		the database.
	 * \return true if success.
	 */
	bool updatePlayer(const Player& player) const;
};

#endif