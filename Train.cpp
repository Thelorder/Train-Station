#include "Train.h"
#include <fstream>
#include "FirstClassWagon.h"
#include "SecondClassWagon.h"
#include "SleepingWagon.h"
#include <iostream>
#include <ctime>
#pragma warning(disable : 4996)


void Train::writeStringToFile(std::ofstream& file, const String& str) const
{
    file << str.c_str();
}

Train::Train(int id, const String& start, const String& end, float distance, float speed)
    : id(id), startStation(start), endStation(end), distance(distance), speed(speed) {}

Train::~Train() {
    std::cout << "Destroying Train " << id << std::endl;
    for (size_t i = 0; i < wagons.get_size(); ++i) {
        delete wagons[i];
    }
}

void Train::addWagon(Wagon* wagon) {
    if (wagon) wagons.push_back(wagon);
}

bool Train::removeWagon(int wagonId) {
    for (size_t i = 0; i < wagons.get_size(); ++i) {
        if (wagons[i]->getId() == wagonId) {
            delete wagons[i];
            wagons.remove(i);
            return true;
        }
    }
    return false;
}

Wagon* Train::getWagon(int wagonId) const {
    for (size_t i = 0; i < wagons.get_size(); ++i) {
        if (wagons[i]->getId() == wagonId) {
            return wagons[i];
        }
    }
    return nullptr;
}

void Train::saveToFile(const String& filename) const {
    std::ofstream file(filename.c_str());
    if (!file) return;

    file << "TRAIN_ID:" << id << "\n"
        << "START:" << startStation.c_str() << "\n"
        << "END:" << endStation.c_str() << "\n"
        << "DISTANCE:" << distance << "\n"
        << "SPEED:" << speed << "\n"
        << "WAGONS:" << wagons.get_size() << "\n";

    for (size_t i = 0; i < wagons.get_size(); ++i) {
        char wagonFile[20];
        snprintf(wagonFile, 20, "%d_wagon.txt", wagons[i]->getId());
        wagons[i]->saveToFile(wagonFile);

        file << "WAGON:" << wagons[i]->getId() << "\n";
    }
}

bool startsWith(const String& str, const char* prefix) {
    const char* s = str.c_str();
    while (*prefix) {
        if (*prefix++ != *s++) return false;
    }
    return true;
}

void Train::loadFromFile(const String& filename) {
    std::ifstream file(filename.c_str());
    if (!file) {
        std::cerr << "Error: Failed to open train file\n";
        return;
    }

    for (size_t i = 0; i < wagons.get_size(); ++i) {
        delete wagons[i];
    }
    wagons.clear();

    String line;
    while ((line = String::read_line(file)) != "") {
        if (line.size() > 9 && line.substr(0, 9) == "TRAIN_ID:") {
            id = line.substr(9).to_int();
        }
        else if (line.size() > 6 && line.substr(0, 6) == "START:") {
            startStation = line.substr(6);
        }
        else if (line.size() > 4 && line.substr(0, 4) == "END:") {
            endStation = line.substr(4);
        }
        else if (line.size() > 9 && line.substr(0, 9) == "DISTANCE:") {
            distance = line.substr(9).to_float();
        }
        else if (line.size() > 6 && line.substr(0, 6) == "SPEED:") {
            speed = line.substr(6).to_float();
        }
        else if (line.size() > 6 && line.substr(0, 6) == "WAGON:") {
            int wagonId = line.substr(6).to_int();
            String wagonFile = String::from_int(wagonId) + "_wagon.txt";

            std::ifstream wfile(wagonFile.c_str());
            if (!wfile) {
                std::cerr << "Warning: Missing wagon file " << wagonFile.c_str() << "\n";
                continue;
            }

            String typeHeader = String::read_line(wfile);
            Wagon* wagon = nullptr;

            if (typeHeader == "=== FIRST CLASS WAGON ===") {
                wagon = new FirstClassWagon(wagonId, 0.0, 0.0); 
            }
            else if (typeHeader == "=== SECOND CLASS WAGON ===") {
                wagon = new SecondClassWagon(wagonId, 0.0, 0.0); 
            }
            else if (typeHeader == "=== SLEEPING WAGON ===") {
                wagon = new SleepingWagon(wagonId, 0.0, 0.0); 
            }

            if (wagon) {
                wagon->loadFromFile(wagonFile);
                wagons.push_back(wagon);
            }
            else {
                std::cerr << "Warning: Invalid wagon type in " << wagonFile.c_str() << "\n";
            }
        }
    }
}

int Train::getId() const
{
    return id;
}

const String& Train::getStartStation() const
{
    return startStation;
}

const String& Train::getEndStation() const
{
    return endStation;
}

float Train::getDistace() const
{
    return distance;
}

float Train::getSpeed() const
{
    return speed;
}

size_t Train::getWagonCount() const
{
    return wagons.get_size();
}

void Train::setDepartureTime(const String& time) {
    departureTime = time;
    calculateArrivalTime(); 
}

void Train::setArrivalTime(const String& time) {
    arrivalTime = time;
}

float Train::calculateTravelTime() const {
    return distance / speed; 
}

void Train::setDeparturePlatform(int platform) {
    departurePlatform = platform;
}

void Train::setArrivalPlatform(int platform) {
    arrivalPlatform = platform;
}

int Train::getDeparturePlatform() const {
    return departurePlatform;
}

int Train::getArrivalPlatform() const {
    return arrivalPlatform;
}

bool Train::hasDeparted() const {
    if (departureTime.empty()) return false;

    time_t now;
    time(&now);

    int day = (departureTime[0] - '0') * 10 + (departureTime[1] - '0');
    int month = (departureTime[3] - '0') * 10 + (departureTime[4] - '0');
    int year = (departureTime[6] - '0') * 1000 + (departureTime[7] - '0') * 100 +
        (departureTime[8] - '0') * 10 + (departureTime[9] - '0');
    int hour = (departureTime[11] - '0') * 10 + (departureTime[12] - '0');
    int minute = (departureTime[14] - '0') * 10 + (departureTime[15] - '0');

    struct tm tm = { 0 };
    tm.tm_mday = day;
    tm.tm_mon = month - 1;  
    tm.tm_year = year - 1900; 
    tm.tm_hour = hour;
    tm.tm_min = minute;
    time_t depTime = mktime(&tm);

    return difftime(depTime, now) < 0;
}

bool Train::hasArrived() const {
    if (arrivalTime.empty()) return false;

    time_t now;
    time(&now);

    int day = (arrivalTime[0] - '0') * 10 + (arrivalTime[1] - '0');
    int month = (arrivalTime[3] - '0') * 10 + (arrivalTime[4] - '0');
    int year = (arrivalTime[6] - '0') * 1000 + (arrivalTime[7] - '0') * 100 +
        (arrivalTime[8] - '0') * 10 + (arrivalTime[9] - '0');
    int hour = (arrivalTime[11] - '0') * 10 + (arrivalTime[12] - '0');
    int minute = (arrivalTime[14] - '0') * 10 + (arrivalTime[15] - '0');

    struct tm tm = { 0 };
    tm.tm_mday = day;
    tm.tm_mon = month - 1;
    tm.tm_year = year - 1900;
    tm.tm_hour = hour;
    tm.tm_min = minute;
    time_t arrTime = mktime(&tm);

    return difftime(arrTime, now) < 0;
}


void Train::calculateArrivalTime() {
    if (departureTime.empty() || distance <= 0 || speed <= 0) return;

    int day = (departureTime[0] - '0') * 10 + (departureTime[1] - '0');
    int month = (departureTime[3] - '0') * 10 + (departureTime[4] - '0');
    int year = (departureTime[6] - '0') * 1000 + (departureTime[7] - '0') * 100 +
        (departureTime[8] - '0') * 10 + (departureTime[9] - '0');
    int hour = (departureTime[11] - '0') * 10 + (departureTime[12] - '0');
    int minute = (departureTime[14] - '0') * 10 + (departureTime[15] - '0');

    struct tm tm = { 0 };
    tm.tm_mday = day;
    tm.tm_mon = month - 1;
    tm.tm_year = year - 1900;
    tm.tm_hour = hour;
    tm.tm_min = minute;
    time_t depTime = mktime(&tm);

    double hours = distance / speed;
    time_t arrTime = depTime + static_cast<time_t>(hours * 3600);

    struct tm* arrTm = localtime(&arrTime);

    char formattedTime[17]; 

    formattedTime[0] = '0' + (arrTm->tm_mday / 10);
    formattedTime[1] = '0' + (arrTm->tm_mday % 10);
    formattedTime[2] = '/';
    formattedTime[3] = '0' + ((arrTm->tm_mon + 1) / 10);
    formattedTime[4] = '0' + ((arrTm->tm_mon + 1) % 10);
    formattedTime[5] = '/';
    int y = arrTm->tm_year + 1900;
    formattedTime[6] = '0' + (y / 1000);
    formattedTime[7] = '0' + ((y % 1000) / 100);
    formattedTime[8] = '0' + ((y % 100) / 10);
    formattedTime[9] = '0' + (y % 10);
    formattedTime[10] = ' ';
    formattedTime[11] = '0' + (arrTm->tm_hour / 10);
    formattedTime[12] = '0' + (arrTm->tm_hour % 10);
    formattedTime[13] = ':';
    formattedTime[14] = '0' + (arrTm->tm_min / 10);
    formattedTime[15] = '0' + (arrTm->tm_min % 10);
    formattedTime[16] = '\0';

    arrivalTime = String(formattedTime);
}

void Train::toFile(std::ofstream& out) const {
    out << "TRAIN_ID:" << id << "\n"
        << "START:" << startStation.c_str() << "\n"
        << "END:" << endStation.c_str() << "\n"
        << "DISTANCE:" << distance << "\n"
        << "SPEED:" << speed << "\n"
        << "DEPARTURE:" << departureTime.c_str() << "\n"
        << "ARRIVAL:" << arrivalTime.c_str() << "\n"
        << "DEPARTURE_PLATFORM:" << departurePlatform << "\n"
        << "ARRIVAL_PLATFORM:" << arrivalPlatform << "\n"
        << "WAGON_COUNT:" << wagons.get_size() << "\n";

    for (size_t i = 0; i < wagons.get_size(); ++i) {
        out << "WAGON_ID:" << wagons[i]->getId() << "\n";
    }
}


void Train::fromFile(std::ifstream& in) {
    String line;
    while ((line = String::read_line(in)) != "") {
        if (startsWith(line, "TRAIN_ID:")) {
            id = String(line.c_str() + 9).to_int();
        }
        else if (startsWith(line, "START:")) {
            startStation = String(line.c_str() + 6);
        }
        else if (startsWith(line, "END:")) {
            endStation = String(line.c_str() + 4);
        }
        else if (startsWith(line, "DISTANCE:")) {
            distance = String(line.c_str() + 9).to_float();
        }
        else if (startsWith(line, "SPEED:")) {
            speed = String(line.c_str() + 6).to_float();
        }
        else if (startsWith(line, "DEPARTURE:")) {
            departureTime = String(line.c_str() + 10);
        }
        else if (startsWith(line, "ARRIVAL:")) {
            arrivalTime = String(line.c_str() + 8);
        }
        else if (startsWith(line, "DEPARTURE_PLATFORM:")) {
            departurePlatform = String(line.c_str() + 19).to_int();
        }
        else if (startsWith(line, "ARRIVAL_PLATFORM:")) {
            arrivalPlatform = String(line.c_str() + 17).to_int();
        }
        else if (startsWith(line, "WAGON_COUNT:")) {
            size_t wagonCount = String(line.c_str() + 12).to_int();

            for (size_t i = 0; i < wagons.get_size(); ++i) {
                delete wagons[i];
            }
            wagons.clear();

            for (size_t i = 0; i < wagonCount; ++i) {
                String wagonLine = String::read_line(in);
                if (startsWith(wagonLine, "WAGON_ID:")) {
                    int wagonId = String(wagonLine.c_str() + 9).to_int();
                    char wagonFile[20];
                    snprintf(wagonFile, 20, "%d_wagon.txt", wagonId);

                    std::ifstream wfile(wagonFile);
                    if (wfile) {
                        String wagonType = String::read_line(wfile);
                        Wagon* wagon = nullptr;

                        if (wagonType == "=== FIRST CLASS WAGON ===") {
                            wagon = new FirstClassWagon();
                        }
                        else if (wagonType == "=== SECOND CLASS WAGON ===") {
                            wagon = new SecondClassWagon();
                        }
                        else if (wagonType == "=== SLEEPING WAGON ===") {
                            wagon = new SleepingWagon();
                        }

                        if (wagon) {
                            wagon->loadFromFile(wagonFile);
                            wagons.push_back(wagon);
                        }
                    }
                }
            }
        }
    }
}