#ifndef PLAYER_H
#define PLAYER_H

#include <string>

class Player {
public:
	Player(unsigned int id) : Player(id, "") {}
	Player(unsigned int id, std::string name) : id(id), name(name) {}

	const unsigned int id;

	std::string getName() { return name; }
	void setName(std::string new_name) { name = new_name; }

private:
	std::string name;
};

#endif