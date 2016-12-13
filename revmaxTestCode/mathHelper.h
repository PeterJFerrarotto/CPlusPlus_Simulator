#ifndef _MATH_HELPER_H
#define _MATH_HELPER_H
#include <math.h>
#include "Vehicle.h"
#include "RequestManager.h"
#include "RideRequest.h"

inline float pythagDistance(float x1, float y1, float x2, float y2){
	return sqrtf(powf(x2 - x1, 2) + powf(y2 - y1, 2));
}

inline float scoreRequest(Vehicle* vehicle, RideRequest* request, RequestManager manager, float maxScore, float weightOfDistanceToRide, float weightOfDistanceOfRide, float weightOfRidesAtDestination, float(*routing)(float, float, float, float)){
	float distanceToRide = routing(vehicle->getCurrentLocation().first, vehicle->getCurrentLocation().second, request->getLocation().first, request->getLocation().second);
	float distanceOfRide = routing(request->getLocation().first, request->getLocation().second, request->getDestination().first, request->getDestination().second);
	int numOfRequestsAtDestination = manager.getRequestsAtLocation(request->getDestination()).size();
	float score = 0;

	//The smaller the distance to the ride, the greater the score.
	score = (1/distanceToRide) * (weightOfDistanceToRide / maxScore);
	if (distanceToRide == 0){
		score = weightOfDistanceToRide / maxScore;
	}

	score += (distanceToRide)* (weightOfDistanceOfRide / maxScore);

	score += (numOfRequestsAtDestination)* (weightOfRidesAtDestination / maxScore);

	return score;
}

#endif