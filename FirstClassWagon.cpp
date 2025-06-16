#include "FirstClassWagon.h"

FirstClassWagon::FirstClassWagon(int id, float basePrice, float comfortFactor)
    : Wagon(id, basePrice, 10, "first"), comfortFactor(comfortFactor) {}

float FirstClassWagon::calculatePrice(float distance, const String& extraParam) const {
    bool hasFood = (strcmp(extraParam.c_str(), "true") == 0);
    return basePrice * comfortFactor + (hasFood ? 10.0f : 0.0f);
}