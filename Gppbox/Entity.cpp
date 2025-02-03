#include <imgui.h>

#include "Entity.hpp"

#include <SFML/OpenGL.hpp>

#include "C.hpp"
#include "Game.hpp"

Entity::Entity(sf::RectangleShape* _spr) : spr(_spr), width(_spr->getSize().x / C::GRID_SIZE), height(_spr->getSize().y / C::GRID_SIZE) {
	if (!textureIdle.loadFromFile("res/Idle.png")) printf("ERR : LOAD FAILED\n");
	if (!textureWalk.loadFromFile("res/Walk.png")) printf("ERR : LOAD FAILED\n");
	if (!textureRun.loadFromFile("res/Run.png")) printf("ERR : LOAD FAILED\n");
	if (!textureJump0.loadFromFile("res/Jump0.png")) printf("ERR : LOAD FAILED\n");
	if (!textureJump1.loadFromFile("res/Jump1.png")) printf("ERR : LOAD FAILED\n");
	if (!textureJump2.loadFromFile("res/Jump2.png")) printf("ERR : LOAD FAILED\n");
	if (!textureHurt.loadFromFile("res/Hurt.png")) printf("ERR : LOAD FAILED\n");
	if (!textureFire.loadFromFile("res/Shot.png")) printf("ERR : LOAD FAILED\n");
	if (!textureReload.loadFromFile("res/Reload.png")) printf("ERR : LOAD FAILED\n");
	if (!textureDeath.loadFromFile("res/Dead.png")) printf("ERR : LOAD FAILED\n");
	if (!textureRunShot.loadFromFile("res/Run_Shot.png")) printf("ERR : LOAD FAILED\n");
	
	sprite.setTexture(textureIdle);
	sprite.setTextureRect(sf::IntRect(0, 0, 128, 128));
	sprite.setScale(sf::Vector2f(.5f, .5f));
	sprite.setOrigin(128 / 2.0f, 128);
}

void Entity::update(double dt){
	Game& g = *Game::me;
	double rate = 1.0 / dt;
	double dfr = 60.0f / rate;

	dy += gravy * dt;
	dy = dy * pow(fry, dfr);
	dx = dx * pow(frx, dfr);
	
	rx += dx * dt;
	ry += dy * dt;
		
	// Check not moving
	if (dx <= 2.0f && dx >= -2.0f && dy<= 2.0f && (animationRow == 1 || animationRow == 2 || animationRow == 10) && !jumping)
	{
		if (firing && animationRow!=6) setAnimationFrame(0, 6);
		else if (!firing && animationRow != 0) setAnimationFrame(0, 0);
	}
	
	animationTime += dt;
	if (animationTime >= frameSpeed) {
		animationTime = 0;
		currentFrame = (currentFrame + 1) % numberOfFrame;
		
		// if we start jumping
		if (animationRow == 3 && currentFrame >= 5)
		{
			setAnimationFrame(0, 4);
		}
		
		// reloading
		else if (animationRow == 8 && currentFrame == 16)
		{
			//Finished to reload
			reloading = false;
			setAnimationFrame(0, 0);
		}
		
		else
		{
			setAnimationFrame(currentFrame, animationRow);
		}
	}
	
	// Collision Left
	do
	{
		if (g.hasCollision(cx - 1 , cy) && rx < 0.5)
		{
			if (animationRow != 0 && !jumping && !firing && !reloading) setAnimationFrame(0, 0);
			dx = 0;
			rx = 0.5f;
		}
		else if (rx < 0)
		{
			if (!jumping && !reloading)
			{
				if (firing && animationRow!=10) setAnimationFrame(currentFrame+1, 10);
				else if (!firing && animationRow != 2) setAnimationFrame(currentFrame+1, 2);
			}
			moveRight = false;
			rx++;
			cx--;
		}
	} while (rx < 0);

	// Collision Right
	do
	{
		if (g.hasCollision(cx + 1, cy) && rx > 0.5)
		{
			if (animationRow != 0 && !jumping && !firing && !reloading) setAnimationFrame(0, 0);
			dx = 0;
			rx = 0.5f;
		}
		else if (rx > 1)
		{
			if (!jumping && !reloading)
			{
				if (firing && animationRow!=10) setAnimationFrame(currentFrame+1, 10);
				else if (!firing && animationRow != 2) setAnimationFrame(currentFrame+1, 2);
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
			if (g.hasCollision(cx, cy  - 2) && ry <= 0.01f)
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
			if (g.hasCollision(cx, cy + 1) && ry >= 0.99f)
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
		if (!g.hasCollision(cx , cy + 1))
		{
			setJumping(true);
		}
	}

	if (waitToUncrouch && !g.hasCollision(cx, cy  - 1))
	{
		unCrouch();
		waitToUncrouch = false;
	}
	
	syncPos();
}

void Entity::setCooPixel(int px, int py){
	cx = px / C::GRID_SIZE;
	cy = py / C::GRID_SIZE;

	rx = (px - (cx * C::GRID_SIZE)) / (float)C::GRID_SIZE;
	ry = (py - (cy * C::GRID_SIZE)) / (float)C::GRID_SIZE;

	syncPos();
}

void Entity::setCooGrid(float coox, float cooy){
	cx = (int)coox;
	rx = coox - cx;

	cy = (int)cooy;
	ry = cooy - cy;
	syncPos();
}

void Entity::syncPos() {
	sprite.setPosition((cx + rx) * C::GRID_SIZE, (cy + ry) * C::GRID_SIZE);
	if (spr)
	{
		sf::Vector2f pos = { (cx + rx) * C::GRID_SIZE, (cy + ry) * C::GRID_SIZE };
		spr->setPosition(pos);
	}
}

void Entity::draw(sf::RenderWindow& win){
	if (spr) win.draw(*spr);
	win.draw(sprite);
}

bool Entity::im()
{
	using namespace ImGui;

	bool chg = false;
	
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
	if (Button("save")) {
		(*Game::me).saveData(cx, cy);
	}
	if (Button("load")) {
		(*Game::me).loadData();
	}
	return chg||chgCoo;
}

void Entity::setJumping(bool onOff){
	if (jumping && onOff || reloading) 
		return;

	if (onOff) {
		stopFire();
		if (animationRow != 3) setAnimationFrame(0, 3);
		gravy = 80;
		jumping = true;
	}
	else {
		if (animationRow != 0) setAnimationFrame(0, 0);
		gravy = 0;
		jumping = false;
	}
}

void Entity::crouch()
{
	if (crouching) return;
	
	crouching = true;
	height *= 0.5;
	spr->setSize({C::GRID_SIZE, C::GRID_SIZE});
	spr->setOrigin({ C::GRID_SIZE * 0.5f, C::GRID_SIZE});
}

void Entity::unCrouch()
{
	if ((*Game::me).hasCollision(cx, cy  - 1))
	{
		waitToUncrouch = true;
		return;
	}

	if (animationRow != 0) setAnimationFrame(0, 0);
	crouching = false;
	height *= 2;
	spr->setSize(Vector2f(C::GRID_SIZE, C::GRID_SIZE * 2) );
	spr->setOrigin({ C::GRID_SIZE * 0.5f, C::GRID_SIZE * 2});
	
}

void Entity::setCrouch(bool onOff)
{
	if (crouching == onOff) return;
	if (onOff)
	{
		crouch();
		if (animationRow != 3) setAnimationFrame(0, 3);
	}
	else
	{
		unCrouch();
		if (animationRow != 0) setAnimationFrame(0, 0);
	}
}


sf::Vector2i Entity::getPosPixel()
{
	return sf::Vector2i( (cx+rx)*C::GRID_SIZE, (cy+ry) * C::GRID_SIZE );
}

void Entity::setAnimationFrame(int _frame, int _animationRow)
{
	animationTime = 0.0f;
	this->animationRow = _animationRow;
	float scaleAbs = abs(sprite.getScale().x);
	sprite.setScale((moveRight ? scaleAbs : -scaleAbs) , scaleAbs);
	switch (_animationRow)
	{
		case 0:
			sprite.setTexture(textureIdle);
			numberOfFrame = 6;
			break;
		case 1:
			sprite.setTexture(textureWalk);
			numberOfFrame = 10;
			break;
		case 2:
			sprite.setTexture(textureRun);
			numberOfFrame = 10;
			break;
		case 3:
			sprite.setTexture(textureJump0);
			numberOfFrame = 6;
			break;
		case 4:
			sprite.setTexture(textureJump1);
			numberOfFrame = 1;
			break;
		case 5:
			sprite.setTexture(textureJump2);
			numberOfFrame = 4;
			break;
		case 6:
			sprite.setTexture(textureFire);
			numberOfFrame = 4;
			break;
		case 7:
			sprite.setTexture(textureHurt);
			numberOfFrame = 5;
			break;
		case 8:
			sprite.setTexture(textureReload);
			numberOfFrame = 17;
			break;
		case 9:
			sprite.setTexture(textureDeath);
			numberOfFrame = 5;
			break;
		case 10:
			sprite.setTexture(textureRunShot);
			numberOfFrame = 10;
			break;
		default:
			sprite.setTexture(textureIdle);
			numberOfFrame = 6;
			break;
	}
	sprite.setTextureRect(sf::IntRect(_frame * 128, 0, 128, 128));
}

void Entity::stopFire()
{
	if (!firing) return;
	firing = false;
	if (dx > 2.0f || dx < -2.0f) setAnimationFrame(currentFrame + 1, 2);
	else setAnimationFrame(0, 0);
}

void Entity::fire()
{
	if (firing || reloading || jumping) return;
	
	// Start Fire System
	firing = true;
	if (dx > 2.0f || dx < -2.0f) setAnimationFrame(currentFrame + 1, 10);
	else setAnimationFrame(0, 6);
	printf("fire\n");
}

void Entity::reload()
{
	if (reloading || jumping) return;

	stopFire();
	reloading = true;
	setAnimationFrame(0, 8);
}




