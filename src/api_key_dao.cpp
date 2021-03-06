#include "api_key_dao.h"

void APIKeyDAO::init()
{
  *db << "create table if not exists keys"
      // TODO(gus): magic number in the description length. UUIDs are 36chars
      // but probably good to make this a constant too!
      // NOTE: upped the UUID length too.
      " (key varchar(200) unique not null, description varchar(200))";
}

void APIKeyDAO::addKey(const std::string& key) const 
{
  this->addKey(key, "");
}

void APIKeyDAO::addKey(const std::string& key,
                        const std::string& description) const
{
  otl_stream o(1,
      // TODO(gus): duplicate keys are ignored, which is good for server startup
      // (when the server will try to add the default keys) but bad in the case
      // where someone is actually trying to add a new key. 
      "insert or ignore into keys "
      "(key, description)"
      " values "
      // TODO(gus): magic number
      "(:key<char[200]>, :description<char[200]>)",
      *db);
  o << key;
  // TODO(gus): I don't like this way of doing it. Maybe std::optional would be better?
  if (!description.empty())
    o << description;
  else
    o << otl_null();
  o.flush();
}

bool APIKeyDAO::checkKey(const std::string& key) const
{
  otl_stream o(1,
      // TODO(gus): magic number
      "select key from keys where key=:key<char[200]>",
      *db);
  o << key;
  o.flush();
  if (o.eof()) return false;
  else return true;
}

void APIKeyDAO::removeKey(const std::string& key) const
{
  otl_stream o(1,
      // TODO(gus): magic number
      "delete from keys where key=:key<char[200]>",
      *db);
  o << key;
  o.flush();
}
