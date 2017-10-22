#include "api_key_dao.h"

void APIKeyDAO::init()
{
  *db << "create table if not exists keys (hash bigint)";
}

void APIKeyDAO::addKey(const std::string& key) const
{
}

bool APIKeyDAO::checkKey(const std::string& key) const
{
  return false;
}

void APIKeyDAO::removeKey(const std::string& key) const
{
  
}
