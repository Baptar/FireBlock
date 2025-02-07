#include <imgui.h>

#include "Player.hpp"

#include "Bullet.h"
#include "C.hpp"
#include "Game.hpp"

Player::Player(): spritePlayer(SpritePlayer(*this))
{
}

void Player::update(double dt){
	spritePlayer.update(dt);
	
	for (auto it = bullets.begin(); it != bullets.end(); ) {
		Bullet* bullet = *it;
		if (bullet == nullptr) it = bullets.erase(it);
		else if (bullet->shouldDestroy) {
			delete bullet;
			it = bullets.erase(it);
		} else {
			bullet->update(dt);
			++it;
		}
	}
	
	Game& g = *Game::me;
	double rate = 1.0 / dt;
	double dfr = 60.0f / rate;

	dy += gravy * dt;
	dy = dy * pow(fry, dfr);
	dx = dx * pow(frx, dfr);
	
	rx += dx * dt;
	ry += dy * dt;

	if (delayFire > 0.0f) delayFire -= dt;
		
	// Check not moving
	if (dx <= 2.0f && dx >= -2.0f && dy<= 2.0f && (spritePlayer.animationRow == 1 || spritePlayer.animationRow == 2 || spritePlayer.animationRow == 10) && !jumping)
	{
		if (firing) spritePlayer.playAnimationSprite(0, 6);
		else if (!firing) spritePlayer.playAnimationSprite(0, 0);
	}
	
	// Collision Left
	do
	{
		if (g.hasPlayerCollision(cx - 1 , cy) && rx < 0.5)
		{
			if (!jumping && !firing && !reloading) spritePlayer.playAnimationSprite(0, 0);
			dx = 0;
			rx = 0.5f;
		}
		else if (rx < 0)
		{
			if (!jumping && !reloading)
			{
				if (firing) spritePlayer.playAnimationSprite(spritePlayer.currentFrame + 1, 10);
				else if (!firing) spritePlayer.playAnimationSprite(spritePlayer.currentFrame + 1, 2);
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
			if (!jumping && !firing && !reloading) spritePlayer.playAnimationSprite(0, 0);
			dx = 0;
			rx = 0.5f;
		}
		else if (rx > 1)
		{
			if (!jumping && !reloading)
			{
				if (firing) spritePlayer.playAnimationSprite(spritePlayer.currentFrame + 1, 10);
				else if (!firing) spritePlayer.playAnimationSprite(spritePlayer.currentFrame + 1, 2);
			}
			moveRight = true;
			rx--;
			cx++;
		}
	} while (rx > 1);

	// Jump
	if (jumping) {
		// Collision when go up
		do
		{
			if (g.hasPlayerCollision(cx, cy  - 2) && ry <= 0.01f)
			{
				gravy = 80;
				dy = 0;
				ry = 0.01f;
			}
			else if (ry < 0)
			{
				ry++;
				cy--;
			}
		} while (ry < 0);

		// Collision when go down
		do
		{
			if (g.hasPlayerCollision(cx, cy + 1) && ry >= 0.99f)
			{
				setJumping(false);
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
	else
	{
		if (!g.hasPlayerCollision(cx , cy + 1))
		{
			setJumping(true);
		}
	}
	syncPos();
	
	for (auto b : bullets)
		b->update(dt);
	
}

void Player::setCooPixel(int px, int py){
	cx = px / C::GRID_SIZE;
	cy = py / C::GRID_SIZE;

	rx = (px - (cx * C::GRID_SIZE)) / (float)C::GRID_SIZE;
	ry = (py - (cy * C::GRID_SIZE)) / (float)C::GRID_SIZE;

	syncPos();
}

void Player::setCooGrid(float coox, float cooy){
	cx = (int)coox;
	rx = coox - cx;

	cy = (int)cooy;
	ry = cooy - cy;
	syncPos();
}

void Player::syncPos() {
	spritePlayer.getSprite().setPosition((cx + rx) * C::GRID_SIZE, (cy + ry) * C::GRID_SIZE);
}

void Player::draw(sf::RenderWindow& win){
	win.draw(spritePlayer.getSprite());
	
	for (Bullet* b : bullets)
		b->draw();
}

void Player::setJumping(bool onOff){
	if (jumping && onOff || reloading) 
		return;
	
	if (onOff) {
		stopFire();
		spritePlayer.playAnimationSprite(0, 3);
		gravy = 110;
		jumping = true;
	}
	else {
		spritePlayer.playAnimationSprite(0, 0);
		gravy = 0;
		jumping = false;
	}
}

sf::Vector2i Player::getPosPixel() const
{
	return sf::Vector2i( (cx+rx)*C::GRID_SIZE, (cy+ry) * C::GRID_SIZE );
}

void Player::stopFire()
{
	if (!firing) return;
	firing = false;
	if (dx > 2.0f || dx < -2.0f) spritePlayer.playAnimationSprite(spritePlayer.currentFrame + 1, 2);
	else spritePlayer.playAnimationSprite(0, 0);
}

void Player::fire()
{
	if (reloading || jumping || delayFire > 0) return;
	delayFire = 0.1f;
	Game::me->camera.addShake(1.2f, 0.2f, 0.2f);
	
	// Start Fire System
	firing = true;
	if (dx > 2.0f || dx < -2.0f) spritePlayer.playAnimationSprite(spritePlayer.currentFrame + 1, 10);
	else spritePlayer.playAnimationSprite(0, 6);
	
	bullets.push_back(new Bullet({cx, cy}, moveRight));
}

void Player::reload()
{
	if (reloading || jumping) return;

	stopFire();
	reloading = true;
	spritePlayer.playAnimationSprite(0, 8);
}

bool Player::im()
{
	using namespace ImGui;

	bool chg = false;
	if (TreeNodeEx("Bullets", 0)) {
		for (auto b : bullets)
			b->im();
		TreePop();
	}
	Value("animation Row", spritePlayer.animationRow);
	Value("currentFrame", spritePlayer.currentFrame);
	Value("jumping", jumping);
	Value("firing", firing);
	Value("reloading", reloading);
	Value("cx", cx);
	Value("cy", cy);

	Value("rx",rx);
	Value("ry",ry);

	sf::Vector2i pix = getPosPixel();
	chg |= DragInt2("pix x/pix y", &pix.x, 1.0f, -2000,2000);
	if (chg) 
		setCooPixel(pix.x, pix.y);

	chg |= DragInt2("cx/cy", &cx, 1.0f, -2000, 2000);
	
	sf::Vector2f coo = { cx + rx, cy + ry };
	bool chgCoo = DragFloat2("coo grid x/y", &coo.x, 1.0f, -2000,2000);
	if (chgCoo) 
		setCooGrid(coo.x, coo.y);
	
	chg |= DragFloat2("dx/dy", &dx, 0.01f, -20,20);
	chg |= DragFloat2("frx/fry", &frx, 0.001f, 0, 1);
	chg |= DragFloat("gravy/fry", &gravy, 0.001f, -2, 2);

	if (Button("reset")) {
		cx = 3;
		cy = 54;
		rx = 0.5f;
		ry = 0.99f;

		dx = dy = 0;
		setJumping(false);
	}
	return chg||chgCoo;
}



