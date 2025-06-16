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
    Ticket(const Train* train, int wagonId, int seatId, float basePrice,const String& cardFile = "");

    void saveToFile(const String& filename) const;

    const String& getId() const { return ticketId; }
    int getTrainId() const { return train->getId(); }
    int getWagonId() const { return wagonId; }
    int getSeatId() const { return seatId; }
};