#pragma once
#include <SFML/System/Vector2.hpp>

class Tween
{
public:
    static float smoothStep(float t) {
        return t * t * (3 - 2 * t);
    }

    static float lerp(float a, float b, float t) {
        return a + t * (b - a);
    }

    static sf::Vector2f lerp(const sf::Vector2f& a, const sf::Vector2f& b, float t) {
        return a + t * (b - a);
    }
};
