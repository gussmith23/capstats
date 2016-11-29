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

	void post_user_handler(const std::shared_ptr<restbed::Session> session);
	void get_user_handler(const std::shared_ptr<restbed::Session> session);
	void get_user_html(const std::shared_ptr<restbed::Session> session);

	/**
	 * Handles a POST request in JSON format to /game.
	 * The format of the game should be:
	 * {
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
	 * }
	 */
	void game_post_json(const std::shared_ptr<restbed::Session> session);

	void game_get_json(const std::shared_ptr<restbed::Session> session);
private:
	uint16_t port;
	std::string databasePath;

	std::shared_ptr<otl_connect> db;
	
	std::shared_ptr<PlayerDAO> playerDAO;
	std::shared_ptr<TeamDAO> teamDAO;
	std::shared_ptr<GameDAO> gameDAO;

	bool addPlayerJson(JsonBox::Object playerJson);
	JsonBox::Value getPlayerJson(long id);

	JsonBox::Value gameToJson(const Game& game);
};
#endif