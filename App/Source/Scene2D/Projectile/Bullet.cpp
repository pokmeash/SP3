#include "Bullet.h"

#include "RenderControl/ShaderManager.h"
#include "System/ImageLoader.h"
#include "Primitives/MeshBuilder.h"
#include "EventControl/EventHandler.h"

Bullet::Bullet()
{}
Bullet::~Bullet()
{}

bool Bullet::Init() {
    cSettings = CSettings::GetInstance();
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    mesh = CMeshBuilder::GenerateQuad(glm::vec4(1, 1, 1, 1), cSettings->TILE_WIDTH, cSettings->TILE_HEIGHT);
    return LoadTexture("Image/Bullet.tga");
}
void Bullet::Update(const double dElapsedTime)
{
    vec2WSOldCoordinates = vec2WSCoordinate;
    vec2WSCoordinate += vec2Velocity;
    EventHandler::GetInstance()->CallThenDelete(new Entity2DMoveEvent(this, vec2WSCoordinate, vec2WSOldCoordinates));
}
