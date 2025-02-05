#pragma once
#include <SFML/Graphics.hpp>

class Player;

class SpritePlayer
{
private:
    sf::Sprite sprite;
    sf::Texture textureIdle;
    sf::Texture textureWalk;
    sf::Texture textureRun;
    sf::Texture textureJump0;
    sf::Texture textureJump1;
    sf::Texture textureJump2;
    sf::Texture textureHurt;
    sf::Texture textureFire;
    sf::Texture textureReload;
    sf::Texture textureDeath;
    sf::Texture textureRunShot;

    int			numberOfFrame = 6;
    float		animationTime = 0;  
    float		frameSpeed = 0.1f;
    int         sizeTexture;

    Player&	    player;
    
    void        CheckFileTexture();
    void        InitTexture();
    void        setAnimationFrame(int _frame, int _animationRow);
    
public:
    int			 currentFrame = 0;
    int			 animationRow = 0; // 0 = idle, 1 = walk, 2 = run, 3 = jump
    sf::Sprite&  getSprite();
    
                SpritePlayer(Player& _player);
    void        update(double _dt);
    void        playAnimationSprite(int _frame, int _animationRow);
    
};
