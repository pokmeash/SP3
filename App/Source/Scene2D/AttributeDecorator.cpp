#include "AttributeDecorator.h"

void AttributeDecorator::setHP(int hp)
{
	attribute->setHP(hp);
}

int AttributeDecorator::getHP()
{
	return attribute->getHP();
}

void AttributeDecorator::setDmg(int dmg)
{
	attribute->setDmg(dmg);
}

int AttributeDecorator::getDmg()
{
	return attribute->getDmg();
}

void AttributeDecorator::addProjSpeed(float speed)
{
	attribute->addProjSpeed(speed);
}

float AttributeDecorator::getProjSpeed()
{
	return attribute->getProjSpeed();
}


