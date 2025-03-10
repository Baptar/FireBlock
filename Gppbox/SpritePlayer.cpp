﻿#include "SpritePlayer.hpp"

#include "Player.hpp"

SpritePlayer::SpritePlayer(Player& _player): player(_player)
{
    CheckFileTexture();
    InitTexture();
}

void SpritePlayer::update(double _dt)
{
    if (durationDamage > 0.0f)
    {
        durationDamage -= _dt;
    }
    else
    {
        sprite.setColor(sf::Color::White);
    }
    
    animationTime += _dt;
    if (animationTime >= frameSpeed) {
        animationTime = 0;
        currentFrame = (currentFrame + 1) % numberOfFrame;
		
        // if we start jumping
        if (animationRow == 3 && currentFrame >= 5)
        {
            setAnimationFrame(0, 4);
        }
		
        // reloading
        else if (animationRow == 8 && currentFrame == 16)
        {
            //Finished to reload
            player.reloading = false;
            player.actualBullets = player.maxBullets;
            setAnimationFrame(0, 0);
        }
        // Hurt
        else if (animationRow == 7 && currentFrame == 4)
        {
            isHurting = false;
            setAnimationFrame(0, 0);
        }
        // death
        else if (animationRow == 9 && currentFrame == 4)
        {
            player.isDead = true;
            return;
        }
		
        else
        {
            setAnimationFrame(currentFrame, animationRow);
        }
    }
}

void SpritePlayer::playAnimationSprite(int _frame, int _animationRow)
{
    if (!(this->animationRow == _animationRow) && !isHurting && !isDieing) setAnimationFrame(_frame, _animationRow);
}

void SpritePlayer::CheckFileTexture()
{
    if (!textureIdle.loadFromFile("res/Idle.png")) printf("ERR : LOAD FAILED\n");
    if (!textureWalk.loadFromFile("res/Walk.png")) printf("ERR : LOAD FAILED\n");
    if (!textureRun.loadFromFile("res/Run.png")) printf("ERR : LOAD FAILED\n");
    if (!textureJump0.loadFromFile("res/Jump0.png")) printf("ERR : LOAD FAILED\n");
    if (!textureJump1.loadFromFile("res/Jump1.png")) printf("ERR : LOAD FAILED\n");
    if (!textureJump2.loadFromFile("res/Jump2.png")) printf("ERR : LOAD FAILED\n");
    if (!textureHurt.loadFromFile("res/Hurt.png")) printf("ERR : LOAD FAILED\n");
    if (!textureFire.loadFromFile("res/Shot.png")) printf("ERR : LOAD FAILED\n");
    if (!textureReload.loadFromFile("res/Reload.png")) printf("ERR : LOAD FAILED\n");
    if (!textureDeath.loadFromFile("res/Dead.png")) printf("ERR : LOAD FAILED\n");
    if (!textureRunShot.loadFromFile("res/Run_Shot.png")) printf("ERR : LOAD FAILED\n");
}

void SpritePlayer::InitTexture()
{
    sprite.setTexture(textureIdle);
    sizeTexture = (int)textureIdle.getSize().y;
    sprite.setTextureRect(sf::IntRect(0, 0, sizeTexture, sizeTexture));
    sprite.setScale(sf::Vector2f(.5f, .5f));
    sprite.setOrigin((float)sizeTexture / 2.0f, (float)sizeTexture);
}

void SpritePlayer::setAnimationFrame(int _frame, int _animationRow)
{
    player.reloading = false;
    animationTime = 0.0f;
    this->animationRow = _animationRow;
    this->currentFrame = _frame % numberOfFrame;
    float scaleAbs = abs(sprite.getScale().x);
    sprite.setScale((player.moveRight ? scaleAbs : -scaleAbs) , scaleAbs);
    switch (_animationRow)
    {
    case 0:
        sprite.setTexture(textureIdle);
        numberOfFrame = 6;
        break;
    case 1:
        sprite.setTexture(textureWalk);
        numberOfFrame = 10;
        break;
    case 2:
        sprite.setTexture(textureRun);
        numberOfFrame = 9;
        break;
    case 3:
        sprite.setTexture(textureJump0);
        numberOfFrame = 6;
        break;
    case 4:
        sprite.setTexture(textureJump1);
        numberOfFrame = 1;
        break;
    case 5:
        sprite.setTexture(textureJump2);
        numberOfFrame = 4;
        break;
    case 6:
        sprite.setTexture(textureFire);
        numberOfFrame = 4;
        break;
    case 7:
        isHurting = true;
        sprite.setColor(sf::Color::Red);
        durationDamage = 0.1f;
        sprite.setTexture(textureHurt);
        numberOfFrame = 5;
        break;
    case 8:
        player.reloading = true;
        sprite.setTexture(textureReload);
        numberOfFrame = 17;
        break;
    case 9:
        isDieing = true;
        sprite.setTexture(textureDeath);
        numberOfFrame = 5;
        break;
    case 10:
        sprite.setTexture(textureRunShot);
        numberOfFrame = 9;
        break;
    default:
        sprite.setTexture(textureIdle);
        numberOfFrame = 6;
        break;
    }
    sprite.setTextureRect(sf::IntRect(_frame * sizeTexture, 0, sizeTexture, sizeTexture));
}

sf::Sprite& SpritePlayer::getSprite() { return sprite; }


