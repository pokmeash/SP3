#pragma once

// Include CEntity2D
#include "Enemy2D.h"

class CSpaceSkeleton : public CEnemy2D
{
public:
	// Constructor
	CSpaceSkeleton(void);

	// Destructor
	virtual ~CSpaceSkeleton(void);

	// Init
	virtual bool Init(void);

	// Update
	virtual void Update(const double dElapsedTime);
};