#include "player_dao.h"
#include "include_otl.h"

extern otl_connect db;

void PlayerDAO::init() {
	db << "create table if not exists players(name varchar(100), telegram_id int)";
}