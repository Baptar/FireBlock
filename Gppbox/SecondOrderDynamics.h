#include <SFML/System/Vector2.hpp>
#include <cmath>

#include "Dice.hpp"
#include "Lib.hpp"

class SecondOrderDynamics {
private:
    sf::Vector2f xp;  // Précédente entrée (position cible)
    sf::Vector2f y, yd; // Variables d'état (position actuelle et vitesse)
    float k1, k2, k3;  // Constantes dynamiques

    float shakeAmplitude = 0.f;
    float shakeFrequency = 0.f;
    float shakeDuration = 0.f;
    float shakeTimer = 0.f;

public:
    SecondOrderDynamics(float f, float z, float r, sf::Vector2f x0) {
        k1 = z / (Dice::getPi() * f);
        k2 = 1.0f / ((2.0f * Dice::getPi() * f) * (2.0f * Dice::getPi() * f));
        k3 = (r * z) / (2.0f * Dice::getPi() * f);
        
        xp = x0;
        y = x0;
        yd = sf::Vector2f(0.f, 0.f);

        std::srand(static_cast<unsigned>(std::time(nullptr)));
    }
    
    sf::Vector2f update(float T, sf::Vector2f x, sf::Vector2f xd = sf::Vector2f(NAN, NAN)) {
        if (std::isnan(xd.x) || std::isnan(xd.y)) {
            xd = (x - xp) / T;
            xp = x;
        }
        
        y = y + T * yd;
        yd = yd + T * ((x + k3 * xd - y - k1 * yd) / k2);

        if (shakeDuration > 0.f) {
            shakeTimer += T;
            float progress = shakeTimer / shakeDuration;

            // Génération d'un offset aléatoire
            float angle = static_cast<float>(std::rand()) / RAND_MAX * 2 * Dice::getPi();
            float magnitude = shakeAmplitude * (1.0f - progress); // Amortissement du shake
            sf::Vector2f offset = sf::Vector2f(std::cos(angle), std::sin(angle)) * magnitude;

            if (shakeTimer >= shakeDuration) {
                resetShake(); // stop shake
            }
            return y + offset;  // Position avec shake
        }
        
        return y;  // Retourne la nouvelle position filtrée
    }

    void setParams(float f, float z, float r)
    {
        k1 = z / (Dice::getPi() * f);
        k2 = 1.0f / ((2.0f * Dice::getPi() * f) * (2.0f * Dice::getPi() * f));
        k3 = (r * z) / (2.0f * Dice::getPi() * f);
    }
    
    void startShake(float amplitude, float frequency, float duration) {
        shakeAmplitude = amplitude;
        shakeFrequency = frequency;
        shakeDuration = duration;
        shakeTimer = 0.f;
    }
    
    void resetShake() {
        shakeDuration = 0.f;  
        shakeTimer = 0.f;
    }
};
