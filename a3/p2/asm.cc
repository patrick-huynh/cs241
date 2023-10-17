#include "scanner.h"
#include <iostream>
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
        while (getline(std::cin, line)) {
            // This example code just prints the scanned tokens on each line.
            std::vector<Token> tokenLine = scan(line);

            // This code is just an example - you don't have to use a range-based
            // for loop in your actual assembler. You might find it easier to
            // use an index-based loop like this:
            for (long unsigned int i = 0; i < tokenLine.size(); i++) {
                auto token = tokenLine[i];
                // std::cout << token << ' ';
                int64_t opcode;
                int instr;
                if (token.getKind() == 0 && token.getLexeme() == "add") {
                    opcode = 0;
                    int64_t d = tokenLine[i + 1].toNumber(); // $d
                    int64_t s = tokenLine[i + 3].toNumber(); // $s
                    int64_t t = tokenLine[i + 5].toNumber(); // $t
                    instr = (opcode << 26) | (s << 21) | (t << 16) | (d << 11) | 32;
                } else if (token.getKind() == 0 && token.getLexeme() == "beq") {
                    opcode = 4;
                    int64_t s = tokenLine[i + 1].toNumber();      // $s
                    int64_t t = tokenLine[i + 3].toNumber();      // $t
                    int64_t offset = tokenLine[i + 5].toNumber(); // $i offset
                    instr = (opcode << 26) | (s << 21) | (t << 16) | (offset & 0xFFFF);
                }
                // Print the instr
                if (token.getKind() == 0) {
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

            // Remove this when you're done playing with the example program!
            // Printing a random newline character as part of your machine code
            // output will cause you to fail the Marmoset tests.
            // std::cout << std::endl;
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
