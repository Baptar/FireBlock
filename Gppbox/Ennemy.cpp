#include <imgui.h>

#include "Ennemy.hpp"
#include "C.hpp"
#include "Game.hpp"

Ennemy::Ennemy(int cx, int cy, sf::RectangleShape* _spr) : spr(_spr) 
{
	if (!textureIdle.loadFromFile("res/Idle.png")) printf("ERR : LOAD FAILED\n");
	if (!textureRun.loadFromFile("res/Run.png")) printf("ERR : LOAD FAILED\n");
	sprite.setTexture(textureIdle);
	sprite.setTextureRect(sf::IntRect(0, 0, 128, 128));
	sprite.setScale(sf::Vector2f(.5f, .5f));
	sprite.setOrigin(128 / 2.0f, 128);

	setCooGrid(cx, cy);
}

Ennemy::Ennemy(int cx, int cy)
{
	if (!textureIdle.loadFromFile("res/Idle.png")) printf("ERR : LOAD FAILED\n");
	if (!textureRun.loadFromFile("res/Run.png")) printf("ERR : LOAD FAILED\n");
	sprite.setTexture(textureIdle);
	sprite.setTextureRect(sf::IntRect(0, 0, 128, 128));
	sprite.setScale(sf::Vector2f(.5f, .5f));
	sprite.setOrigin(128 / 2.0f, 128);

	setCooPixel(cx, cy);
}

void Ennemy::update(double dt)
{
	Game& g = *Game::me;
	double rate = 1.0 / dt;
	double dfr = 60.0f / rate;

	dy += gravy * dt;
	dy = dy * pow(fry, dfr);
	dx = (moveRight ?  15 : -15) * pow(frx, dfr);

	rx += dx * dt;
	ry += dy * dt;

	// Check not moving
	if (dx <= 2.0f && dx >= -2.0f && dy <= 2.0f && !dropping)
	{
		if (animationRow != 0) setAnimationFrame(0, 0);
	}

	animationTime += dt;
	if (animationTime >= frameSpeed) {
		animationTime = 0;
		currentFrame = (currentFrame + 1) % numberOfFrame;
		
		setAnimationFrame(currentFrame, animationRow);
	}
	
	// Collision Left
	do
	{
		if (g.hasPlayerCollision(cx - 1, cy) && rx < 0.5)
		{
			//if (animationRow != 0 && !dropping) setAnimationFrame(0, 0);
			moveRight = true;
			dx = 0.0f;
			rx = 0.5f;
		}
		else if (rx < 0)
		{
			if (!dropping)
			{
				if (animationRow != 1) setAnimationFrame(0, 1);
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
			//if (animationRow != 0 && !dropping) setAnimationFrame(0, 0);
			moveRight = false;
			dx = 0.0f;
			rx = 0.5f;
		}
		else if (rx > 1)
		{
			if (!dropping)
			{
				if (animationRow != 1) setAnimationFrame(0, 1);
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

void Ennemy::setCooPixel(int px, int py)
{
	cx = px / C::GRID_SIZE;
	cy = py / C::GRID_SIZE;

	rx = (px - (cx * C::GRID_SIZE)) / (float)C::GRID_SIZE;
	ry = (py - (cy * C::GRID_SIZE)) / (float)C::GRID_SIZE;

	syncPos();
}

void Ennemy::setCooGrid(float coox, float cooy)
{
	cx = (int)coox;
	rx = coox - cx;

	cy = (int)cooy;
	ry = cooy - cy;
	syncPos();
}

sf::Vector2i Ennemy::getPosPixel()
{
	return sf::Vector2i((cx + rx) * C::GRID_SIZE, (cy + ry) * C::GRID_SIZE);
}

void Ennemy::syncPos()
{
	sprite.setPosition((cx + rx) * C::GRID_SIZE, (cy + ry) * C::GRID_SIZE);
	if (spr)
	{
		sf::Vector2f pos = { (cx + rx) * C::GRID_SIZE, (cy + ry) * C::GRID_SIZE };
		spr->setPosition(pos);
	}
}

void Ennemy::draw(sf::RenderWindow& win)
{
	if (spr) win.draw(*spr);
	win.draw(sprite);
}

bool Ennemy::im()
{
	using namespace ImGui;

	bool chg = false;

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

void Ennemy::setAnimationFrame(int _frame, int _animationRow)
{
	animationTime = 0.0f;
	this->animationRow = _animationRow;
	float scaleAbs = abs(sprite.getScale().x);
	sprite.setScale((moveRight ? scaleAbs : -scaleAbs), scaleAbs);
	switch (_animationRow)
	{
	case 0:
		sprite.setTexture(textureIdle);
		numberOfFrame = 6;
		break;
	case 1:
		sprite.setTexture(textureRun);
		numberOfFrame = 6;
	default:
		sprite.setTexture(textureRun);
		numberOfFrame = 6;
		break;
	}
	sprite.setTextureRect(sf::IntRect(_frame * 128, 0, 128, 128));
}

void Ennemy::setDropping(bool onOff)
{
	if (dropping && onOff)
		return;

	if (onOff) {
		if (animationRow != 2) setAnimationFrame(0, 2);
		gravy = 80;
		dropping = true;
	}
	else {
		if (animationRow != 0) setAnimationFrame(0, 0);
		gravy = 0;
		dropping = false;
	}
}
