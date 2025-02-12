#pragma once

#include <filesystem>
#include <vector>

#include "CameraManager.hpp"
#include "Ennemy.hpp"
#include "SFML/Graphics.hpp"
#include "SFML/Window.hpp"

#include "sys.hpp"

#include "ParticleMan.hpp"

using namespace sf;

class Player;
class HotReloadShader;
class Game {
public:
	sf::Texture						textureWall_x4;
	sf::RenderWindow*				win = nullptr;
	sf::RectangleShape				bg;
	sf::Texture						texBG;
	HotReloadShader *				bgShader = nullptr;
	CameraManager					camera;
	
	float							zoom = 0.28f;
	float							f = 0.7f;
	float							z = 0.78f;
	float							r = 0.0f;
	
	bool							isEditing = false;
	bool							editingWalls = true;
	bool							editingEnemies = false;
	Vector2i						posMouse;
	
	bool							canJumpInput = true;
	bool							closing = false;

	unsigned int					selectedElement = 0;
	std::vector<sf::Vector2i>		walls;
	std::vector<sf::Sprite>			wallSprites;
	std::vector<Player*>			players;
	std::list<Ennemy*>				ennemies;

	ParticleMan beforeParts;
	ParticleMan afterParts;

	static Game* me;

									Game(sf::RenderWindow * _win);
	void							initMainChar(int cx, int cy);
	void							cacheWalls();
	void							drawLinesEdit();

	void							processInput(sf::Event _ev);
	bool							wasPressed = false;
	bool							pressingRight = false;
	bool							pressingLeft = false;
	void							pollInput(double _dt);
	void							onSpacePressed();

	bool							hasPlayerCollision(int _cx, int _cy);
	bool							hasWall(int _cx, int _cy);
	bool							hasEnnemy(int _cx, int _cy);
	bool							hasPlayer(int _cx, int _cy);
	void							addWall(int _cx, int _cy);
	void							addEnnemy(int _cx, int _cy);
	void							removeWallAtPosition(int _cx, int _cy);
	Ennemy*							getEnnemyAtPosition(int _cx, int _cy);

	void							update(double _dt);

	void							draw(sf::RenderWindow& _win);
	
	void							im();

	bool							loadData(const std::filesystem::path& _filePath);
	void							saveData(const std::filesystem::path& _filePath) const;

	Player&							getPlayer() const;
};