#ifndef _RIDE_REQUEST_H
#define _RIDE_REQUEST_H
#include <algorithm>
#include "Matrix.h"

class RideRequest
{
protected:
	std::pair<long, long> location;
	std::pair<long, long> destination;
	//Request time is an integer representing the hour - right now using an hour timestep for testing purposes
	int requestTime;
	int timeMatched;
	bool pickedUp;
	bool matchedToVehicle;
	//Members set upon routing for later use
	long distanceToRequest, distanceOfRequest;
	unsigned requestsAtDestination;

	//Only a calculation flag for distance: will not change, but requests at destination might (if new requests crop up, or if a ride would arrive
	//at a time when requests are forecasted.
	bool distanceOfRequestCalculated;
public:
	RideRequest();
	~RideRequest();

	void setLocation(long latitude, long longitude);
	void setDestination(long latitude, long longitude);
	void setPickedUp(bool pickedUp);
	void setDistanceToRequest(long distanceToRequest);
	void setDistanceOfRequest(long distanceOfRequest);
	void setRequestsAtDestination(unsigned requestsAtDestination);
	void setRequestTime(int time);
	void setTimeMatched(int time);

	std::pair<long, long> getLocation();
	std::pair<long, long> getDestination();
	bool getPickedUp();
	long getDistanceToRequest();
	long getDistanceOfRequest();
	bool getDistanceOfRequestCalculated();
	unsigned getRequestsAtDestination();
	int getRequestTime();
	int getTimeMatched();

	bool getMatchedToVehicle();
	void setMatchedToVehicle(bool matchedToVehicle);
};

#endif