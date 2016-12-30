#include "EventVenue.h"

EventVenue::EventVenue()
{
}


EventVenue::~EventVenue()
{
}

void EventVenue::setLocation(long latitude, long longitude){
	location.first = latitude;
	location.second = longitude;
}

void EventVenue::addEvent(int time, int projectedRequestCount){
	if (events.find(time) != events.end()){
		events[time] += projectedRequestCount;
	}
	else{
		events[time] = projectedRequestCount;
	}
}

std::pair<long, long> EventVenue::getLocation(){
	return location;
}

int EventVenue::getProjectedRequests(int time, int timeRadius){
	int eventCount = 0;
	for (int i = time - timeRadius; i <= time + timeRadius; i++){
		if (events.find(i) != events.end()){
			eventCount += events[i];
		}
	}
	return eventCount;
}