#pragma once
// Include shader
#include "RenderControl\shader.h"

// Include GLM
#include <includes/glm.hpp>
#include <includes/gtc/matrix_transform.hpp>
#include <includes/gtc/type_ptr.hpp>

// Include CEntity2D
#include "../LivingEntity.h"

// Include Keyboard controller
#include "Inputs\KeyboardController.h"

// Include Settings
#include "GameControl\Settings.h"

class Beam : public CEntity2D
{
public:
    // Constructor
    Beam(void);

    // Destructor
    ~Beam(void);

    // Init
    bool Init(void);
    // Update
    virtual void Update(const double dElapsedTime);

    float timer;

    std::vector<CLivingEntity*> hitEntities;
};

