#include "RideRequest.h"


RideRequest::RideRequest()
{
	distanceOfRequestCalculated = false;
	pickedUp = false;
}


RideRequest::~RideRequest()
{
}

void RideRequest::setLocation(long latitude, long longitude){
	location.first = latitude;
	location.second = longitude;
}

void RideRequest::setDestination(long latitude, long longitude){
	destination.first = latitude;
	destination.second = longitude;
}

void RideRequest::setPickedUp(bool pickedUp){
	this->pickedUp = pickedUp;
}

void RideRequest::setDistanceToRequest(long distanceToRequest){
	this->distanceToRequest = distanceToRequest;
}

void RideRequest::setDistanceOfRequest(long distanceOfRequest){
	this->distanceOfRequest = distanceOfRequest;
	distanceOfRequestCalculated = true;
}

void RideRequest::setRequestsAtDestination(unsigned requestsAtDestination){
	this->requestsAtDestination = requestsAtDestination;
}

void RideRequest::setRequestTime(int time){
	requestTime = time;
}

std::pair<long, long> RideRequest::getLocation(){
	return location;
}

std::pair<long, long> RideRequest::getDestination(){
	return destination;
}

bool RideRequest::getPickedUp(){
	return pickedUp;
}

long RideRequest::getDistanceToRequest(){
	return distanceToRequest;
}

long RideRequest::getDistanceOfRequest(){
	return distanceOfRequest;
}

bool RideRequest::getDistanceOfRequestCalculated(){
	return distanceOfRequestCalculated;
}

unsigned RideRequest::getRequestsAtDestination(){
	return requestsAtDestination;
}

int RideRequest::getRequestTime(){
	return requestTime;
}