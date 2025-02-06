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

    float actualzoom = 1.0f;
    float frequency = 1.7f;
    float damping =1.78f;
    float overshoot = 1.0f;

    sf::Vector2f target;
    
public:
    CameraManager(sf::Vector2f center, sf::Vector2f size);
    void update(double dt);

    void setPlayer(Player* player);
    void setActive(sf::RenderWindow& win) const;

    void addShake(float amplitude, float frequency, float duration);

    void im();
};
