#include "RailwaySystem.h"
#include <fstream>
#include <iostream>
#include <stdexcept>

static bool startsWith(const char* str, const char* prefix) {
    while (*prefix) {
        if (*prefix++ != *str++) return false;
    }
    return true;
}

static bool startsWith(const String& str, const char* prefix) {
    return startsWith(str.c_str(), prefix);
}

String RailwaySystem::floatToString(float value) const {
    return String::from_float(value, 2);
}

void RailwaySystem::validateAdmin() const {
    if (!adminLoggedIn) {
        throw std::runtime_error("Admin login required");
    }
}

void RailwaySystem::loadAdmins(const String& filename) {
    std::ifstream file(filename.c_str());
    if (!file) {
        
        throw std::runtime_error("Cannot open admin file. Please ensure admins.txt exists in the program's directory.");
    }

    String line;
    while (!(line = String::read_line(file)).empty()) {
        
        String username, password;
        bool commaFound = false;

        for (size_t i = 0; i < line.size(); i++) {
            char c = line[i];
            if (c == ',') {
                commaFound = true;
                continue;
            }

            if (!commaFound) {
                char temp[2] = { c, '\0' };
                username = username + String(temp);
            }
            else {
                char temp[2] = { c, '\0' };
                password = password + String(temp);
            }
        }

        if (commaFound) {
            adminUsernames.push_back(username);
            adminPasswords.push_back(password);
        }
    }
}

void RailwaySystem::login(const String& username, const String& password) {
    for (size_t i = 0; i < adminUsernames.get_size(); i++) {
        if (adminUsernames[i] == username &&
            adminPasswords[i] == password) {
            adminLoggedIn = true;
            return;
        }
    }
    throw std::runtime_error("Invalid credentials");
}

void RailwaySystem::handleAddStation(const String& name) {

    stations.push_back(Station(name));
    std::cout << "Added station " << name.c_str() << "\n";
}

void RailwaySystem::handleAddTrain(const String& from, const String& to,
    float distance, float speed,
    const String& departureDate,
    const String& departureTime) {
    validateAdmin();

    Station& startStation = findStation(from);
    Station& endStation = findStation(to);

    static int nextTrainId = 1000;
    int trainId = nextTrainId++;

    Train* train = new Train(trainId, from, to, distance, speed);
    train->setDepartureTime(departureDate + " " + departureTime);
    train->calculateArrivalTime();

    int depPlatform = startStation.assignDeparturePlatform();
    int arrPlatform = endStation.assignArrivalPlatform();
    train->setDeparturePlatform(depPlatform);
    train->setArrivalPlatform(arrPlatform);

    startStation.addDepartingTrain(train);
    endStation.addArrivingTrain(train);
    trains.push_back(*train);

    std::cout << "Train added successfully. ID: " << trainId << "\n";
}

void RailwaySystem::handleRemoveTrain(int trainId) {
    validateAdmin();
    Train& train = findTrain(trainId);

    Station& startStation = findStation(train.getStartStation());
    startStation.removeDepartingTrain(trainId);

    for (size_t i = 0; i < trains.get_size(); i++) {
        if (trains[i].getId() == trainId) {
            trains.remove(i);
            break;
        }
    }
    std::cout << "Train " << trainId << " removed\n";
}

void RailwaySystem::handleAddWagon(int trainId, const String& type,
    float basePrice, float extraParam) {
    validateAdmin();
    Train& train = findTrain(trainId);

    static int nextWagonId = 1;
    int wagonId = nextWagonId++;

    Wagon* wagon = nullptr;
    if (type == "first-class") {
        wagon = new FirstClassWagon(wagonId, basePrice, extraParam);
    }
    else if (type == "second-class") {
        wagon = new SecondClassWagon(wagonId, basePrice, extraParam);
    }
    else if (type == "sleeping-class") {
        wagon = new SleepingWagon(wagonId, basePrice, extraParam);
    }
    else {
        throw std::runtime_error("Invalid wagon type");
    }

    train.addWagon(wagon);
    std::cout << type.c_str() << " wagon added to train " << trainId << "\n";
}

void RailwaySystem::handleBuyTicket(int trainId, int wagonId, int seatId,
    const String& ticketFile, float param) {
    Train& train = findTrain(trainId);
    Wagon* wagon = train.getWagon(wagonId);

    if (!wagon) throw std::runtime_error("Wagon not found");
    if (wagon->isSeatBooked(seatId)) throw std::runtime_error("Seat taken");

    String paramStr = floatToString(param);
    float price = wagon->calculatePrice(train.getDistace(), paramStr);

    if (!wagon->bookSeat(seatId)) {
        throw std::runtime_error("Booking failed");
    }

    Ticket ticket(&train, wagonId, seatId, price, "");
    ticket.saveToFile(ticketFile);

    std::cout << "Ticket purchased successfully\n";
}

void RailwaySystem::handlePrintStations() const {
    std::cout << "Available stations:\n";
    for (size_t i = 0; i < stations.get_size(); i++) {
        std::cout << " - " << stations[i].getName().c_str() << "\n";
    }
}

void RailwaySystem::handlePrintTrain(int trainId) const {
    const Train& train = findTrain(trainId);
    train.printDetails();
}

void RailwaySystem::processCommand(const Vector<String>& args) {
    if (args.get_size() == 0) return;

    const String& command = args[0];

    try {
        if (command == "print-stations") {
            handlePrintStations();
        }
        else if (command == "print-schedule" && args.get_size() > 1) {
            handlePrintSchedule(args[1]);
        }
        else if (command == "print-schedule-destination" && args.get_size() > 2) {
            handlePrintScheduleDestination(args[1], args[2]);
        }
        else if (command == "print-schedule-time" && args.get_size() > 3) {
            handlePrintScheduleTime(args[1], args[2], args[3]);
        }
        else if (command == "print-train" && args.get_size() > 1) {
            handlePrintTrain(args[1].to_int());
        }
        else if (command == "print-wagon" && args.get_size() > 2) {
            handlePrintWagon(args[1].to_int(), args[2].to_int());
        }
        else if (command == "buy-ticket" && args.get_size() > 4) {
            float param = args.get_size() > 5 ? args[5].to_float() : 0.0f;
            handleBuyTicket(args[1].to_int(), args[2].to_int(),
                args[3].to_int(), args[4], param);
        }
        else if (command == "buy-ticket-discount" && args.get_size() > 5) {
            float param = args.get_size() > 6 ? args[6].to_float() : 0.0f;
            handleBuyTicketWithDiscount(args[1].to_int(), args[2].to_int(),
                args[3].to_int(), args[4], args[5], param);
        }
        else if (command == "login" && args.get_size() > 2) {
            login(args[1], args[2]);
        }
        else if (command == "logout") {
            logout();
        }
        else if (command == "add-station" && args.get_size() > 1) {
            validateAdmin();
            handleAddStation(args[1]);
        }
        else if (command == "add-train" && args.get_size() > 6) {
            validateAdmin();
            handleAddTrain(args[1], args[2], args[3].to_float(),
                args[4].to_float(), args[5], args[6]);
        }
        else if (command == "remove-train" && args.get_size() > 1) {
            validateAdmin();
            handleRemoveTrain(args[1].to_int());
        }
        else if (command == "add-wagon" && args.get_size() > 3) {
            validateAdmin();
            float extra = args.get_size() > 4 ? args[4].to_float() : 0.0f;
            handleAddWagon(args[1].to_int(), args[2],
                args[3].to_float(), extra);
        }
        else if (command == "remove-wagon" && args.get_size() > 2) {
            validateAdmin();
            handleRemoveWagon(args[1].to_int(), args[2].to_int());
        }
        else if (command == "move-wagon" && args.get_size() > 3) {
            validateAdmin();
            handleMoveWagon(args[1].to_int(), args[2].to_int(),
                args[3].to_int());
        }
        else if (command == "create-discount-card" && args.get_size() > 4) {
            validateAdmin();
            String extra = args.get_size() > 5 ? args[5] : String("");
            handleCreateDiscountCard(args[1], args[2], args[3], args[4]);
        }
        else if (command == "validate-discount-card" && args.get_size() > 1) {
            handleValidateDiscountCard(args[1]);
        }
        else if (command == "help") {
            displayHelp();
        }
        else {
            throw std::runtime_error("Unknown command or insufficient parameters");
        }
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        if (!adminLoggedIn && command == "help") {
            displayHelp();
        }
    }
}

const Train& RailwaySystem::findTrain(int id) const {
    for (size_t i = 0; i < trains.get_size(); i++) {
        if (trains[i].getId() == id) {
            return trains[i];
        }
    }
    throw std::runtime_error("Train not found");
}

Train& RailwaySystem::findTrain(int id) {
    for (size_t i = 0; i < trains.get_size(); i++) {
        if (trains[i].getId() == id) {
            return trains[i];
        }
    }
    throw std::runtime_error("Train not found");
}

void RailwaySystem::handlePrintSchedule(const String& station) const {
    const Station& st = findStation(station);
    st.printSchedule();
}

void RailwaySystem::handlePrintScheduleDestination(const String& station,
    const String& destination) const {
    const Station& st = findStation(station);
    std::cout << "Departures to " << destination.c_str() << ":\n";

    for (size_t i = 0; i < st.getDepartureCount(); i++) {
        if (const Train* train = st.getDepartureTrain(i)) {
            if (train->getEndStation() == destination) {
                train->printDetails();
            }
        }
    }
}

void RailwaySystem::handlePrintScheduleTime(const String& station,
    const String& date,
    const String& time) const {
    const Station& st = findStation(station);
    String targetDateTime = date + " " + time;

    std::cout << "Departures after " << targetDateTime.c_str() << ":\n";
    for (size_t i = 0; i < st.getDepartureCount(); i++) {
        if (const Train* train = st.getDepartureTrain(i)) {
            if (train->getDepartureTime() > targetDateTime) {
                train->printDetails();
            }
        }
    }
}

void RailwaySystem::handlePrintWagon(int trainId, int wagonId) const {
    const Train& train = findTrain(trainId);
    const Wagon* wagon = train.getWagon(wagonId);
    if (!wagon) {
        throw std::runtime_error("Wagon not found");
    }
    wagon->printSeatingPlan();
}

void RailwaySystem::handleBuyTicketWithDiscount(int trainId, int wagonId, int seatId,
    const String& ticketFile,
    const String& discountCard,
    float param) {
    Train& train = findTrain(trainId);
    Wagon* wagon = train.getWagon(wagonId);

    if (!wagon) throw std::runtime_error("Wagon not found");
    if (wagon->isSeatBooked(seatId)) throw std::runtime_error("Seat taken");

    String paramStr = String::from_float(param, 2);
    float price = wagon->calculatePrice(train.getDistace(), paramStr);

    float discount = 0.0f;
    for (size_t i = 0; i < discountCards.get_size(); i++) {
        if (String::from_int(discountCards[i]->getCardNumber()) == discountCard) {
            try {
                discountCards[i]->validate();
                discount = discountCards[i]->calculateDiscount(price);
                break;
            }
            catch (const std::exception& e) {
                std::cerr << "Warning: " << e.what() << " (No discount applied)\n";
                break;
            }
        }
    }

    float finalPrice = price - discount;
    if (!wagon->bookSeat(seatId)) {
        throw std::runtime_error("Booking failed");
    }

    Ticket ticket(&train, wagonId, seatId, finalPrice, discountCard);
    ticket.saveToFile(ticketFile);

    std::cout << "Ticket purchased. Final price: "
        << String::from_float(finalPrice, 2).c_str() << "\n";
}

void RailwaySystem::handleRemoveWagon(int trainId, int wagonId) {
    validateAdmin();
    Train& train = findTrain(trainId);
    if (!train.removeWagon(wagonId)) {
        throw std::runtime_error("Failed to remove wagon");
    }
    std::cout << "Wagon " << wagonId << " removed from train " << trainId << "\n";
}

void RailwaySystem::handleMoveWagon(int srcTrainId, int wagonId, int destTrainId) {
    validateAdmin();
    Train& srcTrain = findTrain(srcTrainId);
    Train& destTrain = findTrain(destTrainId);

    Wagon* wagon = srcTrain.getWagon(wagonId);
    if (!wagon) throw std::runtime_error("Wagon not found");
    if (!srcTrain.removeWagon(wagonId)) {
        throw std::runtime_error("Failed to remove wagon");
    }
    if (!wagon) throw std::runtime_error("Wagon not found");

    destTrain.addWagon(wagon);

    destTrain.addWagon(wagon);
    std::cout << "Wagon " << wagonId << " moved from train "
        << srcTrainId << " to " << destTrainId << "\n";
}

void RailwaySystem::handleCreateDiscountCard(const String& type,
    const String& userName,
    const String& fileName,
    const String& extraParam) {
    validateAdmin();
    static int nextCardId = 100000;

    DiscountCard* card = nullptr;
    if (type == "Age") {
        int age = extraParam.to_int();
        if (age < 1 || age > 120) throw std::runtime_error("Invalid age");
        card = new AgeCard(userName, nextCardId++, age);
    }

    else if (type == "Route") {
        if (extraParam.empty()) throw std::runtime_error("Destination required");
        card = new RouteCard(userName, nextCardId++, extraParam);
    }
    else if (type == "Distance") {
        card = new DistanceCard(userName, nextCardId++, extraParam.to_float());
    }
    else {
        throw std::runtime_error("Invalid card type");
    }

    discountCards.push_back(card);
    card->saveToFile(fileName);
    delete card;
}

void RailwaySystem::handleValidateDiscountCard(const String& cardId) const {
    int inputCardId;
    try {
        inputCardId = cardId.to_int();
    }
    catch (...) {
        throw std::runtime_error("Invalid card ID format - must be a number");
    }

    bool found = false;
    for (size_t i = 0; i < discountCards.get_size(); i++) {
        if (discountCards[i]->getCardNumber() == inputCardId) {
            found = true;
            break;
        }
    }

    if (found) {
        std::cout << "Valid discount card: " << inputCardId << "\n";
    }
    else {
        throw std::runtime_error("Discount card not found");
    }
}

void RailwaySystem::displayHelp() {
    std::cout << "Available commands:\n"
        << "USER COMMANDS:\n"
        << "  print-stations\n"
        << "  print-schedule <station>\n"
        << "  print-schedule-destination <station> <destination>\n"
        << "  print-train <trainID>\n"
        << "  print-wagon <trainID> <wagonID>\n"
        << "  buy-ticket <trainID> <wagonID> <seatID> <ticketFile> [extra]\n"
        << "  buy-ticket-discount <trainID> <wagonID> <seatID> <ticketFile> <cardFile> [extra]\n\n"
        << "ADMIN COMMANDS:\n"
        << "  login <username> <password>\n"
        << "  add-station <name>\n"
        << "  add-train <from> <to> <distance> <speed> <departureDate> <departureTime>\n"
        << "  remove-train <trainID>\n"
        << "  add-wagon <trainID> <type> <basePrice> <extraParam>\n"
        << "  remove-wagon <trainID> <wagonID>\n"
        << "  move-wagon <srcTrainID> <wagonID> <destTrainID>\n"
        << "  create-discount-card <type> <userName> <fileName> <extraParam>\n"
        << "  validate-discount-card <cardID>\n";
}

Station& RailwaySystem::findStation(const String& name) {
    for (size_t i = 0; i < stations.get_size(); i++) {
        if (stations[i].getName() == name) {
            return stations[i];
        }
    }
    String errorMsg = "Station not found: ";
    errorMsg += name;
    throw std::runtime_error(errorMsg.c_str());
}

const Station& RailwaySystem::findStation(const String& name) const {
    for (size_t i = 0; i < stations.get_size(); i++) {
        if (stations[i].getName() == name) {
            return stations[i];
        }
    }
    String errorMsg = "Station not found: ";
    errorMsg += name;
    throw std::runtime_error(errorMsg.c_str());
}
void Train::printDetails() const {
    std::cout << "=== Train ID: " << id << " ===\n"
        << "Route: " << startStation.c_str() << " -> " << endStation.c_str() << "\n"
        << "Departure: " << departureTime.c_str() << " (Platform " << departurePlatform << ")\n"
        << "Arrival: " << arrivalTime.c_str() << " (Platform " << arrivalPlatform << ")\n"
        << "Distance: " << distance << " km\n"
        << "Speed: " << speed << " km/h\n"
        << "Wagons: " << wagons.get_size() << "\n";
    std::cout << "Wagons: " << wagons.get_size() << "\n";
}

void RailwaySystem::saveAllData() {
    std::ofstream file("railway_system.txt");
    if (!file) throw std::runtime_error("Cannot save data");

    file << "[STATIONS]\n";
    for (size_t i = 0; i < stations.get_size(); i++) {
        stations[i].toFile(file);
    }

    file << "[TRAINS]\n";
    for (size_t i = 0; i < trains.get_size(); i++) {
        trains[i].toFileBasic(file); 
    }

    file << "[WAGONS]\n";
    for (size_t i = 0; i < trains.get_size(); i++) {
        trains[i].saveWagons(file); 
    }

    file << "[CARDS]\n";
    for (size_t i = 0; i < discountCards.get_size(); i++) {
        if (discountCards[i]) {
            file << "CARD " << i << "\n";
            discountCards[i]->saveToFile(String("card_") + String::from_int(i) + ".txt");
        }
    }
    file << "[TICKETS]\n";
    saveTickets(file);
}

void RailwaySystem::loadAllData() {
    std::ifstream systemFile("railway_system.txt");
    if (!systemFile) return;  

    String line;
    while (true) {
        line = String::read_line(systemFile);
        if (line.empty()) break;  

        if (line == "[STATIONS]") {
            loadStations(systemFile);
        }
        else if (line == "[TRAINS]") {
            loadTrains(systemFile);
        }
        else if (line == "[CARDS]") {
            loadDiscountCards(systemFile);
        }
        else if (line == "[TICKETS]") {
            loadTickets(systemFile);
        }

    }
}

void RailwaySystem::saveStations(std::ofstream& out) const {
    out << stations.get_size() << "\n";
    for (size_t i = 0; i < stations.get_size(); i++) {
        stations[i].toFile(out);
    }
}

void RailwaySystem::loadStations(std::ifstream& in) {
    size_t count;
    in >> count;
    in.ignore();  

    for (size_t i = 0; i < count; i++) {
        Station station;
        station.fromFile(in);
        stations.push_back(station);
    }
}

void RailwaySystem::saveTrains(std::ofstream& out) const {
    out << trains.get_size() << "\n";
    for (size_t i = 0; i < trains.get_size(); i++) {
        trains[i].toFile(out);  
    }
}

void RailwaySystem::loadTrains(std::ifstream& in) {
    size_t count;
    in >> count;
    in.ignore();

    for (size_t i = 0; i < count; i++) {
        Train train;
        train.fromFile(in);
        trains.push_back(train);
    }
}

void RailwaySystem::saveDiscountCards(std::ofstream& out) const {
    out << "DISCOUNT_CARDS " << discountCards.get_size() << "\n";
    for (size_t i = 0; i < discountCards.get_size(); i++) {
        String filename = String("card_") + String::from_int(i) + ".txt";
        discountCards[i]->saveToFile(filename);
        out << "CARD_FILE:" << filename.c_str() << "\n";
    }
}

void RailwaySystem::loadDiscountCards(std::ifstream& in) {
    for (size_t i = 0; i < discountCards.get_size(); i++) {
        delete discountCards[i];
    }
    discountCards.clear();

    String line = String::read_line(in);
    if (!startsWith(line.c_str(), "DISCOUNT_CARDS ")) return;

    size_t count = String(line.c_str() + 15).to_int();

    for (size_t i = 0; i < count; i++) {
        line = String::read_line(in);
        if (!startsWith(line.c_str(), "CARD_FILE:")) continue;

        String filename = String(line.c_str() + 10);
        std::ifstream cardFile(filename.c_str());
        if (!cardFile) continue;

        String cardType = String::read_line(cardFile);
        DiscountCard* card = nullptr;

        if (cardType == "AGE_CARD") {
            card = new AgeCard();
        }
        else if (cardType == "ROUTE_CARD") {
            card = new RouteCard();
        }

        if (card) {
            card->loadFromFile(filename);
            discountCards.push_back(card);
        }
    }
}

void RailwaySystem::saveTickets(std::ofstream& out) const
{
    out << tickets.get_size() << "\n";
    for (size_t i = 0; i < tickets.get_size(); ++i) {
        String filename = String("ticket_") + String::from_int(i) + ".txt";
        tickets[i]->saveToFile(filename);
        out << "TICKET_FILE:" << filename << "\n";
    }
}

void RailwaySystem::loadTickets(std::ifstream& in)
{
    size_t count = 0;
    String line = String::read_line(in);
    if (!line.empty()) {
        count = line.to_int();
    }

    for (size_t i = 0; i < count; ++i) {
        String ticketLine = String::read_line(in);
        size_t pos = ticketLine.contains("TICKET_FILE:");
        if (pos == (size_t)-1) continue;
        String filename = ticketLine.substr(pos + 12).trim();

        std::ifstream ticketFile(filename.c_str());
        if (!ticketFile) continue;

        String innerLine;
        int trainId = -1, wagonId = -1, seatId = -1;
        while (!(innerLine = String::read_line(ticketFile)).empty()) {
            if (innerLine.size() >= 9 && innerLine.substr(0, 9) == "Train ID:") {
                trainId = innerLine.substr(9).trim().to_int();
            }
            else if (innerLine.size() >= 10 && innerLine.substr(0, 10) == "Wagon ID: ") {
                wagonId = innerLine.substr(10).trim().to_int();
            }
            else if (innerLine.size() >= 9 && innerLine.substr(0, 9) == "Seat ID: ") {
                seatId = innerLine.substr(9).trim().to_int();
            }
        }
        ticketFile.close();

        Train* tptr = nullptr;
        for (size_t j = 0; j < trains.get_size(); ++j) {
            if (trains[j].getId() == trainId) {
                tptr = &trains[j];
                break;
            }
        }
        if (!tptr) continue;

        Ticket* ticket = new Ticket(); 
        ticket->loadFromFile(filename, tptr);
        tickets.push_back(ticket);


        Wagon* wptr = tptr->getWagon(wagonId);
        if (wptr) wptr->bookSeat(seatId);
    }
}
