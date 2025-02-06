#include "CameraManager.h"

#include "C.hpp"

CameraManager::CameraManager(sf::Vector2f center, sf::Vector2f size): view(center, size), secondOrderDynamics(0.7f, 0.78f, 1.0f, sf::Vector2f{0, 0}), target(center)
{
}

void CameraManager::update(double dt)
{
   if (player == nullptr)
   {
      printf("Camera has no player\n");
      return;
   }
   sf::Vector2f playerCenter = sf::Vector2f((float)player->getPosPixel().x + C::GRID_SIZE / 2.f, (float)player->getPosPixel().y + C::GRID_SIZE / 2.f);
   sf::Vector2f viewSize = view.getSize();

   float posXDiff = 100;
   if (player->moveRight) posXDiff *= -1;
   target = {(float)player->getPosPixel().x - posXDiff,(float) player->getPosPixel().y};
   sf::Vector2f cameraPosition = secondOrderDynamics.Update(dt, target);
   view.setCenter(cameraPosition);
   view.zoom(0.6f / actualzoom);
   actualzoom = 0.6f;
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

void CameraManager::addShake(float amplitude, float frequency, float duration)
{
   secondOrderDynamics.startShake(amplitude, frequency, duration);
}

void CameraManager::im()
{
}
