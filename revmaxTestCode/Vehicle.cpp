#include "Vehicle.h"
#include "RideRequest.h"

Vehicle::Vehicle()
{
}


Vehicle::~Vehicle()
{
}

std::pair<long, long> Vehicle::getLastDestination(){
	return requests.back()->getDestination();
}

std::pair<long, long> Vehicle::getCurrentLocation(){
	if (requests.size() != 0){
		return getLastDestination();
	}
	return currentLocation;
}

void Vehicle::addRequest(RideRequest* request){
	requests.push(request);
}

void Vehicle::setLocation(long latitude, long longitude){
	currentLocation.first = latitude;
	currentLocation.second = longitude;
}

void Vehicle::update(){
	if (requests.size() != 0){
		std::pair<float, float> rLocation = requests.front()->getLocation();
		std::pair<float, float> rDestination = requests.front()->getDestination();
		bool rPickedUp = requests.front()->getPickedUp();
		if (currentLocation.first == rLocation.first && currentLocation.second == rLocation.second && !rPickedUp){
			requests.front()->setPickedUp(true);
		}
		else if (currentLocation.first == rLocation.first && currentLocation.second == rLocation.second && rPickedUp){
			distanceWithPassenger += requests.front()->getDistanceOfRequest();
			currentLocation.first = rDestination.first;
			currentLocation.second = rDestination.second;
		}
		else if (currentLocation.first != rLocation.first && currentLocation.second != rLocation.second && !rPickedUp){
			distanceWithoutPassenger += requests.front()->getDistanceToRequest();
			currentLocation.first = rLocation.first;
			currentLocation.second = rLocation.second;
		}

		if (currentLocation.first == rDestination.first && currentLocation.second == rDestination.second && rPickedUp){
			requests.pop();
		}
	}
}

void Vehicle::update(int time){
	if (requests.size() != 0){
		std::pair<float, float> rLocation = requests.front()->getLocation();
		std::pair<float, float> rDestination = requests.front()->getDestination();
		int requestTime = requests.front()->getRequestTime();
		int dropOffTime = requests.front()->getDistanceOfRequest() + requestTime;
		bool rPickedUp = requests.front()->getPickedUp();

		if (time == requestTime && !rPickedUp){
			requests.front()->setPickedUp(true);
			currentLocation.first = rLocation.first;
			currentLocation.second = rLocation.second;
			distanceWithoutPassenger += requests.front()->getDistanceToRequest();
		}
		else if (time == dropOffTime && rPickedUp){
			distanceWithPassenger += requests.front()->getDistanceOfRequest();
			currentLocation.first = rDestination.first;
			currentLocation.second = rDestination.second;
		}

		if (currentLocation.first == rDestination.first && currentLocation.second == rDestination.second && rPickedUp){
			requests.pop();
		}
	}
}

RideRequest* Vehicle::getTopRequest(){
	if (requests.size() == 0){
		return nullptr;
	}
	return requests.front();
}

long Vehicle::getDistanceWithPassenger(){
	return distanceWithPassenger;
}

long Vehicle::getDistanceWithoutPassenger(){
	return distanceWithoutPassenger;
}