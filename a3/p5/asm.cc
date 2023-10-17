#include "scanner.h"
#include <algorithm>
#include <cctype>
#include <iostream>
#include <map>
#include <math.h>
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

        /** Checks if label is already in the map labels*/
        auto isDuplicateLabel = [&](std::string label) {
            return labels.find(label) != labels.end();
        };

        /** Checks if the label format is valid*/
        auto isValidLabel = [](std::string label) {
            if (!isalpha(label.front())) {
                return false;
            }
            return std::all_of(label.begin(), label.end(), [](char ch) {
                return (isalnum(ch));
            });
        };

        /** First iteration: to get the lines*/
        while (getline(std::cin, line)) {
            std::vector<Token> tokenLine = scan(line);

            // bool isLineLabel = true;
            for (long unsigned int i = 0; i < tokenLine.size(); ++i) {
                auto token = tokenLine[i];
                if (token.getKind() == Token::LABEL) {
                    std::string label = token.getLexeme();
                    label = label.substr(0, label.size() - 1);
                    if (!isValidLabel(label) || isDuplicateLabel(label)) {
                        throw std::invalid_argument("invalid or duplicate label");
                    }
                    labels.insert(std::pair<std::string, int>(label, idx));
                    tokenLine.erase(tokenLine.begin() + i); // remove the label declaration
                    --i;
                } else {
                    // isLineLabel = false; // if the only thing on that line is a label
                }
            }
            // if (tokenLine.size() > 0 && !isLineLabel) {
            if (tokenLine.size() > 0) {
                lines.insert(std::pair<int, std::vector<Token>>(idx, tokenLine)); // insert the line to stack
                ++idx;
            }
        }

        /** VALIDATION FUNCTIONS*/

        auto isValidReg = [](Token token) {
            // is reg kind and has value between 0-31
            return token.getKind() == 8 && token.toNumber() >= 0 && token.toNumber() <= 31;
        };

        auto isValidImmVal = [&](Token token, bool onlyNum = false) {
            // is a valid label that has been defined
            if (!onlyNum && token.getKind() == Token::ID) {
                std::string label = token.getLexeme();
                return isDuplicateLabel(label);
            } else if (token.getKind() == Token::INT) {
                int64_t val = token.toNumber();
                return val <= std::pow(2, 32) - 1 && val >= -pow(2, 32);
            } else if (token.getKind() == Token::HEXINT) {
                int64_t val = token.toNumber();
                return val <= std::pow(2, 32) - 1 && val >= -pow(2, 32);
            }
            return false;
        };

        /** instruction validator that accepts 3 registers*/
        auto is3Reg = [&](std::vector<Token> tokenLine, long unsigned i) {
            if (tokenLine.size() - i != 6) {
                return false;
            }
            int numReg = 0;
            for (int j = 1; j <= 5; ++j) {
                auto token = tokenLine[i + j];
                if (j % 2 == 0 && token.getKind() != 3) { // comma check
                    return false;
                } else if (isValidReg(token)) { // check for valid reg
                    ++numReg;
                }
                ++j;
            }
            return numReg == 3;
        };

        /** instruction validator that accepts 2 registers*/
        auto is2Reg = [&](std::vector<Token> tokenLine, long unsigned i) {
            if (tokenLine.size() - i != 4) {
                return false;
            }
            int numReg = 0;
            for (int j = 1; j <= 3; ++j) {
                auto token = tokenLine[i + j];
                if (j % 2 == 0 && token.getKind() != 3) { // comma check
                    return false;
                } else if (isValidReg(token)) { // check for valid reg
                    ++numReg;
                }
                ++j;
            }
            return numReg == 2;
        };

        /** instruction validator that accepts 1 register*/
        auto is1Reg = [&](std::vector<Token> tokenLine, long unsigned i) {
            if (tokenLine.size() - i != 2) {
                return false;
            }
            return isValidReg(tokenLine[i + 1]);
        };

        /** instruction validator that accepts 2 registers and 1 immediate*/
        auto is2Reg1Imm = [&](std::vector<Token> tokenLine, long unsigned i) {
            int numValid = 0;
            if (tokenLine.size() - i != 6) {
                return false;
            }
            for (int j = 1; j <= 5; ++j) {
                auto token = tokenLine[i + j];
                if (j % 2 == 0 && token.getKind() != 3) { // comma check
                    return false;
                } else if (isValidReg(token) || (j == 5 && isValidImmVal(token))) {
                    ++numValid;
                }
            }
            return numValid == 3;
        };

        /** instruction validator that accepts 1 register, 1 immediate, then 1 register*/
        auto is2Reg1ImmAlt = [&](std::vector<Token> tokenLine, long unsigned i) {
            int numValid = 0;
            if (tokenLine.size() - i != 7) {
                return false;
            }
            for (int j = 1; j <= 6; ++j) {
                auto token = tokenLine[i + j];
                if (j == 2 && token.getKind() != 3) { // comma check
                    return false;
                } else if (j == 3 && !isValidImmVal(token, true)) { // immediate check
                    return false;
                } else if (j == 4 && token.getKind() != Token::LPAREN) { // left parenthesis check
                    return false;
                } else if (j == 6 && token.getKind() != Token::RPAREN) { // right parenthesis check
                    return false;
                } else if (isValidReg(token)) {
                    ++numValid;
                }
            }
            return numValid == 2;
        };

        /** .word validator*/
        auto isWord = [&](std::vector<Token> tokenLine, long unsigned i) {
            if (tokenLine.size() - i != 2) {
                return false;
            }
            return isValidImmVal(tokenLine[i + 1]);
        };

        /** print instr*/
        auto printInstr = [](int instr) {
            unsigned char c = instr >> 24;
            std::cout << c;
            c = instr >> 16;
            std::cout << c;
            c = instr >> 8;
            std::cout << c;
            c = instr;
            std::cout << c;
        };

        /** add instruction*/
        auto add = [&](std::vector<Token> tokenLine, long unsigned int i) {
            int64_t opcode = 0;
            int64_t d = tokenLine[i + 1].toNumber(); // $d
            int64_t s = tokenLine[i + 3].toNumber(); // $s
            int64_t t = tokenLine[i + 5].toNumber(); // $t
            int instr = (opcode << 26) | (s << 21) | (t << 16) | (d << 11) | 32;
            printInstr(instr);
        };

        /** sub instruction*/
        auto sub = [&](std::vector<Token> tokenLine, long unsigned int i) {
            int64_t opcode = 0;
            int64_t d = tokenLine[i + 1].toNumber(); // $d
            int64_t s = tokenLine[i + 3].toNumber(); // $s
            int64_t t = tokenLine[i + 5].toNumber(); // $t
            int64_t func = 34;
            int instr = (opcode << 26) | (s << 21) | (t << 16) | (d << 11) | func;
            printInstr(instr);
        };

        /** slt instruction*/
        auto slt = [&](std::vector<Token> tokenLine, long unsigned int i, bool isUnsigned = false) {
            int64_t opcode = 0;
            int64_t d = tokenLine[i + 1].toNumber(); // $d
            int64_t s = tokenLine[i + 3].toNumber(); // $s
            int64_t t = tokenLine[i + 5].toNumber(); // $t
            int64_t func = isUnsigned ? 43 : 42;
            int instr = (opcode << 26) | (s << 21) | (t << 16) | (d << 11) | func;
            printInstr(instr);
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
            int instr = (opcode << 26) | (s << 21) | (t << 16) | (offset & 0xFFFF);
            printInstr(instr);
        };

        /** bne instruction*/
        auto bne = [&](std::vector<Token> tokenLine, long unsigned int i, int64_t curLine) {
            int64_t opcode = 5;
            int64_t s = tokenLine[i + 1].toNumber(); // $s
            int64_t t = tokenLine[i + 3].toNumber(); // $t
            int64_t offset;
            if (tokenLine[i + 5].getKind() == 0) { // if label
                std::string key = tokenLine[i + 5].getLexeme();
                int pos = labels.find(key)->second; // the position in memory
                offset = pos - curLine;
            } else {
                offset = tokenLine[i + 5].toNumber(); // i offset
            }
            int instr = (opcode << 26) | (s << 21) | (t << 16) | (offset & 0xFFFF);
            printInstr(instr);
        };

        /** lw/sw instruction*/
        auto lwSw = [&](std::vector<Token> tokenLine, long unsigned int i, int64_t curLine, bool isSave = false) {
            int64_t opcode = isSave ? 43 : 35;
            int64_t s = tokenLine[i + 5].toNumber();      // $s
            int64_t t = tokenLine[i + 1].toNumber();      // $t
            int64_t offset = tokenLine[i + 3].toNumber(); // i offset
            int instr = (opcode << 26) | (s << 21) | (t << 16) | (offset & 0xFFFF);
            printInstr(instr);
        };

        /** mult instruction*/
        auto mult = [&](std::vector<Token> tokenLine, long unsigned int i, bool isUnsigned = false) {
            int64_t opcode = 0;
            int64_t s = tokenLine[i + 1].toNumber(); // $s
            int64_t t = tokenLine[i + 3].toNumber(); // $t
            int64_t func = isUnsigned ? 25 : 24;
            int instr = (opcode << 26) | (s << 21) | (t << 16) | func;
            printInstr(instr);
        };

        /** mult instruction*/
        auto div = [&](std::vector<Token> tokenLine, long unsigned int i, bool isUnsigned = false) {
            int64_t opcode = 0;
            int64_t s = tokenLine[i + 1].toNumber(); // $s
            int64_t t = tokenLine[i + 3].toNumber(); // $t
            int64_t func = isUnsigned ? 27 : 26;
            int instr = (opcode << 26) | (s << 21) | (t << 16) | func;
            printInstr(instr);
        };

        /** mf instruction*/
        auto mf = [&](std::vector<Token> tokenLine, long unsigned int i, bool fromLow = false) {
            int64_t opcode = 0;
            int64_t d = tokenLine[i + 1].toNumber(); // $d
            int64_t func = fromLow ? 18 : 16;
            int instr = (opcode << 26) | (d << 11) | func;
            printInstr(instr);
        };

        /** lis instruction*/
        auto lis = [&](std::vector<Token> tokenLine, long unsigned int i) {
            int64_t opcode = 0;
            int64_t d = tokenLine[i + 1].toNumber(); // $d
            int64_t func = 20;
            int instr = (opcode << 26) | (d << 11) | func;
            printInstr(instr);
        };

        /** lis instruction*/
        auto jr = [&](std::vector<Token> tokenLine, long unsigned int i, bool isLink = false) {
            int64_t opcode = 0;
            int64_t s = tokenLine[i + 1].toNumber(); // $s
            int64_t func = isLink ? 9 : 8;
            int instr = (opcode << 26) | (s << 21) | func;
            printInstr(instr);
        };

        /** .word instruction*/
        auto word = [&](std::vector<Token> tokenLine, long unsigned int i) {
            if (tokenLine[i + 1].getKind() == 0) { // if label
                const int WORD_SIZE = 4;
                std::string key = tokenLine[i + 1].getLexeme();
                int64_t pos = labels.find(key)->second * WORD_SIZE; // the address the label points
                printInstr(pos);
            } else {
                printInstr(tokenLine[i + 1].toNumber());
            }
        };

        /** Second iteration*/
        std::map<int, std::vector<Token>>::iterator itr;
        for (itr = lines.begin(); itr != lines.end(); ++itr) {
            std::vector<Token> tokenLine = itr->second;
            int64_t curLine = itr->first + 1; // line num
            // iterate thourgh each token in the line
            for (long unsigned int i = 0; i < tokenLine.size(); i++) {
                auto token = tokenLine[i];
                auto kind = token.getKind();
                if (kind == Token::ID) { // ID
                    std::string lexeme = token.getLexeme();
                    if (is3Reg(tokenLine, i)) { // 3 REG
                        if (lexeme == "add") {
                            add(tokenLine, i);
                            break;
                        } else if (lexeme == "sub") {
                            sub(tokenLine, i);
                            break;
                        } else if (lexeme == "slt") {
                            slt(tokenLine, i);
                            break;
                        } else if (lexeme == "sltu") {
                            slt(tokenLine, i, true);
                            break;
                        } else {
                            throw std::invalid_argument("invalid 3 reg");
                        }
                    } else if (is2Reg1Imm(tokenLine, i)) { // 2 REG, 1 IMM
                        if (lexeme == "beq") {
                            beq(tokenLine, i, curLine);
                            break;
                        } else if (lexeme == "bne") {
                            bne(tokenLine, i, curLine);
                            break;
                        } else {
                            throw std::invalid_argument("invalid 2 reg 1 imm");
                        }
                    } else if (is2Reg1ImmAlt(tokenLine, i)) {
                        if (lexeme == "lw") {
                            lwSw(tokenLine, i, curLine);
                            break;
                        } else if (lexeme == "sw") {
                            lwSw(tokenLine, i, curLine, true);
                            break;
                        } else {
                            throw std::invalid_argument("invalid 2 reg 1 imm alt");
                        }
                    } else if (is2Reg(tokenLine, i)) { // 2 REG
                        if (lexeme == "mult") {
                            mult(tokenLine, i);
                            break;
                        } else if (lexeme == "multu") {
                            mult(tokenLine, i, true);
                            break;
                        } else if (lexeme == "div") {
                            div(tokenLine, i);
                            break;
                        } else if (lexeme == "divu") {
                            div(tokenLine, i, true);
                            break;
                        } else {
                            throw std::invalid_argument("invalid 2 reg");
                        }
                    } else if (is1Reg(tokenLine, i)) { // 1 REG
                        if (lexeme == "mfhi") {
                            mf(tokenLine, i);
                            break;
                        } else if (lexeme == "mflo") {
                            mf(tokenLine, i, true);
                            break;
                        } else if (lexeme == "lis") {
                            lis(tokenLine, i);
                            break;
                        } else if (lexeme == "jr") {
                            jr(tokenLine, i);
                            break;
                        } else if (lexeme == "jalr") {
                            jr(tokenLine, i, true);
                            break;
                        } else {
                            throw std::invalid_argument("invalid 1 reg");
                        }
                    } else {
                        throw std::invalid_argument("invalid id");
                    }
                } else if (kind == Token::WORD) { // .word
                    if (isWord(tokenLine, i)) {
                        word(tokenLine, i);
                        break;
                    } else {
                        throw std::invalid_argument("invalid immediate value");
                    }
                } else if (kind == Token::LABEL) { // skip label
                    continue;
                } else {
                    // std::cout << "HERE>" << kind << "<" << std::endl;
                    throw std::invalid_argument("invalid kind");
                }
            }
        }
    } catch (ScanningFailure &f) {
        std::cerr << f.what() << std::endl;
        return 1;
    } catch (std::invalid_argument &m) {
        std::cerr << "ERROR"
                  << " " << m.what() << std::endl;
        return 1;
    }
    // You can add your own catch clause(s) for other kinds of errors.
    // Throwing exceptions and catching them is the recommended way to
    // handle errors and terminate the program cleanly in C++. Do not
    // use the std::exit function, which may leak memory.

    return 0;
}

// std::cout << "HERE>" << numValid << "<" << std::endl;
