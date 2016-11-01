#ifndef _DAO_H_
#define _DAO_H_

class DAO {
public:
	virtual ~DAO() {};
	virtual void init() = 0;
};

#endif