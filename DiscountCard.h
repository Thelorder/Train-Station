#pragma once
#include "String.h"

class DiscountCard {
protected:
	String holderName;
	int cardNumber;
	bool isValid;
public:
    DiscountCard(const String& name, int number)
        : holderName(name), cardNumber(number), isValid(false) {}

    virtual ~DiscountCard() = default;

    virtual float calculateDiscount(float originalPrice) const = 0;
    virtual String getType() const = 0;
    virtual void validate() = 0;

    bool getIsValid() const { return isValid; }
    int getCardNumber() const { return cardNumber; }
    const String& getHolderName() const { return holderName; }

    virtual void saveToFile(const String& filename) const =0;
    virtual void loadFromFile(const String& filename) =0;
};