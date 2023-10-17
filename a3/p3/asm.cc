#include "scanner.h"
#include <iostream>
#include <map>
#include <string>
#include <vector>

/*
 * C++ Starter code for CS241 A3
 *
 * This file contains the main function of your program. By default, it just
 * prints the scanned list of tokens back to standard output.
 */
int main() {
    std::string line;
    try {
        int idx = 0;                       // line index tracker
        std::map<std::string, int> labels; // label stack
        std::map<int, std::vector<Token>> lines;

        /** First iteration: to get the lines*/
        while (getline(std::cin, line)) {
            std::vector<Token> tokenLine = scan(line);

            bool isLineLabel = true;
            for (long unsigned int i = 0; i < tokenLine.size(); i++) {
                auto token = tokenLine[i];
                if (token.getKind() == 1) { // if kind == label
                    std::string label = token.getLexeme();
                    labels.insert(std::pair<std::string, int>(label.substr(0, label.length() - 1), idx));
                } else {
                    isLineLabel = false; // if the only thing on that line is a label
                }
                // std::cout << token << ' ';
            }
            if (tokenLine.size() > 0 && !isLineLabel) {
                lines.insert(std::pair<int, std::vector<Token>>(idx, tokenLine)); // insert the line to stack
                ++idx;
            }
        }

        /** add instruction*/
        auto add = [](std::vector<Token> tokenLine, long unsigned int i) {
            int64_t opcode = 0;
            int64_t d = tokenLine[i + 1].toNumber(); // $d
            int64_t s = tokenLine[i + 3].toNumber(); // $s
            int64_t t = tokenLine[i + 5].toNumber(); // $t
            return (opcode << 26) | (s << 21) | (t << 16) | (d << 11) | 32;
        };

        /** beq instruction*/
        auto beq = [&](std::vector<Token> tokenLine, long unsigned int i, int64_t curLine) {
            int64_t opcode = 4;
            int64_t s = tokenLine[i + 1].toNumber(); // $s
            int64_t t = tokenLine[i + 3].toNumber(); // $t
            int64_t offset;
            if (tokenLine[i + 5].getKind() == 0) { // if label
                std::string key = tokenLine[i + 5].getLexeme();
                int pos = labels.find(key)->second; // the position in memory
                offset = pos - curLine;
            } else {
                offset = tokenLine[i + 5].toNumber(); // $i offset
            }
            return (opcode << 26) | (s << 21) | (t << 16) | (offset & 0xFFFF);
        };

        /** .word instruction*/
        auto word = [&](std::vector<Token> tokenLine, long unsigned int i) {
            if (tokenLine[i + 1].getKind() == 0) { // if label
                const int WORD_SIZE = 4;
                std::string key = tokenLine[i + 1].getLexeme();
                int64_t pos = labels.find(key)->second * WORD_SIZE; // the address the label points
                return pos;
            } else {
                return tokenLine[i + 1].toNumber();
            }
        };

        /** Second iteration*/
        std::map<int, std::vector<Token>>::iterator itr;
        for (itr = lines.begin(); itr != lines.end(); ++itr) {
            std::vector<Token> tokenLine = itr->second;
            int64_t curLine = itr->first + 1;
            for (long unsigned int i = 0; i < tokenLine.size(); i++) {
                auto token = tokenLine[i];
                bool validInstr = true;
                int instr = 0;
                if (token.getKind() == 0) { // ID
                    std::string lexeme = token.getLexeme();
                    if (lexeme == "add") {
                        instr = add(tokenLine, i);
                    } else if (lexeme == "beq") {
                        instr = beq(tokenLine, i, curLine);
                    } else {
                        validInstr = false;
                    }
                } else if (token.getKind() == 2) { // .word
                    instr = word(tokenLine, i);
                } else {
                    validInstr = false;
                }

                // Print the instr
                if (validInstr) {
                    unsigned char c = instr >> 24;
                    std::cout << c;
                    c = instr >> 16;
                    std::cout << c;
                    c = instr >> 8;
                    std::cout << c;
                    c = instr;
                    std::cout << c;
                }
            }
        }
    } catch (ScanningFailure &f) {
        std::cerr << f.what() << std::endl;
        return 1;
    }
    // You can add your own catch clause(s) for other kinds of errors.
    // Throwing exceptions and catching them is the recommended way to
    // handle errors and terminate the program cleanly in C++. Do not
    // use the std::exit function, which may leak memory.

    return 0;
}
