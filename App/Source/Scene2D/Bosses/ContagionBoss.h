#pragma once

// Include CEntity2D
#include "../Enemies/Enemy2D.h"

class CContagionBoss : public CEnemy2D
{
public:
	// Constructor
	CContagionBoss(void);

	// Destructor
	virtual ~CContagionBoss(void);

	// Init
	virtual bool Init(void);

	// Update
	virtual void Update(const double dElapsedTime);

protected:

	float shieldTimer = 0;
	float offset = 0;
	bool phaseOne = false;
	bool phaseTwo = false;

	void spawnMinion();
};
