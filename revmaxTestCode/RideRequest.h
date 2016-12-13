#ifndef _RIDE_REQUEST_H
#define _RIDE_REQUEST_H
#include <algorithm>

class RideRequest
{
protected:
	std::pair<long, long> location;
	std::pair<long, long> destination;
	bool pickedUp;
public:
	RideRequest();
	~RideRequest();

	void setLocation(long latitude, long longitude);
	void setDestination(long latitude, long longitude);
	void setPickedUp(bool pickedUp);

	std::pair<long, long> getLocation();
	std::pair<long, long> getDestination();
	bool getPickedUp();
};

#endif