#pragma once

// Include CEntity2D
#include "../Enemies/Enemy2D.h"

class CBoss2D : public CEnemy2D
{
public:
	// Constructor
	CBoss2D(void);

	// Destructor
	virtual ~CBoss2D(void);

	// Init
	virtual bool Init(void);

	// Update
	virtual void Update(const double dElapsedTime);
protected:
	int temphp;
	float offset;
	int dirx;
	int diry;
	int wallcounter;
	int random;
	glm::vec2 rushdirection;
};