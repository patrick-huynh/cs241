#include "wlp4data.h"
#include <algorithm>
#include <iostream>
#include <memory>
#include <ostream>
#include <sstream>
#include <string>
#include <vector>

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
    int paramSize = 0;
    int localVarSize = 0;

    void add(Variable v, bool isProcedureParam = false) {
        if (isProcedureParam) {
            const int OFFSET_MULT = 4; // procedure params should have positive offsets
            v.offset = (paramSize + 1 ) * OFFSET_MULT;
            ++paramSize;
        } else {
            const int OFFSET_MULT = -4; // local var should have positive offsets
            v.offset = localVarSize * OFFSET_MULT;
            ++localVarSize;
        }
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

    void print() {
        for (auto it = variableTable.begin(); it != variableTable.end(); ++it) {
            std::cout << "key: " << it->first << " | val: " << it->second.id << " | offset: " << it->second.offset << std::endl;
        }
    }

    bool contains(std::string id) {
        return variableTable.find(id) != variableTable.end();
    }
};

class Node {
public:
    std::string rule;
    std::vector<std::shared_ptr<Node>> children;

    Node(std::string nodeRule) {
        if (rule.find(":") != std::string::npos) { // case where there is a type
            rule = nodeRule.substr(0, nodeRule.find(" : "));
        } else {
            rule = nodeRule;
        }
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
            to = to.substr(0, to.find(" : "));
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

void push(int reg) {
    std::string out = "sw $" + std::to_string(reg) + ", -4($30)\nsub $30, $30, $4";
    std::cout << out << std::endl; // 
}

void pop(int reg) {
    std::string out = "add $30,$30,$4\nlw $" + std::to_string(reg) + ", -4($30)";
    std::cout << out << std::endl;
}

void prologue() {
}

// Revert the stack ptr
void epilogue(SymbolTable &varTable, std::string comment = "") {
    int localVarSize = varTable.localVarSize + varTable.paramSize;
    std::cout << "\n; epilogue: wain " << comment << std::endl;
    // revert stack ptr for each var we pushed onto stack
    for (int i = 0; i < localVarSize; ++i) {
        std::cout << "add $30 ,$30 ,$4" << std::endl;
    }
    std::cout << "jr $31" << std::endl;
}

// Revert the stack ptr
void epilogueProcedure(SymbolTable &varTable, std::string comment = "") {
    int localVarSize = varTable.localVarSize + varTable.paramSize;
    std::cout << "\n; epilogue procedure: " << comment << std::endl;
    // revert stack ptr for each var we pushed onto stack
    for (int i = 0; i < localVarSize; ++i) { // pop all args and declarations
        std::cout << "add $30, $30, $4" << std::endl;
    }
    std::cout << "jr $31" << std::endl;
}

void codeProcedure(std::shared_ptr<Node> expr, SymbolTable &varTable); // declared so code can use it

// Gets the code for an expression and loads into $3
void code(std::shared_ptr<Node> expr, SymbolTable &varTable) {
    std::string out;
    std::string from = expr->getFrom();
    if (from == NUM) {
        std::string val = expr->getTo();
        out = "lis $3\n.word " + val;
    } else if (from == ID) { // get from varTable
        std::string id = expr->getTo();
        if (varTable.contains(id)) { // for variable calls
            Variable v = varTable.find(id);
            out = "lw $3, " + std::to_string(v.offset) + "($29)";
        }
    } else if (expr->getTo() == "ID LPAREN arglist RPAREN" || expr->getTo() == "ID LPAREN RPAREN") {
        codeProcedure(expr, varTable); // when calling a procedure
    } else {
        for (auto child : expr->children) {
            code(child, varTable);
        }
    }
    if (out.length() > 0) {
        std::cout << out << std::endl;
    }
}

// takes a dcl node and adds it as a variable
void addVar(std::shared_ptr<Node> dcl, SymbolTable &varTable, bool isProcedureParam = false) {
    auto typeNode = dcl->children.front();
    auto idNode = dcl->children.back();
    std::string type = typeNode->children.front()->getTo();
    std::string id = idNode->getTo();
    Variable v(id, type);
    varTable.add(v, isProcedureParam);
}

void populateWainVar(std::shared_ptr<Node> wain, SymbolTable &varTable) {
    for (auto child : wain->children) {
        std::string from = child->getFrom();
        if (from == DCL) {
            addVar(child, varTable);
        } else if (from == EXPR) {
            code(child, varTable);
        }
    }
    epilogue(varTable);
}

void doArgList(std::shared_ptr<Node> argList, SymbolTable &varTable, int *numOfArgs) {
    if (argList->children.size() == 0) { // no children
        return;
    }
    auto exprNode = argList->children.front();
    if (exprNode->getFrom() == EXPR) {
        code(exprNode, varTable);
        push(3);
        *numOfArgs = *numOfArgs + 1; // increment num of args
    }
    
    if (argList->getTo() == "expr COMMA arglist") {
        // recurse on children (other args)
        auto argListChild = argList->children.back();
        doArgList(argListChild, varTable, numOfArgs);
    }
}

void codeProcedure(std::shared_ptr<Node> factor, SymbolTable &varTable) {
    std::cout << "; calling procedure" << std::endl;
    std::string id = factor->children.front()->getTo(); 
    // Caller saves registers
    std::cout << "; saving registers" << std::endl;
    push(29); // save callers frame pointer
    push(31); // save callers return address
    int numOfArgs = 0;
    if (factor->getTo() == "ID LPAREN arglist RPAREN") {
        doArgList(factor->children.at(2), varTable, &numOfArgs);
    }
    std::string procedureLabel = "F" + id;
    std::string out = "lis $5\n.word " + procedureLabel + "\njalr $5";
    std::cout << out << std::endl;

    std::cout << "; restoring registers" << std::endl;
    pop(31); // // restore callers stack pointer
    pop(29); // // restore callers frame pointer
}

void codeDcls(std::shared_ptr<Node> dcls, SymbolTable &varTable) {
    std::string from = dcls->getFrom();
    if (from == DCL) {
        addVar(dcls, varTable);
    }
    // recurse on all children
    for (auto child : dcls->children) {
        codeDcls(child, varTable);
    }
}

void doParams(std::shared_ptr<Node> paramlist, SymbolTable &varTable) {
    auto paramsChild = paramlist->children.front();
    if (paramlist->children.size() == 3)  { // recurse on the rest of the params first
        doParams(paramlist->children.back(), varTable);
    }
    if (paramsChild->getFrom() == DCL) { // add param
        auto dcl = paramsChild;
        addVar(dcl, varTable, true);
    }
}

void populateProcedure(std::shared_ptr<Node> procedure, SymbolTable &varTable) {
    std::cout << "; procedure" << std::endl;
    std::string procedureId = procedure->children.at(1)->getTo();
    std::cout << "F" << procedureId << ":" << std::endl;
    std::cout << "sub $29, $30, $4 ; update frame pointer for this procedure" << std::endl;
    for (auto child : procedure->children) {
        std::string from = child->getFrom();
        if (from == "params" && child->getTo() != EMPTY) {
            auto paramlist = child->children.front();
            doParams(paramlist, varTable);
        } else if (from == DCLS) {
            // codeDcls(child, varTable); // NOT NEEDED YET
        } else if (from == "statements") {
            // statement
        } else if (from == EXPR) {
            code(child, varTable);
        }
    }
    epilogueProcedure(varTable, procedureId);
}

void traverseTree(std::shared_ptr<Node> n, SymbolTable &varTable) {
    // do something with n here
    std::string from = n->getFrom();
    std::string to = n->getTo();
    if (from == PROCEDURES) {
        // recurse on it's children first, this is so wain is printed first
        traverseTree(n->children.back(), varTable);
        if (n->children.size() == 2) { // this is for the wain procedure
            traverseTree(n->children.front(), varTable);
        }
        
    } else if (n->getFrom() == "main") { // populate the wain procedure
        populateWainVar(n, varTable);
    } else if (from == PROCEDURE) { // populate the procedure
        std::shared_ptr<SymbolTable> pTable(new SymbolTable); // procedure table for each procedure
        populateProcedure(n, *pTable);
    }
}

int main() {
    const std::vector<std::string> terminals = getTerminals();
    std::shared_ptr<Node> root = parseTree(terminals);
    SymbolTable wainTable; // keeps track of all the variables declared in function wain
    try {
        std::cout << PROLOGUE;

        // =========================== ROOT OPERATIONS =====================================
        // root->printTree();
        traverseTree(root->children.at(1), wainTable);

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