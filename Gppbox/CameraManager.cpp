#include "CameraManager.h"

#include "C.hpp"
#include "imgui.h"

CameraManager::CameraManager(sf::Vector2f center, sf::Vector2f size): view(center, size), secondOrderDynamics(0, 0, 0, sf::Vector2f{0, 0}), target(center)
{
   secondOrderDynamics.setParams(frequency, damping, overshoot);
}

void CameraManager::update(double dt)
{
   if (player == nullptr) { printf("Camera has no player\n"); return; }

   float posXDiff = 100;
   if (player->moveRight) posXDiff *= -1;
   target = {(float)player->getPosPixel().x - posXDiff,(float) player->getPosPixel().y};
   sf::Vector2f cameraPosition = secondOrderDynamics.Update(dt, target);
   view.setCenter(cameraPosition);
   view.zoom(zoom / actualzoom);
   actualzoom = zoom;
}

void CameraManager::setPlayer(Player* _player)
{
   player = _player;
   target = sf::Vector2f((float)player->cx * C::GRID_SIZE, (float)player->cy + C::GRID_SIZE);
}

void CameraManager::setActive(sf::RenderWindow& win) const
{
   win.setView(view);
}

void CameraManager::addShake(float _amplitude, float _frequency, float _duration)
{
   secondOrderDynamics.startShake(_amplitude, _frequency, _duration);
}

void CameraManager::im()
{
   using namespace ImGui;
   
   bool chg = false;
   chg |= DragFloat("zoom", &zoom, 0.01f, 0.01f,20);
   chg |= DragFloat("f", &frequency, 0.01f, 0.01f,20);
   chg |= DragFloat("z", &damping, 0.01f, -20,20); //1.33
   chg |= DragFloat("r", &overshoot, 0.01f, -20,20);
   if (chg) secondOrderDynamics.setParams(frequency, damping, overshoot);
}
