#include "SpriteEnnemy.h"

#include "Dice.hpp"
#include "Ennemy.hpp"

SpriteEnnemy::SpriteEnnemy(Ennemy& ennemy) : ennemy(ennemy)
{
    CheckFileTexture();
    InitRandomTextures();
}

void SpriteEnnemy::update(double dt)
{
    if (animationTime >= frameSpeed) {
        if (animationRow == 2 && currentFrame == numberOfFrameDead - 1) return;
        animationTime = 0;
        currentFrame = (currentFrame + 1) % numberOfFrame;
		
        setAnimationFrame(currentFrame, animationRow);
    }
    animationTime += dt;
}

void SpriteEnnemy::playAnimationSprite(int frame, int animationRow)
{
    if (!(this->animationRow == animationRow)) setAnimationFrame(frame, animationRow);
}

void SpriteEnnemy::CheckFileTexture()
{
    if (!textureIdle1.loadFromFile("res/IdleZombie.png")) printf("ERR : LOAD FAILED\n");
    if (!textureRun1.loadFromFile("res/RunZombie.png")) printf("ERR : LOAD FAILED\n");
    if (!textureIdle2.loadFromFile("res/IdleZombie2.png")) printf("ERR : LOAD FAILED\n");
    if (!textureRun2.loadFromFile("res/RunZombie2.png")) printf("ERR : LOAD FAILED\n");
    if (!textureIdle3.loadFromFile("res/IdleZombie3.png")) printf("ERR : LOAD FAILED\n");
    if (!textureRun3.loadFromFile("res/WalkZombie3.png")) printf("ERR : LOAD FAILED\n");
    if (!textureIdle4.loadFromFile("res/IdleZombie4.png")) printf("ERR : LOAD FAILED\n");
    if (!textureRun4.loadFromFile("res/WalkZombie4.png")) printf("ERR : LOAD FAILED\n");
    if (!textureIdle5.loadFromFile("res/IdleZombie5.png")) printf("ERR : LOAD FAILED\n");
    if (!textureRun5.loadFromFile("res/WalkZombie5.png")) printf("ERR : LOAD FAILED\n");
    if (!textureIdle6.loadFromFile("res/IdleZombie6.png")) printf("ERR : LOAD FAILED\n");
    if (!textureRun6.loadFromFile("res/WalkZombie6.png")) printf("ERR : LOAD FAILED\n");

    if (!textureDead1.loadFromFile("res/DeadZombie.png")) printf("ERR : LOAD FAILED\n");
    if (!textureDead2.loadFromFile("res/DeadZombie2.png")) printf("ERR : LOAD FAILED\n");
    if (!textureDead3.loadFromFile("res/DeadZombie3.png")) printf("ERR : LOAD FAILED\n");
    if (!textureDead4.loadFromFile("res/DeadZombie4.png")) printf("ERR : LOAD FAILED\n");
    if (!textureDead5.loadFromFile("res/DeadZombie5.png")) printf("ERR : LOAD FAILED\n");
    if (!textureDead6.loadFromFile("res/DeadZombie6.png")) printf("ERR : LOAD FAILED\n");
    
}

sf::Sprite& SpriteEnnemy::getSprite(){ return sprite; }

void SpriteEnnemy::setAnimationFrame(int _frame, int _animationRow)
{
    
    animationTime = 0.0f;
    this->animationRow = _animationRow;
    float scaleAbs = abs(getSprite().getScale().x);
    getSprite().setScale((ennemy.moveRight ? scaleAbs : -scaleAbs), scaleAbs);
    switch (_animationRow)
    {
    case 0: // Idle
        getSprite().setTexture(textureIdleUsed);
        numberOfFrame = numberOfFrameIdle;
        break;
    case 1: // Run
        getSprite().setTexture(textureRunUsed);
        numberOfFrame = numberOfFrameRun;
        break;
    case 2: // Dead
        getSprite().setTexture(textureDeadUsed);
        numberOfFrame = numberOfFrameDead;
        break;
    default: // Default : Idle
        getSprite().setTexture(textureIdleUsed);
        numberOfFrame = numberOfFrameIdle;
        break;
    }
    getSprite().setTextureRect(sf::IntRect(_frame * sizeTexture, 0, sizeTexture, sizeTexture));
}


void SpriteEnnemy::InitRandomTextures()
{
    switch (Dice::randInt(0, 5))
    {
    case 0:
        textureIdleUsed = textureIdle1;
        textureRunUsed = textureRun1;
        textureDeadUsed = textureDead1;
        numberOfFrameIdle = 8;
        numberOfFrameRun = 7;
        break;
    case 1:
        textureIdleUsed = textureIdle2;
        textureRunUsed = textureRun2;
        textureDeadUsed = textureDead2;
        numberOfFrameIdle = 8;
        numberOfFrameRun = 7;
        break;
    case 2:
        textureIdleUsed = textureIdle3;
        textureRunUsed = textureRun3;
        textureDeadUsed = textureDead3;
        numberOfFrameIdle = 8;
        numberOfFrameRun = 7;
        break;
    case 3:
        textureIdleUsed = textureIdle4;
        textureRunUsed = textureRun4;
        textureDeadUsed = textureDead4;
        numberOfFrameIdle = 6;
        numberOfFrameRun = 10;
		
        break;
    case 4:
        textureIdleUsed = textureIdle5;
        textureRunUsed = textureRun5;
        textureDeadUsed = textureDead5;
        numberOfFrameIdle = 6;
        numberOfFrameRun = 10;
        break;
    case 5:
        textureIdleUsed = textureIdle6;
        textureRunUsed = textureRun6;
        textureDeadUsed = textureDead6;
        numberOfFrameIdle = 6;
        numberOfFrameRun = 10;
        break;
    default:
        textureIdleUsed = textureIdle1;
        textureRunUsed = textureRun1;
        textureDeadUsed = textureDead1;
        numberOfFrameIdle = 8;
        numberOfFrameRun = 7;
        break;
    }
	
    sprite.setTexture(textureIdleUsed);
    sizeTexture = textureIdleUsed.getSize().y;
    sprite.setTextureRect(sf::IntRect(0, 0, sizeTexture, sizeTexture));
    sprite.setScale(sf::Vector2f(.5f, .5f));
    sprite.setOrigin(sizeTexture / 2.0f, sizeTexture);
}


