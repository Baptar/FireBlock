#pragma once

#include <list>
#include <SFML/Graphics.hpp>

#include "HomingMissile.hpp"
#include "SpriteExplosion.h"
#include "SpritePlayer.hpp"

class Bullet;

namespace sf {
	class RenderWindow;
}

class Player{
public:
	SpritePlayer spritePlayer;

	int			missileNumber = 8;
	
	float		maxTimeLaser = 5.0f;
	float		currentTimeLaser = maxTimeLaser;
	float		delayStartRecupLaser = 3.0f;
	float		laserPixelSize = 2.0f;
	int			laserRange = 15;
	std::vector<sf::RectangleShape*> laserSprites;

	
	int			life = 10;
	bool		isDead = false;
	int			maxBullets = 64;
	int			actualBullets = maxBullets;

	float		cameraShakeAmplitude = 1.2f;
	float		cameraShakeFrequency = 0.2f;
	float		cameraShakeDuration = 0.2f;
	
	int			cx = 0;
	int			cy = 0;
	float		rx = 0.5f;
	float		ry = 0.0f;
	
	float		dx = 0.f;
	float		dy = 0.f;
	float		gravy = 0.f;

	float		reculPower = 0.5f;
	float		frx = 0.88f;
	float		fry = 0.95f;

	bool		reloading = false;
	bool		missileLunch = false;
	bool		firing = false;
	bool		firingLaser = false;
	bool		jumping = false;
	bool		crouching = false;
	bool		moveRight = true;
	float		delayFire = 0.0f;

	

	std::list<Bullet*> bullets;
	std::list<HomingMissile*> missiles;

					Player();
	void			update( double dt );
	void			setCooPixel( int px, int py);
	void			setCooGrid( float coox, float cooy);
	void			syncPos();
	void			draw(sf::RenderWindow& win);
	bool			im();
	sf::Vector2i	getPosPixel() const;
	
	void			setJumping(bool onOff);
	void 			stopFire();
	void			fire();
	void			fireMissile();
	void			reload();
	
	void			takeDamage(int damage);
	void			drawLaser(int x0, int y0, int x1, int y1);
	void			createLaserPixel(int x, int y);
	void			fireLaser(double dt);
	void			StopFireLaser();
};