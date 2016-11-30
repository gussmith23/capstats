#include "player_dao.h"
#include "include_otl.h"
#include "capstats_exceptions.h"
#include <string>
#include <mutex>

using namespace std;

void PlayerDAO::init() {
	*db << "create table if not exists players("
		"name varchar(100),"
		"telegramId int,"
		"telegramUsername varchar(100))";
}

bool PlayerDAO::addPlayer(Player &player) const
{
	otl_stream o(1,
		"insert into players "
		"(name, telegramId, telegramUsername)"
		" values "
		"(:name<char[100]>, :telegramId<long>, :telegramUsername<char[100]>)",
		*db);
	
	o	<< player.getName()
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
		"select name, telegramId, telegramUsername from players where rowid=:id<long>", 
		*db);
	o << id;
	string name; long telegramId; string telegramUsername;
	if (!o.eof())
		o >> name >> telegramId >> telegramUsername;
	else return Player(-1);

	Player out(id);
	out.setName(name);
	out.setTelegramId(telegramId);
	out.setTelegramUsername(telegramUsername);

	return out;
}

Player PlayerDAO::findPlayerByTelegramUsername(const string& telegramUsername)
{
	otl_stream o(50,
		"select name, telegramId, rowid from players where telegramUsername=:telegramUsername<char[100]>",
		*db);
	o << telegramUsername;
	string name; long telegramId; long id;
	if (!o.eof())
		o >> name >> telegramId >> id;
	else return Player(-1);

	Player out;
	out.setId(id);
	out.setName(name);
	out.setTelegramId(telegramId);
	out.setTelegramUsername(telegramUsername);

	return out;
}

bool PlayerDAO::updatePlayer(const Player& player) const
{
	otl_stream update(1,
		"update players "
		"set name=:name<char[100]>, telegramId=:telegramId<long>, telegramUsername=:telegramUsername<char[100]> "
		"where rowid=:rowid<long>",
		*db);
	update << player.getName()
		<< player.getTelegramId()
		<< player.getTelegramUsername()
		<< player.getId();
	update.flush();
	return true;
}
