#include <deque>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

const std::string CFG = ".CFG";
const std::string INPUT = ".INPUT";
const std::string ACTIONS = ".ACTIONS";
const std::string END = ".END";
const std::string EMPTY = ".EMPTY";
const std::string PRINT = "print";
const std::string SHIFT = "shift";
const std::string REDUCE = "reduce";
const std::string DOT = ".";

// Prints the current parsing progress of the input sequence
void print(const std::deque<std::string> &unread, const std::vector<std::string> &read) {
    for (std::string symbol : read) {
        std::cout << symbol << " ";
    }

    // Printing the dot between the read and the unread
    if (read.empty()) { // if empty. don't print a space before the dot
        std::cout << DOT << " ";
    } else if (unread.empty()) {
        std::cout << DOT << std::endl;
    } else {
        std::cout << DOT << " ";
    }

    for (std::string input : unread) {
        if (input == unread.back()) {
            std::cout << input << std::endl;
        } else {
            std::cout << input << " ";
        }
    }
}

int main() {
    std::istream &in = std::cin;
    std::string s;

    std::deque<std::string> unread;
    std::vector<std::string> read;
    std::vector<std::vector<std::string>> productions;

    std::getline(in, s); // CFG section (skip header)
    // Keep track of the CFG productions
    while (std::getline(in, s)) {
        if (s == INPUT) {
            break;
        } else {
            std::string symbol;
            std::vector<std::string> production;
            std::istringstream line(s);
            while (line >> symbol) {
                production.push_back(symbol);
            }
            productions.push_back(production);
        }
    }

    while (in >> s) { // INPUT section
        if (s == ACTIONS) {
            break;
        } else {
            unread.push_back(s);
        }
    }

    while (in >> s) { // ACTIONS section
        if (s == END) {
            break;
        } else {
            if (s == REDUCE) {
                in >> s;
                int prodNum = stoi(s);
                std::vector<std::string> production = productions[prodNum];                      // the production we want
                std::string leftSide = production.front();                                       // the left side of the production
                std::vector<std::string> rightSide = {production.begin() + 1, production.end()}; // the right side of the production
                while (!rightSide.empty()) {
                    if (rightSide.back() == EMPTY) { // don't pop the read
                        rightSide.pop_back();
                    } else {
                        rightSide.pop_back();
                        read.pop_back();
                    }
                }
                read.push_back(leftSide);
            } else if (s == SHIFT) {
                std::string symbol = unread.front();
                unread.pop_front();
                read.push_back(symbol);
            } else if (s == PRINT) {
                print(unread, read);
            }
        }
    }

    // TESTING ============================================================

    // for (auto it = productions.begin(); it != productions.end(); ++it) {
    //     std::vector<std::string> production = *it;
    //     for (auto w : production) {
    //         std::cout << w << " ";
    //     }
    //     std::cout << std::endl;
    // }
    return 0;
}
