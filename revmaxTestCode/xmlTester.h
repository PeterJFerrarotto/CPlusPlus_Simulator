#ifndef _XML_TESTER_H
#define _XML_TESTER_H
#include "RequestManager.h"
#include "RideRequest.h"
#include "Vehicle.h"
#include "dirent.h"
#include "rapidxml.hpp"
#include "rapidxml_utils.hpp"
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
			}
		}
	}

public:
	XMLTester(){
		initialize();
	}

	inline void runTests(){
		for (size_t j = 0; j < tests.size(); j++){
			currentTest = tests[j];
			char resultsFile[360] = RESOURCE_FOLDER"Results/";
			strcat_s(resultsFile, currentTest.c_str());
			strcat_s(resultsFile, ".txt");
			std::ofstream outputFile;
			outputFile.open(resultsFile);
			std::cout << "Test " << currentTest << ":" << '\n' << '\n' << '\n';
			outputFile << "Test " << currentTest << '\n';
			outputFile << "Paraeters:" << '\n';
			outputFile << '\t' << "Max Score:" << optimalScores[currentTest] << '\n';
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
					//if (vehicle->getTopRequest() != nullptr){
					//	if (vehicle->getTopRequest()->getPickedUp()){
					//		vehicle->setLocation(vehicle->getTopRequest()->getDestination().first, vehicle->getTopRequest()->getDestination().second);
					//		outputFile << "Vehicle " << vehicleNum << " dropped off passenger at " << vehicle->getTopRequest()->getDestination().first << ", " << vehicle->getTopRequest()->getDestination().second << ".\n";
					//	}
					//	else{
					//		vehicle->setLocation(vehicle->getTopRequest()->getLocation().first, vehicle->getTopRequest()->getLocation().second);
					//		outputFile << "Vehicle " << vehicleNum << " picked up passenger at " << vehicle->getTopRequest()->getLocation().first << ", " << vehicle->getTopRequest()->getLocation().second << ".\n";
					//	}
					//}
					vehicle->update();
					std::vector<RideRequest*> requests = managers[currentTest].getRequestsAtLocation(vehicle->getCurrentLocation());
					if (requests.size() != 0){
						float topScore = 0;
						std::vector<RideRequest*>::iterator highestScorer;
						for (std::vector<RideRequest*>::iterator request = requests.begin(); request != requests.end(); request++){
							if (scoreRequest(vehicle, *request, managers[currentTest], optimalScores[currentTest], weightOfDistanceToRequest[currentTest], weightOfDistanceOfTrip[currentTest], weightOfRequestsAtDestination[currentTest], &pythagDistance) > topScore){
								topScore = scoreRequest(vehicle, *request, managers[currentTest], optimalScores[currentTest], weightOfDistanceToRequest[currentTest], weightOfDistanceOfTrip[currentTest], weightOfRequestsAtDestination[currentTest], &pythagDistance);
								highestScorer = request;
								if (topScore >= optimalScores[currentTest]){
									break;
								}
							}
						}
						if (topScore != 0 && *highestScorer != nullptr){
							vehicle->addRequest(*highestScorer);
							outputFile << "Added passenger to vehicle " << vehicleNum << "'s queue. Passenger info:" << '\n';
							outputFile << '\t' << "Pickup Location: " << (*highestScorer)->getLocation().first << ", " << (*highestScorer)->getLocation().second << '\n';
							outputFile << '\t' << "Destination Location: " << (*highestScorer)->getDestination().first << ", " << (*highestScorer)->getDestination().second << '\n';
							outputFile << '\t' << "Distance to pickup from next location: " << (*highestScorer)->getDistanceToRequest() << '\n';
							outputFile << '\t' << "Distance from pickup to dropoff: " << (*highestScorer)->getDistanceOfRequest() << '\n';
							outputFile << '\t' << "Requests available at destination: " << (*highestScorer)->getRequestsAtDestination() << '\n';
							outputFile << '\t' << "Request Score: " << topScore << '\n';
							requests.erase(highestScorer);
						}
						else{
							outputFile << "Vehicle idling." << '\n';
						}
					}
					else {
						outputFile << "No new requests!" << '\n';
					}
					vehicleNum++;
					outputFile << '\n';
				}
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