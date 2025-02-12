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
    int munition = Game::me->getPlayer().actualBullets;
    lifeText.setString("Life :" + std::to_string(max(life, 0)));

    if (munition == 0) munitionText.setString("Munition : 0 (Press 'R' to reload)");
    else munitionText.setString("Munition :" + std::to_string(Game::me->getPlayer().actualBullets));
    printf("%f",munitionText.getPosition().x);
    window->draw(munitionText);
    window->draw(lifeText);
}

