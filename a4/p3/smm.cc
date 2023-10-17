#include <algorithm>
#include <iostream>
#include <map>
#include <sstream>
#include <stdexcept>
#include <string>
#include <tuple>
#include <vector>

const std::string ALPHABET = ".ALPHABET";
const std::string STATES = ".STATES";
const std::string TRANSITIONS = ".TRANSITIONS";
const std::string INPUT = ".INPUT";
const std::string EMPTY = ".EMPTY";
const int REJECT_VAL = 0;
const int ACCEPT_VAL = 1;

bool isChar(std::string s) {
    return s.length() == 1;
}
bool isRange(std::string s) {
    return s.length() == 3 && s[1] == '-';
}

// Locations in the program that you should modify to store the
// DFA information have been marked with four-slash comments:
//// (Four-slash comment)
int main() {
    std::istream &in = std::cin;
    std::string s;

    std::vector<char> alpha;
    std::map<std::string, int> states;
    std::string initialState;
    std::vector<std::tuple<std::string, char, std::string>> transitions;

    std::getline(in, s); // Alphabet section (skip header)
    // Read characters or ranges separated by whitespace
    while (in >> s) {
        if (s == STATES) {
            break;
        } else {
            if (isChar(s)) {
                //// Variable 's[0]' is an alphabet symbol
                alpha.push_back(s[0]);
            } else if (isRange(s)) {
                for (char c = s[0]; c <= s[2]; ++c) {
                    //// Variable 'c' is an alphabet symbol
                    alpha.push_back(c);
                }
            }
        }
    }

    std::getline(in, s); // States section (skip header)
    // Read states separated by whitespace
    while (in >> s) {
        if (s == TRANSITIONS) {
            break;
        } else {
            static bool initial = true;
            bool accepting = false;
            if (s.back() == '!' && !isChar(s)) {
                accepting = true;
                s.pop_back();
            }
            //// Variable 's' contains the name of a state
            states[s] = REJECT_VAL;
            if (initial) {
                //// The state is initial
                initial = false;
                states[s] = REJECT_VAL;
                initialState = s;
            }
            if (accepting) {
                //// The state is accepting
                states[s] = ACCEPT_VAL;
            }
        }
    }

    std::getline(in, s); // Transitions section (skip header)
    // Read transitions line-by-line
    while (std::getline(in, s)) {
        if (s == INPUT) {
            // Note: Since we're reading line by line, once we encounter the
            // input header, we will already be on the line after the header
            break;
        } else {
            std::string fromState, symbols, toState;
            std::istringstream line(s);
            line >> fromState;
            bool last;
            while (line >> s) {
                if (line.peek() == EOF) { // If we reached the last item on the line
                    // Then it's the to-state
                    toState = s;
                } else { // Otherwise, there is more stuff on the line
                    // We expect a character or range
                    if (isChar(s)) {
                        symbols += s;
                    } else if (isRange(s)) {
                        for (char c = s[0]; c <= s[2]; ++c) {
                            symbols += c;
                        }
                    }
                }
            }
            for (char c : symbols) {
                //// There is a transition from 'fromState' to 'toState' on 'c'
                std::tuple<std::string, char, std::string> trans = {fromState, c, toState};
                transitions.push_back(trans);
            }
        }
    }

    try {
        bool rejectState = false;

        // Input section (already skipped header)
        // this while loop should only run once
        while (in >> s) {
            //// Variable 's' contains an input string for the DFA
            std::string input;
            std::string currentState = initialState;
            auto it = s.begin();
            while (it != s.end()) { // iterate through each character
                char c = *it;
                if (std::find(alpha.begin(), alpha.end(), c) == alpha.end()) {
                    throw std::invalid_argument("not in alphabet");
                }
                bool isTransitioned = false;
                for (auto t : transitions) {
                    std::string from = std::get<0>(t);
                    char transitionChar = std::get<1>(t);
                    std::string to = std::get<2>(t);
                    if (currentState == from && c == transitionChar) {
                        input += c;
                        currentState = to;
                        isTransitioned = true;
                        break;
                    }
                }
                if (!isTransitioned || it == s.end() - 1) {
                    // std::cout << input << std::endl;
                    auto finalState = states.find(currentState);
                    if (finalState != states.end() && finalState->second == ACCEPT_VAL) {
                        std::cout << input << std::endl;
                    } else {
                        throw std::invalid_argument("not in accept state");
                    }
                    input = "";
                    currentState = initialState;
                    if (!isTransitioned) {
                        it = it - 1;
                    }
                }
                ++it;
            }
        }
    } catch (const std::invalid_argument &e) {
        std::cerr << "ERROR" << std::endl;
    }
}