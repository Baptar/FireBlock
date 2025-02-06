#pragma once
#include <SFML/System/Vector2.hpp>

#include "Game.hpp"

class Bullet
{

private:
    int			cx = 0;
    int			cy = 0;
    float		rx = 0.5f;
    float		ry = 0.0f;
    float		dx = 0.f;
    float		dy = 0.f;
    float		speedX = 2.f;
    float		speedY = 0.f;
    float       bloom = 0.5f;
    
    float		gravy = 0.f;
    float		frx = 0.88f;
    float		fry = 0.95f;
    
    bool        moveRight = true;
    float       timeLife = 4.0f;
    int         damage = 1;

    Game&       game;

    sf::CircleShape shape;
    
    bool        isCollindingEnnemy(int _cx, int _cy);
    bool        isCollindingWall(int _cx, int _cy);
    
    
public:
  
                Bullet(sf::Vector2i pos = sf::Vector2i(0,0), bool moveRight = true);
    void        update(double _dt);
    void        draw();
};
