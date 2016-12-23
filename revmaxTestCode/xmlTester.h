#ifndef _XML_TESTER_H
#define _XML_TESTER_H
#include "RequestManager.h"
#include "RideRequest.h"
#include "Vehicle.h"
#include "dirent.h"
#include "rapidxml.hpp"
#include "rapidxml_utils.hpp"
#include "EventVenue.h"
#include <vector>
#include <iostream>
using namespace rapidxml;

#if defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64) || defined(WIN82) || defined(_WIN82)
#define RESOURCE_FOLDER ""
#else
#define RESOURCE_FOLDER "NYUCodebase.app/Contents/Resources/"
#endif



class XMLTester{
protected:
	std::unordered_map<std::string, std::vector<Vehicle*>> vehicles;
	std::unordered_map<std::string, RequestManager> managers;
	std::vector<std::string> tests;
	std::string currentTest;

	std::unordered_map<std::string, float> optimalScores;
	std::unordered_map<std::string, float> weightOfDistanceToRequest;
	std::unordered_map<std::string, float> weightOfDistanceOfTrip;
	std::unordered_map<std::string, float> weightOfRequestsAtDestination;
	std::unordered_map<std::string, unsigned> timesToRun;

	inline xml_document<> * loadXMLFile(const char* filePath){
		file<>* xmlFile = new file<>(filePath);
		xml_document<>* doc = new xml_document<>;
		doc->parse<0>(xmlFile->data());
		return doc;
	}

	inline void enrichRequestData(xml_node<>* requestNode){
		float locLat, locLong;
		float destLat, destLong;
		int time;

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

		if (requestNode->first_node("RequestTime") == nullptr){
			throw "No time specified for request!";
		}
		time = std::stoi(requestNode->first_node("RequestTime")->value());
		RideRequest* request = new RideRequest;
		request->setLocation(locLat, locLong);
		request->setDestination(destLat, destLong);
		request->setRequestTime(time);
		managers[currentTest].addRequest(request);
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
		vehicles[currentTest].push_back(vehicle);
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
		
		managers[currentTest].setLatitudeMin(minLat);
		managers[currentTest].setLongitudeMin(minLong);
		managers[currentTest].setLatitudeMax(maxLat);
		managers[currentTest].setLongitudeMax(maxLong);
		managers[currentTest].setSectionRadius(sectionSize);
		managers[currentTest].initializeRequestMap();
	}

	inline void enrichVenueData(xml_node<>* venueNode){
		float latitude = 0, longitude = 0;
		EventVenue* venue = new EventVenue();
		if (venueNode->first_node("Location") == nullptr){
			throw "No location specified for venue!";
		}

		xml_node<>* locationNode = venueNode->first_node("Location");
		if (locationNode->first_attribute("long") == nullptr || locationNode->first_attribute("lat") == nullptr){
			throw "No latitude or longitude!";
		}

		latitude = std::stof(locationNode->first_attribute("lat")->value());
		longitude = std::stof(locationNode->first_attribute("long")->value());

		if (venueNode->first_node("Events") != nullptr){
			xml_node<>* requestNode = venueNode->first_node("Events")->first_node("Event");
			if (requestNode != nullptr){
				do{
					int time, requestCount;
					if (requestNode->first_attribute("time") == nullptr || requestNode->first_attribute("requestCount") == nullptr){
						throw "No time or request count specified for event!";
					}
					time = std::stoi(requestNode->first_attribute("time")->value());
					requestCount = std::stoi(requestNode->first_attribute("requestCount")->value());
					venue->addEvent(time, requestCount);
					requestNode = requestNode->next_sibling("Event");
				} while (requestNode != nullptr);
			}
		}
		venue->setLocation(latitude, longitude);
		managers[currentTest].addVenue(venue);
	}

	inline void initialize(){
		DIR *dirp;
		struct dirent *dp;
		char fileName[360];
		char fileDirec[360] = RESOURCE_FOLDER"XML/";
		if ((dirp = opendir(fileDirec)) == NULL){
			throw "Could not find current directory!";
		}
		while ((dp = readdir(dirp)) != NULL){
			strcpy_s(fileName, dp->d_name);
			if (fileName[0] != '.' && fileName[1] != '.'){
				strcpy_s(fileDirec, "XML/");
				strcat_s(fileDirec, fileName);
				currentTest = fileName;
				currentTest = currentTest.substr(0, currentTest.find_first_of('.'));
				tests.push_back(currentTest);
				optimalScores[currentTest] = 100;
				weightOfDistanceToRequest[currentTest] = 10;
				weightOfDistanceOfTrip[currentTest] = 20;
				weightOfRequestsAtDestination[currentTest] = 70;
				timesToRun[currentTest] = 10;
				xml_document<>* doc = loadXMLFile(fileDirec);
				if (doc->first_node("Parameters") == nullptr){
					throw "Empty file!";
				}
				xml_node<>* parameters = doc->first_node("Parameters");
				if (parameters->first_attribute("OptimalScore") != nullptr){
					optimalScores[currentTest] = std::stof(parameters->first_attribute("OptimalScore")->value());
				}
				if (parameters->first_attribute("WeightOfDistanceToRequest") != nullptr){
					weightOfDistanceToRequest[currentTest] = std::stof(parameters->first_attribute("WeightOfDistanceToRequest")->value());
				}
				if (parameters->first_attribute("WeightOfDistanceOfTrip") != nullptr){
					weightOfDistanceOfTrip[currentTest] = std::stof(parameters->first_attribute("WeightOfDistanceOfTrip")->value());
				}
				if (parameters->first_attribute("WeightOfRequestsAtDestination") != nullptr){
					weightOfRequestsAtDestination[currentTest] = std::stof(parameters->first_attribute("WeightOfRequestsAtDestination")->value());
				}
				if (parameters->first_attribute("TimesToRun") != nullptr){
					timesToRun[currentTest] = std::stoi(parameters->first_attribute("TimesToRun")->value());
				}

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

				if (parameters->first_node("Venues") != nullptr){
					xml_node<>* venueNode = parameters->first_node("Venues")->first_node("Venue");
					do{
						enrichVenueData(venueNode);
						venueNode = venueNode->next_sibling("Venue");
					} while (venueNode != nullptr);
				}
			}
		}
	}

public:
	XMLTester(){
		initialize();
	}

	inline void runTests(){
		for (size_t j = 0; j < tests.size(); j++){
			int currentTime = 1;
			currentTest = tests[j];
			char resultsFile[360] = RESOURCE_FOLDER"Results/";
			strcat_s(resultsFile, currentTest.c_str());
			strcat_s(resultsFile, ".txt");
			std::ofstream outputFile;
			outputFile.open(resultsFile);
			std::cout << "Test " << currentTest << ":" << '\n' << '\n' << '\n';
			outputFile << "Test " << currentTest << '\n';
			outputFile << "Paraeters:" << '\n';
			outputFile << '\t' << "Max Score: " << optimalScores[currentTest] << '\n';
			outputFile << '\t' << "Weight of Distance to Request: " << weightOfDistanceToRequest[currentTest] << '\n';
			outputFile << '\t' << "Weight of Distance of Request: " << weightOfDistanceOfTrip[currentTest] << '\n';
			outputFile << '\t' << "Weight of Requests at Destination: " << weightOfRequestsAtDestination[currentTest] << '\n';
			outputFile << '\t' << "Times to run: " << timesToRun[currentTest] << '\n' << '\n';
			for (int i = 1; i <= timesToRun[currentTest]; i++){
				std::cout << "Test loop " << i << " of " << timesToRun[currentTest] << '\n' << '\n';
				outputFile << "Test loop " << i << " of " << timesToRun[currentTest] << '\n' << '\n';
				int vehicleNum = 1;
				for (Vehicle* vehicle : vehicles[currentTest]){
					std::cout << "Vehicle: " << vehicleNum << '\n';
					outputFile << "Vehicle: " << vehicleNum << '\n';
					vehicle->update(currentTime);
					std::vector<RideRequest*> requests = managers[currentTest].getRequestsAtLocation(vehicle->getCurrentLocation());
					if (requests.size() != 0){
						float topScore = 0;
						std::vector<RideRequest*>::iterator highestScorer;
						for (std::vector<RideRequest*>::iterator request = requests.begin(); request != requests.end(); request++){
							if (scoreRequest(vehicle, *request, managers[currentTest], currentTime, optimalScores[currentTest], weightOfDistanceToRequest[currentTest], weightOfDistanceOfTrip[currentTest], weightOfRequestsAtDestination[currentTest], &pythagDistance) > topScore){
								topScore = scoreRequest(vehicle, *request, managers[currentTest], currentTime, optimalScores[currentTest], weightOfDistanceToRequest[currentTest], weightOfDistanceOfTrip[currentTest], weightOfRequestsAtDestination[currentTest], &pythagDistance);
								highestScorer = request;
								if (topScore >= optimalScores[currentTest]){
									break;
								}
							}
						}
						if (topScore != 0 && *highestScorer != nullptr){
							int timeToUse = currentTime;
							if (vehicle->getTopRequest() != nullptr){
								timeToUse = vehicle->getTopRequest()->getRequestTime() + vehicle->getTopRequest()->getDistanceOfRequest();
							}
							vehicle->addRequest(*highestScorer);
							outputFile << "Added passenger to vehicle " << vehicleNum << "'s queue at T = " << currentTime << ". Passenger info:" << '\n';
							outputFile << '\t' << "Pickup Location: " << (*highestScorer)->getLocation().first << ", " << (*highestScorer)->getLocation().second << '\n';
							outputFile << '\t' << "Pickup Time: " << (*highestScorer)->getRequestTime() << '\n';
							outputFile << '\t' << "Destination Location: " << (*highestScorer)->getDestination().first << ", " << (*highestScorer)->getDestination().second << '\n';
							outputFile << '\t' << "Distance to pickup from next location: " << (*highestScorer)->getDistanceToRequest() << '\n';
							outputFile << '\t' << "Distance from pickup to dropoff: " << (*highestScorer)->getDistanceOfRequest() << '\n';
							outputFile << '\t' << "Dropoff at T = " << (*highestScorer)->getDistanceOfRequest() + timeToUse << '\n';
							outputFile << '\t' << "Requests available at destination: " << (*highestScorer)->getRequestsAtDestination() << '\n';
							outputFile << '\t' << "Request Score: " << topScore << '\n';
							requests.erase(highestScorer);
						}
						else{
							if (vehicle->getTopRequest() != nullptr){
								if (vehicle->getTopRequest()->getPickedUp()){
									if (currentTime == vehicle->getTopRequest()->getRequestTime()){
										outputFile << "Vehicle picked up request." << '\n';
									}
									else if (currentTime == vehicle->getTopRequest()->getDistanceOfRequest() + vehicle->getTopRequest()->getRequestTime()){
										outputFile << "Vehicle dropped off request." << '\n';
									}
									else{
										outputFile << "Vehicle en route to destination." << '\n';
									}
								}
								else{
									outputFile << "Vehicle en route to pickup." << '\n';
								}
							}
							else{
								outputFile << "Vehicle idling." << '\n';
							}
						}
					}
					else {
						outputFile << "No new requests!" << '\n';
					}
					vehicleNum++;
					outputFile << '\n';
				}
				currentTime++;
				outputFile << '\n';
			}
			outputFile << "-------------------------------------------------------------------" << '\n';
			outputFile << "Complete Test Results:" << '\n' << '\n';
			int vehicleNum = 1;
			for (Vehicle* vehicle : vehicles[currentTest]){
				outputFile << "Vehicle: " << vehicleNum << '\n';
				outputFile << '\t' << "Distance travelled with a passenger: " << vehicle->getDistanceWithPassenger() << '\n';
				outputFile << '\t' << "Distance travelled without a passenger: " << vehicle->getDistanceWithoutPassenger() << '\n';
				outputFile << '\n' << '\n';
			}
			outputFile.close();
		}
	}
};

#endif