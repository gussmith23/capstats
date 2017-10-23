#include "api_key_dao.h"

void APIKeyDAO::init()
{
  *db << "create table if not exists keys"
      // TODO(gus): magic number in the description length. UUIDs are 36chars
      // but probably good to make this a constant too!
      " (key varchar(36) unique not null, description varchar(200))";
}

void APIKeyDAO::addKey(const std::string& key) const 
{
  this->addKey(key, "");
}

void APIKeyDAO::addKey(const std::string& key,
                        const std::string& description) const
{
  otl_stream o(1,
      "insert into keys "
      "(key, description)"
      " values "
      // TODO(gus): magic number
      "(:key<char[36]>, :description<char[200]>)",
      *db);
  o << key;
  if (!description.empty())
    o << description;
  else
    o << otl_null();
  o.flush();
}

bool APIKeyDAO::checkKey(const std::string& key) const
{
  otl_stream o(1,
      "select key from keys where key=:key<char[36]>",
      *db);
  o << key;
  o.flush();
  if (o.eof()) return false;
  else return true;
}

void APIKeyDAO::removeKey(const std::string& key) const
{
  otl_stream o(1,
      "delete from keys where key=:key<char[36]>",
      *db);
  o << key;
  o.flush();
}
