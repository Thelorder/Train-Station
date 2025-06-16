#include "RailwaySystem.h"
#include <iostream>

void runAutomatedTests(RailwaySystem& system) {
    std::cout << "\n=== RUNNING AUTOMATED TESTS ===\n";

    // Helper function to create command vectors
    auto makeCommand = [](const char* cmd, const char* arg1 = nullptr, const char* arg2 = nullptr,
        const char* arg3 = nullptr, const char* arg4 = nullptr, const char* arg5 = nullptr) {
            Vector<String> command;
            command.push_back(String(cmd));
            if (arg1) command.push_back(String(arg1));
            if (arg2) command.push_back(String(arg2));
            if (arg3) command.push_back(String(arg3));
            if (arg4) command.push_back(String(arg4));
            if (arg5) command.push_back(String(arg5));
            return command;
        };

    // Test 1: Basic station operations
    try {
        system.processCommand(makeCommand("add-station", "TestSofia"));
        system.processCommand(makeCommand("add-station", "TestPlovdiv"));
        system.processCommand(makeCommand("print-stations"));
        std::cout << "TEST 1 PASSED: Station operations\n";
    }
    catch (...) {
        std::cout << "TEST 1 FAILED: Station operations\n";
    }

    // Test 2: Admin train management
    try {
        system.processCommand(makeCommand("login", "admin1", "password123"));
        system.processCommand(makeCommand("add-train", "TestSofia", "TestPlovdiv", "150", "100", "15/06/2025", "08:00"));
        system.processCommand(makeCommand("print-train", "1000")); // Assuming first train gets ID 1000
        std::cout << "TEST 2 PASSED: Train management\n";
    }
    catch (...) {
        std::cout << "TEST 2 FAILED: Train management\n";
    }

    // Test 3: Wagon operations
    try {
        system.processCommand(makeCommand("add-wagon", "1000", "first-class", "50", "0.8"));
        system.processCommand(makeCommand("print-wagon", "1000", "1"));
        std::cout << "TEST 3 PASSED: Wagon operations\n";
    }
    catch (...) {
        std::cout << "TEST 3 FAILED: Wagon operations\n";
    }

    // Test 4: Ticket purchasing
    try {
        system.processCommand(makeCommand("buy-ticket", "1000", "1", "5", "test_ticket.txt", "1"));
        std::cout << "TEST 4 PASSED: Ticket purchasing\n";
    }
    catch (...) {
        std::cout << "TEST 4 FAILED: Ticket purchasing\n";
    }

    // Test 5: Discount cards
    try {
        system.processCommand(makeCommand("create-discount-card", "Age", "TestUser", "test_card.txt", "15"));
        system.processCommand(makeCommand("buy-ticket-discount", "1000", "1", "6", "test_discount_ticket.txt", "test_card.txt", "1"));
        std::cout << "TEST 5 PASSED: Discount cards\n";
    }
    catch (...) {
        std::cout << "TEST 5 FAILED: Discount cards\n";
    }

    // Test 6: Data persistence
    try {
        system.saveAllData();
        RailwaySystem newSystem;
        newSystem.loadAllData();
        newSystem.processCommand(makeCommand("print-train", "1000"));
        std::cout << "TEST 6 PASSED: Data persistence\n";
    }
    catch (...) {
        std::cout << "TEST 6 FAILED: Data persistence\n";
    }

    // Cleanup
    try {
        system.processCommand(makeCommand("remove-train", "1000"));
        system.processCommand(makeCommand("logout"));
        std::cout << "CLEANUP COMPLETE\n";
    }
    catch (...) {
        std::cout << "CLEANUP FAILED\n";
    }

    std::cout << "=== TESTS COMPLETE ===\n\n";
}

Vector<String> parseInput(const String& input) {
    Vector<String> args;
    String current;

    for (size_t i = 0; i < input.size(); i++) {
        char c = input[i];
        if (c == ' ') {
            if (!current.empty()) {
                args.push_back(current);
                current.clear();
            }
        }
        else {
           
            char charStr[2] = { c, '\0' };
            current = current + String(charStr);
        }
    }

    if (!current.empty()) {
        args.push_back(current);
    }

    return args;
}

int main() {
    RailwaySystem system;

    try {
        system.loadAdmins("admins.txt");
        system.loadAllData();
    }
    catch (const std::exception& e) {
        std::cerr << "Initialization error: " << e.what() << "\n";
        return 1;
    }

    // Run either automated tests or normal interface
    std::cout << "Run automated tests? (y/n): ";
    String input = String::read_line(std::cin);

    if (input == "y" || input == "Y") {
        runAutomatedTests(system);
    }
    else {
        // Normal interactive mode
        while (true) {
            try {
                std::cout << "> ";
                String input = String::read_line(std::cin);
                if (input == "exit") {
                    system.saveAllData();
                    break;
                }
                Vector<String> args = parseInput(input);
                system.processCommand(args);
            }
            catch (...) {
                std::cerr << "Error processing command\n";
            }
        }
    }

    return 0;
}