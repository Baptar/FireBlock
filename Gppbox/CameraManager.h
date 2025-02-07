#pragma once
#include <SFML/Graphics.hpp>

#include "Player.hpp"
#include "SecondOrderDynamics.h"

class CameraManager
{
private:
    sf::View view;
    Player* player  = nullptr;
    SecondOrderDynamics secondOrderDynamics;
    
    sf::Vector2f target;
    
public:
    float zoom = 0.6f;
    float actualzoom = 1.0f;
    float frequency = 0.7f;
    float damping = 0.78f;
    float overshoot = 1.06f;
    
    CameraManager(sf::Vector2f center, sf::Vector2f size);
    void update(double dt);

    void setPlayer(Player* player);
    void setActive(sf::RenderWindow& win) const;

    void addShake(float amplitude, float frequency, float duration);

    void im();
};
