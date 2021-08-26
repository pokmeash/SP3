#include "ParticleManager.h"
#include "EventControl/EventHandler.h"
#include "EventControl/Entity2DDespawnEvent.h"
#include "EventControl/NextRoomEvent.h"
#include "../Enemies/Enemy2D.h"

ParticleManager::ParticleManager()
{
	EventHandler::GetInstance()->On([&](Event* e) {
		if (e->getName() == NextRoomEvent::BASE_NAME()) {
			for (unsigned i = 0; i < particles.size(); ++i) {
				Particle* p = particles[i];
				if (p) delete p;
			}
			particles.clear();
			return;
		}
		if (e->getName() == Entity2DDespawnEvent::BASE_NAME()) {
			Entity2DDespawnEvent* ev = (Entity2DDespawnEvent*)e;
			if (dynamic_cast<CEnemy2D*>(ev->getEntity())) {
				CEnemy2D* enemy = (CEnemy2D*)ev->getEntity();
				SpawnParticle(Particle::PARTICLE_TYPE::FIRE, enemy->vec2WSCoordinate, 2.f);
			}
		}
	});
}

ParticleManager::~ParticleManager()
{
	for (unsigned i = 0; i < particles.size(); ++i) {
		Particle* p = particles[i];
		if (p) delete p;
	}
	particles.clear();
}

Particle* ParticleManager::SpawnParticle(Particle::PARTICLE_TYPE type, glm::vec2& position, float size, float duration, bool repeat)
{
	Particle* particle = nullptr;
	for (unsigned i = 0; i < particles.size(); ++i) {
		Particle* p = particles[i];
		if (p->bIsActive) continue;
		if (p->getType() != type) continue;
		particle = p;
		break;
	}
	if (!particle) {
		particle = new Particle();
		particles.push_back(particle);
	}
	switch (type) {
	case Particle::PARTICLE_TYPE::EXPLOSION: {
		particle->LoadSprite("Image/Explosion.png", 5, 5);
		break;
	}
	case Particle::PARTICLE_TYPE::FIRE: {
		particle->LoadSprite("Image/Fire.png", 4, 8);
	}
	default: break;
	}
	particle->Init(type, size, duration, repeat);
	particle->vec2WSCoordinate = position;
	particle->bIsActive = true;
	return particle;
}

bool ParticleManager::Update(const double dt)
{
	for (unsigned i = 0; i < particles.size(); ++i) {
		Particle* particle = particles[i];
		if (!particle->bIsActive) continue;
		particle->Update(dt);
	}
	return true;
}

void ParticleManager::Render()
{
	for (unsigned i = 0; i < particles.size(); ++i) {
		Particle* particle = particles[i];
		if (!particle->bIsActive) continue;
		particle->PreRender();
		particle->Render();
		particle->PostRender();
	}
}
