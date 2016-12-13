#include "Vehicle.h"
#include "RequestManager.h"
#include "RideRequest.h"
#include "mathHelper.h"
#include "xmlTester.h"

int main(){
	//RequestManager manager(5, 20, 20, 0, 0);
	//RideRequest* x = new RideRequest;
	//x->setLocation(10.6, 7.9);
	//x->setDestination(13.6, 3.6);
	//manager.addRequest(x);

	//RideRequest* y = new RideRequest;
	//y->setLocation(13.8, 3.7);
	//y->setDestination(14, 14);
	//manager.addRequest(y);

	//Vehicle* vehicle = new Vehicle;
	//vehicle->setLocation(5, 5);

	//float score1 = scoreRequest(vehicle, x, manager, 100, 20, 10, 70, &pythagDistance);
	//float score2 = scoreRequest(vehicle, y, manager, 100, 20, 10, 70, &pythagDistance);
	//if (score1 > score2){
	//	vehicle->addRequest(x);
	//	score1 = 0;
	//}
	//else{
	//	vehicle->addRequest(y);
	//	score2 = 0;
	//}

	//if (score1 == 0){
	//	score2 = scoreRequest(vehicle, y, manager, 100, 20, 10, 70, &pythagDistance);
	//	vehicle->addRequest(y);
	//}
	//else if (score2 == 0){
	//	score1 = scoreRequest(vehicle, x, manager, 100, 20, 10, 70, &pythagDistance);
	//	vehicle->addRequest(x);
	//}

	XMLTester tester(100, 10, 20, 70);
	tester.runTest();
}