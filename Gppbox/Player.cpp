#include <imgui.h>

#include "Player.hpp"

#include "Bullet.hpp"
#include "C.hpp"
#include "Game.hpp"

Player::Player(): spritePlayer(SpritePlayer(*this))
{
}

void Player::update(double dt){
	
	drawLine(cx, cy, cx + 3, cy - 1);
	if (isDead)
	{
		spritePlayer.setAnimationFrame(4,9);
		return;
	}
	
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
			if (!jumping && !reloading && g.pressingLeft)
			{
				if (firing) spritePlayer.playAnimationSprite(spritePlayer.currentFrame + 1, 10);
				else if (!firing) spritePlayer.playAnimationSprite(spritePlayer.currentFrame + 1, 2);
			}
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
			if (!jumping && !reloading && g.pressingRight)
			{
				if (firing) spritePlayer.playAnimationSprite(spritePlayer.currentFrame + 1, 10);
				else if (!firing) spritePlayer.playAnimationSprite(spritePlayer.currentFrame + 1, 2);
			}
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
	Game::me->camera.addShake(cameraShakeAmplitude, cameraShakeFrequency, cameraShakeDuration);
	
	// Start Fire System
	dx += moveRight ? -reculPower : reculPower;
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

void Player::drawLineH(int x0, int y0, int x1, int y1)
{
	if (x0 > x1) {
		std::swap(x0, x1);
		std::swap(y0, y1);
	}

	int dx = x1 - x0;
	int dy = y1 - y0;

	int dir = (dy < 0) ? -1 : 1;
	dy *= dir;

	if (dx != 0) {
		int y = y0;
		int p = 2 * dy - dx;

		for (int i = 0; i <= dx; i++) {
			//putPixel(x0 + i, y);

			printf("x : %d", x0 + i);
			printf(", y : %d\n", y);
			sf::RectangleShape rect = sf::RectangleShape(sf::Vector2f(1.0f, 1.0f));
			rect.setFillColor(Color::White);
			rect.setPosition(x0 + i, y);
			Game::me->win->draw(rect);
			
			if (p >= 0) {
				y += dir;
				p -= 2 * dx;
			}
			p += 2 * dy;
		}
	}
}

void Player::drawLineV(int x0, int y0, int x1, int y1)
{
	if (y0 > y1) {
        std::swap(x0, x1);
        std::swap(y0, y1);
    }

    int dx = x1 - x0;
    int dy = y1 - y0;

    int dir = (dx < 0) ? -1 : 1;
    dx *= dir;

    if (dy != 0) {
        int x = x0;
        int p = 2 * dx - dy;

        for (int i = 0; i <= dy; i++) {
            //putPixel(x, y0 + i);			
        	sf::Vertex point(sf::Vector2f(x, y0 + i), sf::Color::White);
        	Game::me->win->draw(&point, 1, sf::Points);
            if (p >= 0) {
                x += dir;
                p -= 2 * dy;
            }
            p += 2 * dx;
        }
    }
}

void Player::drawLine(int x0, int y0, int x1, int y1)
{
	if (abs(x1 - x0) > abs(y1 - y0)) drawLineH(x0, y0, x1, y1);
	else drawLineV(x0, y0, x1, y1);
}

void Player::takeDamage(int damage)
{
	life -= damage;
	if (life <= 0)
	{
		spritePlayer.isHurting = false;
		spritePlayer.playAnimationSprite(0, 9);
	}
	else
	{
		spritePlayer.playAnimationSprite(0, 7);
	}
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
	Value("life", life);
	Value("animation Row", spritePlayer.animationRow);
	Value("currentFrame", spritePlayer.currentFrame);
	Value("jumping", jumping);
	Value("firing", firing);
	Value("reloading", reloading);
	Value("cx", cx);
	ImGui::SameLine();
	Value("cy", cy);

	Value("rx",rx);
	ImGui::SameLine();
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

	chg |= DragFloat("shake amplitude", &cameraShakeAmplitude, 0.001f, -2, 2);
	chg |= DragFloat("shake frequency", &cameraShakeFrequency, 0.001f, -2, 2);
	chg |= DragFloat("shake duration", &cameraShakeDuration, 0.001f, 0, 2);

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



