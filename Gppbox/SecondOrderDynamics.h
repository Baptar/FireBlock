#include <SFML/System/Vector2.hpp>
#include <cmath>

#include "Dice.hpp"
#include "Lib.hpp"

class SecondOrderDynamics {
private:
    sf::Vector2f xp;  // Précédente entrée (position cible)
    sf::Vector2f y, yd; // Variables d'état (position actuelle et vitesse)
    float k1, k2, k3;  // Constantes dynamiques

public:
    // Constructeur : initialise le système avec fréquence, amortissement et réponse initiale
    SecondOrderDynamics(float f, float z, float r, sf::Vector2f x0) {
        // Calcul des constantes
        k1 = z / (Dice::getPi() * f);
        k2 = 1.0f / ((2.0f * Dice::getPi() * f) * (2.0f * Dice::getPi() * f));
        k3 = (r * z) / (2.0f * Dice::getPi() * f);

        // Initialisation des variables
        xp = x0;
        y = x0;
        yd = sf::Vector2f(0.f, 0.f);
    }

    // Mise à jour du filtre avec un pas de temps T et une nouvelle position cible x
    sf::Vector2f Update(float T, sf::Vector2f x, sf::Vector2f xd = sf::Vector2f(NAN, NAN)) {
        // Si aucune vitesse cible n'est donnée, on la calcule
        if (std::isnan(xd.x) || std::isnan(xd.y)) {
            xd = (x - xp) / T;
            xp = x;
        }

        // Intégration : mise à jour de la position et de la vitesse
        y = y + T * yd;  // Intégration de la position avec la vitesse
        yd = yd + T * ((x + k3 * xd - y - k1 * yd) / k2);  // Intégration de la vitesse avec l'accélération

        return y;  // Retourne la nouvelle position filtrée
    }

    void setParams(float f, float z, float r)
    {
        k1 = z / (Dice::getPi() * f);
        k2 = 1.0f / ((2.0f * Dice::getPi() * f) * (2.0f * Dice::getPi() * f));
        k3 = (r * z) / (2.0f * Dice::getPi() * f);
    }
};
