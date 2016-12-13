#ifndef _REQUEST_MANAGER_H
#define _REQUEST_MANAGER_H
#include <unordered_map>

class RideRequest;
class RequestManager
{
protected:
	std::unordered_map<int, std::unordered_map<int, std::vector<RideRequest*>>> requestMap;
	int latitudeMax, longitudeMax;
	int latitudeMin, longitudeMin;
	int sectionRadius;

	void normalizeCoordinates();
public:
	RequestManager();
	RequestManager(int sectionRadius, int latitudeMax, int longitudeMax, int latitudeMin, int longitudeMin);
	~RequestManager();
	void setSectionRadius(int sectionRadius);
	void setLatitudeMax(int latitudeMax);
	void setLongitudeMax(int longitudeMax);
	void setLatitudeMin(int latitudeMin);
	void setLongitudeMin(int longitudeMin);
	void addRequest(RideRequest* toAdd);
	void initializeRequestMap();

	std::vector<RideRequest*>& getRequestsAtLocation(std::pair<long, long> location);
};

#endif