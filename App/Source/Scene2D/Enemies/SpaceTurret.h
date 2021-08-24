#pragma once

// Include CEntity2D
#include "Enemy2D.h"

class CSpaceTurret : public CEnemy2D
{
public:
	// Constructor
	CSpaceTurret(void);

	// Destructor
	virtual ~CSpaceTurret(void);

	// Init
	virtual bool Init(void);

	// Update
	virtual void Update(const double dElapsedTime);
};