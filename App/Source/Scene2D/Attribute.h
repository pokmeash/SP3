#pragma once
#include <iostream>
using namespace std;

class Attribute
{
protected:
	int HP;
	int maxHP;
	int Dmg;
	int MaxHP;
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
	virtual int getMaxHP()
	{
		return MaxHP;
	}
	virtual float getMoveSpeed()
	{
		return MovingSpeed;
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
	virtual void setMaxHP(int maxhp)
	{
		MaxHP = maxhp;
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
		MaxHP += maxhp;
	}
	virtual void minusHP(int amtOfHP)
	{
		HP -= amtOfHP;
	}

};