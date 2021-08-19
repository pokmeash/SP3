#pragma once
#include <iostream>
using namespace std;

class Attribute
{
private:
	int HP;
	int Dmg;
	float projectileSpeed;
public:
	virtual void setHP(int hp) = 0;
	virtual void addHP(int hp) = 0;
	virtual int getHP() = 0;
	virtual void setDmg(int dmg) = 0;
	virtual void addDmg(int dmg) = 0;
	virtual int getDmg() = 0;
	virtual void addProjSpeed(float speed) = 0;
	virtual float getProjSpeed() = 0;
};