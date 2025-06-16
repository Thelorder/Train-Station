#pragma once
#include "DiscountCard.h"

class AgeCard :public DiscountCard {
private:
	int age;
public:

	AgeCard() : DiscountCard("", 0), age(0) {}
	AgeCard(const String& name, int number, int age);

	float calculateDiscount(float originalPrice) const override;
	String getType() const override { return "Age"; }
	void validate() override;

	void saveToFile(const String& filename) const override;
	void loadFromFile(const String& filename) override;
};