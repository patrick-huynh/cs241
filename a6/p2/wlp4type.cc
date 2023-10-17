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

const std::string EMPTY = ".EMPTY";
const std::string ID = "ID";
const std::string INT = "INT";
const std::string NUM = "NUM";
const std::string NIL = "NULL";
const std::string DCL = "dcl";
const std::string DCLS = "dcls";
const std::string EXPR = "expr";
const std::string TERM = "term";
const std::string FACTOR = "factor";
const std::string LVALUE = "lvalue";

const std::string PROCEDURES = "procedures";

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


void populateType(std::shared_ptr<Node> n, std::map<std::string, Variable> *varTable = NULL) {
    std::istringstream line(n->rule);
    std::string s;
    line >> s;
    
    if (s == DCL) {
        std::string type = n->children.front()->lexeme; // first child: type
        auto changeNode = n->children.back();           // last child: ID
        changeNode->type = getType(type);
    } else if (s == NUM) {
        n->type = "int";
    } else if (s == NIL) {
        n->type = "int*";
    } else if (s == ID && varTable != NULL) {
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
    }
}


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
        tree->printTree();
        std::map<std::string, Variable> wainTable = (tables.find("main"))->second;
        checkSemanticErr(wainTable, procedureTable);

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