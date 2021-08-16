#pragma once

// Include SingletonTemplate
#include "DesignPatterns\SingletonTemplate.h"
// Include vector
#include <vector>
#include "EntityManager.h"


class CRoom2D : public CEntity2D
{
public:
protected:
	CRoom2D();
	~CRoom2D();
	std::vector<CRoom2D*> doorList;
};

class CRoomManager2D : public CSingletonTemplate<CRoomManager2D>
{
	friend CSingletonTemplate<CRoomManager2D>;
public:

protected:
	CRoomManager2D();
	~CRoomManager2D();
	bool getprev();
	bool getnext();
	void getadd();

	std::vector<CRoom2D*> roomList;

	int cursor;
};


