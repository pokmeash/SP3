#pragma once

#include "EntityFactory.h"
#include "FloorManager.h"
#include "DesignPatterns/SingletonTemplate.h"
#include "GameControl/Settings.h"

#include <vector>
class Listener;

class EntityManager : public CSingletonTemplate<EntityManager>
{
	friend CSingletonTemplate<EntityManager>;
public:

    // Handler to the CMap2D instance
    CFloor2D* cMap2D;
    CPhysics2D cPhysics;

    // Init
    bool Init(void);

    // Update
    void Update(const double dElapsedTime);

    // Render
    void Render(void);
    
    std::vector<CEntity2D*> entitylist;
protected:
    Listener* listener; // Constructor

    EntityManager(void);

    // Destructor
    virtual ~EntityManager(void);
};

