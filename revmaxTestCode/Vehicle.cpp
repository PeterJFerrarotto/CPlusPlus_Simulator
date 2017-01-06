#include "Vehicle.h"
#include "RideRequest.h"
#include "Texture.h"
#include "renderingMathHelper.h"

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
	return currentLocation;
}

void Vehicle::addRequest(RideRequest* request){
	requests.push(request);
}

void Vehicle::setLocation(long latitude, long longitude){
	currentLocation.first = latitude;
	currentLocation.second = longitude;
}

void Vehicle::addToRoutingLog(float time, std::pair<float, float> location){
	routingLog.push(std::make_pair(time, location));
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

void Vehicle::update(int time, int timeRadius){
	if (requests.size() != 0){
		std::pair<float, float> rLocation = requests.front()->getLocation();
		std::pair<float, float> rDestination = requests.front()->getDestination();
		int requestTime = requests.front()->getRequestTime();
		int matchTime = requests.front()->getTimeMatched();
		int dropOffTime = requests.front()->getDistanceOfRequest() + requestTime;
		bool rPickedUp = requests.front()->getPickedUp();
		int actualPickupTime = matchTime + requests.front()->getDistanceToRequest();
		if (actualPickupTime < requestTime){
			actualPickupTime = requestTime;
		}
		if ((time >= requestTime && time <= requestTime + timeRadius) && (time == actualPickupTime) && !rPickedUp){
			requests.front()->setPickedUp(true);
			currentLocation.first = rLocation.first;
			currentLocation.second = rLocation.second;
			distanceWithoutPassenger += requests.front()->getDistanceToRequest();
			requests.front()->setRequestTime(actualPickupTime);
		}
		else if ((time >= dropOffTime && time <= dropOffTime + timeRadius) && rPickedUp){
			distanceWithPassenger += requests.front()->getDistanceOfRequest();
			currentLocation.first = rDestination.first;
			currentLocation.second = rDestination.second;
		}

		//if (currentLocation.first == rDestination.first && currentLocation.second == rDestination.second && rPickedUp){
		//	requests.pop();
		//}
	}
}

void Vehicle::popTopRequest(){
	if (requests.size() == 0){
		throw "Popped on an empty queue!";
	}
	requests.pop();
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

std::pair<long, long> Vehicle::getPreviousRenderingLocation(){
	return previousLocation;
}

std::pair<float, float> Vehicle::getCurrentRenderingLocation(){
	return currentRenderingLocation;
}

bool Vehicle::checkRoutingLog(){
	return routingLog.size() > 0;
}

std::pair<int, std::pair<long, long>> Vehicle::getNextRoutingNode(){
	return routingLog.front();
}

bool Vehicle::getHasPassenger(){
	return hasPassenger;
}

void Vehicle::setHasPassenger(bool hasPassenger){
	this->hasPassenger = hasPassenger;
}

void Vehicle::prepareForRendering(){
	if (routingLog.size() != 0){
		currentRenderingLocation = routingLog.front().second;
		previousLocation = routingLog.front().second;
		previousTime = routingLog.front().first;
		routingLog.pop();
	}
}

void Vehicle::updateForRendering(float time){
	if (routingLog.size() != 0){
		currentRenderingLocation.first = motion(previousLocation.first, routingLog.front().second.first, time, routingLog.front().first);
		currentRenderingLocation.second = motion(previousLocation.second, routingLog.front().second.second, time, routingLog.front().first);
		bool a = routingLog.front().second.first >= previousLocation.first ? currentRenderingLocation.first >= routingLog.front().second.first : currentRenderingLocation.first <= routingLog.front().second.first;
		bool b = routingLog.front().second.second >= previousLocation.second ? currentRenderingLocation.second >= routingLog.front().second.second : currentRenderingLocation.second <= routingLog.front().second.second;
		if (a && b){
			currentRenderingLocation.first = routingLog.front().second.first;
			currentRenderingLocation.second = routingLog.front().second.second;
			previousLocation.first = currentRenderingLocation.first;
			previousLocation.second = currentRenderingLocation.second;
			previousTime = routingLog.front().first;
			routingLog.pop();
			updateForRendering(time);
		}
	}
}

float Vehicle::getRenderingAngle(){
	if (routingLog.size() != 0){
		float distY = routingLog.front().second.first - currentRenderingLocation.first;
		float distX = routingLog.front().second.second - currentRenderingLocation.second;
		float angle = atan(distY / distX);
		return angle;
	}
	return 0;
}