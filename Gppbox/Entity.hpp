#pragma once

#include <SFML/Graphics.hpp>

namespace sf {
	class RenderWindow;
}

class Entity{
public:
	sf::RectangleShape * spr = 0;
	int			cx = 0;
	int			cy = 0;
	float		rx = 0.5f;
	float		ry = 0.0f;
	
	float		dx = 0.f;
	float		dy = 0.f;
	float		gravy = 0.f;

	float		height = 0.f;
	float		width = 0.f;

	float		frx = 0.88f;
	float		fry = 1.0f;
	bool		jumping = false;
	bool		crouching = false;
	bool		waitToUncrouch = false;
	bool		moveRight = true;
	sf::Vector2f velocityPlayer;
	sf::Vector2f accelerationPlayer;
	sf::Vector2f previousPositionPlayer;
	sf::Vector2f previousVelocityPlayer;

					Entity(sf::RectangleShape* spr);

	void			update( double dt );
	void			setCooPixel( int px, int py);
	void			setCooGrid( float coox, float cooy);
	void			syncPos();
	void			draw(sf::RenderWindow& win);
	bool			im();

	sf::Vector2i	getPosPixel();

	void			setJumping(bool onOff);
	void			crouch();
	void			unCrouch();
	void			setCrouch(bool onOff);

	void			setPlayerVelocity(double dt);
	void			setPlayerAcceleration(double dt);
	
};