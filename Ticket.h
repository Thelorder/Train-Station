#pragma once
#include "String.h"
#include "Train.h"
#include "DiscountCard.h"

class Ticket {
private:
    String ticketId;
    const Train* train;
    int wagonId;
    int seatId;
    float basePrice;
    float discountAmount;
    String purchaseDateTime;
    String cardUsed;

    static String getCurrentDateTime();
    static String formatPrice(float price);

public:
    Ticket() : train(nullptr), wagonId(0), seatId(0), basePrice(0), discountAmount(0) {}
    Ticket(const Train* train, int wagonId, int seatId, float basePrice,const String& cardFile = "");

    void saveToFile(const String& filename) const;
    void loadFromFile(const String& filename, const Train* trainPtr);

    const String& getId() const { return ticketId; }
    int getTrainId() const { return train->getId(); }
    int getWagonId() const { return wagonId; }
    int getSeatId() const { return seatId; }
};