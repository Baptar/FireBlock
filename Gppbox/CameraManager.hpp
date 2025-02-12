#pragma once
#include <SFML/Graphics.hpp>

#include "Player.hpp"
#include "SecondOrderDynamics.hpp"

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
    float frequency = 0.5f;
    float damping = 0.78f;
    float overshoot = 1.06f;
    
    CameraManager(sf::Vector2f _center, sf::Vector2f _size);
    void update(double _dt);

    void setPlayer(Player* _player);
    void setActive(sf::RenderWindow& _win) const;

    void addShake(float _amplitude, float _frequency, float _duration);

    void im();
};
