#pragma once
#include "DiscountCard.h"

class DistanceCard :public DiscountCard {
    float requiredDistance;
    float traveledDistance;

public:


    DistanceCard(const String& name, int number, float reqDist, float travDist = 0);

    float calculateDiscount(float originalPrice) const override;
    String getType() const override { return "Distance"; }
    void validate() override;

    void saveToFile(const String& filename) const override;
    void loadFromFile(const String& filename) override;
};