#include "UI.h"

#include <iostream>

#include "Game.hpp"

UI::UI(sf::RenderWindow* _win): window(_win)
{
    // load font
    if (!font.loadFromFile("res/MAIAN.TTF")) {
        std::cout << "ERROR NO FONT" << '\n';
        return;
    }
    
    munitionText.setFont(font);
    munitionText.setString("Munition :");
    munitionText.setPosition(0, 50);

    lifeText.setFont(font);
    lifeText.setString("Life :");
    lifeText.setPosition(0, 100);
}

void UI::draw()
{
    int life = Game::me->getPlayer().life;
    lifeText.setString("Life :" + std::to_string(max(life, 0)));
    printf("%f",munitionText.getPosition().x);
    window->draw(munitionText);
    window->draw(lifeText);
}

