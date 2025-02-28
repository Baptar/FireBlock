#include <imgui.h>

#include "Player.hpp"

#include "Bullet.hpp"
#include "C.hpp"
#include "Game.hpp"

Player::Player(): spritePlayer(SpritePlayer(*this)){}

void Player::update(double dt){
	// Manage destroy bullets
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
	// Manage destroy missiles
	for (auto it = missiles.begin(); it != missiles.end(); ) {
		HomingMissile* missile = *it;
		if (missile == nullptr) it = missiles.erase(it);
		else if (missile->shouldDestroy) {
			delete missile;
			it = missiles.erase(it);
		} else {
			missile->update(dt);
			++it;
		}
	}
	
	if (isDead)
	{
		spritePlayer.setAnimationFrame(4,9);
		return;
	}

	if (!firingLaser)
	{
		if (delayStartRecupLaser > 0.0f) delayStartRecupLaser-= dt;
		else currentTimeLaser = min(float(currentTimeLaser + dt), maxTimeLaser);
	}
	
	spritePlayer.update(dt);
	
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
			if (!reloading)	setJumping(true);
			else
			{
				gravy = 110;
				jumping = true;
			}
		}
	}
	syncPos();
	
	for (auto b : bullets)
		b->update(dt);
	for (auto b : missiles)
		b->update(dt);
}

sf::Vector2i Player::getPosPixel() const{ return sf::Vector2i( (cx+rx)*C::GRID_SIZE, (cy+ry) * C::GRID_SIZE ); }

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

void Player::syncPos()
{
	spritePlayer.getSprite().setPosition((cx + rx) * C::GRID_SIZE, (cy + ry) * C::GRID_SIZE);
}

void Player::draw(sf::RenderWindow& win){
	win.draw(spritePlayer.getSprite());
	
	for (Bullet* b : bullets)
		b->draw();

	for (HomingMissile* b : missiles)
		b->draw();
	
	for (sf::RectangleShape* r : laserSprites) 
		win.draw(*r);
}

void Player::setJumping(bool onOff){
	if (jumping && onOff || reloading) 
		return;
	
	if (onOff) {
		StopFireLaser();
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

void Player::reload()
{
	if (reloading || jumping) return;

	StopFireLaser();
	stopFire();
	spritePlayer.playAnimationSprite(0, 8);
}

void Player::takeDamage(int damage)
{
	life -= damage;
	if (life <= 0 && !spritePlayer.isDieing)
	{
		//spritePlayer.isDieing = true;
		spritePlayer.isHurting = false;
		spritePlayer.playAnimationSprite(0, 9);
	}
	else
	{
		if (!reloading && !firing) spritePlayer.playAnimationSprite(0, 7);
		else
		{
			// spritePlayer.isHurting = true;
			spritePlayer.sprite.setColor(sf::Color::Red);
			spritePlayer.durationDamage = 0.1f;
		}
	}
}

#pragma region FireNormal
void Player::fire()
{
	if (reloading || jumping || delayFire > 0) return;
	if (actualBullets <= 0)
	{
		stopFire();
		return;
	}
	delayFire = 0.1f;
	Game::me->camera.addShake(cameraShakeAmplitude, cameraShakeFrequency, cameraShakeDuration);
	
	// Start Fire System
	actualBullets--;
	dx += moveRight ? -reculPower : reculPower;
	firing = true;
	if (dx > 2.0f || dx < -2.0f) spritePlayer.playAnimationSprite(spritePlayer.currentFrame + 1, 10);
	else spritePlayer.playAnimationSprite(0, 6);
	
	bullets.push_back(new Bullet({cx, cy}, moveRight));
}

void Player::stopFire()
{
	if (!firing) return;
	firing = false;
	if (dx > 2.0f || dx < -2.0f) spritePlayer.playAnimationSprite(spritePlayer.currentFrame + 1, 2);
	else spritePlayer.playAnimationSprite(0, 0);
}
#pragma endregion FireNormal

#pragma region Laser
void Player::fireLaser(double dt)
{
	if (currentTimeLaser <= 0.0f)
	{
		if (firingLaser) StopFireLaser();
		return;
	}
	
	Game& g = *Game::me;

	//firingLaser = true;

	currentTimeLaser -= dt;
	delayStartRecupLaser = 1.5f;
	for(auto laser : laserSprites)
		delete laser;
	laserSprites.clear();

	int x0 = moveRight ? (cx+rx+0.8f) * C::GRID_SIZE : (cx+rx-0.8f) * C::GRID_SIZE;
	int y0 = (cy+ry - 1.15) * C::GRID_SIZE;

	int laserLength = laserRange;
	for(int i=0; i<laserRange; i++)
	{
		int j = moveRight ? i : -i;
		if(Ennemy* ennemy  = g.getEnnemyAtPosition(cx + j, cy - 1))
		{
			ennemy->takeDamage(0.05f, moveRight);

			laserLength = i - 1;
			break;
		}
		if (g.hasWall(cx + j, cy - 1))
		{
			laserLength = i - 1;
			break;
		}
	}

	int laserRangePixel = laserLength * C::GRID_SIZE;
	if(!moveRight)
		laserRangePixel *= -1; 

	drawLaser(x0, y0, x0 + laserRangePixel, y0);
	
	// recul
	dx += moveRight ? -reculPower / 100.0f : reculPower/ 100.0f;
	
	firingLaser = true;
}

void Player::StopFireLaser()
{
	for(auto laser : laserSprites)
		delete laser;
	laserSprites.clear();

	firingLaser = false;
}


void Player::drawLaser(int x0, int y0, int x1, int y1)
{
	int dx = x1 - x0;
	int dy = y1 - y0;
	if(dx != 0)
	{
		int D = 2 * dy - abs(dx);
		int y = y0;
		for(int i=0; dx < 0 ? i>dx : i<dx; dx < 0 ? i-- : i++)
		{
			createLaserPixel(x0 + i, y);
			if(D > 0)
			{ 
				y++;
				D -= 2 * abs(dx);
			}
			D += 2 * dy;
		}
	}
}

void Player::createLaserPixel(int x, int y)
{
	auto laserPixel = new sf::RectangleShape({1.0f, laserPixelSize});
	laserPixel->setOrigin(0.5f, laserPixelSize * 0.5f);
	laserPixel->setPosition(x, y);
	laserPixel->setFillColor(sf::Color::Red);
	laserSprites.push_back(laserPixel);
}
#pragma endregion Laser

#pragma region Missile
void Player::fireMissile()
{
	if (missileNumber > 0) missileNumber--;
	else return;
	
	missiles.push_back(new HomingMissile({static_cast<float>(cx), static_cast<float>(cy)}, moveRight));
}
#pragma endregion Missile

#pragma region Debug
bool Player::im()
{
	using namespace ImGui;

	bool chg = false;
	if (TreeNodeEx("Bullets", 0)) {
		for (auto b : bullets)
			b->im();
		TreePop();
	}
	if (TreeNodeEx("Missiles", 0)) {
		for (auto b : missiles)
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
		life = 10;
		isDead = false;
		actualBullets = maxBullets;
		spritePlayer.playAnimationSprite(0, 0);
		cx = 3;
		cy = 54;
		rx = 0.5f;
		ry = 0.99f;

		dx = dy = 0;
		setJumping(false);
	}
	return chg||chgCoo;
}
#pragma endregion Debug



