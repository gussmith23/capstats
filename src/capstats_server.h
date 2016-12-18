#ifndef CAPSTATS_SERVER_H
#define CAPSTATS_SERVER_H

#include "player_dao.h"
#include "game_dao.h"
#include "team_dao.h"
#include "JsonBox.h"
#include <restbed>
#include "include_otl.h"
#include <memory>
#include <string>
#include <vector>

class CapstatsServer {
public:
	CapstatsServer(uint16_t port, std::string databasePath) :
		databasePath(databasePath),
		port(port),
		db(std::shared_ptr<otl_connect>(new otl_connect)),
		teamDAO(std::shared_ptr<TeamDAO>(new TeamDAO(db))),
		playerDAO(std::shared_ptr<PlayerDAO>(new PlayerDAO(db))),
		gameDAO(std::shared_ptr<GameDAO>(new GameDAO(db, teamDAO))) {};

	void init();
	int run();

	/**
	 * Handles a post request in JSON format to /player.
	 * The format of the player should be:
	 *	{
	 *		'name' : name,
	 *		'telegramId' : telegramId
	 *	}
	 */
	void player_post_json(const std::shared_ptr<restbed::Session> session);
	/**
	 * Get a single player by id. E.g. GET /player/[id].
	 */
	void player_get_json(const std::shared_ptr<restbed::Session> session);
	/**
	 * Search for a player. E.g. GET /player?telegramId=1. 
	 */
	void playerWithoutId_get_json(const std::shared_ptr<restbed::Session> session);
	/**
	 * Update a player at a given url. That is, the PUT request should be made
	 * to /player/[id] to update the player with id [id].
	 */
	void player_put_json(const std::shared_ptr<restbed::Session> session);

	/**
	 * Handles a POST request in JSON format to /game.
	 * The format of the game should be:
	 *	{
	 *		teams : {
	 *			teamid : [playerid, playerid, playerid, ...],
	 *			teamid : [playerid, playerid, playerid, ...],
	 *			...
	 *		},
	 *		scores : {
	 *			teamid : score,
	 *			teamid : score,
	 *			...
	 *		},
	 *		time : time
	 *	}
	 */
	void game_post_json(const std::shared_ptr<restbed::Session> session);
	void game_get_json(const std::shared_ptr<restbed::Session> session);
	void game_put_json(const std::shared_ptr<restbed::Session> session);

private:
	uint16_t port;
	std::string databasePath;

	std::shared_ptr<otl_connect> db;
	
	std::shared_ptr<PlayerDAO> playerDAO;
	std::shared_ptr<TeamDAO> teamDAO;
	std::shared_ptr<GameDAO> gameDAO;

	JsonBox::Value playerToJson(const Player& player);
	Player jsonToPlayer(const JsonBox::Value& json);
	JsonBox::Value playersToJson(const std::vector<Player>& players);
	JsonBox::Value gameToJson(const Game& game);
	Game jsonToGame(const JsonBox::Value& json);
};
#endif