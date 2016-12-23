#ifndef _VEHICLE_H
#define _VEHICLE_H
#include <queue>

class RideRequest;
class Vehicle
{
protected:
	std::queue<RideRequest*> requests;
	std::pair<long, long> currentLocation;

	std::pair<long, long> getLastDestination();

	long distanceWithPassenger;
	long distanceWithoutPassenger;
public:
	Vehicle();
	~Vehicle();
	std::pair<long, long> getCurrentLocation();

	void addRequest(RideRequest* request);

	void setLocation(long latitude, long longitude);

	//Update assuming that vehicles always take one hour to arrive at 
	void update();

	void update(int time);

	RideRequest* getTopRequest();

	long getDistanceWithPassenger();
	long getDistanceWithoutPassenger();
};

#endif