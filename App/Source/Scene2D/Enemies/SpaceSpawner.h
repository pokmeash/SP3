#pragma once

// Include CEntity2D
#include "Enemy2D.h"

class CSpaceSpawner : public CEnemy2D
{
public:
	// Constructor
	CSpaceSpawner(void);

	// Destructor
	virtual ~CSpaceSpawner(void);

	// Init
	virtual bool Init(void);

	// Update
	virtual void Update(const double dElapsedTime);

protected:
	float spawnTimer = 0;
};