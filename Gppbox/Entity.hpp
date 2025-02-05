#pragma once

#include <SFML/Graphics.hpp>

namespace sf {
	class RenderWindow;
}

class Entity{
public:
	sf::Sprite sprite;
	sf::Texture textureIdle;
	sf::Texture textureWalk;
	sf::Texture textureRun;
	sf::Texture textureJump0;
	sf::Texture textureJump1;
	sf::Texture textureJump2;
	sf::Texture textureHurt;
	sf::Texture textureFire;
	sf::Texture textureReload;
	sf::Texture textureDeath;
	sf::Texture textureRunShot;
	
	
	sf::RectangleShape * spr = 0;

	int			numberOfFrame = 6;
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
	float		fry = 0.95f;

	int			currentFrame = 0;   
	float		animationTime = 0;  
	float		frameSpeed = 0.1f;
	int			animationRow = 0; // 0 = idle, 1 = walk, 2 = run, 3 = jump

	bool		reloading = false;
	bool		firing = false;
	bool		jumping = false;
	bool		crouching = false;
	bool		waitToUncrouch = false;
	bool		moveRight = true;

					Entity(sf::RectangleShape* spr);

	void			update( double dt );
	void			setCooPixel( int px, int py);
	void			setCooGrid( float coox, float cooy);
	void			syncPos();
	void			draw(sf::RenderWindow& win);
	bool			im();

	sf::Vector2i	getPosPixel();
	void			setAnimationFrame(int frame, int animationRow);
	
	void			setJumping(bool onOff);
	void			crouch();
	void			unCrouch();
	void			setCrouch(bool onOff);
	void 			stopFire();
	void			fire();
	void			reload();
	
};