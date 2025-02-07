#pragma once
#include <SFML/Graphics.hpp>

#include "SpriteEnnemy.h"

namespace sf
{
    class RectangleShape;
}

class Ennemy
{
public:
    SpriteEnnemy spriteEnnemy;
    int         life = 10;
    bool        isDead = false;
    
    int			cx = 0;
    int			cy = 0;
    float		rx = 0.5f;
    float		ry = 0.99f;

    float       speedXMin = 2.0f;
    float       speedXMax = 10.0f;
    float       speedX;
    float		dx = 0.f;
    float		dy = 0.f;
    float		gravy = 0.f;

    float		frx = 0.88f;
    float		fry = 0.95f;

    bool        dropping = false;
    bool		moveRight = true;    
    
                    Ennemy(int _cx, int _cy);
    void            update(double _dt);
    void			setCooPixel(int _px, int _py);
    void			setCooGrid(float _coox, float _cooy);
    sf::Vector2i	getPosPixel() const;
    void			syncPos();
    void			draw(sf::RenderWindow& _win);
    bool			im();
    void            setDropping(bool _onOff);
    void            takeDamage(int _damage);

};
