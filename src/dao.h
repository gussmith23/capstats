#ifndef _DAO_H_
#define _DAO_H_

#include "include_otl.h"
#include <memory>

class DAO {
public:
	DAO(std::shared_ptr<otl_connect> db) { DAO::db = db; }
	virtual ~DAO() {};
	virtual void init() {};
protected:
	std::shared_ptr<otl_connect> db;
};

#endif