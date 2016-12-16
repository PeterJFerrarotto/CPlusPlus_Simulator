#ifndef _RIDE_REQUEST_H
#define _RIDE_REQUEST_H
#include <algorithm>

class RideRequest
{
protected:
	std::pair<long, long> location;
	std::pair<long, long> destination;
	bool pickedUp;
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

	std::pair<long, long> getLocation();
	std::pair<long, long> getDestination();
	bool getPickedUp();
	long getDistanceToRequest();
	long getDistanceOfRequest();
	bool getDistanceOfRequestCalculated();
	unsigned getRequestsAtDestination();
};

#endif