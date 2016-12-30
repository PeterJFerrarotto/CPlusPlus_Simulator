#ifndef _EVENT_VENUE_H
#define _EVENT_VENUE_H
#include "RideRequest.h"
#include <map>

class EventVenue
{
private:
	//Fixed value of the location of the venue: I doubt a venue will be moving anytime soon. Unless it's a mobile art show or something.
	std::pair<long, long> location;
	//Map of time to the number of projected requests per the venue's capacity and expected turnout
	//Application to later code: can use number of tickets sold for the event (assuming this is public information) to populate the projected events
	std::map<int, int> events;
	
public:
	EventVenue();
	~EventVenue();

	void setLocation(long latitude, long longitude);
	void addEvent(int time, int projectedRequestCount);

	std::pair<long, long> getLocation();

	int getProjectedRequests(int time, int timeRadius);
};

#endif