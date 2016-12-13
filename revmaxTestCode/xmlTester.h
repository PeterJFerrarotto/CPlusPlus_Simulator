#ifndef _XML_TESTER_H
#define _XML_TESTER_H
#include "RequestManager.h"
#include "RideRequest.h"
#include "Vehicle.h"
#include "dirent.h"
#include "rapidxml.hpp"
#include "rapidxml_utils.hpp"
#include <vector>
using namespace rapidxml;

class XMLTester{
protected:
	std::vector<Vehicle*> vehicles;
	RequestManager manager;

	float maxScore;
	float weightOfDistanceToRequest;
	float weightOfDistanceOfTrip;
	float weightOfRequestsAtDestination;

	inline xml_document<> * loadXMLFile(const char* filePath){
		file<>* xmlFile = new file<>(filePath);
		xml_document<>* doc = new xml_document<>;
		doc->parse<0>(xmlFile->data());
		return doc;
	}

	inline void enrichRequestData(xml_node<>* requestNode){
		float locLat, locLong;
		float destLat, destLong;

		if (requestNode->first_node("Location") == nullptr){
			throw "No request location!";
		}
		xml_node<>* locationNode = requestNode->first_node("Location");
		if (locationNode->first_attribute("long") == nullptr || locationNode->first_attribute("lat") == nullptr){
			throw "No latitude or longitude!";
		}
		locLat = std::stof(locationNode->first_attribute("lat")->value());
		locLong = std::stof(locationNode->first_attribute("long")->value());

		if (requestNode->first_node("Destination") == nullptr){
			throw "No request destination!";
		}
		xml_node<>* destinationNode = requestNode->first_node("Destination");
		if (destinationNode->first_attribute("long") == nullptr || destinationNode->first_attribute("lat") == nullptr){
			throw "No latitude or longitude!";
		}
		destLat = std::stof(destinationNode->first_attribute("lat")->value());
		destLong = std::stof(destinationNode->first_attribute("long")->value());

		RideRequest* request = new RideRequest;
		request->setLocation(locLat, locLong);
		request->setDestination(destLat, destLong);
		manager.addRequest(request);
	}

	inline void enrichVehicleData(xml_node<>* vehicleNode){
		float lat, longitude;
		if (vehicleNode->first_node("Location") == nullptr){
			throw "No location!";
		}

		xml_node<>* currentNode = vehicleNode->first_node("Location");
		if (currentNode->first_attribute("long") == nullptr || currentNode->first_attribute("lat") == nullptr){
			throw "No latitude or longitude!";
		}

		lat = std::stof(currentNode->first_attribute("lat")->value());
		longitude = std::stof(currentNode->first_attribute("long")->value());

		Vehicle* vehicle = new Vehicle;
		vehicle->setLocation(lat, longitude);
		vehicles.push_back(vehicle);
	}

	inline void enrichManagerData(xml_node<>* managerNode){
		int minLat = 0, minLong = 0;
		int maxLat = 20, maxLong = 20;
		int sectionSize = 5;

		if (managerNode->first_attribute("minLat") != nullptr){
			minLat = std::stoi(managerNode->first_attribute("minLat")->value());
		}

		if (managerNode->first_attribute("minLong") != nullptr){
			minLong = std::stoi(managerNode->first_attribute("minLong")->value());
		}

		if (managerNode->first_attribute("maxLat") != nullptr){
			maxLat = std::stoi(managerNode->first_attribute("maxLat")->value());
		}

		if (managerNode->first_attribute("maxLong") != nullptr){
			maxLong = std::stoi(managerNode->first_attribute("maxLong")->value());
		}

		if (managerNode->first_attribute("sectionSize") != nullptr){
			sectionSize = std::stoi(managerNode->first_attribute("sectionSize")->value());
		}
		manager.setLatitudeMin(minLat);
		manager.setLongitudeMin(minLong);
		manager.setLatitudeMax(maxLat);
		manager.setLongitudeMax(maxLong);
		manager.setSectionRadius(sectionSize);
		manager.initializeRequestMap();
	}

	void initialize(){
		DIR *dirp;
		struct dirent *dp;
		char fileName[360];
		char fileDirec[360] = "XML/";
		if ((dirp = opendir("./XML")) == NULL){
			throw "Could not find current directory!";
		}
		while ((dp = readdir(dirp)) != NULL){
			strcpy_s(fileName, dp->d_name);
			if (fileName[0] != '.' && fileName[1] != '.'){
				strcpy_s(fileDirec, "XML/");
				strcat_s(fileDirec, fileName);
				xml_document<>* doc = loadXMLFile(fileDirec);
				if (doc->first_node("Parameters") == nullptr){
					throw "Empty file!";
				}
				xml_node<>* parameters = doc->first_node("Parameters");
				if (parameters->first_node("RequestManager") == nullptr){
					throw "No request manager!";
				}
				enrichManagerData(parameters->first_node("RequestManager"));
				if (parameters->first_node("Requests") == nullptr){
					throw "No requests!";
				}
				xml_node<>* requestNode = parameters->first_node("Requests")->first_node("Request");
				do{
					enrichRequestData(requestNode);
					requestNode = requestNode->next_sibling("Request");
				} while (requestNode != nullptr);

				if (parameters->first_node("Vehicles") == nullptr){
					throw "No vehicles!";
				}
				xml_node<>* vehicleNode = parameters->first_node("Vehicles")->first_node("Vehicle");
				do{
					enrichVehicleData(vehicleNode);
					vehicleNode = vehicleNode->next_sibling("Vehicle");
				} while (vehicleNode != nullptr);
			}
		}
	}

public:
	XMLTester(float maxScore, float weightOfDistanceToRide, float weightOfDistanceOfRide, float weightOfRidesAtDestination) : maxScore(maxScore), weightOfDistanceToRequest(weightOfDistanceToRide), weightOfDistanceOfTrip(weightOfDistanceOfRide), weightOfRequestsAtDestination(weightOfRequestsAtDestination){
		initialize();
	}

	inline void runTest(){
		while (true){
			for (Vehicle* vehicle : vehicles){
				if (vehicle->getTopRequest() != nullptr){
					if (vehicle->getTopRequest()->getPickedUp()){
						vehicle->setLocation(vehicle->getTopRequest()->getDestination().first, vehicle->getTopRequest()->getDestination().second);
					}
					else{
						vehicle->setLocation(vehicle->getTopRequest()->getLocation().first, vehicle->getTopRequest()->getLocation().second);
					}
				}
				vehicle->update();
				std::vector<RideRequest*> requests = manager.getRequestsAtLocation(vehicle->getCurrentLocation());
				if (requests.size() != 0){
					int topScore = 0;
					std::vector<RideRequest*>::iterator highestScorer;
					for (std::vector<RideRequest*>::iterator request = requests.begin(); request != requests.end(); request++){
						if (scoreRequest(vehicle, *request, manager, maxScore, weightOfDistanceToRequest, weightOfDistanceOfTrip, weightOfRequestsAtDestination, &pythagDistance) > topScore){
							topScore = scoreRequest(vehicle, *request, manager, maxScore, weightOfDistanceToRequest, weightOfDistanceOfTrip, weightOfRequestsAtDestination, &pythagDistance);
							highestScorer = request;
							if (topScore >= maxScore){
								break;
							}
						}
					}
					if (*highestScorer != nullptr){
						vehicle->addRequest(*highestScorer);
						requests.erase(highestScorer);
					}
				}
			}
		}
	}
};

#endif