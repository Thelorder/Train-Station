#pragma once
#include "Wagon.h"

class SecondClassWagon : public Wagon {
    float luggagePricePerKg;
public:
    SecondClassWagon() : Wagon(0, 0.0f, 20, "second"), luggagePricePerKg(0.0f) {}
    SecondClassWagon(int id, float basePrice, float luggagePricePerKg);
    float calculatePrice(float distance, const String& extraParam) const override;
    int getSeatsPerRow() const override { return 4; } 
    String getTypeHeader() const override {
        return "=== SECOND CLASS WAGON ===";
    }
};