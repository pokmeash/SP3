#pragma once

// Include CEntity2D
#include "Enemy2D.h"

class CSpaceCannon : public CEnemy2D
{
public:
	// Constructor
	CSpaceCannon(void);

	// Destructor
	virtual ~CSpaceCannon(void);

	// Init
	virtual bool Init(void);

	// Update
	virtual void Update(const double dElapsedTime);
};
