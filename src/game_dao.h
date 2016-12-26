#ifndef GAME_DAO_H
#define GAME_DAO_H

#include "dao.h"
#include "game.h"
#include "team_dao.h"
#include "points_dao.h"
#include <memory>
#include <vector>

class GameDAO : DAO {
  public:
    GameDAO(std::shared_ptr<otl_connect> db, std::shared_ptr<TeamDAO> teamDAO, std::shared_ptr<PointsDAO> pointsDAO) : DAO(db), teamDAO(teamDAO), pointsDAO(pointsDAO) { };

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

    /**
     * Updates game in the database. Note that the update will happen based
     * on the id of the Game object, so it must be a valid id.
     * \param game the game to update, containing the new data to put in
     *		the database.
     * \return true if success.
     */
    bool updateGame(const Game& player) const;

    /**
     * Find a game with the given fields.
     */
    std::vector<Game> findGames(long id = -1, std::vector<long> playerIds = {}) const;

  private:
    std::shared_ptr<TeamDAO> teamDAO;
    std::shared_ptr<PointsDAO> pointsDAO;
};

#endif // !GAME_DAO_H
