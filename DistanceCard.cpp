#include "DistanceCard.h"
#include <fstream>

DistanceCard::DistanceCard(const String& name, int number, float reqDist, float travDist)
    : DiscountCard(name, number), requiredDistance(reqDist), traveledDistance(travDist) {}

float DistanceCard::calculateDiscount(float originalPrice) const {
    if (!isValid) return 0;

    if (traveledDistance <= requiredDistance) {
        return originalPrice * 0.5f; 
    }
    return originalPrice * 0.3f;
}

void DistanceCard::validate() {
    isValid = (cardNumber >= 100000 && cardNumber <= 999999);
}

void DistanceCard::saveToFile(const String& filename) const {
    std::ofstream file(filename.c_str());
    if (!file) throw std::runtime_error("Cannot save distance card");

    file << "===Distance card===\n"
        << holderName << "\n"
        << requiredDistance << " km\n"
        << cardNumber << "\n"
        << "=============\n";
}

void DistanceCard::loadFromFile(const String& filename) {
    std::ifstream file(filename.c_str());
    if (!file) throw std::runtime_error("Cannot load distance card");

    String line;
    int lineNum = 0;
    while (!(line = String::read_line(file)).empty()) {
        line = line.trim();

        if (lineNum == 1) holderName = line;
        else if (line.contains(" km")) {
            size_t pos = line.contains(" km");
            requiredDistance = line.substr(0, pos).to_float();
        }
        else if (line.size() == 6 && line[0] >= '0' && line[0] <= '9') {
            cardNumber = line.to_int();
        }
        lineNum++;
    }
    validate();
}