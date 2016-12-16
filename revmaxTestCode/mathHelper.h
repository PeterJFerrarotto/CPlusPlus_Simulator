#ifndef _MATH_HELPER_H
#define _MATH_HELPER_H
#include <math.h>
#include "Vehicle.h"
#include "RequestManager.h"
#include "RideRequest.h"

inline float pythagDistance(float x1, float y1, float x2, float y2){
	return sqrtf(powf(x2 - x1, 2) + powf(y2 - y1, 2));
}

inline float scoreRequest(Vehicle* vehicle, RideRequest* request, RequestManager manager, float optimalScore, float weightOfDistanceToRide, float weightOfDistanceOfRide, float weightOfRidesAtDestination, float(*routing)(float, float, float, float)){
	float distanceToRide = routing(vehicle->getCurrentLocation().first, vehicle->getCurrentLocation().second, request->getLocation().first, request->getLocation().second);
	float distanceOfRide = request->getDistanceOfRequestCalculated() ? request->getDistanceOfRequest() : routing(request->getLocation().first, request->getLocation().second, request->getDestination().first, request->getDestination().second);
	int numOfRequestsAtDestination = manager.getRequestsAtLocation(request->getDestination()).size();
	float score = 0;

	//The smaller the distance to the ride, the greater the score.
	score = (1/distanceToRide) * (weightOfDistanceToRide/optimalScore);
	if (distanceToRide == 0){
		score = weightOfDistanceToRide;
	}
	request->setDistanceToRequest(distanceToRide);
	request->setDistanceOfRequest(distanceOfRide);
	request->setRequestsAtDestination(numOfRequestsAtDestination);
	score += (distanceOfRide) * (weightOfDistanceOfRide/optimalScore);

	score += (numOfRequestsAtDestination)* (weightOfRidesAtDestination/optimalScore);

	return score;
}

#endif