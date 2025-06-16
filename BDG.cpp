#pragma once
#include <iostream>
#include "RailwaySystem.h"
#include "String.h"
#include "Vector.hpp"

// Utility to run a command and print result
inline bool runCommand(RailwaySystem& system, const Vector<String>& cmd, bool expectSuccess = true, const char* failMsg = nullptr) {
    try {
        system.processCommand(cmd);
        if (expectSuccess)
            return true;
        std::cout << (failMsg ? failMsg : "Expected failure but command succeeded!\n");
        return false;
    }
    catch (const std::exception& ex) {
        if (!expectSuccess) return true; // Error expected: test passes
        std::cout << (failMsg ? failMsg : "Error: ") << ex.what() << "\n";
        return false;
    }
    catch (...) {
        if (!expectSuccess) return true; // Error expected: test passes
        std::cout << (failMsg ? failMsg : "Unknown error!\n");
        return false;
    }
}

inline void runAutomatedTests(RailwaySystem& system) {
    std::cout << "\n=== RUNNING AUTOMATED TESTS ===\n";

    auto makeCommand = [](const char* cmd, std::initializer_list<const char*> args = {}) {
        Vector<String> command;
        command.push_back(String(cmd));
        for (const char* arg : args) {
            command.push_back(String(arg));
        }
        return command;
        };

    int testTrain1 = 1000;
    int testTrain2 = 1001;
    int movedWagonID = 2;
    bool allPassed = true;

    // 1. Admin login
    bool ok = runCommand(system, makeCommand("login", { "ad", "123" }), true, "Admin login failed: ");
    if (ok) std::cout << "Admin login: PASS\n"; else { std::cout << "Admin login: FAIL\n"; allPassed = false; }

    // 2. Add stations
    ok = runCommand(system, makeCommand("add-station", { "TestSofia" }));
    if (ok) std::cout << "Add station Sofia: PASS\n"; else { std::cout << "Add station Sofia: FAIL\n"; allPassed = false; }
    ok = runCommand(system, makeCommand("add-station", { "TestPlovdiv" }));
    if (ok) std::cout << "Add station Plovdiv: PASS\n"; else { std::cout << "Add station Plovdiv: FAIL\n"; allPassed = false; }
    ok = runCommand(system, makeCommand("add-station", { "TestVarna" }));
    if (ok) std::cout << "Add station Varna: PASS\n"; else { std::cout << "Add station Varna: FAIL\n"; allPassed = false; }

    // 3. Print stations
    ok = runCommand(system, makeCommand("print-stations"));
    if (ok) std::cout << "Print stations: PASS\n"; else { std::cout << "Print stations: FAIL\n"; allPassed = false; }

    // 4. Add trains
    ok = runCommand(system, makeCommand("add-train", { "TestSofia", "TestPlovdiv", "150", "100", "15/06/2025", "08:00" }));
    if (ok) std::cout << "Add train 1000: PASS\n"; else { std::cout << "Add train 1000: FAIL\n"; allPassed = false; }
    ok = runCommand(system, makeCommand("add-train", { "TestPlovdiv", "TestVarna", "250", "120", "16/06/2025", "10:00" }));
    if (ok) std::cout << "Add train 1001: PASS\n"; else { std::cout << "Add train 1001: FAIL\n"; allPassed = false; }

    // 5. Print schedule and schedule-destination
    ok = runCommand(system, makeCommand("print-schedule", { "TestSofia" }));
    if (ok) std::cout << "Print schedule Sofia: PASS\n"; else { std::cout << "Print schedule Sofia: FAIL\n"; allPassed = false; }
    ok = runCommand(system, makeCommand("print-schedule-destination", { "TestSofia", "TestPlovdiv" }));
    if (ok) std::cout << "Print schedule-destination: PASS\n"; else { std::cout << "Print schedule-destination: FAIL\n"; allPassed = false; }

    // 6. Print train
    ok = runCommand(system, makeCommand("print-train", { String::from_int(testTrain1).c_str() }));
    if (ok) std::cout << "Print train 1000: PASS\n"; else { std::cout << "Print train 1000: FAIL\n"; allPassed = false; }

    // 7. Add wagons to train 1000
    ok = runCommand(system, makeCommand("add-wagon", { String::from_int(testTrain1).c_str(), "first-class", "50", "0.8" }));
    if (ok) std::cout << "Add wagon 1 to 1000: PASS\n"; else { std::cout << "Add wagon 1 to 1000: FAIL\n"; allPassed = false; }
    ok = runCommand(system, makeCommand("add-wagon", { String::from_int(testTrain1).c_str(), "second-class", "80", "0.5" }));
    if (ok) std::cout << "Add wagon 2 to 1000: PASS\n"; else { std::cout << "Add wagon 2 to 1000: FAIL\n"; allPassed = false; }

    // 8. Add wagon to train 1001 (for move test)
    ok = runCommand(system, makeCommand("add-wagon", { String::from_int(testTrain2).c_str(), "second-class", "60", "0.3" }));
    if (ok) std::cout << "Add wagon 1 to 1001: PASS\n"; else { std::cout << "Add wagon 1 to 1001: FAIL\n"; allPassed = false; }

    // 9. Print wagon
    ok = runCommand(system, makeCommand("print-wagon", { String::from_int(testTrain1).c_str(), "1" }));
    if (ok) std::cout << "Print wagon 1: PASS\n"; else { std::cout << "Print wagon 1: FAIL\n"; allPassed = false; }
    ok = runCommand(system, makeCommand("print-wagon", { String::from_int(testTrain1).c_str(), "2" }));
    if (ok) std::cout << "Print wagon 2: PASS\n"; else { std::cout << "Print wagon 2: FAIL\n"; allPassed = false; }

    // 10. Buy ticket (wagon 1, seat 5)
    ok = runCommand(system, makeCommand("buy-ticket", { String::from_int(testTrain1).c_str(), "1", "5", "test_ticket.txt", "1" }));
    if (ok) std::cout << "Buy ticket: PASS\n"; else { std::cout << "Buy ticket: FAIL\n"; allPassed = false; }

    // 11. Discount card and buy ticket with discount
    ok = runCommand(system, makeCommand("create-discount-card", { "Age", "TestUser", "test_card.txt", "15" }));
    if (ok) std::cout << "Create discount card: PASS\n"; else { std::cout << "Create discount card: FAIL\n"; allPassed = false; }
    ok = runCommand(system, makeCommand("validate-discount-card", { "test_card.txt" }));
    if (ok) std::cout << "Validate discount card: PASS\n"; else { std::cout << "Validate discount card: FAIL\n"; allPassed = false; }
    ok = runCommand(system, makeCommand("buy-ticket-discount", { String::from_int(testTrain1).c_str(), "1", "6", "test_discount_ticket.txt", "test_card.txt", "1" }));
    if (ok) std::cout << "Buy ticket discount: PASS\n"; else { std::cout << "Buy ticket discount: FAIL\n"; allPassed = false; }

    // 12. Print arrival/departure boards
    ok = runCommand(system, makeCommand("print-arrival-board", { "TestPlovdiv" }));
    if (ok) std::cout << "Print arrival board: PASS\n"; else { std::cout << "Print arrival board: FAIL\n"; allPassed = false; }
    ok = runCommand(system, makeCommand("print-departure-board", { "TestSofia" }));
    if (ok) std::cout << "Print departure board: PASS\n"; else { std::cout << "Print departure board: FAIL\n"; allPassed = false; }

    // 13. Move wagon 2 from train 1000 to 1001
    ok = runCommand(system, makeCommand("move-wagon", { String::from_int(testTrain1).c_str(), "2", String::from_int(testTrain2).c_str() }));
    if (ok) std::cout << "Move wagon 2 from 1000 to 1001: PASS\n"; else { std::cout << "Move wagon 2 from 1000 to 1001: FAIL\n"; allPassed = false; }

    // 14. Remove wagon (from train 1001 now after move)
    ok = runCommand(system, makeCommand("remove-wagon", { String::from_int(testTrain2).c_str(), "2" }));
    if (ok) std::cout << "Remove wagon 2 from 1001: PASS\n"; else { std::cout << "Remove wagon 2 from 1001: FAIL\n"; allPassed = false; }

    // 15. Remove train
    ok = runCommand(system, makeCommand("remove-train", { String::from_int(testTrain1).c_str() }));
    if (ok) std::cout << "Remove train 1000: PASS\n"; else { std::cout << "Remove train 1000: FAIL\n"; allPassed = false; }
    ok = runCommand(system, makeCommand("remove-train", { String::from_int(testTrain2).c_str() }));
    if (ok) std::cout << "Remove train 1001: PASS\n"; else { std::cout << "Remove train 1001: FAIL\n"; allPassed = false; }

    // 16. Remove station (should succeed if station is now unused)
    ok = runCommand(system, makeCommand("remove-station", { "TestVarna" }));
    if (ok) std::cout << "Remove station Varna: PASS\n"; else { std::cout << "Remove station Varna: FAIL\n"; allPassed = false; }

    // 17. Logout
    ok = runCommand(system, makeCommand("logout"));
    if (ok) std::cout << "Logout: PASS\n"; else { std::cout << "Logout: FAIL\n"; allPassed = false; }

    // 18. User login (should fail, expect error)
    ok = runCommand(system, makeCommand("login", { "user", "userpass" }), false);
    if (ok) std::cout << "User login (should fail): PASS\n"; else { std::cout << "User login (should fail): FAIL\n"; allPassed = false; }

    // 19. Invalid command (should fail)
    ok = runCommand(system, makeCommand("foobar"), false);
    if (ok) std::cout << "Invalid command (should fail): PASS\n"; else { std::cout << "Invalid command (should fail): FAIL\n"; allPassed = false; }

    // 20. Test insufficient parameter (should fail)
    ok = runCommand(system, makeCommand("print-schedule-destination", { "TestSofia" }), false);
    if (ok) std::cout << "Insufficient params for print-schedule-destination (should fail): PASS\n"; else { std::cout << "Insufficient params for print-schedule-destination (should fail): FAIL\n"; allPassed = false; }

    std::cout << "\n=== TEST SUMMARY ===\n";
    std::cout << "OVERALL RESULT: " << (allPassed ? "SUCCESS" : "FAILURE") << "\n";
    std::cout << "====================\n";
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
            catch (const std::exception& ex) {
                std::cerr << "Error: " << ex.what() << "\n";
            }
            catch (...) {
                std::cerr << "Unknown error processing command\n";
            }
        }
    }
    return 0;
}