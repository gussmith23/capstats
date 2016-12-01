#include "player_dao.h"
#include "include_otl.h"
#include "capstats_exceptions.h"
#include <string>
#include <mutex>
#include <sstream>

using namespace std;

void PlayerDAO::init() {
	*db << "create table if not exists players("
		"name varchar(100),"
		"telegramId int unique,"
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

std::vector<Player> PlayerDAO::findPlayers(long id, const std::string & name, 
	long telegramId, const std::string & telegramUsername)
{
	stringstream selectStringBase;
	selectStringBase << "select name, telegramId, telegramUsername, rowid ";
	selectStringBase << "from players ";
	
	vector<string> whereClauses;
	if (id >= 0) whereClauses.push_back("rowid=:rowid<long>");
	if (name != "") whereClauses.push_back("name=:name<char[100]>");
	if (telegramId >= 0) whereClauses.push_back("telegramId=:telegramId<long>");
	if (telegramUsername != "") whereClauses.push_back("telegramUsername=:telegramUsername<char[100]>");

	stringstream whereClauseString;
	if (whereClauses.size() > 0) {
		whereClauseString << "where ";
		std::copy(whereClauses.begin(), whereClauses.end() - 1, ostream_iterator<string>(whereClauseString, " AND "));
		whereClauseString << whereClauses.back();
	}

	string selectString = selectStringBase.str() + whereClauseString.str();
	otl_stream select(50,
		selectString.c_str(),
		*db);

	if (id >= 0) select << id;
	if (name != "") select << name;
	if (telegramId >= 0) select << telegramId;
	if (telegramUsername != "") select << telegramUsername;

	vector<Player> out;
	while (!select.eof()) {
		long id_out = -1; 
		string name_out = "";
		long telegramId_out = -1;
		string telegramUsername_out = "";

		select >> name_out >> telegramId_out >> telegramUsername_out >> id_out;

		Player p;
		p.setId(id_out); p.setName(name_out); p.setTelegramId(telegramId_out);
		p.setTelegramUsername(telegramUsername_out);

		out.push_back(p);
	}

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
