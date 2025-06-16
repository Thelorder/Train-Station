#pragma once
#include "Wagon.h"

class FirstClassWagon : public Wagon {
    float comfortFactor;
public:
    FirstClassWagon() : Wagon(0, 0.0f, 10, "first"), comfortFactor(0.8f) {}
        FirstClassWagon(int id, float basePrice, float comfortFactor);
        float calculatePrice(float distance, const String& extraParam) const override;
        int getSeatsPerRow() const override { return 2; } 
        String getTypeHeader() const override {
            return "=== FIRST CLASS WAGON ===";
        }
};