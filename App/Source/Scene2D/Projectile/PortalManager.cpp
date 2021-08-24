#include "PortalManager.h"
#include "EventControl/EventHandler.h"
#include "../FloorManager.h"

PortalManager::PortalManager() : bluePortal(NULL), orangePortal(NULL), initalized(false)
{
}

PortalManager::~PortalManager()
{
	if (bluePortal) {
		delete bluePortal;
	}
	bluePortal = NULL;
	if (orangePortal) {
		delete orangePortal;
	}
	orangePortal = NULL;
}

void PortalManager::placePortal(glm::vec2 pos)
{
	if (bluePortal && !orangePortal) {
		orangePortal = new Portal(pos, Portal::TYPE::ORANGE);
		orangePortal->Init();
	}
	else if (orangePortal && !bluePortal) {
		bluePortal = new Portal(pos, Portal::TYPE::BLUE);
		bluePortal->Init();
	}
	else if (bluePortal && orangePortal) {
		delete bluePortal;
		bluePortal = NULL;
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
	EventHandler::GetInstance()->On([&](Event* e) {
		if (e->getName() == NextRoomEvent::BASE_NAME()) {
			delete bluePortal;
			bluePortal = NULL;
			delete orangePortal;
			orangePortal = NULL;
			return;
		}
		if (bluePortal && orangePortal) {
			if (e->getName() == Player2DMoveEvent::BASE_NAME()) {
				Player2DMoveEvent* ev = (Player2DMoveEvent*)e;
				if (cPhysics.CalculateDistance(bluePortal->vec2WSCoordinate, ev->getTo()) <= 1.f) {
					CPlayer2D::GetInstance()->vec2WSCoordinate = orangePortal->vec2WSCoordinate + CPlayer2D::GetInstance()->vec2Velocity;
					while (cPhysics.CalculateDistance(orangePortal->vec2WSCoordinate, CPlayer2D::GetInstance()->vec2WSCoordinate) <= 1.f) {
						CPlayer2D::GetInstance()->vec2WSCoordinate += CPlayer2D::GetInstance()->vec2Velocity;
					}
				} else if (cPhysics.CalculateDistance(orangePortal->vec2WSCoordinate, ev->getTo()) <= 1.f) {
					CPlayer2D::GetInstance()->vec2WSCoordinate = bluePortal->vec2WSCoordinate + CPlayer2D::GetInstance()->vec2Velocity;
					while (cPhysics.CalculateDistance(bluePortal->vec2WSCoordinate, CPlayer2D::GetInstance()->vec2WSCoordinate) <= 1.f) {
						CPlayer2D::GetInstance()->vec2WSCoordinate += CPlayer2D::GetInstance()->vec2Velocity;
					}
				}
				return;
			}
			if (e->getName() == Entity2DMoveEvent::BASE_NAME()) {
				Entity2DMoveEvent* ev = (Entity2DMoveEvent*)e;
				if (cPhysics.CalculateDistance(bluePortal->vec2WSCoordinate, ev->getTo()) <= 1.f) {
					ev->getEntity()->vec2WSCoordinate = orangePortal->vec2WSCoordinate + ev->getEntity()->vec2Velocity;
					while (cPhysics.CalculateDistance(orangePortal->vec2WSCoordinate, ev->getEntity()->vec2WSCoordinate) <= 1.f) {
						ev->getEntity()->vec2WSCoordinate += ev->getEntity()->vec2Velocity;
					}
				} else if (cPhysics.CalculateDistance(orangePortal->vec2WSCoordinate, ev->getTo()) <= 1.f) {
					ev->getEntity()->vec2WSCoordinate = bluePortal->vec2WSCoordinate + ev->getEntity()->vec2Velocity;
					while (cPhysics.CalculateDistance(bluePortal->vec2WSCoordinate, ev->getEntity()->vec2WSCoordinate) <= 1.f) {
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