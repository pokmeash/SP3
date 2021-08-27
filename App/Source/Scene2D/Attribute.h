#pragma once
#include <iostream>
using namespace std;

class Attribute
{
protected:
	int HP;
	int maxHP;
	int Dmg;
	float projectileSpeed;
public:
	virtual int getHP() {
		return HP;
	}
	virtual int getDmg()
	{
		return Dmg;
	}
	virtual int getMaxHP() {
		return maxHP;
	}
	virtual float getProjSpeed()
	{
		return projectileSpeed;
	}
	virtual void setHP(int hp)
	{
		HP = hp;
		maxHP = hp;
	}
	virtual void setDmg(int dmg)
	{
		Dmg = dmg;
	}
	virtual void setProjSpeed(float speed)
	{
		projectileSpeed = speed;
	}
	virtual void addHP(int hp)
	{
		HP += hp;
	}
	virtual void addDmg(int dmg)
	{
		Dmg += dmg;
	}
	virtual void addProjSpeed(float speed)
	{
		projectileSpeed += speed;
	}

};