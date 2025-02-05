#include <imgui.h>

#include "Ennemy.hpp"

#include "C.hpp"
#include "Game.hpp"

Ennemy::Ennemy(int _cx, int _cy): spriteEnnemy(SpriteEnnemy(*this))
{
	setCooGrid(_cx, _cy);
}

void Ennemy::update(double _dt)
{
	spriteEnnemy.update(_dt);
	
	Game& g = *Game::me;
	double rate = 1.0 / _dt;
	double dfr = 60.0f / rate;

	dy += gravy * _dt;
	dy = dy * pow(fry, dfr);
	dx = (moveRight ?  15 : -15) * pow(frx, dfr);

	rx += dx * _dt;
	ry += dy * _dt;

	// Check not moving
	if (dx <= 2.0f && dx >= -2.0f && dy <= 2.0f && !dropping)
	{
		spriteEnnemy.playAnimationSprite(0, 0);
	}
	
	// Collision Left
	do
	{
		if (g.hasPlayerCollision(cx - 1, cy) && rx < 0.5)
		{
			moveRight = true;
			dx = 0.0f;
			rx = 0.5f;
		}
		else if (rx < 0)
		{
			if (!dropping)
			{
				spriteEnnemy.playAnimationSprite(0, 1);
			}
			moveRight = false;
			rx++;
			cx--;
		}
	} while (rx < 0);

	// Collision Right
	do
	{
		if (g.hasPlayerCollision(cx + 1, cy) && rx > 0.5)
		{
			moveRight = false;
			dx = 0.0f;
			rx = 0.5f;
		}
		else if (rx > 1)
		{
			if (!dropping)
			{
				spriteEnnemy.playAnimationSprite(0, 1);
			}
			moveRight = true;
			rx--;
			cx++;
		}
	} while (rx > 1);

	if (dropping)
	{
		// Collision when go down
		do
		{
			if (g.hasPlayerCollision(cx, cy + 1) && ry >= 0.99f)
			{
				setDropping(false);
				dy = 0;
				dx *= 0.5f;
				ry = 0.99f;
			}
			else if (ry > 1)
			{
				ry--;
				cy++;
			}
		} while (ry > 1);
	}
	
	if (!g.hasPlayerCollision(cx, cy + 1))
	{
		setDropping(true);
	}
	syncPos();
}

void Ennemy::setCooPixel(int _px, int _py)
{
	cx = _px / C::GRID_SIZE;
	cy = _py / C::GRID_SIZE;

	rx = (_px - (cx * C::GRID_SIZE)) / (float)C::GRID_SIZE;
	ry = (_py - (cy * C::GRID_SIZE)) / (float)C::GRID_SIZE;

	syncPos();
}

void Ennemy::setCooGrid(float _coox, float _cooy)
{
	cx = (int)_coox;
	rx = _coox - cx;

	cy = (int)_cooy;
	ry = _cooy - cy;
	syncPos();
}

sf::Vector2i Ennemy::getPosPixel() const
{
	return sf::Vector2i((cx + rx) * C::GRID_SIZE, (cy + ry) * C::GRID_SIZE);
}

void Ennemy::syncPos()
{
	spriteEnnemy.getSprite().setPosition((cx + rx) * C::GRID_SIZE, (cy + ry) * C::GRID_SIZE);
}

void Ennemy::draw(sf::RenderWindow& _win)
{
	_win.draw(spriteEnnemy.getSprite());
}

void Ennemy::setDropping(bool _onOff)
{
	if (dropping && _onOff)
		return;

	if (_onOff) {
		spriteEnnemy.playAnimationSprite(0, 2);
		gravy = 80;
		dropping = true;
	}
	else {
		spriteEnnemy.playAnimationSprite(0, 0);
		gravy = 0;
		dropping = false;
	}
}

bool Ennemy::im()
{
	using namespace ImGui;

	bool chg = false;

	Value("Dropping", dropping);
	Value("cx", cx);
	Value("cy", cy);

	Value("rx", rx);
	Value("ry", ry);

	sf::Vector2i pix = getPosPixel();
	chg |= DragInt2("pix x/pix y", &pix.x, 1.0f, -2000, 2000);
	if (chg)
		setCooPixel(pix.x, pix.y);

	chg |= DragInt2("cx/cy", &cx, 1.0f, -2000, 2000);

	sf::Vector2f coo = { cx + rx, cy + ry };
	bool chgCoo = DragFloat2("coo grid x/y", &coo.x, 1.0f, -2000, 2000);
	if (chgCoo)
		setCooGrid(coo.x, coo.y);

	chg |= DragFloat2("dx/dy", &dx, 0.01f, -20, 20);
	chg |= DragFloat2("frx/fry", &frx, 0.001f, 0, 1);
	chg |= DragFloat("gravy/fry", &gravy, 0.001f, -2, 2);

	return chg || chgCoo;
}
