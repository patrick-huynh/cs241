#include <algorithm>
#include <fstream>
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

const std::string ID = "ID";
const std::string NUM = "NUM";
const std::string LPAREN = "LPAREN";
const std::string RPAREN = "RPAREN";
const std::string LBRACE = "LBRACE";
const std::string RBRACE = "RBRACE";
const std::string RETURN = "RETURN";
const std::string IF = "IF";
const std::string ELSE = "ELSE";
const std::string WHILE = "WHILE";
const std::string PRINTLN = "PRINTLN";
const std::string WAIN = "WAIN";
const std::string BECOMES = "BECOMES";
const std::string INT = "INT";
const std::string EQ = "EQ";
const std::string NE = "NE";
const std::string LT = "LT";
const std::string GT = "GT";
const std::string LE = "LE";
const std::string GE = "GE";
const std::string PLUS = "PLUS";
const std::string MINUS = "MINUS";
const std::string STAR = "STAR";
const std::string SLASH = "SLASH";
const std::string PCT = "PCT";
const std::string COMMA = "COMMA";
const std::string SEMI = "SEMI";
const std::string NEW = "NEW";
const std::string DELETE = "DELETE";
const std::string LBRACK = "LBRACK";
const std::string RBRACK = "RBRACK";
const std::string AMP = "AMP";
const std::string null = "NULL";

std::map<std::string, std::string> tokens = {
    {"id", "ID"},
    {"num", "NUM"},
    {"(", "LPAREN"},
    {")", "RPAREN"},
    {"{", "LBRACE"},
    {"}", "RBRACE"},
    {"return", "RETURN"},
    {"if", "IF"},
    {"else", "ELSE"},
    {"while", "WHILE"},
    {"println", "PRINTLN"},
    {"wain", "WAIN"},
    {"=", "BECOMES"},
    {"int", "INT"},
    {"==", "EQ"},
    {"!=", "NE"},
    {"<", "LT"},
    {">", "GT"},
    {"<=", "LE"},
    {">=", "GE"},
    {"+", "PLUS"},
    {"-", "MINUS"},
    {"*", "STAR"},
    {"/", "SLASH"},
    {"%", "PCT"},
    {",", "COMMA"},
    {";", "SEMI"},
    {"new", "NEW"},
    {"delete", "DELETE"},
    {"[", "LBRACK"},
    {"]", "RBRACK"},
    {"&", "AMP"},
    {"NULL", "NULL"}};

bool isChar(std::string s) {
    return s.length() == 1;
}
bool isRange(std::string s) {
    return s.length() == 3 && s[1] == '-';
}

void outputLexSyntax(std::string input) {
    auto token = tokens.find(input);
    std::string output;
    if (token != tokens.end()) {
        output = token->second;
    } else if (input[0] >= '0' && input[0] <= '9') { // num case
        output = "NUM";
        try {
            int num = stoi(input);
            std::cout << output << " " << num << std::endl;
        } catch (...) {
            std::cerr << "ERROR" << std::endl;
        }
        return;
    } else {
        output = "ID";
    }
    std::cout << output << " " << input << std::endl;
}

// Locations in the program that you should modify to store the
// DFA information have been marked with four-slash comments:
//// (Four-slash comment)
int main() {
    std::istream &in = std::cin;
    std::string s;

    std::string filename = "wlp4.dfa";
    std::ifstream rfile;
    rfile.open(filename);

    std::vector<char> alpha;
    std::map<std::string, int> states;
    std::string initialState;
    std::vector<std::tuple<std::string, char, std::string>> transitions;

    std::getline(rfile, s); // Alphabet section (skip header)
    // Read characters or ranges separated by whitespace
    while (rfile >> s) {
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

    std::getline(rfile, s); // States section (skip header)
    // Read states separated by whitespace
    while (rfile >> s) {
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

    std::getline(rfile, s); // Transitions section (skip header)
    // Read transitions line-by-line
    while (std::getline(rfile, s)) {
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
                    auto finalState = states.find(currentState);
                    if (finalState != states.end() && finalState->second == ACCEPT_VAL) {
                        if (input.size() >= 2 && input[0] == '/' && input[1] == '/') {
                            std::getline(in, s);
                            break;
                        } else {
                            outputLexSyntax(input);
                        }

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
        std::cerr << "ERROR " << e.what() << std::endl;
    }
}