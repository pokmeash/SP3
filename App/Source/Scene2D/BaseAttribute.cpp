#include "BaseAttribute.h"

void BaseAttribute::setHP(int hp)
{
	HP = hp;
}

int BaseAttribute::getHP()
{
	return HP;
}

void BaseAttribute::setDmg(int dmg)
{
	Dmg = dmg;
}

int BaseAttribute::getDmg()
{
	return Dmg;
}

void BaseAttribute::addProjSpeed(float speed)
{
	projectileSpeed += speed;
}

float BaseAttribute::getProjSpeed()
{
	return projectileSpeed;
}
