#pragma once
#include "../BackgroundEntity.h"
#include "Primitives/Meshbuilder.h"

class Particle : public CBackgroundEntity {
public:
	enum PARTICLE_TYPE {
		DEATH,
		SHOOT,
		EXPLOSION,
		BEAM,
		FIRE,
		TOTAL_PARTICLE_TYPE,
	};

	Particle();
	virtual ~Particle();
	virtual bool Init(PARTICLE_TYPE type, float size, float interval, bool repeat);
	virtual void Update(const double dt);
	//virtual void Render(void);
	virtual void LoadSprite(std::string filename, unsigned rows, unsigned cols);
	virtual Particle::PARTICLE_TYPE getType();
	virtual float getDuration();
	virtual void unspawn();
protected:
	PARTICLE_TYPE type;
	float duration;
	float totalDuration;
	unsigned numOfStates;
	int currentFrame;
};