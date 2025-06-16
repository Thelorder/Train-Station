#pragma once
#include "DiscountCard.h"
#include "AgeCard.h"
#include "RouteCard.h"
#include "DistanceCard.h"

class CardFactory {
public:
    static DiscountCard* createFromFile(const String& filename) {
        std::ifstream file(filename.c_str());
        if (!file) return nullptr;

        String firstLine = String::read_line(file).trim();
        file.close();

        if (firstLine == "===Age card===") {
            AgeCard* card = new AgeCard("", 0, 0);
            card->loadFromFile(filename);
            return card;
        }
        else if (firstLine == "===Route card===") {
            RouteCard* card = new RouteCard("", 0, "");
            card->loadFromFile(filename);
            return card;
        }
        else if (firstLine == "===Distance card===") {
            DistanceCard* card = new DistanceCard("", 0, 0);
            card->loadFromFile(filename);
            return card;
        }
        return nullptr;
    }
};