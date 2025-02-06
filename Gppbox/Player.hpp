#pragma once

#include <queue>
#include <SFML/Graphics.hpp>

#include "Bullet.h"
#include "SpritePlayer.h"

namespace sf {
	class RenderWindow;
}

class Player{
public:
	SpritePlayer spritePlayer;
	int			cx = 0;
	int			cy = 0;
	float		rx = 0.5f;
	float		ry = 0.0f;
	
	float		dx = 0.f;
	float		dy = 0.f;
	float		gravy = 0.f;

	float		frx = 0.88f;
	float		fry = 0.95f;

	bool		reloading = false;
	bool		firing = false;
	bool		jumping = false;
	bool		crouching = false;
	bool		moveRight = true;

	std::vector<Bullet> bullets;

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
};