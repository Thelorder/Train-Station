#pragma once
#include "String.h"
#include "Vector.hpp"

class Wagon {
protected:
    int id;
    float basePrice;
    Vector<int> seats;
    String wagonType;

    String formatSeatNumber(int num) const {
        char buffer[3];
        if (num < 10) {
            buffer[0] = '0';
            buffer[1] = '0' + num;
        }
        else {
            buffer[0] = '0' + (num / 10);
            buffer[1] = '0' + (num % 10);
        }
        buffer[2] = '\0';
        return String(buffer);
    }

    virtual int getSeatsPerRow() const = 0;

public:
    Wagon(int id, float basePrice, size_t seatCount, const String& type);
    virtual ~Wagon() = default;

    bool bookSeat(int seatId);
    bool isSeatBooked(int seatId) const;

    virtual void saveToFile(const String& filename) const;
    virtual void loadFromFile(const String& filename);

    virtual float calculatePrice(float distance, const String& extraParam) const = 0;
    void printDetails() const;
    void printSeatingPlan() const;

    int getId() const { return id; }
    const String& getType() const { return wagonType; }
    size_t getSeatCount() const { return seats.get_size(); }
    virtual String getTypeHeader() const = 0;

    virtual void toFile(std::ofstream& out) const {
        out << "WAGON " << id << "\n";
        out << wagonType.c_str() << "\n";
        out << basePrice << "\n";
    }
};