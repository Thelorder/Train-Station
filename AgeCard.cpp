#include "AgeCard.h"
#include <fstream>


AgeCard::AgeCard(const String& name, int number, int age)
    : DiscountCard(name, number), age(age) {}

float AgeCard::calculateDiscount(float originalPrice) const {
    if (!isValid) return 0;

    if (age <= 10) return originalPrice; 
    if (age <= 18) return originalPrice * 0.5f; 
    return originalPrice * 0.2f; 
}

void AgeCard::validate() {
    isValid = (cardNumber >= 100000 && cardNumber <= 999999);
}

void AgeCard::saveToFile(const String& filename) const {
    std::ofstream file(filename.c_str());
    if (!file) throw std::runtime_error("Cannot save age card");

    file << "===Age card===\n"
        << holderName << "\n"
        << age << " years old\n"
        << cardNumber << "\n"
        << "=============\n";
}

void AgeCard::loadFromFile(const String& filename) {
    std::ifstream file(filename.c_str());
    if (!file) throw std::runtime_error("Cannot load age card");

    String line;
    while (!(line = String::read_line(file)).empty()) {
        line = line.trim();

        if (line.contains(" years old")) {
            size_t pos = line.contains(" years old");
            age = line.substr(0, pos).to_int();
        }
        else if (line.size() == 6 && line[0] >= '0' && line[0] <= '9') {
            cardNumber = line.to_int();
        }
        else if (!line.contains("===") && !line.empty()) {
            holderName = line;
        }
    }
    validate();
}