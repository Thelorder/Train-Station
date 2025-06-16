#include "SecondClassWagon.h"

SecondClassWagon::SecondClassWagon(int id, float basePrice, float luggagePricePerKg)
    : Wagon(id, basePrice, 20, "second"), luggagePricePerKg(luggagePricePerKg) {}

float SecondClassWagon::calculatePrice(float distance, const String& extraParam) const {
    return basePrice + (luggagePricePerKg * atof(extraParam.c_str()));
}