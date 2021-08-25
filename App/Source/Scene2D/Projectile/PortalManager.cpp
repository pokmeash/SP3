#include "PortalManager.h"
#include "EventControl/EventHandler.h"
#include "../FloorManager.h"

PortalManager::PortalManager() : bluePortal(NULL), orangePortal(NULL), initalized(false)
{
}

PortalManager::~PortalManager()
{
	if (bluePortal) {
		if (bluePortal->getDestination()) {
			bluePortal->getDestination()->setDestination(NULL);
		}
		bluePortal->setDestination(NULL);
		delete bluePortal;
	}
	bluePortal = NULL;
	if (orangePortal) {
		if (orangePortal->getDestination()) {
			orangePortal->getDestination()->setDestination(NULL);
		}
		orangePortal->setDestination(NULL);
		delete orangePortal;
	}
	orangePortal = NULL;
	EventHandler::GetInstance()->Remove(listener);
}

Portal* PortalManager::getPortal(glm::vec2 pos)
{
	if (bluePortal && glm::length(bluePortal->vec2WSCoordinate - pos) <= .5f) return bluePortal;
	if (orangePortal && glm::length(orangePortal->vec2WSCoordinate - pos) <= .5f) return orangePortal;
	return NULL;
}

void PortalManager::placePortal(glm::vec2 pos)
{
	if (bluePortal && !orangePortal) {
		orangePortal = new Portal(pos, Portal::TYPE::ORANGE);
		orangePortal->Init();
		bluePortal->setDestination(orangePortal);
		orangePortal->setDestination(bluePortal);
	}
	else if (orangePortal && !bluePortal) {
		bluePortal = new Portal(pos, Portal::TYPE::BLUE);
		bluePortal->Init();
		bluePortal->setDestination(orangePortal);
		orangePortal->setDestination(bluePortal);
	}
	else if (bluePortal && orangePortal) {
		if (bluePortal->getDestination()) {
			bluePortal->getDestination()->setDestination(NULL);
		}
		bluePortal->setDestination(NULL);
		delete bluePortal;
		bluePortal = NULL;
		if (orangePortal->getDestination()) {
			orangePortal->getDestination()->setDestination(NULL);
		}
		orangePortal->setDestination(NULL);
		delete orangePortal;
		orangePortal = NULL;
	}
	else if (!bluePortal && !orangePortal) {
		bluePortal = new Portal(pos, Portal::TYPE::BLUE);
		bluePortal->Init();
	}
}

bool PortalManager::Init(void)
{
	if (initalized) return true;
	listener = EventHandler::GetInstance()->On([&](Event* e) {
		if (bluePortal && orangePortal) {
			if (e->getName() == NextRoomEvent::BASE_NAME()) {
				if (bluePortal->getDestination()) {
					bluePortal->getDestination()->setDestination(NULL);
				}
				bluePortal->setDestination(NULL);
				delete bluePortal;
				bluePortal = NULL;
				if (orangePortal->getDestination()) {
					orangePortal->getDestination()->setDestination(NULL);
				}
				orangePortal->setDestination(NULL);
				delete orangePortal;
				orangePortal = NULL;
				return;
			}
			if (e->getName() == Player2DMoveEvent::BASE_NAME()) {
				Player2DMoveEvent* ev = (Player2DMoveEvent*)e;
				if (cPhysics.CalculateDistance(bluePortal->vec2WSCoordinate, ev->getTo()) <= .5f) {
					glm::vec2 dist = ev->getTo() - bluePortal->vec2WSCoordinate;
					CPlayer2D::GetInstance()->vec2WSCoordinate = dist + orangePortal->vec2WSCoordinate + CPlayer2D::GetInstance()->vec2Velocity;
					while (cPhysics.CalculateDistance(orangePortal->vec2WSCoordinate, CPlayer2D::GetInstance()->vec2WSCoordinate) <= .5f) {
						CPlayer2D::GetInstance()->vec2WSCoordinate += CPlayer2D::GetInstance()->vec2Velocity;
					}
				} else if (cPhysics.CalculateDistance(orangePortal->vec2WSCoordinate, ev->getTo()) <= .5f) {
					glm::vec2 dist = ev->getTo() - orangePortal->vec2WSCoordinate;
					CPlayer2D::GetInstance()->vec2WSCoordinate = dist + bluePortal->vec2WSCoordinate + CPlayer2D::GetInstance()->vec2Velocity;
					while (cPhysics.CalculateDistance(bluePortal->vec2WSCoordinate, CPlayer2D::GetInstance()->vec2WSCoordinate) <= .5f) {
						CPlayer2D::GetInstance()->vec2WSCoordinate += CPlayer2D::GetInstance()->vec2Velocity;
					}
				}
				return;
			}
			if (e->getName() == Entity2DMoveEvent::BASE_NAME()) {
				Entity2DMoveEvent* ev = (Entity2DMoveEvent*)e;
				if (cPhysics.CalculateDistance(bluePortal->vec2WSCoordinate, ev->getTo()) <= .5f) {
					glm::vec2 dist = ev->getTo() - bluePortal->vec2WSCoordinate;
					ev->getEntity()->vec2WSCoordinate = dist + orangePortal->vec2WSCoordinate + ev->getEntity()->vec2Velocity;
					while (cPhysics.CalculateDistance(orangePortal->vec2WSCoordinate, ev->getEntity()->vec2WSCoordinate) <= .5f) {
						ev->getEntity()->vec2WSCoordinate += ev->getEntity()->vec2Velocity;
					}
				} else if (cPhysics.CalculateDistance(orangePortal->vec2WSCoordinate, ev->getTo()) <= .5f) {
					glm::vec2 dist = ev->getTo() - orangePortal->vec2WSCoordinate;
					ev->getEntity()->vec2WSCoordinate = dist + bluePortal->vec2WSCoordinate + ev->getEntity()->vec2Velocity;
					while (cPhysics.CalculateDistance(bluePortal->vec2WSCoordinate, ev->getEntity()->vec2WSCoordinate) <= .5f) {
						ev->getEntity()->vec2WSCoordinate += ev->getEntity()->vec2Velocity;
					}
				}
			}
		}
	});
	initalized = true;
	return true;
}

bool PortalManager::Update(const double dt)
{
	static bool pressed = false;
	bool portal = CSettings::GetInstance()->iKeybinds[CSettings::TRIGGER_PORTAL] <= GLFW_MOUSE_BUTTON_LAST && CMouseController::GetInstance()->IsButtonDown(CSettings::GetInstance()->iKeybinds[CSettings::TRIGGER_PORTAL]);
	if ((portal || CKeyboardController::GetInstance()->IsKeyPressed(CSettings::GetInstance()->iKeybinds[CSettings::TRIGGER_PORTAL])) && !pressed) {
		glm::i32vec2 mousepos(0, 0);
		int x = CMouseController::GetInstance()->GetMousePositionX();
		int y = CMouseController::GetInstance()->GetMousePositionY();
		CSettings::GetInstance()->ConvertMouseToIndexSpace(x, y, &mousepos.x, &mousepos.y);
		if (mousepos.x >= 0 && mousepos.x < CSettings::GetInstance()->NUM_TILES_XAXIS &&
			mousepos.y >= 0 && mousepos.y < CSettings::GetInstance()->NUM_TILES_YAXIS) {
			if (CFloorManager::GetInstance()->GetMapInfo(mousepos.y, mousepos.x) < 100) {
				glm::vec2 m(0.f, 0.f);
				CSettings::GetInstance()->ConvertMouseToWSSpace(x, y, &m.x, &m.y);
				placePortal(m);
			}
		}
		pressed = true;
	}
	if ((!portal || CKeyboardController::GetInstance()->IsKeyReleased(CSettings::GetInstance()->iKeybinds[CSettings::TRIGGER_PORTAL])) && pressed) {
		pressed = false;
	}
	if (bluePortal) {
		bluePortal->Update(dt);
	}
	if (orangePortal) {
		orangePortal->Update(dt);
	}
	return true;
}

void PortalManager::Render()
{
	if (bluePortal) {
		bluePortal->PreRender();
		bluePortal->Render();
		bluePortal->PostRender();
	}
	if (orangePortal) {
		orangePortal->PreRender();
		orangePortal->Render();
		orangePortal->PostRender();
	}
}

void PortalManager::Reset()
{
	if (bluePortal) {
		if (bluePortal->getDestination()) {
			bluePortal->getDestination()->setDestination(NULL);
		}
		bluePortal->setDestination(NULL);
		delete bluePortal;
	}
	bluePortal = NULL;
	if (orangePortal) {
		if (orangePortal->getDestination()) {
			orangePortal->getDestination()->setDestination(NULL);
		}
		orangePortal->setDestination(NULL);
		delete orangePortal;
		orangePortal = NULL;
	}
}
