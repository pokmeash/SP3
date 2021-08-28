#include "EntityFactory.h"
#include "EventControl/EventHandler.h"
#include "EventControl/Entity2DSpawnEvent.h"
#include "EventControl/Entity2DDespawnEvent.h"
#include "EntityManager.h"
#include "Projectile/PortalManager.h"
#include "Scene2D.h"
#include "Particles/ParticleManager.h"
#include "FloorManager.h"

EntityFactory::EntityFactory()
{}
EntityFactory::~EntityFactory()
{}

Bullet* EntityFactory::ProduceBullets(glm::f32vec2 EntityVec2Index, glm::f32vec2 EntityVec2Vel, glm::vec3 EntityVec3Scale, CEntity2D::ENTITY_TYPE type)
{
	Bullet* temp = nullptr;
	for (unsigned i = 0; i < EntityManager::GetInstance()->entitylist.size(); ++i) {
		CEntity2D* entity = EntityManager::GetInstance()->entitylist[i];
		if (entity->bIsActive) continue;
		if (entity->type != type) continue;
		temp = (Bullet*)entity;
		break;
	}
	if (!temp) {
		temp = new Bullet();
		temp->Init();
		EntityManager::GetInstance()->entitylist.push_back(temp);
	}
	
	temp->counter = CPlayer2D::GetInstance()->getRicochetTimes();
	temp->vec2WSCoordinate = EntityVec2Index;
	temp->vec2Velocity = EntityVec2Vel;
	temp->scale = EntityVec3Scale;
	temp->rotation = atan2f(EntityVec2Vel.y, EntityVec2Vel.x);
	temp->type = type;
	temp->bIsActive = true;
	EventHandler::GetInstance()->CallThenDelete(new Entity2DSpawnEvent(temp));
	return temp;
}

Grenade* EntityFactory::ProduceGrenade(glm::f32vec2 EntityVec2Index, glm::f32vec2 EntityVec2Vel, glm::vec3 EntityVec3Scale, CEntity2D::ENTITY_TYPE type)
{
	Grenade* temp = nullptr;
	for (unsigned i = 0; i < EntityManager::GetInstance()->entitylist.size(); ++i) {
		CEntity2D* entity = EntityManager::GetInstance()->entitylist[i];
		if (entity->bIsActive) continue;
		if (entity->type != type) continue;
		temp = (Grenade*)entity;
		break;
	}
	if (!temp) {
		temp = new Grenade();
		temp->Init();
		EntityManager::GetInstance()->entitylist.push_back(temp);
	}
	temp->timer = 2;
	temp->vec2WSCoordinate = EntityVec2Index;
	temp->vec2Velocity = EntityVec2Vel;
	temp->scale = EntityVec3Scale;
	temp->rotation = 0;
	temp->type = type;
	temp->bIsActive = true;
	EventHandler::GetInstance()->CallThenDelete(new Entity2DSpawnEvent(temp));
	return temp;
}

std::vector<Beam*> EntityFactory::ProduceBeam(glm::vec2 pos, glm::vec2 dir, CEntity2D::ENTITY_TYPE type, unsigned length)
{
	std::vector<Beam*> beams;
	for (unsigned i = 0; i < EntityManager::GetInstance()->entitylist.size() && beams.size() < length; ++i) {
		CEntity2D* entity = EntityManager::GetInstance()->entitylist[i];
		if (entity->bIsActive) continue;
		if (entity->type != type) continue;
		beams.push_back((Beam*)entity);
	}
	if (beams.size() < length) {
		while (beams.size() < length) {
			Beam* temp = new Beam();
			temp->Init();
			EntityManager::GetInstance()->entitylist.push_back(temp);
			beams.push_back(temp);
		}
	}
	CFloorManager* cMap2D = CFloorManager::GetInstance();
	glm::i32vec2 index, micro;
	int counter = CPlayer2D::GetInstance()->getRicochetTimes();
	for (unsigned i = 0; i < beams.size(); ++i) {
		Beam* beam = beams[i];
		if (PortalManager::GetInstance()->getPortal(pos) && PortalManager::GetInstance()->getPortal(pos)->getDestination()) {
			Portal* portal = PortalManager::GetInstance()->getPortal(pos)->getDestination();
			pos = pos - PortalManager::GetInstance()->getPortal(pos)->vec2WSCoordinate + portal->vec2WSCoordinate + dir * 0.8f;
		}
		pos += dir * 0.7f;
		CSettings::GetInstance()->ConvertFloatToIndexSpace(CSettings::x, pos.x, &index.x, &micro.x);
		CSettings::GetInstance()->ConvertFloatToIndexSpace(CSettings::y, pos.y, &index.y, &micro.y);
		if (index.x > 0 && index.x < CSettings::GetInstance()->NUM_TILES_XAXIS && index.y > 0 && index.y < CSettings::GetInstance()->NUM_TILES_YAXIS) {
			if (cMap2D->GetMapInfo(index.y + 1, index.x) >= 100 && dir.y > 0)
			{
				dir.y *= -1;
				counter--;
			} else if (cMap2D->GetMapInfo(index.y - 1, index.x) >= 100 && dir.y < 0 && micro.y <= CSettings::GetInstance()->NUM_STEPS_PER_TILE_YAXIS * 0.5f)
			{
				dir.y *= -1;
				counter--;
			}
			if (counter < 0) {
				CSoundController::GetInstance()->Replay(CSoundController::SOUNDS::LASER);
				ParticleManager::GetInstance()->SpawnParticle(Particle::PARTICLE_TYPE::EXPLOSION, pos += dir * 0.7f, 0.5f);
				break;
			}
			if (cMap2D->GetMapInfo(index.y, index.x + 1) >= 100 && dir.x > 0)
			{
				dir.x *= -1;
				counter--;
			} else if (cMap2D->GetMapInfo(index.y, index.x - 1) >= 100 && dir.x < 0 && micro.x <= CSettings::GetInstance()->NUM_STEPS_PER_TILE_XAXIS * 0.25f)
			{
				dir.x *= -1;
				counter--;
			}
			if (counter < 0) {
				CSoundController::GetInstance()->Replay(CSoundController::SOUNDS::LASER);
				ParticleManager::GetInstance()->SpawnParticle(Particle::PARTICLE_TYPE::EXPLOSION, pos += dir * 0.7f, 0.5f);
				break;
			}
		}
		beam->vec2WSCoordinate = pos;
		beam->vec2Velocity = dir;
		beam->type = type;
		beam->timer = 0.1f;
		beam->bIsActive = true;
		EventHandler::GetInstance()->CallThenDelete(new Entity2DSpawnEvent(beam));
		for (unsigned j = 0; j < CScene2D::GetInstance()->enemyVector.size(); ++j) {
			CLivingEntity* enemy = (CLivingEntity*)CScene2D::GetInstance()->enemyVector[j];
			if (!enemy->bIsActive) continue;
			if (glm::length(enemy->vec2WSCoordinate - pos) <= enemy->scale.x * 0.5f) {
				enemy->addHP(-CPlayer2D::GetInstance()->getDmg());
				beam->hitEntities.push_back(enemy);
				if (enemy->getHP() <= 0) {
					if (CFloorManager::GetInstance()->GetMapInfo(enemy->i32vec2Index.y, enemy->i32vec2Index.x) == 0)
					{
						srand(time(NULL));
						int random = rand() % 5 + 1;
						if (random == 1)
						{
							CFloorManager::GetInstance()->SetMapInfo(enemy->i32vec2Index.y, enemy->i32vec2Index.x, 30);
						}
					}
					enemy->bIsActive = false;
					EventHandler::GetInstance()->CallThenDelete(new Entity2DDespawnEvent(enemy));
				}
			}
		}
		if (i + 1 == beams.size()) {
			CSoundController::GetInstance()->Replay(CSoundController::SOUNDS::LASER);
			ParticleManager::GetInstance()->SpawnParticle(Particle::PARTICLE_TYPE::EXPLOSION, pos += dir * 0.7f, 0.5f);
		}
	}
	return beams;
}
