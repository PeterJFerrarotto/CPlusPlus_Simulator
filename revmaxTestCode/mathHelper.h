#ifndef _MATH_HELPER_H
#define _MATH_HELPER_H
#include <math.h>
#include <random>

inline float pythagDistance(float x1, float y1, float x2, float y2){
	return sqrtf(powf(x2 - x1, 2) + powf(y2 - y1, 2));
}

inline float randomRangedLong(float bottom, float top){
	static std::random_device rd; // obtain a random number from hardware
	static std::mt19937 eng(rd()); // seed the generator
	static std::uniform_real_distribution<> distr(bottom, top); // define the range

	return distr(eng);
}

inline int randomRangedInt(int bottom, int top){
	//static std::random_device rd; // obtain a random number from hardware
	//static std::mt19937 eng(rd()); // seed the generator
	//static std::uniform_int_distribution<> distr(bottom, top); // define the range

	//return distr(eng);
	return (rand() % top) + bottom;
}
#endif