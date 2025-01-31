#pragma once

#include <vector>

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
	sf::RenderWindow*				win = nullptr;
	float							zoom = 0.55f;
	float							f = 0.7f;
	float							z = 0.78f;
	float							r = 0.0f;
	sf::View						cameraView;
	
	sf::RectangleShape				bg;
	HotReloadShader *				bgShader = nullptr;

	sf::Texture						tex;

	bool							closing = false;
	
	std::vector<sf::Vector2i>		walls;
	std::vector<sf::RectangleShape> wallSprites;
	std::vector<Entity*>			ents;

	ParticleMan beforeParts;
	ParticleMan afterParts;

	static Game* me;
		Game(sf::RenderWindow * win);

	void initMainChar();
	void cacheWalls();

	void processInput(sf::Event ev);
	bool wasPressed = false;
	void pollInput(double dt);
	void onSpacePressed();

	bool hasCollision(float gx, float gy, int width, int height);

	void update(double dt);

	void draw(sf::RenderWindow& win);

	bool isWall(float gx, float gy, int width, int height);
	void removeWallAtPosition(float cx, float cy);
	void im();

	void saveData(int cx, int cy);
	void loadData();

	Entity& getPlayer() const;	
};