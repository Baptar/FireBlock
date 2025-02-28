#pragma once

#include <SFML/Graphics.hpp>

#include "Ennemy.hpp"
#include "SpriteExplosion.h"

class Game;

class HomingMissile {
public:
    SpriteExplosion spriteExplosion;
    sf::Texture texture;
    sf::Sprite sprite;
    sf::RectangleShape rectangle;
    float lastAngle = 0;
    
    HomingMissile(sf::Vector2f startPos, bool moveRight);
    void update(double dt);
    void draw();
    bool shouldDestroy;
    bool shouldExplose;
    void im();
    
    Game& game;
    void moveTowardsTarget(double dt);
    void findTarget();
    void missileTouched();
    bool moveRight;
    bool foundTarget;

    //sf::CircleShape sprite;
    sf::Vector2f position;
    int cx, cy;
    float rx, ry;
    float dx, dy;
    float speed;
    Ennemy* target;
};