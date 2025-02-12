#pragma once
#include <SFML/Graphics.hpp>

class Ennemy;

class SpriteEnnemy
{
private:
    sf::Sprite sprite;
    //zombie 1
    sf::Texture textureIdle1;
    sf::Texture textureRun1;
    sf::Texture textureDead1;
    sf::Texture textureHurt1;
    sf::Texture textureAttack1;
    //zombie 2
    sf::Texture textureIdle2;
    sf::Texture textureRun2;
    sf::Texture textureDead2;
    sf::Texture textureHurt2;
    sf::Texture textureAttack2;
    //zombie 3
    sf::Texture textureIdle3;
    sf::Texture textureRun3;
    sf::Texture textureDead3;
    sf::Texture textureHurt3;
    sf::Texture textureAttack3;
    //zombie 4
    sf::Texture textureIdle4;
    sf::Texture textureRun4;
    sf::Texture textureDead4;
    sf::Texture textureHurt4;
    sf::Texture textureAttack4;
    //zombie 5
    sf::Texture textureIdle5;
    sf::Texture textureRun5;
    sf::Texture textureDead5;
    sf::Texture textureHurt5;
    sf::Texture textureAttack5;
    //zombie 6
    sf::Texture textureIdle6;
    sf::Texture textureRun6;
    sf::Texture textureDead6;
    sf::Texture textureHurt6;
    sf::Texture textureAttack6;

    sf::Texture textureIdleUsed;
    sf::Texture textureRunUsed;
    sf::Texture textureDeadUsed;
    sf::Texture textureHurtUsed;
    sf::Texture textureAttackUsed;

    int			currentFrame = 0;   
    float		animationTime = 0;  
    float		frameSpeed = 0.1f;
    int			animationRow = 0; // 0 = idle, 1 = run
    int			numberOfFrame = 6;
    int         numberOfFrameIdle;
    int         numberOfFrameRun;
    int         numberOfFrameHurt;
    int         numberOfFrameAttack;
    int         numberOfFrameDead = 5;
    int         sizeTexture;

    Ennemy&     ennemy;
    
    void        CheckFileTexture();
    void        InitRandomTextures();
    void        setAnimationFrame(int frame, int animationRow);
    
public:
    sf::Sprite&  getSprite();
    bool        finishedAnimDeath = false;
    bool        finishedAnimHurt = true;
    bool        finishedAnimAttack = true;
    
                SpriteEnnemy(Ennemy& ennemy);
    void        update(double dt);
    void        playAnimationSprite(int frame, int animationRow);
    
};
