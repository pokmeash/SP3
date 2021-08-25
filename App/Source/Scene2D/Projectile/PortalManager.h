#include "Portal.h"
#include "DesignPatterns/SingletonTemplate.h"
#include "../App/Source/Scene2D/Physics2D.h"
#include <vector>

class PortalManager : public CSingletonTemplate<PortalManager> {
	friend class CSingletonTemplate<PortalManager>;
protected:
	Portal* orangePortal;
	Portal* bluePortal;
	CPhysics2D cPhysics;
	bool initalized;
	PortalManager();
	virtual ~PortalManager();

public:
	void placePortal(glm::vec2 pos);
	Portal* getPortal(glm::vec2 pos);
	bool Init(void);
	bool Update(const double dt);
	void Render();
	void Reset();
};