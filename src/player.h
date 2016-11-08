#ifndef PLAYER_H
#define PLAYER_H

#include <string>

class Player {
public:
	Player(long telegramId, std::string name) : telegramId(telegramId), name(name) {}

	inline std::string getName() const { return name; }
	inline void setName(const std::string &name) { Player::name = name; }

	inline long getTelegramId() const { return telegramId; }
	inline void setTelegramId(long telegramId) { Player::telegramId = telegramId; }

private:
	std::string name;
	long telegramId;
};

#endif