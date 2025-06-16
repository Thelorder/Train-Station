#include "Wagon.h"
#include <fstream>
#include <iostream>

Wagon::Wagon(int id, float basePrice, size_t seatCount, const String& type)
    : id(id), basePrice(basePrice), wagonType(type) {
    for (size_t i = 0; i < seatCount; ++i) {
        seats.push_back(0);
    }
}

Wagon::~Wagon()
{

    std::cout << "Destroying Wagon " << id << std::endl;
}

bool Wagon::bookSeat(int seatId) {
    if (seatId < 0 || seatId >= (int)seats.get_size()) return false;
    seats[seatId] = 1;
    return true;
}

bool Wagon::isSeatBooked(int seatId) const {
    return seatId >= 0 && seatId < (int)seats.get_size() && seats[seatId] == 1;
}

void Wagon::saveToFile(const String& filename) const {
    std::ofstream file(filename.c_str());
    if (!file) return;

    file << getTypeHeader() << "\n";

    const int seatsPerRow = getSeatsPerRow();
    for (size_t i = 0; i < seats.get_size(); ++i) {
        if (i % seatsPerRow == 0) file << "| ";
        file << (seats[i] ? "xx" : formatSeatNumber(i + 1)) << " | ";
        if ((i + 1) % seatsPerRow == 0) file << "\n";
    }
}

void Wagon::loadFromFile(const String& filename) {
    std::ifstream file(filename.c_str());
    if (!file) return;

    String::read_line(file);
    char buffer[256];
    int seatCounter = 0;
    while (file.getline(buffer, 256) && seatCounter < seats.get_size()) {
        const char* ptr = buffer;
        while (*ptr && seatCounter < seats.get_size()) {
            if (*ptr == '|') {
                ptr++;
                if (ptr[0] == 'x' && ptr[1] == 'x') {
                    seats[seatCounter++] = 1;
                    ptr += 2;
                }
                else if (isdigit(ptr[0]) && isdigit(ptr[1])) {
                    seats[seatCounter++] = 0;
                    ptr += 2;
                }
            }
            else {
                ptr++;
            }
        }
    }
}

void Wagon::printDetails() const
{
    std::cout << "=== Wagon " << id << " ===\n"
        << "Type: " << wagonType << "\n"
        << "Base Price: " << basePrice << " BGN\n"
        << "Seats: " << seats.get_size() << "\n"
        << "Available: ";

    for (size_t i = 0; i < seats.get_size(); ++i) {
        std::cout << (seats[i] ? "XX " : String::from_int(i + 1) + " ");
    }
    std::cout << "\n";
}

void Wagon::printSeatingPlan() const
{
    int totalSeats = getSeatCount();       // e.g., 10 or 20 or 5
    int seatsPerRow = getSeatsPerRow();    // e.g., 2, 4, or 1

    std::cout << " ";
    for (int i = 0; i < seatsPerRow * 3 - 1; ++i) std::cout << "_";
    std::cout << "\n";

    for (int i = 0; i < totalSeats; i += seatsPerRow) {
        std::cout << "|";
        for (int j = 0; j < seatsPerRow; ++j) {
            int seatNum = i + j + 1;
            if (seatNum > totalSeats) break;

            String seatStr;
            if (isSeatBooked(seatNum)) {
                seatStr = "xx";
            }
            else {
                seatStr = formatSeatNumber(seatNum); 
            }

            std::cout << seatStr.c_str();
            if (j != seatsPerRow - 1) std::cout << " ";
        }
        for (int j = totalSeats - i; j < seatsPerRow; ++j) {
            std::cout << "   ";
        }
        std::cout << "|\n";
    }

    std::cout << " ";
    for (int i = 0; i < seatsPerRow * 3 - 1; ++i) std::cout << "_";
    std::cout << "\n";
}
