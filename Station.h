#pragma once
#include "Train.h"
#include "String.h"
#include "Vector.hpp"
#include <ctime>
#include <fstream>
#include <iostream>

class Station {
    String name;
    Vector<int> platforms;
    Vector<Train*> departingTrains;
    Vector<Train*> arrivingTrains;

public:
    Station() = default;
    explicit Station(const String& stationName);

    Station(const Station& other) :
        name(other.name),
        platforms(other.platforms),
        departingTrains(other.departingTrains.get_size()),
        arrivingTrains(other.arrivingTrains.get_size())
    {
        
        for (size_t i = 0; i < other.departingTrains.get_size(); i++) {
            departingTrains.push_back(new Train(*other.departingTrains[i]));
        }
        for (size_t i = 0; i < other.arrivingTrains.get_size(); i++) {
            arrivingTrains.push_back(new Train(*other.arrivingTrains[i]));
        }
    }

    Station& operator=(const Station& other) {
        if (this != &other) {
           
            for (size_t i = 0; i < departingTrains.get_size(); i++) {
                delete departingTrains[i];
            }
            for (size_t i = 0; i < arrivingTrains.get_size(); i++) {
                delete arrivingTrains[i];
            }

            name = other.name;
            platforms = other.platforms;

            departingTrains = Vector<Train*>(other.departingTrains.get_size());
            arrivingTrains = Vector<Train*>(other.arrivingTrains.get_size());

            for (size_t i = 0; i < other.departingTrains.get_size(); i++) {
                departingTrains.push_back(new Train(*other.departingTrains[i]));
            }
            for (size_t i = 0; i < other.arrivingTrains.get_size(); i++) {
                arrivingTrains.push_back(new Train(*other.arrivingTrains[i]));
            }
        }
        return *this;
    }

    ~Station() {
        std::cout << "Station destructor for " << name.c_str() << std::endl;
        departingTrains.clear();
        arrivingTrains.clear();
    }

    bool addDepartingTrain(Train* train);
    bool removeDepartingTrain(int trainId);

    void addArrivingTrain(Train* train);

    void printSchedule() const;
    void printArrivalBoard() const;
    void printDepartureBoard() const;

    int assignDeparturePlatform();
    int assignArrivalPlatform();

    const String& getName() const { return name; }
    size_t getPlatformCount() const { return platforms.get_size(); }
    size_t getDepartureCount() const { return departingTrains.get_size(); }
    const Train* getDepartureTrain(size_t index) const {
        if (index < departingTrains.get_size()) {
            return departingTrains[index];
        }
        return nullptr;
    }

    size_t getArrivalCount() const { return arrivingTrains.get_size(); }
    const Train* getArrivalTrain(size_t index) const {
        if (index < arrivingTrains.get_size()) {
            return arrivingTrains[index];
        }
        return nullptr;
    }

    void saveToFile(const String& filename) const;
    void loadFromFile(const String& filename);
    void toFile(std::ofstream& out) const;  
    void fromFile(std::ifstream& in);

private:
    bool trainExists(int trainId) const;
};