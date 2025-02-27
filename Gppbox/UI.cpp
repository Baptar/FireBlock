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
    munitionText.setPosition(0, 100);

    lifeText.setFont(font);
    lifeText.setString("Life :");
    lifeText.setPosition(0, 50);

    laserText.setFont(font);
    laserText.setString("Death ray :");
    laserText.setPosition(0, 150);
    
    missileText.setFont(font);
    missileText.setString("Missile :");
    missileText.setPosition(0, 200);

    /*commandText.setFont(font);
    commandText.setString("Move Right : D\nMove Left : Q\nJump : Space\n");
    commandText.setPosition(0, 250);*/
}

void UI::draw()
{
    // life
    int life = Game::me->getPlayer().life;
    lifeText.setString("Life : " + std::to_string(max(life, 0)));

    int missile = Game::me->getPlayer().missileNumber;
    missileText.setString("Missile : " + std::to_string(max(missile, 0)));

    //munition
    int munition = Game::me->getPlayer().actualBullets;
    if (munition == 0) munitionText.setString("Munition : 0 (Press 'R' to reload)");
    else munitionText.setString("Munition : " + std::to_string(Game::me->getPlayer().actualBullets));

    //laser
    int timeLaser = int(Game::me->getPlayer().currentTimeLaser + 1);
    switch (timeLaser)
    {
        case 0:
            laserText.setString("Death ray : " + std::to_string(timeLaser) + "s (wait to reload)");
            break;
        case 6:
            laserText.setString("Death ray : " + std::to_string(timeLaser) + "s (max)");
            break;
        default:
            laserText.setString("Death ray : " + std::to_string(timeLaser) + "s");
            break;
    }

    // draw
    window->draw(munitionText);
    window->draw(lifeText);
    window->draw(commandText);
    window->draw(laserText);
    window->draw(missileText);
}

