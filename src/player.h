#ifndef PLAYER_H
#define PLAYER_H

#include <string>

class Player {
public:
	Player() : id(0), telegramId(0), name("") {};
	Player(long id) : Player() { Player::id = id; }

	inline std::string getName() const { return name; }
	inline void setName(const std::string &name) { Player::name = name; }

	inline long getTelegramId() const { return telegramId; }
	inline void setTelegramId(long telegramId) { Player::telegramId = telegramId; }

	inline long getId() const { return id; }
	inline void setId(long id) { Player::id = id; }

	inline std::string getTelegramUsername() const { return telegramUsername; }
	inline void setTelegramUsername(const std::string &telegramUsername) { Player::telegramUsername = telegramUsername; }

private:
	std::string name;
	long telegramId;
	long id;
	std::string telegramUsername;
};

#endif