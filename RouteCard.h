#pragma once
#include "DiscountCard.h"

class RouteCard :public DiscountCard {
	String destination;
public:
    RouteCard() : DiscountCard("", 0), destination("") {}
    RouteCard(const String& name, int number, const String& dest);

    float calculateDiscount(float originalPrice) const override;
    String getType() const override { return "Route"; }
    void validate() override;

    void saveToFile(const String& filename) const override;
    void loadFromFile(const String& filename) override;
};