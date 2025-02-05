#pragma once
#include <SFML/Graphics.hpp>

namespace sf
{
    class RectangleShape;
}

class Ennemy
{
public:
    double life = 0.0;
    sf::RectangleShape * spr = 0;
    sf::Sprite sprite;
    sf::Texture textureIdle;
    sf::Texture textureRun;

    int			numberOfFrame = 6;
    int			cx = 0;
    int			cy = 0;
    float		rx = 0.5f;
    float		ry = 0.0f;
	
    float		dx = 0.f;
    float		dy = 0.f;
    float		gravy = 0.f;

    float		height = 0.f;
    float		width = 0.f;

    float		frx = 0.88f;
    float		fry = 0.95f;

    bool        dropping = false;
    bool		moveRight = true;
    int			currentFrame = 0;   
    float		animationTime = 0;  
    float		frameSpeed = 0.1f;
    int			animationRow = 0; // 0 = idle, 1 = walk, 2 = run, 3 = jump

                    Ennemy(int cx, int cy, sf::RectangleShape* spr);
                    Ennemy(int cx, int cy);
    void            update(double dt);
    void			setCooPixel(int px, int py);
    void			setCooGrid(float coox, float cooy);
    sf::Vector2i	getPosPixel();
    void			syncPos();
    void			draw(sf::RenderWindow& win);
    bool			im();
    void			setAnimationFrame(int frame, int animationRow);
    void            setDropping(bool onOff);

};
