#ifndef PLAYER_H
#define PLAYER_H

#include <string>

class Player {
public:
	Player(unsigned int telegramId, std::string name) : telegramId(telegramId), name(name) {}

	inline std::string getName() const { return name; }
	inline void setName(const std::string new_name) { name = new_name; }

	inline unsigned int getTelegramId() const { return telegramId; }
	inline void setTelegramId(const unsigned int newTelegramId) { telegramId = newTelegramId; }

private:
	std::string name;
	unsigned int telegramId;
};

#endif