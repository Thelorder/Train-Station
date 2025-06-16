#include "Station.h"
#include <ctime>
#include <iostream>
#include "Train.h"


Station::Station(const String& stationName) : name(stationName) {
    for (int i = 1; i <= 5; i++) {
        platforms.push_back(i);
    }
}

bool Station::addDepartingTrain(Train* train) {
    if (!train || train->getStartStation() != name) {
        return false;
    }

    if (trainExists(train->getId())) {
        return false;
    }

    int platform = assignDeparturePlatform();
    if (platform == -1) return false;

    train->setDeparturePlatform(platform);
    departingTrains.push_back(train);
    return true;
}

bool Station::removeDepartingTrain(int trainId) {
    for (size_t i = 0; i < departingTrains.get_size(); i++) {
        if (departingTrains[i]->getId() == trainId) {
            departingTrains.remove(i);
            return true;
        }
    }
    return false;
}

void Station::addArrivingTrain(Train* train) {
    if (!train || train->getEndStation() != name) {
        return;
    }

    int platform = assignArrivalPlatform();
    if (platform != -1) {
        train->setArrivalPlatform(platform);
        arrivingTrains.push_back(train);
    }
}

void Station::printSchedule() const {
    std::cout << "== Schedule for station " << name.c_str() << " ==" << std::endl;
    printArrivalBoard();
    printDepartureBoard();
}

void Station::printArrivalBoard() const {
    std::cout << "\nArrivals:\n";
    std::cout << "---------------------------------------------------------------\n";
    std::cout << "| Arrival Time    | Platform | Train ID | From          | Status   |\n";
    std::cout << "---------------------------------------------------------------\n";

    for (size_t i = 0; i < arrivingTrains.get_size(); i++) {
        const Train* t = arrivingTrains[i];
        std::cout << "| " << t->getArrivalTime().c_str() << "  | "
            << t->getArrivalPlatform() << "       | "
            << t->getId() << "      | "
            << t->getStartStation().c_str() << "    | "
            << (t->hasArrived() ? "Arrived " : "En route") << " |\n";
    }
    std::cout << "---------------------------------------------------------------\n";
}

void Station::printDepartureBoard() const {
    std::cout << "\nDepartures:\n";
    std::cout << "-------------------------------------------------------------------------------\n";
    std::cout << "| Departure Time | Arrival Time    | To            | Platform | Train ID | Status    |\n";
    std::cout << "-------------------------------------------------------------------------------\n";

    for (size_t i = 0; i < departingTrains.get_size(); i++) {
        const Train* t = departingTrains[i];
        std::cout << "| " << t->getDepartureTime().c_str() << " | "
            << t->getArrivalTime().c_str() << " | "
            << t->getEndStation().c_str() << "    | "
            << t->getDeparturePlatform() << "       | "
            << t->getId() << " | "
            << (t->hasDeparted() ? "Departed " : "Scheduled") << " |\n";
    }
    std::cout << "-------------------------------------------------------------------------------\n";
}

int Station::assignDeparturePlatform() {
    static int lastAssigned = 0;
    for (size_t i = 0; i < platforms.get_size(); i++) {
        int platform = platforms[(lastAssigned + i) % platforms.get_size()];
        bool available = true;

        for (size_t j = 0; j < departingTrains.get_size(); j++) {
            if (departingTrains[j]->getDeparturePlatform() == platform) {
                available = false;
                break;
            }
        }

        if (available) {
            lastAssigned = (lastAssigned + i + 1) % platforms.get_size();
            return platform;
        }
    }
    return -1;
}

int Station::assignArrivalPlatform() {
    static int lastAssigned = 0;
    for (size_t i = 0; i < platforms.get_size(); i++) {
        int platform = platforms[(lastAssigned + i) % platforms.get_size()];
        bool available = true;

        for (size_t j = 0; j < arrivingTrains.get_size(); j++) {
            if (arrivingTrains[j]->getArrivalPlatform() == platform) {
                available = false;
                break;
            }
        }

        if (available) {
            lastAssigned = (lastAssigned + i + 1) % platforms.get_size();
            return platform;
        }
    }
    return -1;
}

void Station::saveToFile(const String& filename) const {
    std::ofstream file(filename.c_str());
    if (!file) {
        throw std::runtime_error("Failed to open file for writing");
    }

    file << "=== STATION " << name << " ===\n";

    file << "PLATFORMS:";
    for (size_t i = 0; i < platforms.get_size(); i++) {
        file << " " << platforms[i];
    }
    file << "\n";

    file << "DEPARTING_TRAINS: " << String::from_int(departingTrains.get_size()) << "\n";
    for (size_t i = 0; i < departingTrains.get_size(); i++) {
        String idStr = String::from_int(departingTrains[i]->getId());
        while (idStr.size() < 4) {
            idStr = String("0") + idStr;
        }
        file << "TRAIN: " << idStr << "_train.txt\n";
    }

    file << "ARRIVING_TRAINS: " << String::from_int(arrivingTrains.get_size()) << "\n";
    for (size_t i = 0; i < arrivingTrains.get_size(); i++) {
        String idStr = String::from_int(arrivingTrains[i]->getId());
        while (idStr.size() < 4) {
            idStr = String("0") + idStr;
        }
        file << "TRAIN: " << idStr << "_train.txt\n";
    }
}

void Station::loadFromFile(const String& filename) {
    std::ifstream file(filename.c_str());
    if (!file) {
        throw std::runtime_error("Failed to open station file");
    }

    departingTrains.clear();
    arrivingTrains.clear();
    platforms.clear();

    String line;
    while (!(line = String::read_line(file)).empty()) {
        line = line.trim();

        if (line.contains("=== STATION ")) {
            size_t start = 12; 
            size_t end = line.size() - 4; 
            if (end > start) {
                name = line.substr(start, end - start).trim();
            }
        }
        else if (line.contains("PLATFORMS:")) {
            size_t start = line.contains("PLATFORMS:") + 10;
            String platformsStr = line.substr(start).trim();
            Vector<String> tokens = platformsStr.split(' ');
            for (size_t i = 0; i < tokens.get_size(); i++) {
                if (!tokens[i].empty()) {
                    platforms.push_back(tokens[i].to_int());
                }
            }
        }
        else if (line.contains("DEPARTING_TRAINS:")) {
            size_t start = line.contains("DEPARTING_TRAINS:") + 17;
            int count = line.substr(start).trim().to_int();
            for (int i = 0; i < count; i++) {
                line = String::read_line(file).trim();
                if (line.contains("TRAIN: ")) {
                    size_t trainStart = line.contains("TRAIN:") + 6;
                    String trainFile = line.substr(trainStart).trim();

                    std::ifstream trainIn(trainFile.c_str());
                    if (!trainIn) {
                        std::cerr << "Error: Failed to open train file: " << trainFile.c_str() << "\n";
                        continue;
                    }

                    Train* train = new Train(0, "", "", 0, 0);
                    try {
                        train->loadFromFile(trainFile);
                        departingTrains.push_back(train);
                    }
                    catch (...) {
                        delete train;
                        throw;
                    }
                }
            }
        }
        else if (line.contains("ARRIVING_TRAINS:")) {
            size_t start = line.contains("ARRIVING_TRAINS:") + 16;
            int count = line.substr(start).trim().to_int();
            for (int i = 0; i < count; i++) {
                line = String::read_line(file).trim();
                if (line.contains("TRAIN: ")) {
                    size_t trainStart = line.contains("TRAIN:") + 6;
                    String trainFile = line.substr(trainStart).trim();

                    std::ifstream trainIn(trainFile.c_str());
                    if (!trainIn) {
                        std::cerr << "Error: Failed to open train file: " << trainFile.c_str() << "\n";
                        continue;
                    }

                    Train* train = new Train(0, "", "", 0, 0);
                    try {
                        train->loadFromFile(trainFile);
                        departingTrains.push_back(train);
                    }
                    catch (...) {
                        delete train;
                        throw;
                    }
                }
            }
        }
    }
}

void Station::toFile(std::ofstream& out) const {
    if (!out) {
        throw std::runtime_error("Cannot write to file");
    }
    out << "STATION " << name << "\n";
    out << "PLATFORMS " << String::from_int(platforms.get_size());
    for (size_t i = 0; i < platforms.get_size(); i++) {
        out << " " << platforms[i];
    }
    out << "\n";

    out << "DEPARTING " << String::from_int(departingTrains.get_size()) << "\n";
    for (size_t i = 0; i < departingTrains.get_size(); i++) {
        out << String::from_int(departingTrains[i]->getId()) << "\n";
    }

    out << "ARRIVING " << String::from_int(arrivingTrains.get_size()) << "\n";
    for (size_t i = 0; i < arrivingTrains.get_size(); i++) {
        out << String::from_int(arrivingTrains[i]->getId()) << "\n";
    }
}

void Station::fromFile(std::ifstream& in) {
    if (!in) {
        throw std::runtime_error("Cannot read from file");
    }

    departingTrains.clear();
    arrivingTrains.clear();
    platforms.clear();

    String line = String::read_line(in);
    if (!line.contains("STATION ")) {
        throw std::runtime_error("Invalid station file format");
    }
    name = line.substr(8);

    line = String::read_line(in);
    if (!line.contains("PLATFORMS ")) {
        throw std::runtime_error("Invalid platforms format");
    }
    size_t start = line.contains("PLATFORMS ") + 10;
    String platformsStr = line.substr(start);
    Vector<String> platformTokens = platformsStr.split(' ');
    for (size_t i = 0; i < platformTokens.get_size(); i++) {
        if (!platformTokens[i].empty()) {
            platforms.push_back(platformTokens[i].to_int());
        }
    }

    line = String::read_line(in);
    if (!line.contains("DEPARTING ")) {
        throw std::runtime_error("Invalid departing trains format");
    }
    start = line.contains("DEPARTING ") + 10;
    int depCount = line.substr(start).to_int();
    for (int i = 0; i < depCount; i++) {
        line = String::read_line(in);
    }

    line = String::read_line(in);
    if (!line.contains("ARRIVING ")) {
        throw std::runtime_error("Invalid arriving trains format");
    }
    start = line.contains("ARRIVING ") + 9;
    int arrCount = line.substr(start).to_int();
    for (int i = 0; i < arrCount; i++) {
        line = String::read_line(in);
    }
}

bool Station::trainExists(int trainId) const {
    for (size_t i = 0; i < departingTrains.get_size(); i++) {
        if (departingTrains[i]->getId() == trainId) {
            return true;
        }
    }
    return false;
}