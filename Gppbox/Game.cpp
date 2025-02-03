
#include <imgui.h>
#include <vector>

#include "C.hpp"
#include "Game.hpp"

#include <fstream>
#include <iostream>

#include "Entity.hpp"

#include "HotReloadShader.hpp"

Game*		Game::me = 0;
static int	cols = C::RES_X / C::GRID_SIZE;
static int	lastLine = C::RES_Y / C::GRID_SIZE - 1;

struct Data {
	char name[50];
	Vector2i position;
};

Game::Game(sf::RenderWindow * win) {
	this->win = win;
	me = this;
	bg = sf::RectangleShape(Vector2f((float)win->getSize().x, (float)win->getSize().y));
	bg2= sf::RectangleShape(Vector2f((float)win->getSize().x, (float)win->getSize().y));

	bool isOk = tex.loadFromFile("res/night.png");
	if (!isOk) {
		printf("ERR : LOAD FAILED\n");
	}
	bg.setTexture(&tex);
	bg.setSize(sf::Vector2f(C::RES_X, C::RES_Y));

	bg2.setTexture(&tex);
	bg2.setSize(sf::Vector2f(C::RES_X, C::RES_Y));

	bgShader = new HotReloadShader("res/bg.vert", "res/bg.frag");
	
	for (int i = 0; i < C::RES_X / C::GRID_SIZE; ++i) 
		walls.push_back( Vector2i(i, lastLine) );

	// Left Wall
	walls.push_back(Vector2i(0, lastLine-1));
	walls.push_back(Vector2i(0, lastLine-2));
	walls.push_back(Vector2i(0, lastLine-3));

	// Right Wall
	walls.push_back(Vector2i(cols-1, lastLine - 1));
	walls.push_back(Vector2i(cols-1, lastLine - 2));
	walls.push_back(Vector2i(cols-1, lastLine - 3));

	//walls.push_back(Vector2i(cols >>2, lastLine - 1));
	//walls.push_back(Vector2i(cols >>2, lastLine - 2));
	walls.push_back(Vector2i(cols >>2, lastLine - 3));
	walls.push_back(Vector2i(cols >>2, lastLine - 4));
	walls.push_back(Vector2i((cols >> 2) + 1, lastLine - 4));
	cacheWalls();

	initMainChar();
}

void Game::initMainChar(){	
	auto spr = new sf::RectangleShape({ C::GRID_SIZE, C::GRID_SIZE * 2});
	spr->setFillColor(sf::Color::Transparent);
	//spr->setOutlineColor(sf::Color::Red);
	//spr->setOutlineThickness(2);
	spr->setOrigin({ C::GRID_SIZE * 0.5f, C::GRID_SIZE * 2 });
	auto e = new Entity(spr);
	
	//auto e = new Entity();
	e->setCooGrid(3, int(C::RES_Y / C::GRID_SIZE) - 10);
	e->ry = 0.99f;
	e->syncPos();
	ents.push_back(e);
	printf("ent added");
}

void Game::cacheWalls(){
	wallSprites.clear();
	for (Vector2i & w : walls) {
		sf::RectangleShape rect(Vector2f(16,16));
		rect.setPosition((float)w.x * C::GRID_SIZE, (float)w.y * C::GRID_SIZE);
		rect.setFillColor(sf::Color(0x07ff07ff));
		wallSprites.push_back(rect);
	}
}

void Game::processInput(sf::Event ev) {
	if (ev.type == sf::Event::Closed) {
		win->close();
		closing = true;
		return;
	}
	
	if (ev.type == sf::Event::KeyReleased) {
		int here = 0;
		if (ev.key.code == Keyboard::K) {
			int there = 0;
			walls.clear();
			cacheWalls();
		}

		/*if (ev.key.code == sf::Keyboard::LControl)
		{
			auto mainChar = ents[0];
			if (mainChar) {
				mainChar->unCrouch();
			}
		}*/

		if (ev.key.code == Keyboard::R) {
			getPlayer().reload();
		}
		
		if (ev.key.code == sf::Keyboard::Space)
		{
			getPlayer().gravy = 120.0f;
		}
	}
	if (ev.type == sf::Event::JoystickButtonReleased)
	{
		if (ev.joystickButton.button == 0) //bottom
		{
			auto mainChar = ents[0];
			if (mainChar && !mainChar->jumping && !mainChar->reloading) {
				mainChar->setJumping(true);
			}
		}
		if (ev.joystickButton.button == 1) // right
		{
		}
		if (ev.joystickButton.button == 2) // left
		{
		}
		if (ev.joystickButton.button == 3) // top
		{
		}
		if (ev.joystickButton.button == 5) // R1
		{
			// fire
		}
	}
	if (ev.type == sf::Event::MouseButtonReleased)
	{
		if (ev.mouseButton.button == sf::Mouse::Left
		/*&& ImGui::IsWindowHovered()*/)
		{
			printf("mouse left\n");
			Vector2i mousePositionWindow = sf::Mouse::getPosition(*win);
			sf::Vector2f mousePosWorld = win->mapPixelToCoords(mousePositionWindow, cameraView);
			//walls.push_back(Vector2i(mousePosWorld.x/ C::GRID_SIZE, mousePosWorld.y/ C::GRID_SIZE));

			removeWallAtPosition((int)(mousePosWorld.x/ C::GRID_SIZE), (int)(mousePosWorld.y/ C::GRID_SIZE));
			cacheWalls();
		}
		if (ev.mouseButton.button == sf::Mouse::Right)
		{
			printf("Stop Fire\n");
			// Stop Fire
			auto mainChar = ents[0];
			if (mainChar) {
				mainChar->stopFire();
			}
		}
	}
}

static double g_time = 0.0;
static double g_tickTimer = 0.0;


void Game::pollInput(double dt) {
	
	float lateralSpeed = 10.0;
	float maxSpeed = 40.0;

	// Move Left
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Q) || sf::Joystick::getAxisPosition(0, Joystick::X) < -90.0f) {
		if (ents.size()) {
			auto mainChar = ents[0];
			if (mainChar) {
				mainChar->dx = -lateralSpeed;
			}
		}
	}
	
	// Move Right
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D) || sf::Joystick::getAxisPosition(0, Joystick::X) > 90.0f) {
		if (ents.size()) {
			auto mainChar = ents[0];
			if (mainChar) {
				mainChar->dx = lateralSpeed;
			}
		}
	}
	
	// Jump
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space) || sf::Joystick::isButtonPressed(0, 0)) {
		if (ents.size()) {
			auto mainChar = ents[0];
			if (mainChar && !mainChar->jumping && !mainChar->reloading && !hasCollision(mainChar->cx, mainChar->cy - 1)) {
				mainChar->dy -= 40;
				mainChar->setJumping(true);
			}
		}
	}
	
	// Adjust power of Jump
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space)|| sf::Joystick::isButtonPressed(0, 0)) {
		if (!wasPressed) {
			onSpacePressed();
			wasPressed = true;
		}
	}
	else {
		wasPressed = false;
	}

	if (sf::Mouse::isButtonPressed(sf::Mouse::Right))
	{
		if (!getPlayer().jumping) getPlayer().fire();
	}
	
	// Crouch
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LControl)) {
		/*if (ents.size()) {
			auto mainChar = ents[0];
			if (mainChar) {
				mainChar->crouch();
			}
		}*/
	}
}

static sf::VertexArray va;
static RenderStates vaRs;
static std::vector<sf::RectangleShape> rects;

int blendModeIndex(sf::BlendMode bm) {
	if (bm == sf::BlendAlpha) return 0;
	if (bm == sf::BlendAdd) return 1;
	if (bm == sf::BlendNone) return 2;
	if (bm == sf::BlendMultiply) return 3;
	return 4;
};

void Game::update(double dt) {
	pollInput(dt);

	g_time += dt;
	bg.setOrigin(bg.getSize().x / 2, bg.getSize().y / 2);
	bg.setPosition(cameraView.getCenter());
	bg.setSize(Vector2f(C::RES_X * zoom, C::RES_Y * zoom));

	for (auto e : ents) 
		e->update(dt);

	if (bgShader) bgShader->update(dt);

	beforeParts.update(dt);
	afterParts.update(dt);
}

 void Game::draw(sf::RenderWindow & win) {
	if (closing) return;

	sf::RenderStates states = sf::RenderStates::Default;
	sf::Shader * sh = &bgShader->sh;
	states.blendMode = sf::BlendAdd;
	states.shader = sh;
	states.texture = &tex;
	sh->setUniform("texture", tex);
	//sh->setUniform("time", g_time);
	win.draw(bg, states);

	beforeParts.draw(win);

	for (sf::RectangleShape & r : wallSprites)
		win.draw(r);

	for (sf::RectangleShape& r : rects) 
		win.draw(r);
	
	for (auto e: ents)
		e->draw(win);

	afterParts.draw(win);
}

void Game::onSpacePressed() {
	printf("SPACE Start \n");
}

bool Game::hasCollision(int gx, int gy)
{
	// can't go left too much
	if (gx < 0)
		return true;

	// can't go right too much
	auto wallRightX = (C::RES_X / C::GRID_SIZE);
	if (gx >= wallRightX)
		return true;

	// check collision with wall
	return isWall(gx, gy);
}

bool Game::isWall(int cx, int cy){
	for (Vector2i & w : walls)
	{
		if (w.x == cx && (w.y == cy ||w.y == cy - 1)) return true;
	}
	return false;
}

void Game::removeWallAtPosition(int cx, int cy)
{
	bool hasWallHere = false;
	std::vector<sf::Vector2i> newWalls;
	for (Vector2i & w : walls)
	{
		if (cx == w.x && cy == w.y) hasWallHere = true;
		else newWalls.push_back(w);
	}
	if (!hasWallHere) walls.push_back(Vector2i(cx, cy));
	else {walls = newWalls;}
}

void Game::im(){
	using namespace ImGui;
	int hre = 0;

	if (TreeNodeEx("Walls", 0)) {
		for (auto& w : walls) {
			Value("x",w.x);
			Value("y",w.y);
		}
		TreePop();

	}
	if (TreeNodeEx("Entities", ImGuiTreeNodeFlags_DefaultOpen)) {
		for (auto e : ents)
			e->im();
		TreePop();
	}
	bool chg = false;
	chg |= DragFloat("zoom", &zoom, 0.01f, -20,20);
	chg |= DragFloat("f", &f, 0.01f, -20,20);
	chg |= DragFloat("z", &z, 0.01f, -20,20);
	chg |= DragFloat("r", &r, 0.01f, -20,20);
}

void Game::saveData(int cx, int cy)
{
	
}

void Game::loadData()
{
	
}

Entity& Game::getPlayer() const
{
	return *ents[0];
}






