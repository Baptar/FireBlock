#pragma once
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>

class SpriteExplosion
{
public:
    SpriteExplosion(int cx, int cy);
    sf::Sprite	spriteExplosion;
    sf::Texture textureExplosion;
    void update(double dt);
    
    int			currentFrame = 0;   
    float		animationTime = 0;  
    float		frameSpeed = 0.1f;
    int			numberOfFrame = 7;
    int         sizeTexture;

    bool        finishedAnim;
    
    void        setAnimationFrame(int frame);
    bool play;
};
