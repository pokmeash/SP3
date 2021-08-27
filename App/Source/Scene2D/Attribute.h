#pragma once
#include <iostream>
using namespace std;

class Attribute
{
protected:
	int HP;
	int maxHP = 0;
	int Dmg;
	float MovingSpeed;
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
	virtual float getMoveSpeed()
	{
		return MovingSpeed;
	}
	virtual void setHP(int hp)
	{
		HP = hp;
		if (hp > maxHP)
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
	virtual void setMaxHP(int maxhp)
	{
		maxHP = maxhp;
	}
	virtual void setMoveSpeed(float MoveSpeed)
	{
		MovingSpeed = MoveSpeed;
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
	virtual void addMoveSpeed(float MoveSpeed)
	{
		MovingSpeed += MoveSpeed;
	}
	virtual void addMaxHP(int maxhp)
	{
		maxHP += maxhp;
	}
	virtual void minusHP(int amtOfHP)
	{
		HP -= amtOfHP;
	}

};