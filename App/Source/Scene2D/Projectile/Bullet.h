#pragma once
// Include shader
#include "RenderControl\shader.h"

// Include GLM
#include <includes/glm.hpp>
#include <includes/gtc/matrix_transform.hpp>
#include <includes/gtc/type_ptr.hpp>

// Include CEntity2D
#include "Primitives/Entity2D.h"

// Include Keyboard controller
#include "Inputs\KeyboardController.h"

// Include Settings
#include "GameControl\Settings.h"

class Bullet : public CEntity2D
{
public:
    // Constructor
    Bullet(void);

    // Destructor
    ~Bullet(void);

    // Init
    bool Init(void);
    // Update
    virtual void Update(const double dElapsedTime);
private:
    int hitcount;
};

