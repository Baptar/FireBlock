#pragma once

#include <filesystem>
#include <vector>

#include "Ennemy.hpp"
#include "SFML/Graphics.hpp"
#include "SFML/System.hpp"
#include "SFML/Window.hpp"

#include "sys.hpp"

#include "Particle.hpp"
#include "ParticleMan.hpp"

using namespace sf;

class Player;
class HotReloadShader;
class Game {
public:
	sf::Texture						textureWall_x4;

	sf::RenderWindow*				win = nullptr;
	float							zoom = 0.28f;
	float							f = 0.7f;
	float							z = 0.78f;
	float							r = 0.0f;
	sf::View						cameraView;
	bool							isEditing = false;
	Vector2i						posMouse;
	
	sf::RectangleShape				bg;
	HotReloadShader *				bgShader = nullptr;

	sf::Texture						tex;

	bool							closing = false;

	unsigned int					selectedElement = 0;
	std::vector<sf::Vector2i>		walls;
	std::vector<sf::Sprite>			wallSprites;
	std::vector<Player*>			ents;
	std::vector<Ennemy*>			ennemies;

	ParticleMan beforeParts;
	ParticleMan afterParts;

	static Game* me;

		 Game(sf::RenderWindow * _win);
	void initMainChar();
	void cacheWalls();

	void processInput(sf::Event _ev);
	bool wasPressed = false;
	void pollInput(double _dt);
	void onSpacePressed();

	bool hasPlayerCollision(int _cx, int _cy);
	bool hasWall(int _cx, int _cy);
	bool hasEnnemy(int _cx, int _cy);
	bool hasPlayer(int _cx, int _cy);
	void addWall(int _cx, int _cy);
	void addEnnemy(int _cx, int _cy);

	void update(double _dt);

	void draw(sf::RenderWindow& _win);

	
	void removeWallAtPosition(int _cx, int _cy);
	void im();

	bool loadData(const std::filesystem::path& _filePath);
	void saveData(const std::filesystem::path& _filePath) const;

	Player& getPlayer() const;
};