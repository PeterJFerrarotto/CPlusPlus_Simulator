#include "RequestManager.h"
#include "RideRequest.h"

RequestManager::RequestManager()
{
}

RequestManager::RequestManager(int sectionRadius, int latitudeMax, int longitudeMax, int latitudeMin, int longitudeMin) : sectionRadius(sectionRadius), latitudeMax(latitudeMax), longitudeMax(longitudeMax), latitudeMin(latitudeMin), longitudeMin(longitudeMin) {
	initializeRequestMap();
}

RequestManager::~RequestManager()
{
}

void RequestManager::initializeRequestMap(){
	normalizeCoordinates();
	for (int i = latitudeMin; i <= latitudeMax; i += sectionRadius){
		for (int j = longitudeMin; j <= longitudeMax; j += sectionRadius){
			requestMap[i][j] = {};
		}
	}
}

void RequestManager::normalizeCoordinates(){
	if (latitudeMin % sectionRadius != 0){
		latitudeMin -= latitudeMin % sectionRadius;
	}
	if (longitudeMin % sectionRadius != 0){
		longitudeMin -= longitudeMin % sectionRadius;
	}

	if ((int)latitudeMax % sectionRadius != 0){
		latitudeMax += latitudeMax % sectionRadius;
	}
	if ((int)longitudeMax % sectionRadius != 0){
		longitudeMax += longitudeMax % sectionRadius;
	}
}

void RequestManager::addRequest(RideRequest* request){
	int latitudeToUse = (int)request->getLocation().first;
	int longitudeToUse = (int)request->getLocation().second;
	if (latitudeToUse % sectionRadius > sectionRadius / 2){
		latitudeToUse += sectionRadius - (latitudeToUse % sectionRadius);
	}
	else if (latitudeToUse % sectionRadius <= sectionRadius / 2){
		latitudeToUse -= latitudeToUse % sectionRadius;
	}

	if (longitudeToUse % sectionRadius > sectionRadius / 2){
		longitudeToUse += sectionRadius - (longitudeToUse % sectionRadius);
	}
	else if (longitudeToUse % sectionRadius <= sectionRadius / 2){
		longitudeToUse -= longitudeToUse % sectionRadius;
	}
	requestMap[latitudeToUse][longitudeToUse].push_back(request);
}

std::vector<RideRequest*>& RequestManager::getRequestsAtLocation(std::pair<long, long> location){
	int latitudeToUse = (int)location.first;
	int longitudeToUse = (int)location.second;

	//This could later be re-worked into ranged SQL queries.
	if (latitudeToUse % sectionRadius > sectionRadius / 2){
		latitudeToUse += sectionRadius - (latitudeToUse % sectionRadius);
	}
	else if (latitudeToUse % sectionRadius <= sectionRadius / 2){
		latitudeToUse -= latitudeToUse % sectionRadius;
	}

	if (longitudeToUse % sectionRadius > sectionRadius / 2){
		longitudeToUse += sectionRadius - (longitudeToUse % sectionRadius);
	}
	else if (longitudeToUse % sectionRadius <= sectionRadius / 2){
		longitudeToUse -= longitudeToUse % sectionRadius;
	}

	return requestMap[latitudeToUse][longitudeToUse];
}

void RequestManager::setSectionRadius(int sectionRadius){
	this->sectionRadius = sectionRadius;
}

void RequestManager::setLatitudeMax(int latitudeMax){
	this->latitudeMax = latitudeMax;
}

void RequestManager::setLongitudeMax(int longitudeMax){
	this->longitudeMax = longitudeMax;
}

void RequestManager::setLatitudeMin(int latitudeMin){
	this->latitudeMin = latitudeMin;
}

void RequestManager::setLongitudeMin(int longitudeMin){
	this->longitudeMin = longitudeMin;
}