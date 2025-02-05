
#include <imgui.h>
#include <vector>

#include "C.hpp"
#include "Game.hpp"

#include <fstream>
#include <iostream>
#include <sstream>

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

	bool isOk = tex.loadFromFile("res/night.png");
	if (!isOk) {
		printf("ERR : LOAD FAILED\n");
	}
	if (!textureWall_x0Diag.loadFromFile("res/wall_x4Diag.png")) printf("ERR : LOAD FAILED\n");
	if (!textureWall_x02Diag.loadFromFile("res/wall_x0_2Diag.png")) printf("ERR : LOAD FAILED\n");
	if (!textureWall_x0.loadFromFile("res/wall_x0.png")) printf("ERR : LOAD FAILED\n");
	if (!textureWall_x1.loadFromFile("res/wall_x1.png")) printf("ERR : LOAD FAILED\n");
	if (!textureWall_x2.loadFromFile("res/wall_x2.png")) printf("ERR : LOAD FAILED\n");
	if (!textureWall_x2Mid.loadFromFile("res/wall_x2mid.png")) printf("ERR : LOAD FAILED\n");
	if (!textureWall_x3.loadFromFile("res/wall_x3.png")) printf("ERR : LOAD FAILED\n");
	if (!textureWall_x4.loadFromFile("res/wall_x4.png")) printf("ERR : LOAD FAILED\n");
	
	bg.setTexture(&tex);
	bg.setSize(sf::Vector2f(C::RES_X, C::RES_Y));

	bg2.setTexture(&tex);
	bg2.setSize(sf::Vector2f(C::RES_X, C::RES_Y));

	bgShader = new HotReloadShader("res/bg.vert", "res/bg.frag");
	
	for (int i = 0; i < C::RES_X / C::GRID_SIZE; ++i)
	{
		walls.push_back( Vector2i(i, lastLine) );
		walls.push_back( Vector2i(i, lastLine+1) );
	}

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
	/*bool collisionTop;
	bool collisionBottom;
	bool collisionLeft;
	bool collisionRight;
	bool collisionTopLeft;
	bool collisionTopRight;
	bool collisionBottomLeft;
	bool collisionBottomRight;*/
	for (Vector2i & w : walls) {
		sf::Sprite s;
		
		/*collisionTop = hasWall(w.x, w.y - 1);
		collisionBottom = hasWall(w.x, w.y + 1);
		collisionLeft = hasWall(w.x - 1, w.y);
		collisionRight = hasWall(w.x + 1, w.y);

		collisionTopLeft = hasWall(w.x -1 , w.y - 1);
		collisionTopRight = hasWall(w.x + 1, w.y - 1);
		collisionBottomLeft = hasWall(w.x - 1, w.y + 1);
		collisionBottomRight = hasWall(w.x + 1, w.y + 1);
		
		// x3
		if (!collisionTop && collisionBottom && !collisionLeft && !collisionRight)
		{
			s.setTexture(textureWall_x3);
			s.setPosition(float(w.x * C::GRID_SIZE), float(w.y * C::GRID_SIZE));
		}
		else if (!collisionTop && !collisionBottom && collisionLeft && !collisionRight)
		{
			s.setTexture(textureWall_x3);
			sf::FloatRect bounds = s.getLocalBounds();
			s.setOrigin(bounds.width / 2, bounds.height / 2);
			s.setPosition(w.x * C::GRID_SIZE + bounds.width / 2, w.y * C::GRID_SIZE + bounds.height / 2);
			s.setRotation(90);
		}
		else if (collisionTop && !collisionBottom && !collisionLeft && !collisionRight)
		{
			s.setTexture(textureWall_x3);
			sf::FloatRect bounds = s.getLocalBounds();
			s.setOrigin(bounds.width / 2, bounds.height / 2);
			s.setPosition(w.x * C::GRID_SIZE + bounds.width / 2, w.y * C::GRID_SIZE + bounds.height / 2);
			s.setRotation(180);
		}
		else if (!collisionTop && !collisionBottom && !collisionLeft && collisionRight)
		{
			s.setTexture(textureWall_x3);
			sf::FloatRect bounds = s.getLocalBounds();
			s.setOrigin(bounds.width / 2, bounds.height / 2);
			s.setPosition(w.x * C::GRID_SIZE + bounds.width / 2, w.y * C::GRID_SIZE + bounds.height / 2);
			s.setRotation(270);
		}
		
		// x2
		else if (!collisionTop && collisionBottom && !collisionLeft && collisionRight)
		{
			s.setTexture(textureWall_x2);
			s.setPosition(float(w.x * C::GRID_SIZE), float(w.y * C::GRID_SIZE));
		}
		else if (!collisionTop && collisionBottom && collisionLeft && !collisionRight)
		{
			s.setTexture(textureWall_x2);
			sf::FloatRect bounds = s.getLocalBounds();
			s.setOrigin(bounds.width / 2, bounds.height / 2);
			s.setPosition(w.x * C::GRID_SIZE + bounds.width / 2, w.y * C::GRID_SIZE + bounds.height / 2);
			s.setRotation(90);
		}
		else if (collisionTop && !collisionBottom && collisionLeft && !collisionRight)
		{
			s.setTexture(textureWall_x2);
			sf::FloatRect bounds = s.getLocalBounds();
			s.setOrigin(bounds.width / 2, bounds.height / 2);
			s.setPosition(w.x * C::GRID_SIZE + bounds.width / 2, w.y * C::GRID_SIZE + bounds.height / 2);
			s.setRotation(180);
		}
		else if (collisionTop && !collisionBottom && !collisionLeft && collisionRight)
		{
			s.setTexture(textureWall_x2);
			sf::FloatRect bounds = s.getLocalBounds();
			s.setOrigin(bounds.width / 2, bounds.height / 2);
			s.setPosition(w.x * C::GRID_SIZE + bounds.width / 2, w.y * C::GRID_SIZE + bounds.height / 2);
			s.setRotation(270);
		}
		// x2 Mid
		else if (!collisionTop && !collisionBottom && collisionLeft && collisionRight)
		{
			s.setTexture(textureWall_x2Mid);
			s.setPosition(float(w.x * C::GRID_SIZE), float(w.y * C::GRID_SIZE));
		}
		else if (collisionTop && collisionBottom && !collisionLeft && !collisionRight)
		{
			s.setTexture(textureWall_x2Mid);
			sf::FloatRect bounds = s.getLocalBounds();
			s.setOrigin(bounds.width / 2, bounds.height / 2);
			s.setPosition(w.x * C::GRID_SIZE + bounds.width / 2, w.y * C::GRID_SIZE + bounds.height / 2);
			s.setRotation(90);
		}
		// x1
		else if (!collisionTop && collisionBottom && collisionLeft && collisionRight)
		{
			s.setTexture(textureWall_x1);
			s.setPosition(float(w.x * C::GRID_SIZE), float(w.y * C::GRID_SIZE));
		}
		else if (collisionTop && collisionBottom && collisionLeft && !collisionRight)
		{
			s.setTexture(textureWall_x1);
			sf::FloatRect bounds = s.getLocalBounds();
			s.setOrigin(bounds.width / 2, bounds.height / 2);
			s.setPosition(w.x * C::GRID_SIZE + bounds.width / 2, w.y * C::GRID_SIZE + bounds.height / 2);
			s.setRotation(90);
		}
		else if (collisionTop && !collisionBottom && collisionLeft && collisionRight)
		{
			s.setTexture(textureWall_x1);
			sf::FloatRect bounds = s.getLocalBounds();
			s.setOrigin(bounds.width / 2, bounds.height / 2);
			s.setPosition(w.x * C::GRID_SIZE + bounds.width / 2, w.y * C::GRID_SIZE + bounds.height / 2);
			s.setRotation(180);
		}
		else if (collisionTop && collisionBottom && !collisionLeft && collisionRight)
		{
			s.setTexture(textureWall_x1);
			sf::FloatRect bounds = s.getLocalBounds();
			s.setOrigin(bounds.width / 2, bounds.height / 2);
			s.setPosition(w.x * C::GRID_SIZE + bounds.width / 2, w.y * C::GRID_SIZE + bounds.height / 2);
			s.setRotation(270);
		}
		// x0
		else if (collisionTop && collisionBottom && collisionLeft && collisionRight)
		{
			if (collisionTopLeft && collisionTopRight && collisionBottomRight && !collisionBottomLeft)
			{
				s.setTexture(textureWall_x0Diag);
				sf::FloatRect bounds = s.getLocalBounds();
				s.setOrigin(bounds.width / 2, bounds.height / 2);
				s.setPosition(w.x * C::GRID_SIZE + bounds.width / 2, w.y * C::GRID_SIZE + bounds.height / 2);
				s.setRotation(90);
			}
			else if (collisionTopLeft && collisionTopRight && !collisionBottomRight && collisionBottomLeft)
			{
				s.setTexture(textureWall_x0Diag);
				s.setPosition(float(w.x * C::GRID_SIZE), float(w.y * C::GRID_SIZE));
			}
			else if (collisionTopLeft && !collisionTopRight && collisionBottomRight && collisionBottomLeft)
			{
				s.setTexture(textureWall_x0Diag);
				sf::FloatRect bounds = s.getLocalBounds();
				s.setOrigin(bounds.width / 2, bounds.height / 2);
				s.setPosition(w.x * C::GRID_SIZE + bounds.width / 2, w.y * C::GRID_SIZE + bounds.height / 2);
				s.setRotation(270);
			}
			else if (!collisionTopLeft && collisionTopRight && collisionBottomRight && collisionBottomLeft)
			{
				s.setTexture(textureWall_x0Diag);
				sf::FloatRect bounds = s.getLocalBounds();
				s.setOrigin(bounds.width / 2, bounds.height / 2);
				s.setPosition(w.x * C::GRID_SIZE + bounds.width / 2, w.y * C::GRID_SIZE + bounds.height / 2);
				s.setRotation(180);
			}
			else if (!collisionTopLeft && collisionTopRight && !collisionBottomRight && collisionBottomLeft)
			{
				s.setTexture(textureWall_x02Diag);
				s.setPosition(float(w.x * C::GRID_SIZE), float(w.y * C::GRID_SIZE));
			}
			else if (collisionTopLeft && !collisionTopRight && collisionBottomRight && !collisionBottomLeft)
			{
				s.setTexture(textureWall_x02Diag);
				sf::FloatRect bounds = s.getLocalBounds();
				s.setOrigin(bounds.width / 2, bounds.height / 2);
				s.setPosition(w.x * C::GRID_SIZE + bounds.width / 2, w.y * C::GRID_SIZE + bounds.height / 2);
				s.setRotation(90);
			}
			else if (collisionTopLeft && !collisionTopRight && collisionBottomRight && !collisionBottomLeft)
			{
				s.setTexture(textureWall_x02Diag);
				sf::FloatRect bounds = s.getLocalBounds();
				s.setOrigin(bounds.width / 2, bounds.height / 2);
				s.setPosition(w.x * C::GRID_SIZE + bounds.width / 2, w.y * C::GRID_SIZE + bounds.height / 2);
				s.setRotation(90);
			}
			else
			{
				s.setTexture(textureWall_x0);
				s.setPosition(float(w.x * C::GRID_SIZE), float(w.y * C::GRID_SIZE));
			}
		}
		// x0
		else
		{
			s.setTexture(textureWall_x4);
			s.setPosition(float(w.x * C::GRID_SIZE), float(w.y * C::GRID_SIZE));
		}*/
		s.setTexture(textureWall_x4);
		s.setPosition(float(w.x * C::GRID_SIZE), float(w.y * C::GRID_SIZE));
		wallSprites.push_back(s);
	}
}

void Game::cacheEnnemies()
{
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
			//getPlayer().gravy = 120.0f;
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
			if (mainChar && !mainChar->jumping && !mainChar->reloading && !hasPlayerCollision(mainChar->cx, mainChar->cy - 1)) {
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
		if (isEditing && !ImGui::IsWindowHovered()
		&& !ImGui::IsAnyItemHovered()
		&& !ImGui::IsAnyItemActive()
		&& !ImGui::IsAnyItemFocused())
		{
			int posX = (posMouse.x  / C::GRID_SIZE);
			int posY = (posMouse.y / C::GRID_SIZE);
			if (hasWall(posX, posY))
			{
				removeWallAtPosition(posX, posY);
				cacheWalls();
			}
			else if (hasEnnemy(posX, posY))
			{
				//
				cacheEnnemies();
			}	
		}
		else if (!getPlayer().jumping) getPlayer().fire();
	}

	if (sf::Mouse::isButtonPressed(sf::Mouse::Left)
		&& !ImGui::IsWindowHovered()
		&& !ImGui::IsAnyItemHovered()
		&& !ImGui::IsAnyItemActive()
		&& !ImGui::IsAnyItemFocused())
	{
		if (isEditing)
		{
			//sf::Vector2f mousePosWorld = win->mapPixelToCoords(mousePositionWindow, cameraView);
			int posX = (posMouse.x  / C::GRID_SIZE);
			int posY = (posMouse.y / C::GRID_SIZE);
			if (!hasWall(posX, posY) && selectedElement == 0 && !hasPlayer(posX, posY))
			{
				addWall(posX, posY);
				cacheWalls();
			}
			else if (!hasEnnemy(posX, posY) && selectedElement == 1 && !hasPlayer(posX, posY))
			{
				addEnnemy(posX, posY);
				cacheEnnemies();
			}	
		}
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
	if (!isEditing)
	{
		bg.setOrigin(bg.getSize().x / 2, bg.getSize().y / 2);
		bg.setPosition(cameraView.getCenter());
		bg.setSize(Vector2f(C::RES_X * zoom, C::RES_Y * zoom));
	}
	else
	{
		bg.setOrigin(win->getDefaultView().getCenter());
		bg.setPosition(win->getDefaultView().getCenter());
		bg.setSize(Vector2f(win->getSize().x, win->getSize().y));
	}
	
	posMouse = sf::Mouse::getPosition(*win);
	
	if (isEditing)
	{
		
	}	
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
	/*for (sf::RectangleShape & r : wallSprites)
		win.draw(r);*/
	
	for (sf::Sprite& s : wallSprites)
	{
		win.draw(s);
	}
	
	for (sf::RectangleShape& r : rects) 
		win.draw(r);
	
	for (auto e: ents)
		e->draw(win);

	afterParts.draw(win);
}

void Game::onSpacePressed() {
	printf("SPACE Start \n");
}

bool Game::hasPlayerCollision(int cx, int cy)
{
	// can't go left too much
	if (cx < 0)
		return true;

	// can't go right too much
	auto wallRightX = (C::RES_X / C::GRID_SIZE);
	if (cx >= wallRightX)
		return true;

	// check collision with wall
	for (Vector2i & w : walls)
	{
		if (w.x == cx && (w.y == cy ||w.y == cy - 1)) return true;
	}
	return false;
}

bool Game::hasWall(int cx, int cy){
	for (Vector2i & w : walls)
	{
		if (w.x == cx && w.y == cy) return true;
	}
	return false;
}

bool Game::hasEnnemy(int cx, int cy)
{
	for (auto e : ennemies)
	{
		if (e->cx == cx && (e->cy == cy ||e->cy == cy - 1)) return true;
	}
	return false;
}

bool Game::hasPlayer(int cx, int cy)
{
	return getPlayer().cx == cx && (getPlayer().cy == cy || getPlayer().cy - 1 == cy);
}

void Game::addWall(int cx, int cy)
{
	walls.push_back(Vector2i(cx, cy));
}

void Game::addEnnemy(int cx, int cy)
{
	Ennemy* ennemy = new Ennemy(cx, cy);
	ennemies.push_back(ennemy);
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

	if (Button("save")) {
		saveData("save.sav");
	}
	ImGui::SameLine();
	if (Button("load")) {
		loadData("save.sav");
	}
	ImGui::SameLine();
	if (Checkbox("edit mode", &isEditing)) {

	}
	
	if (TreeNodeEx("Walls", 0)) {
		for (auto& w : walls) {
			Value("x",w.x);
			Value("y",w.y);
		}
		TreePop();

	}
	if (TreeNodeEx("Ennemies", ImGuiTreeNodeFlags_DefaultOpen)) {
		for (auto e : ennemies)
			e->im();
		TreePop();
	}
	if (TreeNodeEx("Player", ImGuiTreeNodeFlags_DefaultOpen)) {
		for (auto e : ents)
			e->im();
		TreePop();
	}
	if (TreeNodeEx("Camera", ImGuiTreeNodeFlags_DefaultOpen)) {
		bool chg = false;
		chg |= DragFloat("zoom", &zoom, 0.01f, -20,20);
		chg |= DragFloat("f", &f, 0.01f, -20,20);
		chg |= DragFloat("z", &z, 0.01f, -20,20);
		chg |= DragFloat("r", &r, 0.01f, -20,20);
		TreePop();
	}
}

bool Game::loadData(const std::filesystem::path& filePath)
{
	if (!std::filesystem::exists(filePath)) return false;

	std::ifstream in(filePath);

	walls.clear();
	for (auto e : ennemies)
		delete e;
	ennemies.clear();

	
	std::string line;
	while (std::getline(in, line))
	{
		std::istringstream iss(line);
		int x, y, type;
		if (!(iss >> x >> y >> type)) { break; } // error

		if (type == 0)
		{
			addWall(x, y);
		}

		if (type == 1)
		{
			addEnnemy(x, y);
		}
	}
	cacheWalls();
	cacheEnnemies();
	return true;
}

void Game::saveData(const std::filesystem::path& filePath) const
{
	std::ofstream out(filePath);

	for (auto wall : walls)
	{
		out << wall.x << " " << wall.y << " " << 0<<"\n";
	}
	for (auto entt : ennemies)
	{
		out << entt->cx << " " << entt->cy << " " << 1<<"\n";
	}
}


Entity& Game::getPlayer() const
{
	return *ents[0];
}






