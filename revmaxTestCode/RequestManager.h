#ifndef _REQUEST_MANAGER_H
#define _REQUEST_MANAGER_H
#include <unordered_map>
#include "Matrix.h"

class RideRequest;
//class EventVenue;
class Texture;
class ShaderProgram;
class RequestManager
{
protected:
	std::unordered_map<int, std::unordered_map<int, std::vector<RideRequest*>>> requestMap;
	//std::unordered_map<int, std::unordered_map<int, std::vector<EventVenue*>>> venueMap;
	std::vector<RideRequest*> allRideRequests;
	//std::vector<EventVenue*> allVenues;
	int latitudeMax, longitudeMax;
	int latitudeMin, longitudeMin;
	int sectionRadius;

	void normalizeCoordinates();
	Matrix modelMatrix;
	Texture* lineTexture;
	Texture* gridTexture;
	Texture* requestTexture;
	//Texture* venueTexture;
	Texture* destinationTexture;
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
	//void addVenue(EventVenue* toAdd);
	void initializeRequestMap();

	std::pair<int, int> getMinCoords();
	std::pair<int, int> getMaxCoords();
	int getSectionRadius();

	std::vector<RideRequest*>& getRequestsAtLocation(std::pair<long, long> location);

	int getNumberOfRequestsAtLocation(std::pair<long, long> location, int time, int timeRadius);

	void render(ShaderProgram* program, float time, float timeRadius, float scaleX, float scaleY);

	void setLineTexture(Texture* texture);
	void setRequestTexture(Texture* texture);
	//void setVenueTexture(Texture* texture);
	void setDestinationTexture(Texture* texture);
	void setGridTexture(Texture* gridTexture){ this->gridTexture = gridTexture; }
	void freeMemory();

	std::vector<RideRequest*>& getAllRideRequests(){ return allRideRequests; }
	//std::vector<EventVenue*>& getAllEventVenues(){ return allVenues; }
};

#endif