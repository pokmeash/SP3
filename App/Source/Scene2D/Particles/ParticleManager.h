#pragma once
#include "DesignPatterns/SingletonTemplate.h"
#include "Particle.h"
#include <vector>

class ParticleManager : public CSingletonTemplate<ParticleManager> {
	friend class CSingletonTemplate<ParticleManager>;
protected:
	std::vector<Particle*> particles;
	ParticleManager();
	virtual ~ParticleManager();
public:
	Particle* SpawnParticle(Particle::PARTICLE_TYPE type, glm::vec2& position, float size = 1.f, float duration = 1.f, bool repeat = false);
	bool Update(const double dt);
	void Render();
};