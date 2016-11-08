#ifndef GAME_H
#define GAME_H

#include <vector>
#include <ctime>

class Game {
public:
	Game(std::time_t time) { Game::time = time; }

	inline void setDate(std::time_t time) { Game::time = time; }
	inline std::time_t getDate() const { return time; };
public:
	std::time_t time;
};

#endif