#include "Ticket.h"
#include "CardFactory.h"
#include <fstream>
#include <ctime>
#pragma warning(disable : 4996)

String Ticket::formatPrice(float price) {
    int whole = static_cast<int>(price);
    int cents = static_cast<int>((price - whole) * 100 + 0.5f); 

    String result = String::from_int(whole) + ".";
    if (cents < 10) result += "0";
    result += String::from_int(cents) + " lv.";
    return result;
}

String Ticket::getCurrentDateTime() {
    time_t now = time(nullptr);
    struct tm* timeinfo = localtime(&now);

    char buffer[20];
    strftime(buffer, sizeof(buffer), "%d/%m/%Y %H:%M", timeinfo);
    return String(buffer);
}

Ticket::Ticket(const Train* t, int wagon, int seat, float price, const String& cardFile)
    : train(t), wagonId(wagon), seatId(seat), basePrice(price), cardUsed(cardFile) {

    time_t now = time(nullptr);
    ticketId = String::from_int(train->getId()) + "_" +
        String::from_int(wagonId) + "_" +
        String::from_int(seatId) + "_" +
        String::from_int(now);
    purchaseDateTime = getCurrentDateTime();

    discountAmount = 0;
    if (!cardFile.empty()) {
        DiscountCard* card = CardFactory::createFromFile(cardFile);
        if (card && card->getIsValid()) {
            discountAmount = card->calculateDiscount(basePrice);
        }
        delete card;
    }
}

void Ticket::saveToFile(const String& filename) const {
    std::ofstream file(filename.c_str());
    if (!file) throw std::runtime_error("Cannot save ticket");

    file << "|==========Train Ticket==========|\n";
    file << "Ticket: " << train->getStartStation() << " - " << train->getEndStation() << "\n";
    file << "Train ID: " << train->getId() << "\n";
    file << "Wagon ID: " << wagonId << "\n";
    file << "Seat ID: " << seatId << "\n";
    file << "Departure time: " << train->getDepartureTime() << "\n";
    file << "Arrival time: " << train->getArrivalTime() << "\n";
    file << "Departure platform: " << train->getDeparturePlatform() << "\n";
    file << "Discount: " << (int)discountAmount << " lv.\n";
    file << "Price: " << (int)(basePrice - discountAmount) << " lv.\n";
    file << "|================================|\n";
}

void Ticket::loadFromFile(const String& filename, const Train* trainPtr)
{
    std::ifstream file(filename.c_str());
    if (!file) throw std::runtime_error("Cannot load ticket file");

    String line;
    while (!(line = String::read_line(file)).empty()) {
        line = line.trim();

        if (line.size() >= 8 && line.substr(0, 8) == "Ticket: ") {
        }
        else if (line.size() >= 10 && line.substr(0, 10) == "Train ID: ") {
        }
        else if (line.size() >= 10 && line.substr(0, 10) == "Wagon ID: ") {
            wagonId = line.substr(10).trim().to_int();
        }
        else if (line.size() >= 9 && line.substr(0, 9) == "Seat ID: ") {
            seatId = line.substr(9).trim().to_int();
        }
        else if (line.size() >= 16 && line.substr(0, 16) == "Departure time: ") {
        }
        else if (line.size() >= 14 && line.substr(0, 14) == "Arrival time: ") {
        }
        else if (line.size() >= 20 && line.substr(0, 20) == "Departure platform: ") {
        }
        else if (line.size() >= 15 && line.substr(0, 15) == "Purchase time: ") {
            purchaseDateTime = line.substr(15).trim();
        }
        else if (line.size() >= 10 && line.substr(0, 10) == "Discount: ") {
            String discStr = line.substr(10).trim();
            size_t pos = discStr.contains(" lv.");
            if (pos != (size_t)-1) discStr = discStr.substr(0, pos);
            discountAmount = discStr.to_float();
        }
        else if (line.size() >= 7 && line.substr(0, 7) == "Price: ") {
            String priceStr = line.substr(7).trim();
            size_t pos = priceStr.contains(" lv.");
            if (pos != (size_t)-1) priceStr = priceStr.substr(0, pos);
     
            basePrice = priceStr.to_float() + discountAmount;
        }
    }

    train = trainPtr;
}
