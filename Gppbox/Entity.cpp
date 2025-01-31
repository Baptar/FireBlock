#include <imgui.h>

#include "Entity.hpp"
#include "C.hpp"
#include "Game.hpp"

Entity::Entity(sf::RectangleShape* _spr) : spr(_spr), width(_spr->getSize().x / C::GRID_SIZE), height(_spr->getSize().y / C::GRID_SIZE) {

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
	
	// Collision Left
	do
	{
		if (g.hasCollision(cx - 1 , cy, width, height) && rx < 0.5)
		{
			dx = 0;
			rx = 0.5f;
		}
		else if (rx < 0)
		{
			moveRight = false;
			rx++;
			cx--;
		}
	} while (rx < 0);

	// Collision Right
	do
	{
		if (g.hasCollision(cx + 1, cy, width, height) && rx > 0.5)
		{
			dx = 0;
			rx = 0.5f;
		}
		else if (rx > 1)
		{
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
			if (g.hasCollision(cx, cy  - 1, width, height) && ry <= 0.01f)
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
			if (g.hasCollision(cx, cy + 1, width, height) && ry >= 0.99f)
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
		if (!g.hasCollision(cx , cy + 1, width, height))
		{
			setJumping(true);
		}
	}

	if (waitToUncrouch && !g.hasCollision(cx, cy  - 1, width, height * 2))
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
	sf::Vector2f pos = { (cx + rx) * C::GRID_SIZE, (cy + ry) * C::GRID_SIZE };
	spr->setPosition(pos);
}

void Entity::draw(sf::RenderWindow& win){
	if (spr)
		win.draw(*spr);
}

bool Entity::im()
{
	using namespace ImGui;

	bool chg = false;
	
	Value("jumping", jumping);
	Value("crouching", crouching);
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
	if (jumping && onOff) 
		return;

	if (onOff) {
		gravy = 80;
		jumping = true;
	}
	else {
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
	if ((*Game::me).hasCollision(cx, cy  - 1, width, height * 2))
	{
		waitToUncrouch = true;
		return;
	}
	
	crouching = false;
	height *= 2;
	spr->setSize(Vector2f(C::GRID_SIZE, C::GRID_SIZE * 2) );
	spr->setOrigin({ C::GRID_SIZE * 0.5f, C::GRID_SIZE * 2});
	
}

void Entity::setCrouch(bool onOff)
{
	if (crouching == onOff) return;
	if (onOff) { crouch();}
	else { unCrouch();}
}


sf::Vector2i Entity::getPosPixel()
{
	return sf::Vector2i( (cx+rx)*C::GRID_SIZE, (cy+ry) * C::GRID_SIZE );
}


