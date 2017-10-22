#include "api_key_dao.h"

void APIKeyDAO::init()
{
  *db << "create table if not exists keys (hash bigint)";
}

void addKey(const std::string key) 
{
}

bool checkKey(const std::string key) 
{
  return false;
}

void removeKey(const std::string key)
{
  
}
