#ifndef _MATH_HELPER_H
#define _MATH_HELPER_H
#include <math.h>
#include "Vehicle.h"
#include <random>
#include "RequestManager.h"
#include "RideRequest.h"

inline float pythagDistance(float x1, float y1, float x2, float y2){
	return sqrtf(powf(x2 - x1, 2) + powf(y2 - y1, 2));
}

inline long randomRangedLong(long bottom, long top){
	static std::random_device rd; // obtain a random number from hardware
	static std::mt19937 eng(rd()); // seed the generator
	static std::uniform_real_distribution<> distr(bottom, top); // define the range

	return distr(eng);
}

inline int randomRangedInt(int bottom, int top){
	static std::random_device rd; // obtain a random number from hardware
	static std::mt19937 eng(rd()); // seed the generator
	static std::uniform_int_distribution<> distr(bottom, top); // define the range

	return distr(eng);
}



inline float scoreRequest(Vehicle* vehicle, RideRequest* request, RequestManager manager, int time, int timeRadius, float weightOfDistanceOfRide, int maxRideRequests, float(*routing)(float, float, float, float)){
	float distanceToRide = routing(vehicle->getCurrentLocation().first, vehicle->getCurrentLocation().second, request->getLocation().first, request->getLocation().second);
	//Assumption: each unit is travelled in one hour.
	int timeToUse = time;
	if (vehicle->getTopRequest() != nullptr){
		timeToUse = vehicle->getTopRequest()->getRequestTime() + vehicle->getTopRequest()->getDistanceOfRequest();
	}
	if (request->getRequestTime() > timeToUse + ceil(distanceToRide) + timeRadius || request->getRequestTime() < timeToUse + ceil(distanceToRide) - timeRadius){
		return -1;
	}
	float distanceOfRide = request->getDistanceOfRequestCalculated() ? request->getDistanceOfRequest() : routing(request->getLocation().first, request->getLocation().second, request->getDestination().first, request->getDestination().second);
	//Currently using the calculated distance as the time.
	int timeOfRide = (int)distanceOfRide;
	int numOfRequestsAtDestination = manager.getNumberOfRequestsAtLocation(request->getDestination(), request->getRequestTime() + timeOfRide, timeRadius);
	float score = 0;

	//The smaller the distance to the ride, the greater the score.
	score = (distanceOfRide/(distanceToRide + distanceOfRide)) * 10;
	request->setDistanceToRequest(ceil(distanceToRide));
	request->setDistanceOfRequest(distanceOfRide);
	request->setRequestsAtDestination(numOfRequestsAtDestination);
	score += ((distanceOfRide) * (weightOfDistanceOfRide)) * 10;

	if (numOfRequestsAtDestination < 30){
		score += (-3 + (numOfRequestsAtDestination * (3/maxRideRequests)));
	}
	return score;
}

#endif