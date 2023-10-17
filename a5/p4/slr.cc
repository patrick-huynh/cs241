#include <deque>
#include <iostream>
#include <sstream>
#include <stack>
#include <stdexcept>
#include <string>
#include <vector>

const std::string CFG = ".CFG";
const std::string INPUT = ".INPUT";
const std::string ACTIONS = ".ACTIONS";
const std::string TRANSITIONS = ".TRANSITIONS";
const std::string REDUCTIONS = ".REDUCTIONS";
const std::string END = ".END";
const std::string EMPTY = ".EMPTY";
const std::string ACCEPT = ".ACCEPT";
const std::string END_OF_FILE = "EOF";
const std::string PRINT = "print";
const std::string SHIFT = "shift";
const std::string REDUCE = "reduce";
const std::string DOT = ".";
const std::string REJECT = "reject";

// Defining structs
struct transition {
    std::string fromState;
    std::string symbol;
    std::string toState;
};

struct reduction {
    std::string stateNum;
    std::string ruleNum;
    std::string tag;
};

// Prints the current parsing progress of the input sequence
void print(const std::vector<std::string> &unread, const std::deque<std::string> &symStack) {
    std::deque<std::string> copySymStack = symStack;
    while (!copySymStack.empty()) {
        std::string symbol = copySymStack.back();
        copySymStack.pop_back();
        std::cout << symbol << " ";
    }

    // Printing the dot between the read and the unread
    if (symStack.empty()) { // if empty. don't print a space before the dot
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

std::string getNextState(std::string fromState, std::string symbol, std::vector<transition> &transitions) {
    for (auto it = transitions.begin(); it != transitions.end(); ++it) {
        transition t = *it;
        if (t.fromState == fromState && t.symbol == symbol) {
            return t.toState;
        }
    }
    return REJECT;
}

int getReduction(std::string state, std::string symbol, std::vector<reduction> &reductions) {
    for (auto it = reductions.begin(); it != reductions.end(); ++it) {
        reduction r = *it;
        if (r.stateNum == state && r.tag == symbol) {
            return stoi(r.ruleNum);
        }
    }
    return -1;
}

int main() {
    std::istream &in = std::cin;
    std::string s;

    std::vector<std::string> unread;
    std::vector<std::string> read;
    std::vector<std::vector<std::string>> productions;

    std::deque<std::string> stateStack;
    std::deque<std::string> symStack;
    std::vector<transition> transitions;
    std::vector<reduction> reductions;

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
        if (s == TRANSITIONS) {
            break;
        } else {
            unread.push_back(s);
        }
    }

    while (std::getline(in, s)) { // TRANSITIONS section
        if (s == REDUCTIONS) {
            break;
        } else {
            std::string fromState;
            std::string symbol;
            std::string toState;
            std::istringstream line(s);
            line >> fromState;
            line >> symbol;
            line >> toState;
            transitions.push_back({fromState, symbol, toState}); // Push transition to vector of transitions
        }
    }

    while (std::getline(in, s)) { // REDUCTIONS section
        if (s == END) {
            break;
        } else {
            std::string stateNum;
            std::string ruleNum;
            std::string tag;
            std::istringstream line(s);
            line >> stateNum;
            line >> ruleNum;
            line >> tag;
            reductions.push_back({stateNum, ruleNum, tag}); // Push reduction to vector of reductions
        }
    }

    // SLR(1) algorithm
    try {
        stateStack.push_front("0"); // push first state
        int symbolCounter = 0;
        print({unread.begin() + symbolCounter, unread.end()}, symStack); // Print at beginning
        std::vector<std::string> input = unread;
        input.push_back(ACCEPT); // Add the accept string to the end so when we reach it successfully, it is accepted
        for (std::string a : input) {
            while (1) {
                int i = getReduction(stateStack.front(), a, reductions);
                if (i == -1) {
                    break;
                } else {
                    std::vector<std::string> rule = productions[i];
                    while (rule.size() > 1) {
                        if (rule.back() != EMPTY) { // Do not pop from symStack or stateStack if the right side of rule has an empty
                            symStack.pop_front();
                            stateStack.pop_front();
                        }
                        rule.pop_back();
                    }
                    symStack.push_front(rule[0]);
                    std::string nextState = getNextState(stateStack.front(), symStack.front(), transitions);
                    stateStack.push_front(nextState);
                }
                print({unread.begin() + symbolCounter, unread.end()}, symStack); // Print after reduce
                if (a == ACCEPT) {                                               // case when we successfully read all input and accept
                    return 0;
                }
            }
            symStack.push_front(a);
            std::string nextState = getNextState(stateStack.front(), a, transitions);
            if (nextState == REJECT) {
                throw std::invalid_argument(std::to_string(symbolCounter + 1));
            }
            stateStack.push_front(nextState);
            ++symbolCounter;                                                 // increment symbolCounter since we just read a symbol
            print({unread.begin() + symbolCounter, unread.end()}, symStack); // Print after shift
        }

    } catch (const std::invalid_argument &e) {
        std::cerr << "ERROR at " << e.what() << std::endl;
    }

    // TESTING ============================================================
    // for (auto it = reductions.begin(); it != reductions.end(); ++it) {
    //     reduction r = *it;
    //     std::cout << r.stateNum << " ";
    //     std::cout << r.ruleNum << " ";
    //     std::cout << r.tag << " ";
    //     std::cout << std::endl;
    // }
    return 0;
}
