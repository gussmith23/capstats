#include "player_dao.h"
#include "include_otl.h"
#include "capstats_exceptions.h"
#include <string>
#include <mutex>

using namespace std;

extern otl_connect db;
extern mutex dbMutex;

void PlayerDAO::init() {
	db << "create table if not exists players(name varchar(100), telegram_id int)";
}

long PlayerDAO::addPlayer(const Player &player) const
{
	lock_guard<mutex> lock(dbMutex);

	otl_stream o(1,
		"insert into players (name, telegram_id) values (:name<char[100]>, :telegram_id<long>)",
		db);
	o << player.getName() << player.getTelegramId();
	o.flush();
	otl_stream lastRowidStream(
		1,
		"select last_insert_rowid()",
		db);
	lastRowidStream.flush();
	string rowid;
	lastRowidStream >> rowid;
	
	return stol(rowid);
}

Player PlayerDAO::getPlayer(long id) const
{
	otl_stream o((50), "select name, telegram_id from players where rowid=:id<long>", db);
	o << id;
	string name; long telegramId;
	if (!o.eof())
		o >> name >> telegramId;
	else throw PlayerNotFoundException();

	Player out;
	out.setName(name);
	out.setTelegramId(telegramId);

	return out;
}
