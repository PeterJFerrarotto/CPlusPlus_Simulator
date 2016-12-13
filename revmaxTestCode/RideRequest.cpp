#include "RideRequest.h"


RideRequest::RideRequest()
{
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

std::pair<long, long> RideRequest::getLocation(){
	return location;
}

std::pair<long, long> RideRequest::getDestination(){
	return destination;
}

bool RideRequest::getPickedUp(){
	return pickedUp;
}