#pragma once

#include "EntityFactory.h"
#include "Map2D.h"
#include "DesignPatterns/SingletonTemplate.h"
#include "GameControl/Settings.h"
#include <vector>

class EntityManager : public CSingletonTemplate<EntityManager>
{
	friend CSingletonTemplate<EntityManager>;
public:

    // Handler to the CMap2D instance
    CMap2D* cMap2D;

    // Constructor
    EntityManager(void);

    // Destructor
    ~EntityManager(void);

    // Init
    bool Init(void);

    // Update
    void Update(const double dElapsedTime);

    // Render
    void Render(void);

    std::vector<CEntity2D*> entitylist;
};

