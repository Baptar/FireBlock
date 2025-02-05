#pragma once

#include <cstdlib>

#define _USE_MATH_DEFINES
#include <cmath>
#include <random>

class Dice {
public:
	static inline constexpr double pi = 3.14159265358979323846;

	static int randSign() {
		return (rand() % 2) ? 1 : -1;
	}

	static int roll(int min, int max) {
		return min + rand() % (max-min);
	}

	static bool toss() {
		return roll(0, 1) == 1;
	}
	
	static float randF() {
		return 1.0f * rand() / RAND_MAX;
	}

	static int randInt(int min, int max)
	{
		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_int_distribution<> distrib(min, max);
		return distrib(gen);
	}

	static int randFloat(float min, float max)
	{
		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_real_distribution<float> distrib(min, max);
		return distrib(gen);
	}

	static float angleRad() {
		return randF() * 2 * (float)pi;
	}

	static float angleDeg() {
		return randF() * 360;
	}

	static float getPi()
	{
		return (float)pi;
	}
};
