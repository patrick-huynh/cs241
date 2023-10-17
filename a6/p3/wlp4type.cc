#include "wlp4data.h"
#include <algorithm>
#include <iostream>
#include <map>
#include <memory>
#include <ostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>


class Variable {
public:
    std::string id;
    std::string type;
    Variable(std::string idName, std::string typeName) {
        id = idName;
        type = typeName;
    }
};

class Procedure {
public:
    std::string id;
    std::vector<std::string> paramTypes;

    Procedure(std::string idName, std::vector<std::string> types) {
        id = idName;
        paramTypes = types;
    }
};

class Node {
public:
    std::string rule;
    std::string kind;
    std::string type;
    std::string lexeme;
    std::vector<std::shared_ptr<Node>> children;

    Node(std::string val) {
        rule = val;
    }

    void printTree() {
        std::cout << rule;
        if (type.length() != 0) {
            std::cout << " : " << type;
        }
        std::cout << std::endl;
        for (auto child : children) {
            child->printTree();
        }
    }

    void printChildren() {
        for (auto child : children) {
            std::cout << child->kind << std::endl;
        }
    }

    int numberOfNodes() {
        int count = 0;
        for (auto child : children) {
            count += child->numberOfNodes();
        }
        return 1 + count;
    }
};


int numberOfChildren(std::string s, std::vector<std::string> &terminals) {
    std::istringstream line(s);
    std::string tokenKind;
    line >> tokenKind;
    if (tokenKind == EMPTY) {
        return 0;
    }
    int count = 0;
    std::string temp;
    while (line >> temp) {
        if (temp != EMPTY) {
            ++count;
        }
    }
    return count;
}


std::shared_ptr<Node> getTree(std::istream &in, std::vector<std::string> &terminals) {
    std::string s;
    std::getline(in, s);
    std::vector<std::shared_ptr<Node>> children;
    std::string tokenKind;
    std::istringstream line(s);
    line >> tokenKind;
    if (find(terminals.begin(), terminals.end(), tokenKind) == terminals.end()) { // s is not a terminal, so it has children
        for (int i = 0; i < numberOfChildren(s, terminals); ++i) {
            children.push_back(getTree(in, terminals));
        }
    }
    std::shared_ptr<Node> n(new Node(s));
    std::string tokenLexeme;
    line >> tokenLexeme;
    n->kind = tokenKind;
    n->lexeme = tokenLexeme;
    while (line >> tokenLexeme) {
        n->lexeme += ' ' + tokenLexeme;
    }
    n->children = children;
    return n;
}


std::string getType(std::string type) {
    auto typeIt = kindToType.find(type);
    if (typeIt != kindToType.end()) {
        return typeIt->second;
    } else {
        throw std::invalid_argument("can't find type");
    }
}


// accepts a dcls node
void populateVarTable(std::shared_ptr<Node> dcls, std::map<std::string, Variable> &varTable) {
    if (dcls->lexeme != EMPTY) {
        auto dcl = dcls->children.at(1);   // access second child: dcl
        auto type = dcl->children.front(); // access first child: type
        auto id = dcl->children.back(); // access last child: ID
        std::string declaredType = getType(type->lexeme);
        std::string valueType = getType(dcls->children.at(3)->kind); // 4-th child of dcls is  NUM or NULL;

        if (declaredType != valueType) {
            throw std::invalid_argument("the type of a variable declared does not match the type of its initialization value");
        }

        // Inserting the variable into the varTable for it's procedure
        std::string variableId = id->lexeme;
        if (varTable.find(variableId) != varTable.end()) {
            throw std::invalid_argument("duplicate variable initialized");
        }
        Variable v(variableId, declaredType);
        varTable.insert({variableId, v});

        // recurse on the other dcls
        populateVarTable(dcls->children.front(), varTable);
    }
}


bool isTyped(std::shared_ptr<Node> n) {
    return n->kind == EXPR || n->kind == TERM || n->kind == FACTOR || n->kind == LVALUE;
}


// Declaring function so it can be used before in populateType
void handlePlus(std::shared_ptr<Node> n, std::string leftType, std::string rightType);
void handleMinus(std::shared_ptr<Node> n, std::string leftType, std::string rightType);
void handleIntOp(std::shared_ptr<Node> n, std::string leftType, std::string rightType);
bool handleArithmetic(std::string operation, std::shared_ptr<Node> n);
bool handlePointer(std::string operation, std::shared_ptr<Node> n);
bool handleNewInt(std::shared_ptr<Node> n);

void populateType(std::shared_ptr<Node> n, const std::map<std::string, Variable> *varTable = NULL) {
    std::istringstream line(n->rule);
    std::string s; // s is the first string in the rule
    line >> s;
    
    if (s == NUM || s == NIL) { // base case: n is a NUM or NIL
        n->type = getType(s);
    } else if (s == DCL) { // n is a variable dcl
        std::string type = n->children.front()->lexeme; // first child: type
        auto changeNode = n->children.back();           // last child: ID
        changeNode->type = getType(type);
    } else if (s == ID && varTable != NULL) { // If n is an ID, look in the varTable of the procedure
        auto varIt = varTable->find(n->lexeme); // the variable ID
        if (varIt != varTable->end()) {
            Variable v = varIt->second; // found in the variable 
            std::string type = v.type;
            n->type = type;
        } else {
            throw std::invalid_argument("return variable not defined");
        }
    } else {
        for (auto child : n->children) {
            populateType(child, varTable);
            std::string type = child->type;
            if (type != "" && isTyped(n)) {
                n->type = type;
            }
        }
        // ======================================== MANUAL TAKEOVER FOR TYPE CHECKING OPERATIONS ======================================================
        if (varTable != NULL) { // to avoid running on the first traversal
            std::string operation = n->children.size() == 2 ? n->children.at(0)->kind : "";
            handlePointer(operation, n); // if it is a pointer operation

            std::string operation2 = n->children.size() == 3 ? n->children.at(1)->kind : "";
            handleArithmetic(operation2, n); // if it is an operation, it will do err checks and populateType

            if(n->rule == "factor NEW INT LBRACK expr RBRACK") {
                handleNewInt(n);
            }   
        }
    }
}


bool isArithmetic(std::string op) {
    return op == PLUS || op == MINUS || op == STAR || op == SLASH || op == PCT;
}


bool handleArithmetic(std::string operation, std::shared_ptr<Node> n) {
    if (isArithmetic(operation)) {
        auto left = n->children.front();
        auto right = n->children.back();
        // populateType(left); // do not need
        // populateType(right);
        std::string leftType = left->type;
        std::string rightType = right->type;
        if (operation == PLUS) {
            handlePlus(n, leftType, rightType);
        } else if (operation == MINUS) {
            handleMinus(n, leftType, rightType);
        } else if (operation == STAR || operation == SLASH || operation == PCT) {
            handleIntOp(n, leftType, rightType);
        }
        return true;
    }
    return false;
}


void handlePlus(std::shared_ptr<Node> n, std::string leftType, std::string rightType) {
    const std::string intType = getType(INT);
    const std::string intPtrType = getType(INT_STAR);

    std::string nType = "";
    if (leftType == intType) {
        if (rightType == intType) {
            nType = intType;
        } else if (rightType == intPtrType) {
            nType = intPtrType;
        }
    } else if (leftType == intPtrType) {
        if (rightType == intType) {
            nType = intPtrType;
        }
    }

    if (nType.length() == 0) {
        throw std::invalid_argument("add types are invalid");
    } else {
        n->type = nType;
    }
}   


void handleMinus(std::shared_ptr<Node> n, std::string leftType, std::string rightType) {
    const std::string intType = getType(INT);
    const std::string intPtrType = getType(INT_STAR);

    std::string nType = "";
    if (leftType == intPtrType) {
        if (rightType == intType) {
            nType = intPtrType;
        } else if (rightType == intPtrType) {
            nType = intType;
        }
    } else if (leftType == intType) {
        if (rightType == intType) {
            nType = intType;
        }
    }

    if (nType.length() == 0) {
        throw std::invalid_argument("sub types are invalid");
    } else {
        n->type = nType;
    }
}


void handleIntOp(std::shared_ptr<Node> n, std::string leftType, std::string rightType) {
    const std::string intType = getType(INT);
    if (leftType == intType && rightType == intType) {
        n->type = intType;
    } else {
        throw std::invalid_argument("one or more of op types are invalid");       
    }
}

bool handlePointer(std::string operation, std::shared_ptr<Node> n) {
    if (operation != AMP && operation != STAR) { // early return
        return false;
    }

    const std::string intType = getType(INT);
    const std::string intPtrType = getType(INT_STAR);
    
    auto operandNode = n->children.back();
    // populateType(operandNode); // MAYBE NOT NEEDED
    std::string operandType = operandNode->type;

    if (operandType.length() == 0) { // end early, id is not typed yet MAYBE NOT NEEDED
        return false;
    } else if (operation == AMP && operandType == intType) {
        n->type = intPtrType;
    } else if (operation == STAR && operandType == intPtrType) {
        n->type = intType;
    } else {
        throw std::invalid_argument("pointer operation types are not valid");
    }
    return true;
}

bool handleNewInt(std::shared_ptr<Node> n) {
    const std::string intType = getType(INT);
    const std::string intPtrType = getType(INT_STAR);
    auto exprNode = n->children.at(3);
    // populateType(exprNode); // MAYBE NOT NEEDED
    std::string type = exprNode->type;
    
    // MAYBE NOT NEEDED
    //    if (type.length() == 0) {
    //        return false;
    //    }

    if (type == intType) {
        n->type = intPtrType;
        return true;
    } else {
        throw std::invalid_argument("parameter to new int is invalid type");
    }
}


// =================================== END OF TYPE HANDLING ========================================


void populateReturnType(std::shared_ptr<Node> expr, std::map<std::string, Variable> &varTable) {
    populateType(expr, &varTable);
    std::string type = expr->type;
     if (type != "int") {
         throw std::invalid_argument("invalid return type");
     }
}


void populateTable(std::shared_ptr<Node> n, std::map<std::string, std::map<std::string, Variable>> &tables, std::map<std::string, Procedure> &procedureTable) {
    std::istringstream line(n->rule);
    std::string s;
    line >> s;
    if (s == PROCEDURES) {
        std::map<std::string, Variable> varTable; // varTable for this procedure
        std::string procedureId;
        line >> procedureId;

        // Not needed yet
        // if (procedureTable.find(procedureId) != procedureTable.end()) { // check for duplicates
        //     throw std::invalid_argument("duplicate procedure found");
        // }

        // Create procedure and insert into procedureTable
        std::vector<std::string> paramTypes;
        Procedure p(procedureId, paramTypes);
        procedureTable.insert({procedureId, p});

        // We access the i-th child because that is where the expr statement is, we validate the return type
        auto child = n->children.front();  // procedure rule
        auto it = child->children.begin(); // iterator to first child
        std::istringstream line(child->rule);
        line >> s;
        while (line >> s) {
            if (s == DCL) { // get and insert param types
                auto itNode = *it;
                std::string type = itNode->children.front()->lexeme;
                std::string id = itNode->children.back()->lexeme;
                Variable v(id, getType(type));
                if (varTable.find(id) == varTable.end()) {
                    varTable.insert({id, v}); // insert params variables to varTable
                } else {
                    throw std::invalid_argument("duplicate param");
                }
                
                auto procedureIt = procedureTable.find(procedureId); // use existing procedure and update it's paramTypes
                std::vector<std::string> *paramTypes = &(procedureIt->second.paramTypes);
                paramTypes->push_back(type);
            } else if (s == DCLS) {
                populateVarTable(*it, varTable);
                tables.insert({procedureId, varTable});
            } else if (s == "expr") { // check and populate return type of wain
                populateReturnType(*it, varTable);
            }
            ++it;
        }
    } else {
        // Recurse on children
        for (auto child : n->children) {
            populateTable(child, tables, procedureTable);
        }
    }
}


void checkSemanticErr(std::map<std::string, Variable> &wainTable, std::map<std::string, Procedure> &procedureTable) {
    Procedure wain = (procedureTable.find("main"))->second;
    // Checks if there is 2 params
    if (wain.paramTypes.size() < 2) {
        throw std::invalid_argument("wain doesn't have enough params");
    }

    // Checks if the second param is of type int
    if (wain.paramTypes.at(1) != INT) {
        throw std::invalid_argument("second param of wain is not int");
    }
}


int main() {
    // Reading in terminals for WLP4
    std::istringstream istr(WLP4_TERMINALS);
    std::istream &inn = istr;
    std::vector<std::string> terminals;
    std::string s;
    std::getline(inn, s); // skip .REDUCTIONS header
    while (inn >> s) {
        terminals.push_back(s);
    }

    // Reading and making tree
    std::istream &in = std::cin;
    try {
        auto tree = getTree(in, terminals);

        std::map<std::string, std::map<std::string, Variable>> tables; // outer map stores the procedures, inner map stores the variables in the procedure
        std::map<std::string, Procedure> procedureTable;
        populateType(tree);
        populateTable(tree, tables, procedureTable);
        std::map<std::string, Variable> wainTable = (tables.find("main"))->second;
        populateType(tree, &wainTable); // populate type again with variable dcl
        checkSemanticErr(wainTable, procedureTable);

        /*  PRINT TREE HERE */
         tree->printTree(); 

        // ===== TESTING =====================================================
        //  for (auto it = tables.begin(); it != tables.end(); ++it) {
        //      std::cout << it->first << std::endl;
        //     //  std::string key = it->first;
        //     //  Procedure p = it->second;
        //     //  std::string id = p.id;
        //     //  std::cout << key << " | " << id << std::endl;
        //  }

    } catch (const std::invalid_argument &e) {
        std::cerr << "ERROR " << e.what() << std::endl;
    }
    return 0;
}