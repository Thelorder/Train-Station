#include "Ticket.h"
#include "CardFactory.h"
#include <fstream>
#include <ctime>
#pragma warning(disable : 4996)

String Ticket::formatPrice(float price) {
    int whole = static_cast<int>(price);
    int cents = static_cast<int>((price - whole) * 100 + 0.5f); // Rounding

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