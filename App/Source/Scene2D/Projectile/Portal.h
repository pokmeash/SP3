#pragma once
#include "Primitives/Entity2D.h"

class Portal : public CEntity2D {
public:
	enum TYPE {
		BLUE,
		ORANGE,
	};
	Portal(glm::vec2& pos, TYPE type);
	virtual ~Portal();
	virtual bool Init(void);
	virtual void Update(const double dt);
protected:
	Portal::TYPE type;
};