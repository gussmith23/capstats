#ifndef API_KEY_DAO_H
#define API_KEY_DAO_H

#include <string>

#include "dao.h"

class APIKeyDAO : DAO {
 public:
  APIKeyDAO(std::shared_ptr<otl_connect> db) : DAO(db) { };

  void init();

  /*
  TODO(gus) These methods, as opposed to the others I've made so far for other
  DAOs, do not return bools on success. I'm starting to think that should be an
  exceptional case, so I'll try that out here. In the future either the rest
  should be converted to this style, or this should be converted to the other
  style.
  */
  
  void addKey(const std::string& key) const;
  void addKey(const std::string& key, const std::string& description) const;

  bool checkKey(const std::string& key) const;

  void removeKey(const std::string& key) const;
};

#endif // !API_KEY_DAO_H
