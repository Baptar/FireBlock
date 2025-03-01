﻿#pragma once
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
    float		speedX = 20.f;
    float		speedY = 0.f;
    float       bloom = 1.5f;
    
    float		gravy = 0.f;
    float		frx = 0.88f;
    float		fry = 0.95f;
    
    bool        moveRight = true;
    float       timeLife = 1.2f;
    float       damage = 1.0f;

    Game&       game;

    sf::CircleShape shape;
    
    bool        isCollindingEnnemy(int _cx, int _cy);
    bool        isCollindingWall(int _cx, int _cy);
    void		syncPos();
    
public:
    bool        shouldDestroy = false;
    
                Bullet(sf::Vector2i _pos = sf::Vector2i(0,0), bool _moveRight = true);
    void        update(double _dt);
    void        draw();
    void		im();
};
