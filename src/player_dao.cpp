#include "player_dao.h"
#include "include_otl.h"
#include "capstats_exceptions.h"
#include <string>
#include <mutex>

using namespace std;

void PlayerDAO::init() {
	*db << "create table if not exists players("
		"name varchar(100),"
		"telegram_id int,"
		"telegram_username varchar(100))";
}

bool PlayerDAO::addPlayer(Player &player) const
{
	otl_stream o(1,
		"insert into players "
		"(name, telegram_id, telegram_username)"
		" values "
		"(:name<char[100]>, :telegram_id<long>, :telegram_username<char[100]>)",
		*db);
	o
		<< player.getName()
		<< player.getTelegramId()
		<< player.getTelegramUsername();
	o.flush();
	otl_stream lastRowidStream(
		1,
		"select last_insert_rowid()",
		*db);
	lastRowidStream.flush();
	string rowid;
	lastRowidStream >> rowid;
	
	player.setId(stol(rowid));

	return true;
}

Player PlayerDAO::getPlayer(long id) const
{
	otl_stream o(50, 
		"select name, telegram_id, telegram_username from players where rowid=:id<long>", 
		*db);
	o << id;
	string name; long telegramId; string telegramUsername;
	if (!o.eof())
		o >> name >> telegramId >> telegramUsername;
	else return Player(-1);

	Player out;
	out.setName(name);
	out.setTelegramId(telegramId);
	out.setTelegramUsername(telegramUsername);

	return out;
}
