#ifndef GAME_H
#define GAME_H

#include <vector>
#include <ctime>
#include <map>

class Game {
public:
	inline Game() : id(-1), time(0), teams(std::multimap<int, long>()), points(std::multimap<int, int>()),
            playerPoints(std::multimap<int,int>()) {};
	inline Game(long id) : Game() { Game::id = id; }

	inline void setTime(std::time_t time) { Game::time = time; }
	inline std::time_t getTime() const { return time; }

	inline void setTeams(const std::multimap<int, long> &teams) { Game::teams = teams; }
	inline std::multimap<int, long> getTeams() const { return teams; }

	inline void setPoints(const std::multimap<int, int> &points) { Game::points = points; }
	inline std::multimap<int, int> getPoints() const { return points; }

	inline void setPlayerPoints(const std::multimap<int, int> &playerPoints)
            { Game::playerPoints = playerPoints; }
	inline std::multimap<int, int> getPlayerPoints() const { return playerPoints; }

	inline void setId(long id) { Game::id = id; }
	inline long getId() const { return id; }

private:
	std::time_t time;
	std::multimap<int, long> teams;
	std::multimap<int, int> points;
	std::multimap<int, int> playerPoints;
	long id;
};

#endif
