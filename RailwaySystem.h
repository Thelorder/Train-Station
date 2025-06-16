#pragma once
#include "Vector.hpp"
#include "String.h"
#include "Station.h"
#include "Train.h"
#include "DiscountCard.h"
#include "FirstClassWagon.h"
#include "SecondClassWagon.h"
#include "SleepingWagon.h"
#include "AgeCard.h"
#include "RouteCard.h"
#include "DistanceCard.h"
#include <iostream>
#include "Ticket.h"

class RailwaySystem {
private:

    Vector<Station> stations;
    Vector<Train> trains;
    Vector<DiscountCard*> discountCards;
    Vector<String> adminUsernames;
    Vector<String> adminPasswords;
    bool adminLoggedIn = false;

    const Train& findTrain(int id) const;
    Train& findTrain(int id);
    const Station& findStation(const String& name) const;
    Station& findStation(const String& name);
    void validateAdmin() const;
    String floatToString(float value) const;
 //   void saveTicketToFile(const String& filename, int trainId,
   //     int wagonId, int seatId, float price) const;

    void handlePrintStations() const;
    void handlePrintSchedule(const String& station) const;
    void handlePrintScheduleDestination(const String& station,
        const String& destination) const;
    void handlePrintScheduleTime(const String& station,
        const String& date,
        const String& time) const;
    void handlePrintTrain(int trainId) const;
    void handlePrintWagon(int trainId, int wagonId) const;
    void handleBuyTicket(int trainId, int wagonId, int seatId,
        const String& ticketFile, float param);
    void handleBuyTicketWithDiscount(int trainId, int wagonId, int seatId,
        const String& ticketFile,
        const String& discountCard,
        float param);

    void handleAddStation(const String& name);
    void handleAddTrain(const String& from, const String& to,
        float distance, float speed,
        const String& departureDate,
        const String& departureTime);
    void handleRemoveTrain(int trainId);
    void handleAddWagon(int trainId, const String& type,
        float basePrice, float extraParam);
    void handleRemoveWagon(int trainId, int wagonId);
    void handleMoveWagon(int srcTrainId, int wagonId, int destTrainId);
    void handleCreateDiscountCard(const String& type, const String& userName,
        const String& fileName,
        const String& extraParam);
    void handleValidateDiscountCard(const String& cardId) const;

    void saveStations(std::ofstream& out) const;
    void loadStations(std::ifstream& in);
    void saveTrains(std::ofstream& out) const;
    void loadTrains(std::ifstream& in);
    void saveDiscountCards(std::ofstream& out) const;
    void loadDiscountCards(std::ifstream& in);

public:

    void saveAllData();
    void loadAllData();

    void loadAdmins(const String& filename);
    void processCommand(const Vector<String>& args);
    void login(const String& username, const String& password);
    void logout() { adminLoggedIn = false; }
    bool isAdminLoggedIn() const { return adminLoggedIn; }

    static void displayHelp();

    ~RailwaySystem() {
        for (size_t i = 0; i < discountCards.get_size(); i++) {
            if (discountCards[i]) {
                delete discountCards[i];
            }
        }
        discountCards.clear();

 
        for (size_t i = 0; i < trains.get_size(); i++) {
            trains[i].clearWagons(); 
        }
    }
};