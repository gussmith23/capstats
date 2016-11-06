#include "player_dao.h"
#include "include_otl.h"
#include <string>

using namespace std;

extern otl_connect db;

void PlayerDAO::init() {
	db << "create table if not exists players(name varchar(100), telegram_id int)";
}

void PlayerDAO::addPlayer(const Player player) const
{
	otl_stream o;
	o.setBufSize(100);

	db <<= "insert into players (name, telegram_id) values (:name<char[100]>, :telegram_id<unsigned>)";

	db >> o;

	o << player.getName() << player.getTelegramId();
}

Player PlayerDAO::getPlayer(const unsigned int telegramId) const
{
	otl_stream o((50), "select name from players where telegram_id=:telegram_id<unsigned>", db);
	o << telegramId;
	string name;
	if (!o.eof())
		o >> name;
	else throw string("Player not found");

	return Player(telegramId, name);
}
