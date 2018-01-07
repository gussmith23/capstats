#ifndef CAPSTATS_SERVER_H
#define CAPSTATS_SERVER_H

#include "api_key_dao.h"
#include "player_dao.h"
#include "player_points_dao.hpp"
#include "game_dao.h"
#include "team_dao.h"
#include "JsonBox.h"
#include "restbed"
#include "include_otl.h"

#include <memory>
#include <string>
#include <vector>

struct DefaultUUID {
  std::string uuid;
  std::string description;
};

class CapstatsServer {
public:
	CapstatsServer(uint16_t port, 
                        std::string databasePath, 
                        const std::vector<DefaultUUID>& defaultUUIDs,
                        bool checkAPIKeys) :
                checkAPIKeys(checkAPIKeys),
                defaultUUIDs(defaultUUIDs),
		databasePath(databasePath),
		port(port),
		db(std::shared_ptr<otl_connect>(new otl_connect)),
		teamDAO(std::shared_ptr<TeamDAO>(new TeamDAO(db))),
		pointsDAO(std::shared_ptr<PointsDAO>(new PointsDAO(db))),
		playerPointsDAO(std::shared_ptr<PlayerPointsDAO>(new PlayerPointsDAO(db))),
		playerDAO(std::shared_ptr<PlayerDAO>(new PlayerDAO(db))),
		gameDAO(std::shared_ptr<GameDAO>(new GameDAO(db, teamDAO, pointsDAO))),
                apiKeyDAO(std::make_shared<APIKeyDAO>(db)) 
                {}

        // TODO(gus): get rid of this; RAII
	void init();
	int run();

	/**
	 * Handles a post request in JSON format to /player.
	 * The format of the player should be:
	 *	{
	 *		'name' : name,
	 *		'telegramId' : telegramId,
	 *		'telegramUsername' : telegramUsername
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
	 *		points : {
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
	/**
	 * Search for a game. E.g. GET /game?someField=1.
         * TODO add test.
	 */
	void gameWithoutId_get_json(const std::shared_ptr<restbed::Session> session);

private:
        /**
         * Returns true if authentication is not needed, or if authentication is needed
         * and the API key attached is valid. Otherwise, returns false.
         */
        bool authenticate(const std::shared_ptr<const restbed::Request> request);

	uint16_t port;
	std::string databasePath;

	std::shared_ptr<otl_connect> db;

        std::vector<DefaultUUID> defaultUUIDs;

        bool checkAPIKeys;
	
	std::shared_ptr<PlayerDAO> playerDAO;
	std::shared_ptr<TeamDAO> teamDAO;
	std::shared_ptr<PointsDAO> pointsDAO;
	std::shared_ptr<PlayerPointsDAO> playerPointsDAO;
        std::shared_ptr<GameDAO> gameDAO;
	std::shared_ptr<APIKeyDAO> apiKeyDAO;

	JsonBox::Value playerToJson(const Player& player);
	Player jsonToPlayer(const JsonBox::Value& json);
	JsonBox::Value playersToJson(const std::vector<Player>& players);
	JsonBox::Value gameToJson(const Game& game);
	Game jsonToGame(const JsonBox::Value& json);
	JsonBox::Value gamesToJson(const std::vector<Game>& games);
};
#endif
