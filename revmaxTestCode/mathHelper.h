#ifndef _MATH_HELPER_H
#define _MATH_HELPER_H
#include <math.h>
#include "Vehicle.h"
#include "RequestManager.h"
#include "RideRequest.h"

inline float pythagDistance(float x1, float y1, float x2, float y2){
	return sqrtf(powf(x2 - x1, 2) + powf(y2 - y1, 2));
}

inline float scoreRequest(Vehicle* vehicle, RideRequest* request, RequestManager manager, int time, int timeRadius, float weightOfDistanceOfRide, float(*routing)(float, float, float, float)){
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

	if (0 <= numOfRequestsAtDestination < 5){
		score -= 3;
	}
	else if (5 <= numOfRequestsAtDestination < 10){
		score -= 2;
	}
	else if (10 <= numOfRequestsAtDestination < 20){
		score -= 1;
	}
	//else if (20 <= numOfRequestsAtDestination < 40){
	//	score -= 2;
	//}
	//else if (40 <= numOfRequestsAtDestination < 80){
	//	score -= 1;
	//}
	return score;
}

#endif