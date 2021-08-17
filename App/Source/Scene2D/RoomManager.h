#pragma once

// Include SingletonTemplate
#include "DesignPatterns\SingletonTemplate.h"
// Include vector
#include <vector>
#include "EntityManager.h"
#include "Map2D.h"


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
	void getprev();
	void getnext();
	void GenerateRoom();
	void NextRoom(int door);
	void currentRoom();

	std::vector<CRoom2D*> roomList;

	// The handler containing the instance of the 2D Map
	CMap2D* cMap2D;

	int cursor;
};


