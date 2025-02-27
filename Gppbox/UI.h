#pragma once
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Text.hpp>

class UI
{
public:
    sf::Font font;
    sf::Text gameOverText;
    sf::Text munitionText;
    sf::Text lifeText;
    sf::Text laserText;
    sf::Text missileText;
    sf::Text commandText;
    sf::RenderWindow * window;

    
    UI(sf::RenderWindow * _win);
    void draw();
};
