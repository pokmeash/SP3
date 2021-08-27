#include "Particle.h"
#include "ParticleManager.h"
#include "EventControl/EventHandler.h"
#include "EventControl/Entity2DDespawnEvent.h"

Particle::Particle() : CBackgroundEntity("")
{
}

Particle::~Particle()
{
	if (animatedSprites) delete animatedSprites;
}

bool Particle::Init(PARTICLE_TYPE type, float size, float interval, bool repeat)
{
	cSettings = CSettings::GetInstance();
	this->type = type;
	this->duration = interval;
	scale = glm::vec3(size, size, size);
	if (textureName == "") {
		std::cout << "No texture file is attached to this particle!" << std::endl;
		return false;
	}

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	if (!LoadTexture(textureName.c_str())) {
		std::cout << "Failed to load particle texture!" << std::endl;
		return false;
	}
	if (!animatedSprites) return false;
	animatedSprites->PlayAnimation("anim", repeat ? -1 : 1, interval);
	bIsActive = true;
	return true;
}

void Particle::Update(const double dt)
{
	if (!bIsActive) return;
	duration -= dt;
	if (duration <= 0.f) {
		bIsActive = false;
		EventHandler::GetInstance()->CallThenDelete(new Entity2DDespawnEvent(this));
		return;
	}
	if (animatedSprites) animatedSprites->Update(dt);
}

void Particle::LoadSprite(std::string filename, unsigned rows, unsigned cols)
{
	textureName = filename;
	if (!LoadTexture(textureName.c_str())) {
		std::cout << "Failed to load particle texture!" << std::endl;
		return;
	}
	if (animatedSprites) {
		delete animatedSprites;
		animatedSprites = NULL;
	}
	numOfStates = rows * cols;
	animatedSprites = CMeshBuilder::GenerateSpriteAnimation(rows, cols, CSettings::GetInstance()->TILE_WIDTH, CSettings::GetInstance()->TILE_HEIGHT);
	animatedSprites->AddAnimation("anim", 0, numOfStates);
}

Particle::PARTICLE_TYPE Particle::getType()
{
	return type;
}

float Particle::getDuration()
{
	return duration;
}

void Particle::unspawn()
{
	bIsActive = false;
}
