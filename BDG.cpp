#pragma once
#include <iostream>
#include "RailwaySystem.h"
#include "String.h"
#include "Vector.hpp"

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
        std::cout << "Railway Management System ready. Type 'help' for commands.\n";
    }
    catch (const std::exception& e) {
        std::cerr << "Initialization error: " << e.what() << "\n";
        return 1;
    }

    while (true) {
        try {
            std::cout << "> ";
            String input = String::read_line(std::cin);

            if (input == "exit") {
                system.saveAllData();
                std::cout << "Saving data and exiting...\n";
                break;
            }

            Vector<String> args = parseInput(input);
            if (args.get_size() > 0) {
                system.processCommand(args);
            }
        }
        catch (const std::exception& ex) {
            std::cerr << "Error: " << ex.what() << "\n";
        }
        catch (...) {
            std::cerr << "Unknown error occurred\n";
        }
    }

    return 0;
}