
#include <imgui.h>
#include <vector>

#include "C.hpp"
#include "Game.hpp"

#include <fstream>
#include <imgui_internal.h>
#include <sstream>

#include "Player.hpp"

#include "HotReloadShader.hpp"

Game*		Game::me = 0;
static int	cols = C::RES_X / C::GRID_SIZE;
static int	lastLine = C::RES_Y / C::GRID_SIZE - 1;

Game::Game(sf::RenderWindow * _win): camera({C::RES_X / 2.f, C::RES_Y / 2.f}, {C::RES_X / 2.5f, C::RES_Y / 2.5f})
{
	this->win = _win;
	me = this;
	bg = sf::RectangleShape(Vector2f((float)_win->getSize().x, (float)_win->getSize().y));

	bool isOk = texBG.loadFromFile("res/night.png");
	if (!isOk) { printf("ERR : LOAD FAILED\n"); }
	bg.setTexture(&texBG);
	bg.setSize(sf::Vector2f(C::RES_X, C::RES_Y));
	if (!textureWall_x4.loadFromFile("res/wall_x4.png")) printf("ERR : LOAD FAILED\n");

	bgShader = new HotReloadShader("res/bg.vert", "res/bg.frag");

	if (!loadData("save.sav"))
	{
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

		// Middle Wall
		walls.push_back(Vector2i(cols >>2, lastLine - 3));
		walls.push_back(Vector2i(cols >>2, lastLine - 4));
		walls.push_back(Vector2i((cols >> 2) + 1, lastLine - 4));

		initMainChar(3, int(C::RES_Y / C::GRID_SIZE) - 10);
	}
	cacheWalls();
}

void Game::initMainChar(int cx, int cy){	
	{
		auto e = new Player();
		e->setCooGrid(cx, cy <= lastLine ? cy : 3);
		e->ry = 0.99f;
		e->syncPos();
		players.push_back(e);

		camera.setPlayer(e);
	}
}

void Game::cacheWalls(){
	wallSprites.clear();
	for (Vector2i & w : walls) {
		sf::Sprite s;
		s.setTexture(textureWall_x4);
		s.setPosition(float(w.x * C::GRID_SIZE), float(w.y * C::GRID_SIZE));
		wallSprites.push_back(s);
	}
}

void Game::drawLinesEdit()
{
	Color color = Color::White;
	color.a = 100;
	
	for (int i = 0; i < lastLine; i++)
	{
		sf::RectangleShape line(sf::Vector2f(C::RES_X, 0.6f));
		line.setPosition(0, i * C::GRID_SIZE);
		line.setFillColor(color);
		win->draw(line);
	}
	for (int j = 0; j < cols; j++)
	{
		sf::RectangleShape line(sf::Vector2f(0.6f, C::RES_Y));
		line.setPosition(j * C::GRID_SIZE, 0);
		line.setFillColor(color);
		win->draw(line);
	}
}

void Game::processInput(sf::Event _ev) {
	if (_ev.type == sf::Event::Closed) {
		win->close();
		closing = true;
		return;
	}
	
	if (_ev.type == sf::Event::KeyReleased) {
		if (_ev.key.code == Keyboard::R) {
			getPlayer().reload();
		}
		if (_ev.key.code == Keyboard::Q) {
			pressingLeft = false;
		}
		if (_ev.key.code == Keyboard::D) {
			pressingRight = false;
		}
		
		if (_ev.key.code == sf::Keyboard::Space)
		{
			canJumpInput = true;
			//getPlayer().gravy = 120.0f;
		}
	}
	if (_ev.type == sf::Event::JoystickButtonReleased)
	{
		if (_ev.joystickButton.button == 0) //bottom
		{
			auto mainChar = players[0];
			if (mainChar && !mainChar->jumping && !mainChar->reloading && canJumpInput) {
				canJumpInput = false;
				mainChar->setJumping(true);
				
			}
		}
		if (_ev.joystickButton.button == 1) // right
		{
		}
		if (_ev.joystickButton.button == 2) // left
		{
		}
		if (_ev.joystickButton.button == 3) // top
		{
		}
		if (_ev.joystickButton.button == 5) // R1
		{
			// fire
		}
	}
	if (_ev.type == sf::Event::MouseButtonReleased)
	{
		if (_ev.mouseButton.button == sf::Mouse::Right)
		{
			// Stop Fire
			auto mainChar = players[0];
			if (mainChar) {
				mainChar->stopFire();
			}
		}
	}
}

static double g_time = 0.0;
static double g_tickTimer = 0.0;


void Game::pollInput(double _dt) {
	
	float lateralSpeed = 10.0;
	float maxSpeed = 40.0;

	// Move Left
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Q) || sf::Joystick::getAxisPosition(0, Joystick::X) < -90.0f) {
		if (players.size()) {
			auto mainChar = players[0];
			if (mainChar && !mainChar->isDead) {
				mainChar->moveRight = false;
				pressingLeft = true;
				if (mainChar->reloading) mainChar->dx = -lateralSpeed / 2.0f;
				else mainChar->dx = -lateralSpeed;
			}
		}
	}
	
	// Move Right
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D) || sf::Joystick::getAxisPosition(0, Joystick::X) > 90.0f) {
		if (players.size()) {
			auto mainChar = players[0];
			if (mainChar && !mainChar->isDead) {
				mainChar->moveRight = true;
				pressingRight = true;
				if (mainChar->reloading) mainChar->dx = lateralSpeed / 2.0f;
				else mainChar->dx = lateralSpeed;
			}
		}
	}
	
	// Jump
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space) || sf::Joystick::isButtonPressed(0, 0)) {
		if (players.size()) {
			auto mainChar = players[0];
			if (mainChar && !mainChar->jumping && !mainChar->reloading && !hasPlayerCollision(mainChar->cx, mainChar->cy - 1) && canJumpInput && !mainChar->isDead) {
				canJumpInput = false;
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
			else 
			{
				auto e = getEnnemyAtPosition(posX, posY);
				if (e != nullptr)
				{
					for (auto it = ennemies.begin(); it != ennemies.end(); ) {
						Ennemy* enn = *it;
						if (enn == e) {
							delete enn;
							it = ennemies.erase(it);
						} else {
							++it;
						}
					}
				}
			}	
		}
		else if (!isEditing && !getPlayer().jumping && !getPlayer().isDead)
		{
			getPlayer().fire();
		}
	}
	if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
	{
		if (isEditing &&
			!ImGui::IsWindowHovered() &&
			!ImGui::IsAnyItemHovered() &&
			!ImGui::IsAnyItemActive() &&
			!ImGui::IsAnyItemFocused())
		{
			int posX = (posMouse.x  / C::GRID_SIZE);
			int posY = (posMouse.y / C::GRID_SIZE);
			if (!hasWall(posX, posY) && editingWalls && !hasPlayer(posX, posY) && !hasEnnemy(posX, posY))
			{
				addWall(posX, posY);
				cacheWalls();
			}
			else if (!hasWall(posX, posY) && !hasWall(posX, posY - 1) && !hasEnnemy(posX, posY) && editingEnemies && !hasPlayer(posX, posY))
			{
				addEnnemy(posX, posY);
			}	
		}
		else if (!isEditing && !getPlayer().jumping && !getPlayer().isDead) getPlayer().drawLine(getPlayer().cx, getPlayer().cy, getPlayer().cx + 10, getPlayer().cy);
	}
}

static sf::VertexArray va;
static RenderStates vaRs;
static std::vector<sf::RectangleShape> rects;

int blendModeIndex(sf::BlendMode _bm) {
	if (_bm == sf::BlendAlpha) return 0;
	if (_bm == sf::BlendAdd) return 1;
	if (_bm == sf::BlendNone) return 2;
	if (_bm == sf::BlendMultiply) return 3;
	return 4;
};

void Game::update(double _dt) {
	pollInput(_dt);

	g_time += _dt;
	if (bgShader) bgShader->update(_dt);
	beforeParts.update(_dt);
	
	if (!isEditing)
	{
		//bg.setOrigin(bg.getSize().x / 2, bg.getSize().y / 2);
		//bg.setPosition(cameraView.getCenter());
		//bg.setSize(Vector2f(C::RES_X * zoom, C::RES_Y * zoom));

		for (auto e : players) 
			e->update(_dt);

		for (auto e : ennemies)
			e->update(_dt);
	}
	else
	{
		//bg.setOrigin(win->getDefaultView().getCenter());
		//bg.setPosition(win->getDefaultView().getCenter());
		//bg.setSize(Vector2f(win->getSize().x, win->getSize().y));
	}
	afterParts.update(_dt);

	camera.update(_dt);
	posMouse = sf::Mouse::getPosition(*win);
}

 void Game::draw(sf::RenderWindow & _win) {
	if (closing) return;

	sf::RenderStates states = sf::RenderStates::Default;
	sf::Shader * sh = &bgShader->sh;
	states.blendMode = sf::BlendAdd;
	states.shader = sh;
	states.texture = &texBG;
	sh->setUniform("texture", texBG);
	//sh->setUniform("time", g_time);
	_win.draw(bg, states);

	View defaultView = _win.getView();

	if (!isEditing)
		camera.setActive(_win);
	
	beforeParts.draw(_win);
	
	for (sf::Sprite& s : wallSprites)
		_win.draw(s);
	
	
	for (sf::RectangleShape& r : rects) 
		_win.draw(r);

	if (isEditing) drawLinesEdit();

	for (auto e : ennemies)
	{
		//e->syncPos();
		e->draw(_win);
	}

	for (auto e: players)
		e->draw(_win);
		

	afterParts.draw(_win);
	_win.setView(defaultView);
}

void Game::onSpacePressed() {
}

bool Game::hasPlayerCollision(int _cx, int _cy)
{
	// can't go left too much
	if (_cx < 0)
		return true;

	// can't go right too much
	auto wallRightX = (C::RES_X / C::GRID_SIZE);
	if (_cx >= wallRightX)
		return true;

	// check collision with wall
	for (Vector2i & w : walls)
	{
		if (w.x == _cx && (w.y == _cy ||w.y == _cy - 1)) return true;
	}
	return false;
}

bool Game::hasWall(int _cx, int _cy){
	for (Vector2i & w : walls)
	{
		if (w.x == _cx && w.y == _cy) return true;
	}
	return false;
}

bool Game::hasEnnemy(int _cx, int _cy)
{
	for (auto e : ennemies)
	{
		if (e->isDead) continue;
		if (e->cx == _cx && (e->cy == _cy ||e->cy - 1 == _cy)) return true;
	}
	return false;
}

bool Game::hasPlayer(int cx, int cy)
{
	return getPlayer().cx == cx && (getPlayer().cy == cy || getPlayer().cy - 1 == cy);
}

void Game::addWall(int _cx, int _cy)
{
	walls.push_back(Vector2i(_cx, _cy));
}

void Game::addEnnemy(int _cx, int _cy)
{
	Ennemy* ennemy = new Ennemy(_cx, _cy);
	ennemy->ry = 0.99f;
	ennemy->rx = 0.5f;
	ennemy->syncPos();
	ennemies.push_back(ennemy);
}

void Game::removeWallAtPosition(int _cx, int _cy)
{
	bool hasWallHere = false;
	std::vector<sf::Vector2i> newWalls;
	for (Vector2i & w : walls)
	{
		if (_cx == w.x && _cy == w.y) hasWallHere = true;
		else newWalls.push_back(w);
	}
	if (!hasWallHere) walls.push_back(Vector2i(_cx, _cy));
	else {walls = newWalls;}
}

Ennemy* Game::getEnnemyAtPosition(int _cx, int _cy)
{
	for (auto e : ennemies)
	{
		if (e->isDead) continue;
		if (e->cx == _cx && (e->cy == _cy ||e->cy - 1 == _cy)) return e;
	}
	return nullptr;
}

void Game::saveData(const std::filesystem::path& _filePath) const
{
	std::ofstream out(_filePath);

	for (auto wall : walls)
	{
		out << wall.x << " " << wall.y << " " << 0<<"\n";
	}
	for (auto ennemy : ennemies)
	{
		if (!ennemy->isDead)
		out << ennemy->cx << " " << ennemy->cy << " " << 1<<"\n";
	}
	for (auto p : players)
	{
		out << p->cx << " " << p->cy << " " << 2<<"\n";
	}
}

bool Game::loadData(const std::filesystem::path& _filePath)
{
	if (!std::filesystem::exists(_filePath)) return false;

	std::ifstream in(_filePath);

	walls.clear();
	for (auto e : ennemies)
		delete e;
	ennemies.clear();

	for (auto p : players)
		delete p;
	players.clear();

	bool hasPlayer = false;
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
		
		if (type == 2)
		{
			initMainChar(x, y);
			hasPlayer = true;
		}
	}
	
	cacheWalls();
	return true;
}

Player& Game::getPlayer() const
{
	return *players[0];
}

void Game::im(){
	using namespace ImGui;
	int hre = 0;

	// save button
	if (Button("save")) {
		saveData("save.sav");
	}
	// load button
	ImGui::SameLine();
	if (Button("load")) {
		loadData("save.sav");
	}
	// edit checkbox
	Checkbox("edit mode", &isEditing);

	bool disableEnemies = !editingWalls;
	bool disableWalls = !editingEnemies;

	// Bouton "Ennemies"
	if (disableEnemies) {
		ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
		ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 0.5f);
	}

	ImGui::SameLine();
	if (ImGui::Button("Ennemies")) {
		editingEnemies = true;
		editingWalls = false;
	}

	if (disableEnemies) {  // Assure qu'on pop uniquement si on a push
		ImGui::PopItemFlag();
		ImGui::PopStyleVar();
	}

	// Bouton "Wall"
	if (disableWalls) {
		ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
		ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 0.5f);
	}

	ImGui::SameLine();
	if (ImGui::Button("Wall")) {
		editingWalls = true;
		editingEnemies = false;
	}

	if (disableWalls) {
		ImGui::PopItemFlag();
		ImGui::PopStyleVar();
	}
	
	if (TreeNodeEx("Walls", 0)) {
		for (auto& w : walls) {
			Value("x",w.x);
			ImGui::SameLine();
			Value("y",w.y);
		}
		TreePop();

	}
	if (TreeNodeEx("Ennemies", 0)) {
		for (auto e : ennemies)
			e->im();
		TreePop();
	}
	if (TreeNodeEx("Player",0)) {
		for (auto e : players)
			e->im();
		TreePop();
	}
	if (TreeNodeEx("Camera", 0)) {
		camera.im();
		TreePop();
	}
}


/*ImGuiTreeNodeFlags_DefaultOpen*/


