#pragma once

// Include CEntity2D
#include "Enemy2D.h"

class CSpaceFly : public CEnemy2D
{
public:
	// Constructor
	CSpaceFly(void);

	// Destructor
	virtual ~CSpaceFly(void);

	// Init
	virtual bool Init(void);

	// Update
	virtual void Update(const double dElapsedTime);
protected:

	virtual bool InteractWithPlayer(void);
};