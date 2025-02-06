#include "Bullet.h"

#include "C.hpp"
#include "imgui.h"

Bullet::Bullet(sf::Vector2i _pos, bool _moveRight) : cx(_pos.x), cy(_pos.y), moveRight(_moveRight), game(*Game::me)
{
    speedX = moveRight ? speedX : -speedX; 
    speedY = Dice::randFloat(0.0f, bloom);
    shape.setRadius(1.f);
    shape.setFillColor(Color::White);
    shape.setOrigin(shape.getRadius(), shape.getRadius());
}

void Bullet::update(double _dt)
{
    if (timeLife <= 0)
    {
        shouldDestroy = true;
    }
    if (shouldDestroy) return;

    timeLife -= _dt;
    double rate = 1.0 / _dt;
    double dfr = 60.0f / rate;

    dy += gravy * _dt;
    dy = speedY * pow(fry, dfr);
    dx = speedX * pow(frx, dfr);

    rx += dx * _dt;
    ry += dy * _dt;

    // Collision Left
    do
    {
        if ((isCollindingEnnemy(cx, cy) || isCollindingWall(cx, cy)) && rx < 0.5)
        {
            //destroy
            shouldDestroy = true;
        }
        else if (rx < 0)
        {
            moveRight = false;
            rx++;
            cx--;
        }
    } while (rx < 0);

    // Collision Right
    do
    {
        if ((isCollindingEnnemy(cx, cy) || isCollindingWall(cx, cy)) && rx > 0.5)
        {
            // destroy
            shouldDestroy = true;
        }
        else if (rx > 1)
        {
            rx--;
            cx++;
        }
    } while (rx > 1);

    // Collision Bottom
    do
    {
        if ((isCollindingEnnemy(cx, cy) || isCollindingWall(cx, cy)) && ry >= 0.99f)
        {
            //destroy
            shouldDestroy = true;
        }
        else if (ry > 1)
        {
            ry--;
            cy++;
        }
    } while (ry > 1);

    // Collision Top
    do
    {
        if ((isCollindingEnnemy(cx, cy) || isCollindingWall(cx, cy)) && ry <= 0.01f)
        {
            // destroy
            shouldDestroy = true;
        }
        else if (ry < 0)
        {
            ry++;
            cy--;
        }
    } while (ry < 0);

    syncPos();
}

void Bullet::draw()
{
    game.win->draw(shape);
}

bool Bullet::isCollindingEnnemy(int _cx, int _cy)
{
    for (auto e : game.ennemies)
    {
        if (e->isDead) continue;
        if (e->cx == _cx && (e->cy == _cy ||e->cy == _cy - 1))
        {
            e->takeDamage(damage);
            return true;
        }
    }
    return false;
}

bool Bullet::isCollindingWall(int _cx, int _cy)
{
    for (Vector2i & w : game.walls)
    {
        if (w.x == _cx && w.y == _cy)
        {
            return true;
        }
    }
    return false;
}

void Bullet::im()
{
    using namespace ImGui;

    bool chg = false;
	
    Value("speedX", speedX);
    Value("speedY", speedY);
    Value("bloom", bloom);
    Value("moveRight", moveRight);
    Value("shape x", shape.getPosition().x);
    Value("shape y", shape.getPosition().y);
    Value("cx", cx);
    Value("cy", cy);

    Value("rx",rx);
    Value("ry",ry);

    chg |= DragInt2("cx/cy", &cx, 1.0f, -2000, 2000);
	
    sf::Vector2f coo = { cx + rx, cy + ry };
    
}

void Bullet::syncPos()
{
    shape.setPosition((cx + rx) * C::GRID_SIZE, (cy + ry) * C::GRID_SIZE);
}
