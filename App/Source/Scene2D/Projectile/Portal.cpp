#include "Portal.h"
#include "Primitives/MeshBuilder.h"

Portal::Portal(glm::vec2& pos, TYPE type) : type(type), destination(NULL)
{
	vec2WSCoordinate = pos;
    rotation = 0.f;
    scale = glm::vec3(1, 1, 1);
}

Portal::~Portal()
{
    cSettings = NULL;
}

bool Portal::Init(void)
{
    cSettings = CSettings::GetInstance();
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    animatedSprites = CMeshBuilder::GenerateSpriteAnimation(1, 4, cSettings->TILE_WIDTH, cSettings->TILE_HEIGHT);
    animatedSprites->AddAnimation("portal", 0, 4);
    animatedSprites->PlayAnimation("portal", -1, .5f);
    bIsActive = true;
    return LoadTexture(type == TYPE::BLUE ? "Image/BluePortal.png" : "Image/OrangePortal.png");
}

void Portal::Update(const double dt)
{
    animatedSprites->Update(dt);
}

Portal* Portal::getDestination()
{
    return destination;
}

void Portal::setDestination(Portal* portal)
{
    destination = portal;
}
