#pragma once
#include <iostream>
#include "Attribute.h"
using namespace std;

class BaseAttribute : public Attribute
{
private:
	int HP = 3;
	int Dmg = 1;
	float projectileSpeed = 0.5;
public:
	void setHP(int hp);
	void addHP(int hp);
	int getHP();
	void setDmg(int dmg);
	void addDmg(int dmg);
	int getDmg();
	void addProjSpeed(float speed);
	float getProjSpeed();

	
};