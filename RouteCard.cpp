#include "RouteCard.h"
#include <fstream>

RouteCard::RouteCard(const String& name, int number, const String& dest)
    : DiscountCard(name, number), destination(dest) {}

float RouteCard::calculateDiscount(float originalPrice) const {
    return (this->destination == destination) ? originalPrice : 0;
}

void RouteCard::validate() {
    isValid = (cardNumber >= 100000 && cardNumber <= 999999);
}

void RouteCard::saveToFile(const String& filename) const {
    std::ofstream file(filename.c_str());
    if (!file) throw std::runtime_error("Cannot save route card");

    file << "===Route card===\n"
        << holderName << "\n"
        << destination << "\n"
        << cardNumber << "\n"
        << "=============\n";
}

void RouteCard::loadFromFile(const String& filename) {
    std::ifstream file(filename.c_str());
    if (!file) throw std::runtime_error("Cannot load route card");

    String line;
    int lineNum = 0;
    while (!(line = String::read_line(file)).empty()) {
        line = line.trim();

        if (lineNum == 1) holderName = line;
        else if (lineNum == 2) destination = line;
        else if (line.size() == 6 && line[0] >= '0' && line[0] <= '9') {
            cardNumber = line.to_int();
        }
        lineNum++;
    }
    validate();
}