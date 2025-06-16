#pragma once
#include "Wagon.h"

class SleepingWagon : public Wagon {
    float pricePer100Km;
public:
   inline SleepingWagon() : Wagon(0, 0.0f, 5, "sleeping"), pricePer100Km(0.0f) {}
   inline SleepingWagon(int id, float basePrice, float pricePer100Km);
    inline float calculatePrice(float distance, const String& extraParam) const override;
    int getSeatsPerRow() const override { return 1; } 
    inline String getTypeHeader() const override {
        return "=== SLEEPING WAGON ===";
    }
    inline void toFile(std::ofstream& out) const override {
        Wagon::toFile(out); 
        out << pricePer100Km << "\n";
        out << "END_SLEEPING_WAGON\n";
    }
};

SleepingWagon::SleepingWagon(int id, float basePrice, float pricePer100Km)
    : Wagon(id, basePrice, 5, "sleeping"), pricePer100Km(pricePer100Km) {}

float SleepingWagon::calculatePrice(float distance, const String& extraParam) const {
    return basePrice + (pricePer100Km * distance / 100.0f);
}