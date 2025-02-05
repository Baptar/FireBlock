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

class Entity;
class HotReloadShader;
class Game {
public:
	sf::Texture						textureWall_x0;
	sf::Texture						textureWall_x1;
	sf::Texture						textureWall_x2;
	sf::Texture						textureWall_x3;
	sf::Texture						textureWall_x4;
	sf::Texture						textureWall_x0Diag;
	sf::Texture						textureWall_x02Diag;
	sf::Texture						textureWall_x2Mid;
	sf::RenderWindow*				win = nullptr;
	float							zoom = 0.28f;
	float							f = 0.7f;
	float							z = 0.78f;
	float							r = 0.0f;
	sf::View						cameraView;
	bool							isEditing = false;
	Vector2i						posMouse;
	
	sf::RectangleShape				bg;
	sf::RectangleShape				bg2;
	HotReloadShader *				bgShader = nullptr;

	sf::Texture						tex;

	bool							closing = false;

	unsigned int					selectedElement = 0;
	std::vector<sf::Vector2i>		walls;
	std::vector<sf::Sprite>			wallSprites;
	std::vector<sf::Sprite>			ennemmySprites;
	std::vector<Entity*>			ents;
	std::vector<Ennemy*>			ennemies;

	ParticleMan beforeParts;
	ParticleMan afterParts;

	static Game* me;


	
		Game(sf::RenderWindow * win);

	void initMainChar();
	void cacheWalls();
	void cacheEnnemies();

	void processInput(sf::Event ev);
	bool wasPressed = false;
	void pollInput(double dt);
	void onSpacePressed();

	bool hasPlayerCollision(int cx, int cy);
	bool hasWall(int cx, int cy);
	bool hasEnnemy(int cx, int cy);
	bool hasPlayer(int cx, int cy);
	void addWall(int cx, int cy);
	void addEnnemy(int cx, int cy);

	void update(double dt);

	void draw(sf::RenderWindow& win);

	
	void removeWallAtPosition(int cx, int cy);
	void im();

	bool loadData(const std::filesystem::path& filePath);
	void saveData(const std::filesystem::path& filePath) const;

	Entity& getPlayer() const;
};