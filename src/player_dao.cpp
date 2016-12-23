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

  if (player.getName().size() > 0) o << player.getName();
  else o << otl_null();

  if (player.getTelegramId() >= 0) o << player.getTelegramId();
  else o << otl_null();

  if (player.getTelegramUsername().size() > 0) o << player.getTelegramUsername(); 
  else o <<otl_null();

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

  if (o.eof()) return Player(-1);

  string name; long telegramId; string telegramUsername;
  o >> name;
  if (o.is_null()) name = "";
  o >> telegramId;
  if (o.is_null()) telegramId = -1;
  o >> telegramUsername;
  if (o.is_null()) telegramUsername = "";

  Player out(id);
  out.setName(name);
  out.setTelegramId(telegramId);
  out.setTelegramUsername(telegramUsername);

  return out;
}

Player PlayerDAO::findPlayerByTelegramUsername(const string& telegramUsername)
{
  otl_stream o(50,
      "select rowid from players where telegramUsername=:telegramUsername<char[100]>",
      *db);
  o << telegramUsername;

  if (o.eof()) return Player(-1);

  long id;
  o >> id;

  return getPlayer(id);
}

std::vector<Player> PlayerDAO::findPlayers(long id, const std::string & name, 
    long telegramId, const std::string & telegramUsername)
{
  stringstream selectStringBase;
  selectStringBase << "select rowid ";
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
  while (!select.eof()) 
  {
    long id; 
    select >> id;
    out.push_back(getPlayer(id));
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
