#include "RoomManager.h"
#include <iostream>
using namespace std;

// Include Shader Manager
#include "RenderControl\ShaderManager.h"

#include "System\filesystem.h"



/**
 @brief Constructor This constructor has protected access modifier as this class will be a Singleton
 */
CRoomManager2D::CRoomManager2D(void)
{
	cursor = 1;
}

/**
 @brief Destructor
 */
CRoomManager2D::~CRoomManager2D(void)
{

}

bool CRoomManager2D::getprev()
{
	cursor--;
}

bool CRoomManager2D::getnext()
{
	cursor++;
}

void CRoomManager2D::GenerateRoom()
{
	int Randint = rand() % 100 + 1; // rand int between 1 - 100
	cMap2D->SetCurrentLevel(Randint);
}

void CRoomManager2D::NextRoom(int door)
{

}

void CRoomManager2D::currentRoom()
{
	cMap2D->SetCurrentLevel(cursor);
}