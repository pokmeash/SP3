#include "Bullet.h"

#include"RenderControl/ShaderManager.h"
#include"System/ImageLoader.h"
#include"Primitives/MeshBuilder.h"

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
    vec2WSCoordinate += vec2Velocity;
    CSettings::GetInstance()->ConvertFloatToIndexSpace(CSettings::GetInstance()->x, vec2WSCoordinate.x, &i32vec2Index.x, &i32vec2NumMicroSteps.x);
    CSettings::GetInstance()->ConvertFloatToIndexSpace(CSettings::GetInstance()->y, vec2WSCoordinate.y, &i32vec2Index.y, &i32vec2NumMicroSteps.y);
   // std::cout << "Bullet::Update\n";
   
}
