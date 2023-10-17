#include "wlp4data.h"
#include <algorithm>
#include <iostream>
#include <memory>
#include <ostream>
#include <sstream>
#include <string>
#include <vector>

const int OFFSET_MULT = -4;
int stackSize = 0;

int getStackSize(bool isAdd) {
    int retVal = stackSize;
    if (isAdd) { // increment stack size
        ++stackSize; 
    } else { // decrement stack size
        --stackSize;
    }
    return retVal;
}

int getOffset(bool isPush) {
    if (isPush) {
        return getStackSize(true) * OFFSET_MULT;
    } else {
        return getStackSize(false) * OFFSET_MULT;
    }
}



class Variable {
public:
    std::string id;
    std::string type;
    int offset; // offset from $29

    Variable(std::string varId, std::string varType, int varOffset = 0) {
        id = varId;
        type = varType;
        offset = varOffset;
    }
};

class SymbolTable {
public:
    std::map<std::string, Variable> variableTable;

    void add(Variable v) {
        // v.offset = getOffset(true);
        std::string id = v.id;
        variableTable.insert({id, v});
    }

    Variable find(std::string id) {
        auto out = variableTable.find(id);
        if (out != variableTable.end()) {
            return out->second;
        } else {
            throw std::invalid_argument("can't find variable in table");
        }
    }
};

class Node {
public:
    std::string rule;
    std::vector<std::shared_ptr<Node>> children;

    Node(std::string nodeRule) {
        if (nodeRule.find(":") != std::string::npos) {
            nodeRule = nodeRule.substr(0, nodeRule.find(" : "));
        } // remove the typing
        rule = nodeRule;
    }

    // Gets the "from" string from the rule (first word)
    std::string getFrom() {
        std::string fromWord = rule.substr(0, rule.find(" "));
        return fromWord;
    }

    std::string getTo() {
        std::string to;
        if (rule.find(":") != std::string::npos) { // case where there is a type
            to = rule.substr(rule.find(" ") + 1);
            to = to.substr(0, to.find(" "));
        } else {
            to = rule.substr(rule.find(" ") + 1);
        }
        return to;
    }

    void printTree() {
        std::cout << rule;
        std::cout << std::endl;
        for (auto child : children) {
            child->printTree();
        }
    }
};

// Gets the number of children for the parseTree function
int numberOfChildren(std::string s, const std::vector<std::string> &terminals) {
    std::istringstream line(s);
    std::string tokenKind;
    line >> tokenKind;
    if (tokenKind == EMPTY) {
        return 0;
    }
    int count = 0;
    std::string temp;
    while (line >> temp) {
        if (temp == ":") {
            break;
        } else if (temp != EMPTY) {
            ++count;
        }
    }
    return count;
}

// Parses the tree from std::cin
std::shared_ptr<Node> parseTree(const std::vector<std::string> &terminals) {
    std::istream &in = std::cin;
    std::string rule;
    std::getline(in, rule);
    std::vector<std::shared_ptr<Node>> children;

    // iterating through the rule to create children
    std::istringstream iss(rule);
    std::string s;
    iss >> s;
    if (find(terminals.begin(), terminals.end(), s) == terminals.end()) {
        for (int i = 0; i < numberOfChildren(rule, terminals); ++i) {
            children.push_back(parseTree(terminals));
        }
    }
    std::shared_ptr<Node> n(new Node(rule));
    n->children = children;
    return n;
}

// Reading in terminals for WLP4
std::vector<std::string> getTerminals() {
    std::istringstream istr(WLP4_TERMINALS);
    std::istream &in = istr;
    std::vector<std::string> terminals;
    std::string s;
    std::getline(in, s); // skip .TERMINALS header
    while (in >> s) {
        terminals.push_back(s);
    }
    return terminals;
}

std::string commentCode(std::string code, std::string comment) {
    return code + ";" + comment;
}

void prologue() {
}

// Revert the stack ptr
void epilogue(SymbolTable &varTable) {
    int size = stackSize;
    std::cout << "\n; start epilogue" << std::endl;
    // revert stack ptr for each var we pushed onto stack
    for (int i = 0; i < size; ++i) {
        std::cout << "add $30 ,$30 ,$4" << std::endl;
    }
    std::cout << "jr $31" << std::endl;
}

// Pushes reg to the stack, returns offset from $29
int push(int reg) {
    int offset = getOffset(true);
    std::string out = "sw $" + std::to_string(reg) + ", -4($30)\nsub $30, $30, $4";
    std::cout << out << std::endl;
    return offset;
}

// Pops reg from the stack, returns offset from $29
int pop(int reg) {
    int offset = getOffset(false);
    std::string out = "add $30,$30,$4\nlw $" + std::to_string(reg) + ", -4($30)";
    std::cout << out << std::endl;
    return offset;
}

// Gets the code for expr and loads into $3
void code(std::shared_ptr<Node> expr, SymbolTable &varTable) {
    std::string out;
    std::string from = expr->getFrom();
    if (from == NUM) {
        std::string val = expr->getTo();
        out = "lis $3\n.word " + val;
    } else if (from == ID) { // get from varTable
        std::string id = expr->getTo();
        Variable v = varTable.find(id);
        out = "lw $3, " + std::to_string(v.offset) + "($29)";
    } else {
        for (auto child : expr->children) {
            code(child, varTable);
        }
    }

    // Avoid empty lines
    if (out.length() != 0) {
        std::cout << out << std::endl;
    }
}

// takes a dcl node and adds it as a variable (only for wain parameters)
void addVar(std::shared_ptr<Node> dcl, SymbolTable &varTable, int reg = 3) {
    auto typeNode = dcl->children.front();
    auto idNode = dcl->children.back();
    std::string type = typeNode->children.front()->getTo();
    std::string id = idNode->getTo();
    int offset = push(reg); // pushes variable to stack
    Variable v(id, type, offset);
    varTable.add(v);
}


// adds variable dcls to the variable table is generalized for all procedures
void doDcls(std::shared_ptr<Node> dcls, SymbolTable &varTable) {
    std::string from = dcls->getFrom();
    if (dcls->rule == "dcls dcls dcl BECOMES NUM SEMI") {
        doDcls(dcls->children.front(), varTable); // recurse on rest of dcls
        std::cout << "; adding var dcl" << std::endl;
        code(dcls->children.at(3), varTable);
        auto dcl = dcls->children.at(1);
        addVar(dcl, varTable);
    }
}

// Gets the first ID node in preorder traversal of n
std::shared_ptr<Node> getIdNode(std::shared_ptr<Node> n) {
    if (n->getFrom() != ID) { // case: lvalue → LPAREN lvalue RPAREN
        if (n->rule == "lvalue LPAREN lvalue RPAREN") {
            return getIdNode(n->children.at(1));
        } else if (n->rule == "lvalue ID") { // case: lvalue → ID
            return getIdNode(n->children.front());
        }
    }
    return n;
}

void codeStatement(std::shared_ptr<Node> statement, SymbolTable &varTable) {
    std::string to = statement->getTo();
    if (to == "lvalue BECOMES expr SEMI") { // lvalue statement
        std::cout << "; lvalue statement" << std::endl;
        auto lvalueNode = statement->children.front();
        auto idNode = getIdNode(lvalueNode);
        std::string id = idNode->getTo(); 
        Variable v = varTable.find(id);
        std::string offset = std::to_string(v.offset); // offset of the lvalue
        code(statement->children.at(2), varTable);
        std::cout << "sw $3, " + offset + "($29)" << std::endl;
    }
}

void doStatements(std::shared_ptr<Node> statements, SymbolTable &varTable) {
    if (statements->rule == "statements statements statement") {
        doStatements(statements->children.front(), varTable);
        auto statement = statements->children.back();
        codeStatement(statement, varTable);
    }
}

void populateWainVar(std::shared_ptr<Node> wain, SymbolTable &varTable) {
    int param = 1;
    for (auto child : wain->children) {
        std::string from = child->getFrom();
        if (from == DCL) {
            addVar(child, varTable, param);
            ++param;
        } else if (from == DCLS) {
            auto dcls = child;
            doDcls(dcls, varTable);
        } else if (from == STATEMENTS) {
            auto statements = child;
            doStatements(statements, varTable);
        } else if (from == EXPR) {
            std::cout << "; wain body (expr)" << std::endl;
            code(child, varTable);
        }
    }
}

void traverseTree(std::shared_ptr<Node> n, SymbolTable &varTable) {
    // do something with n here
    if (n->getFrom() == "main") {
        populateWainVar(n, varTable);
        return;
    }
    // recurse on it's children
    for (auto child : n->children) {
        traverseTree(child, varTable);
    }
}


int main() {
    const std::vector<std::string> terminals = getTerminals();
    std::shared_ptr<Node> root = parseTree(terminals);
    SymbolTable wainTable; // keeps track of all the variables declared in function wain
    try {
        std::cout << PROLOGUE << std::endl;

        // =========================== ROOT OPERATIONS =====================================
        // root->printTree();
        traverseTree(root, wainTable);
        epilogue(wainTable);

        // ============================= TESTING =====================================
        // for (auto it = wainTable.variableTable.begin(); it != wainTable.variableTable.end(); ++it) {
        //     std::cout << it->second.offset << std::endl;
        // }
    } catch (const std::invalid_argument &e) {
        std::cerr << "ERROR " << e.what() << std::endl;
    }
    // ============================ START =====================================

    return 0;
}