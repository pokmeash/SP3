#pragma once
#include <iostream>
#include "Attribute.h"
using namespace std;

class AttributeDecorator : public Attribute
{
protected:
	Attribute* attribute;
public:
	AttributeDecorator(Attribute* baseAttribute) : attribute(baseAttribute) {}
	void setHP(int hp);
	int getHP();
	void setDmg(int dmg);
	int getDmg();
	void addProjSpeed(float speed);
	float getProjSpeed();
};
