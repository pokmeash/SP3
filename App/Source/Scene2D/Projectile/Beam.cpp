#include "Beam.h"

#include "RenderControl/ShaderManager.h"
#include "System/ImageLoader.h"
#include "Primitives/MeshBuilder.h"
#include "EventControl/EventHandler.h"

Beam::Beam()
{
    rotation = 0.f;
    scale = glm::vec3(1, 1, 1);
}
Beam::~Beam()
{}

bool Beam::Init() {
    cSettings = CSettings::GetInstance();
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    mesh = CMeshBuilder::GenerateQuad(glm::vec4(1, 1, 1, 1), cSettings->TILE_WIDTH, cSettings->TILE_HEIGHT);
    return LoadTexture("Image/Beam.tga");
}

void Beam::Update(const double dElapsedTime)
{
    vec2WSOldCoordinates = vec2WSCoordinate;
    vec2WSCoordinate += vec2Velocity;
    if (vec2WSOldCoordinates != vec2WSCoordinate) {
        if (EventHandler::GetInstance()->CallDeleteIsCancelled(new Entity2DMoveEvent(this, vec2WSCoordinate, vec2WSOldCoordinates))) {
            vec2WSCoordinate = vec2WSOldCoordinates;
        }
    }
    rotation = atan2f(vec2Velocity.y, vec2Velocity.x);
    CSettings::GetInstance()->ConvertFloatToIndexSpace(CSettings::GetInstance()->x, vec2WSCoordinate.x, &i32vec2Index.x, &i32vec2NumMicroSteps.x);
    CSettings::GetInstance()->ConvertFloatToIndexSpace(CSettings::GetInstance()->y, vec2WSCoordinate.y, &i32vec2Index.y, &i32vec2NumMicroSteps.y);
}
