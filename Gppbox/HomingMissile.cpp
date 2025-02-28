#include "HomingMissile.hpp"
#include <cmath>

#include "C.hpp"
#include "Game.hpp"
#include "imgui.h"

HomingMissile::HomingMissile(sf::Vector2f startPos, bool _moveRight)
    : spriteExplosion(0,0), position(startPos), speed(100.0f), target(nullptr), shouldDestroy(false), game(*Game::me),
      moveRight(_moveRight)
{
    if (!texture.loadFromFile("res/missile.png")) printf("ERR : LOAD FAILED\n");
    sprite.setTexture(texture);
    sprite.setScale(sf::Vector2f(.2f, .2f));
    sprite.setOrigin(texture.getSize().x / 2.0f, texture.getSize().y / 2.0f);
    rectangle.setOrigin(25, 7.5);

    cx = static_cast<int>(startPos.x);
    cy = static_cast<int>(startPos.y);
    rx = startPos.x - cx;
    ry = startPos.y - cy;
    dx = 0;
    dy = 0;

    //sprite.setRadius(5.0f);  // Rayon du cercle
    //sprite.setFillColor(sf::Color::Red); // Rouge pour bien le voir
    // sprite.setOrigin(5.0f, 5.0f); // Centrer le cercle
    rectangle.setPosition(position);
    rectangle.setSize({50, 15});
    sprite.setPosition(position); // Position initiale

    findTarget();
}

void HomingMissile::update(double dt) {
    if (shouldDestroy) return;

    if (spriteExplosion.finishedAnim)
    {
        shouldDestroy = true;
        return;
    }
    spriteExplosion.update(dt);

    if (shouldExplose) return;
    /*if (!target) {
        findTarget();
    }*/
    
    moveTowardsTarget(dt);
}

void HomingMissile::moveTowardsTarget(double dt) {
    sf::Vector2f targetPos;
    
    //if (!target) return;
    if (target !=nullptr)
    {
        if (target->isDead) target = nullptr;
        else targetPos = {(target->cx + target->rx), (target->cy + target->ry - 1)};
    }
    
    float targetX = targetPos.x;
    float targetY = targetPos.y;
    
    float angle = 0.0f;
    if (target)
    {
        angle = atan2(targetY - (cy + ry), targetX - (cx + rx));
        lastAngle = angle;
       
        dy = sin(lastAngle) * speed * dt;
    }
    else
    {
        angle = lastAngle;
        
    }
    if (foundTarget) dx = cos(lastAngle) * speed * dt;
    else
    {
        if (moveRight)
        {
            dx = cos(lastAngle) * speed * dt;
            //spriteExplosion.spriteExplosion.setScale({0.2, 0.2});
        }
        else
        {
            dx = -cos(lastAngle) * speed * dt;
            //spriteExplosion.spriteExplosion.setScale({-0.2, 0.2});
        }
    }
    
    do {
        // Right
        if (dx > 0 && (Game::me->hasWall(cx + 1, cy) || Game::me->hasEnnemy(cx + 1, cy))) {
            dx = 0;
            rx = 0.5f;
            missileTouched();
        }
        // Left
        else if (dx < 0 && (Game::me->hasWall(cx - 1, cy) || Game::me->hasEnnemy(cx - 1, cy))) {
            dx = 0;
            rx = 0.5f;
            missileTouched();
        }
        else {
            rx += dx / C::GRID_SIZE;
            if (rx > 1) { rx--; cx++; moveRight = true;}
            if (rx < 0) { rx++; cx--; moveRight = false;}
        }
    } while (rx < 0 || rx > 1);
    
    do {
        if (dy > 0 && (Game::me->hasWall(cx, cy + 1) || Game::me->hasEnnemy(cx, cy + 1))) {
            dy = 0;
            ry = 0.5f;
            missileTouched();
        } else if (dy < 0 && (Game::me->hasWall(cx, cy - 1) || Game::me->hasEnnemy(cx, cy - 1))) {
            dy = 0;
            ry = 0.5f;
            missileTouched();
        } else {
            ry += dy / C::GRID_SIZE;
            if (ry > 1) { ry--; cy++; }
            if (ry < 0) { ry++; cy--; }
        }
    } while (ry < 0 || ry > 1);
    
    position.x = (cx + rx);
    position.y = (cy + ry);
    Vector2f newPos = position;
    newPos.x *= C::GRID_SIZE;
    newPos.y *= C::GRID_SIZE;
    if (foundTarget)
    {
        sprite.setRotation(lastAngle * 180.0f / 3.14159f);
        rectangle.setRotation(lastAngle * 180.0f / 3.14159f);
    }
    else
    {
        if (moveRight) sprite.setRotation(0);
        else sprite.setRotation(180);
    }
    sprite.setPosition(newPos);
    
    rectangle.setPosition(newPos);
    

    float distance = sqrt((position.x - targetPos.x) * (position.x - targetPos.x) + (position.y - targetPos.y) * (position.y - targetPos.y));
    if (distance < 2.0f)
        missileTouched();
}

void HomingMissile::findTarget() {
    //target = Game::me->getClosestEnemy(position);
    if (!game.ennemies.empty())
    {
        for (auto ennemy : Game::me->ennemies)
        {
            if (!ennemy->isDead)
            {
                foundTarget = true;
                target = ennemy;
                return;
            }
        }
    }
}

void HomingMissile::missileTouched()
{
    float		cameraShakeAmplitude = 7.0f;
    float		cameraShakeFrequency = 1.0f;
    float		cameraShakeDuration = 0.5f;
    Game& g = *Game::me;
    g.camera.addShake(cameraShakeAmplitude, cameraShakeFrequency, cameraShakeDuration);
    for (int i = -2; i < 3 ; i++)
        for (int j = -2; j < 3 ; j++)
        {
            if (Ennemy* ennemy = g.getEnnemyAtPosition(cx + i, cy + j))
                ennemy->takeDamage(10, moveRight);
        }
    
    shouldExplose = true;
    spriteExplosion.spriteExplosion.setPosition((cx + rx)*C::GRID_SIZE, (cy + ry)*C::GRID_SIZE);
    spriteExplosion.setAnimationFrame(0);
}

void HomingMissile::draw() {
    if (!shouldExplose && !shouldDestroy) game.win->draw(sprite);
    game.win->draw(spriteExplosion.spriteExplosion);
}

void HomingMissile::im()
{
    using namespace ImGui;

    Value("frame", spriteExplosion.currentFrame);
    Value("Should Explode", shouldExplose);
    Value("Should Destroy", shouldDestroy);
    Value("finishedAnim", spriteExplosion.finishedAnim);
    Value("MoveRight", moveRight);
    Value("cx", cx);
    Value("cy", cy);
    Value("rx", rx);
    Value("ry", ry);
    Value("speed", speed);
    Value("dx", dx);
    Value("dy", dy);
    Value("pos sprite x ", spriteExplosion.spriteExplosion.getPosition().x);
    Value("pos sprite y ", spriteExplosion.spriteExplosion.getPosition().y);
}
