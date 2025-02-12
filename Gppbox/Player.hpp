#pragma once

#include <list>
#include <SFML/Graphics.hpp>
#include "SpritePlayer.hpp"

class Bullet;

namespace sf {
	class RenderWindow;
}

class Player{
public:
	SpritePlayer spritePlayer;
	
	int			life = 10;
	bool		isDead = false;
	
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
	bool		firing = false;
	bool		jumping = false;
	bool		crouching = false;
	bool		moveRight = true;
	float		delayFire = 0.0f;

	float cameraShakeAmplitude = 1.2f;
	float cameraShakeFrequency = 0.2f;
	float cameraShakeDuration = 0.2f;

	std::list<Bullet*> bullets;

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
	void			reload();

	void			drawLineH(int x0, int y0, int x1, int y1);
	void			drawLineV(int x0, int y0, int x1, int y1);
	void 			drawLine(int x0, int y0, int x1, int y1);
	void			takeDamage(int damage);
};