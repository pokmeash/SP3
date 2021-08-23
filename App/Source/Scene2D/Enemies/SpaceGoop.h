#pragma once

// Include CEntity2D
#include "Enemy2D.h"

class CSpaceGoop : public CEnemy2D
{
public:
	// Constructor
	CSpaceGoop(void);

	// Destructor
	virtual ~CSpaceGoop(void);

	// Init
	virtual bool Init(void);

	// Update
	virtual void Update(const double dElapsedTime);
};