#include "Bullet.h"

Bullet::Bullet(sf::Vector2i _pos, bool _moveRight) : cx(_pos.x), cy(_pos.y), moveRight(_moveRight), game(*Game::me)
{
    speedX = moveRight ? speedX : -speedX; 
    speedY = Dice::randFloat(0.0f, bloom);
    shape.setRadius(5.f);
    shape.setFillColor(Color::White);
}

void Bullet::update(double _dt)
{
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
        if ((isCollindingEnnemy(cx - 1, cy) || isCollindingWall(cx - 1, cy)) && rx < 0.5)
        {
            //destroy
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
        if ((isCollindingEnnemy(cx + 1, cy) || isCollindingWall(cx + 1, cy)) && rx > 0.5)
        {
            // destroy
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
        if ((isCollindingEnnemy(cx, cy + 1) || isCollindingWall(cx, cy + 1)) && ry >= 0.99f)
        {
            //destroy
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
        if ((isCollindingEnnemy(cx, cy - 1) || isCollindingWall(cx, cy - 1)) && ry <= 0.01f)
        {
            // destroy
        }
        else if (ry < 0)
        {
            ry++;
            cy--;
        }
    } while (ry < 0);

    timeLife -= _dt;
    if (timeLife <= 0)
    {
        game.getPlayer().bullets.pop();
    }
}

void Bullet::draw()
{
    game.win->draw(shape);
}

bool Bullet::isCollindingEnnemy(int _cx, int _cy)
{
    for (auto e : game.ennemies)
    {
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
