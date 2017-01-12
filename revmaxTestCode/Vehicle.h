#ifndef _VEHICLE_H
#define _VEHICLE_H
#include <queue>
#include <unordered_map>
#include "ShaderProgram.h"
#include "Matrix.h"

class RideRequest;
class Vehicle
{
protected:
	std::queue<RideRequest*> requests;
	std::pair<long, long> currentLocation;

	std::pair<long, long> getLastDestination();


	long distanceWithPassenger;
	long distanceWithoutPassenger;

	//Below code is for rendering uses only!!!
	//std::unordered_map<int, std::queue<std::pair<long, long>>> routingLog;
	std::queue<std::pair<int, std::pair<long, long>>> routingLog;

	std::pair<long, long> previousLocation;
	float previousTime;
	std::pair<float, float> currentRenderingLocation;

	bool hasPassenger;
public:
	Vehicle();
	~Vehicle();
	void freeMemory();
	std::pair<long, long> getCurrentLocation();

	void addRequest(RideRequest* request);

	void setLocation(long latitude, long longitude);

	//Update assuming that vehicles always take one hour to arrive at 
	void update();

	void update(int time);

	void update(int time, int timeRadius);

	void updateForRendering(float time);

	void popTopRequest();

	void addToRoutingLog(float time, std::pair<float, float> location);

	RideRequest* getTopRequest();

	long getDistanceWithPassenger();
	long getDistanceWithoutPassenger();

	bool getHasPassenger();
	void setHasPassenger(bool hasPassenger);

	std::pair<long, long> getPreviousRenderingLocation();
	std::pair<float, float> getCurrentRenderingLocation();
	std::pair<int, std::pair<long, long>> getNextRoutingNode();
	void prepareForRendering();
	bool checkRoutingLog();
	float getRenderingAngle();
};

#endif