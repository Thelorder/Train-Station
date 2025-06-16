#pragma once
#include "String.h"
#include "Vector.hpp"
#include "Wagon.h"	
#include <ctime>
#include "FirstClassWagon.h"
#include "SecondClassWagon.h"
#include "SleepingWagon.h"


class Train {
private:
	int id;
	String startStation;
	String endStation;
	float distance;
	float speed;
	String departureTime;
	String arrivalTime;
	int departurePlatform;
	int arrivalPlatform;
	Vector<Wagon*> wagons;

	void writeStringToFile(std::ofstream& file, const String& str) const;
public:

	Train() : id(0), startStation(""), endStation(""), distance(0), speed(0) {}
	Train(int id, const String& start, const String& end, float distance, float speed);
	~Train();

	void setDeparturePlatform(int platform);
	void setArrivalPlatform(int platform);
	int getDeparturePlatform() const;
	int getArrivalPlatform() const;

	void addWagon(Wagon* wagon);
	bool removeWagon(int wagonId);
	Wagon* getWagon(int wagonId) const;

	void saveToFile(const String& filename)const;
	void loadFromFile(const String& filename);
	void printDetails() const;

	int getId() const;
	const String& getStartStation() const;
	const String& getEndStation() const;
	float getDistace() const;
	float getSpeed() const;
	size_t getWagonCount() const;
	const String& getDepartureTime() const { return departureTime; }
	const String& getArrivalTime() const { return arrivalTime; }

	void setDepartureTime(const String& time);
	void setArrivalTime(const String& time);

	bool hasDeparted() const;
	bool hasArrived() const;

	void calculateArrivalTime();
	float calculateTravelTime() const;

	void toFile(std::ofstream& out) const;
	void fromFile(std::ifstream& in);

};