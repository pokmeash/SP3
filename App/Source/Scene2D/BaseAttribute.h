#pragma once
#include <iostream>
#include "Attribute.h"
using namespace std;
// Player
class BaseAttribute : public Attribute
{
public:
	BaseAttribute(int hp, int dmg, float projspeed)
	{
		setHP(hp);
		setDmg(dmg);
		setProjSpeed(projspeed);
	}


	
};