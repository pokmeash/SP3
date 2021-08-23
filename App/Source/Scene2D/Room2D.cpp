#include "Room2D.h"

Room2D::Room2D(DOOR_TYPE typeValue)
	: type(typeValue),
	prevRoom(0),
	nextRoom(0),
	locked(true)
{
}

Room2D::~Room2D()
{
}