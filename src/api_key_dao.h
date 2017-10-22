#ifndef API_KEY_DAO_H
#define API_KEY_DAO_H

#include "dao.h"

class APIKeyDAO : DAO {
 public:
  APIKeyDAO(std::shared_ptr<otl_connect> db) : DAO(db) { };

  void init();
  
  void addKey(const std::string key) const;

  bool checkKey(const std::string key) const;

  void removeKey(const std::string key) const;
};

#endif // !API_KEY_DAO_H
