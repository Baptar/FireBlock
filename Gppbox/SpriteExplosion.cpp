#include "SpriteExplosion.h"

#include "C.hpp"

SpriteExplosion::SpriteExplosion(int cx, int cy)
{
    if (!textureExplosion.loadFromFile("res/explo.png")) printf("ERR : LOAD FAILED\n");
    spriteExplosion.setTexture(textureExplosion);
    sizeTexture = textureExplosion.getSize().y;

    //spriteExplosion.setPosition({float(cx *  C::GRID_SIZE), float(cy *  C::GRID_SIZE)});
    spriteExplosion.setOrigin(sizeTexture / 2, sizeTexture / 2);
    spriteExplosion.setTextureRect(sf::IntRect(0, 0, sizeTexture, sizeTexture));
    spriteExplosion.setScale(sf::Vector2f(1.0f, 1.0f));
}

void SpriteExplosion::update(double dt)
{
    if (animationTime >= frameSpeed) {
        if (currentFrame == numberOfFrame-1)
        {
            play = false;
            finishedAnim = true;
            return;
        }
        
        animationTime = 0;
        currentFrame = (currentFrame + 1) % numberOfFrame;
		
        setAnimationFrame(currentFrame);
    }
    if (play) animationTime += dt;
}

void SpriteExplosion::setAnimationFrame(int _frame)
{
    play = true;
    animationTime = 0.0f;
    this->currentFrame = _frame % numberOfFrame;
    
    numberOfFrame = 25;
    
    spriteExplosion.setTextureRect(sf::IntRect(_frame * sizeTexture, 0, sizeTexture, sizeTexture));
}
