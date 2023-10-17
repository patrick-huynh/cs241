#include <stdexcept>
#include <ostream>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <sstream>
#include <map>
#include "wlp4data.h"
#include <memory>

const std::string EMPTY = ".EMPTY";
const std::string ID = "ID";
const std::string INT = "INT";
const std::string NUM = "NUM";
const std::string DCL = "dcl";
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
           std::cout << child->rule << std::endl;
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
    int count = 0;
    std::istringstream line(s);
    std::string temp;
    while (line >> temp) {
        if (temp == EMPTY) {// empty gets no children
            return 0;
        }
        ++count;
    }
    return count - 1;
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

void populateVarTable (std::shared_ptr<Node> n, std::map<std::string, Variable> &varTable) {
    if (n->kind == ID) {
        Variable v(n->lexeme, n->type);
        varTable.insert({n->lexeme, v});
    } else {
        for (auto child : n->children) {
            populateVarTable(child, varTable);
        }
    }

    // if (n->type == DCL) { // variable declarations
    //     std::string type = n->children.front()->lexeme;
    //     std::string id = n->children.back()->lexeme;
    //     Variable v;
    //     v.type = type;
    //     v.id = id;
    //     if (varTable.size() == 1 && type != INT) { // semantic err: 2nd var must be int
    //         throw std::invalid_argument("second param not int");
    //     }
    //     varTable.insert({id, v});
    // } else {
    //     // Recurse on all children
    //     for (auto child : n->children) {
    //         populateVarTable(child, varTable);
    //     }
    // }
}

void populateTable(std::shared_ptr<Node> n, std::map<std::string, std::map<std::string, Variable>> &tables, std::map<std::string, Procedure> &procedureTable) {
    std::istringstream line(n->rule);
    std::string s;
    line >> s;
    if (s == PROCEDURES) {
        std::string procedureId;
        line >> procedureId;
        std::map<std::string, Variable> varTable; // varTable for this procedure
        populateVarTable(n, varTable);
        tables.insert({procedureId, varTable}); // create map for that procedure

        // Create procedure and nsert into procedureTable
        std::vector<std::string> paramTypes;
        Procedure p(procedureId, paramTypes);
        procedureTable.insert({procedureId, p});

        // We access the i-th child because that is where the expr statement is, we validate the return type
        auto child = n->children.front(); // procedure rule
        auto it = child->children.begin(); // iterator to first child
        std::istringstream line(child->rule);
        line >> s;
        while (line >> s) { 
            if (s == "expr") {
                Node &returnNode = *(*it);
                std::string type = returnNode.type;
                if (type != "int") { // CHANGE THIS TO != WHEN DONE
                    throw std::invalid_argument("invalid return type"); 
                }
                break;
            } else if (s == DCL) { // get and insert param types
                auto itNode = *it;
                std::string type = itNode->children.front()->lexeme;
                std::string id = itNode->children.back()->lexeme;
                auto procedureIt = procedureTable.find(procedureId); // use existing procedure and update it's paramTypes
                std::vector<std::string> *paramTypes = &(procedureIt->second.paramTypes);
                paramTypes->push_back(type);
            }
            ++it; 
        }
    } else {
        // Recurse on all children
        for (auto child : n->children) {
            populateTable(child, tables, procedureTable);
        }
    }
}

bool isTyped(std::shared_ptr<Node> n) {
    return  n->kind == EXPR || n->kind == TERM || n->kind == FACTOR || n->kind == LVALUE;
}

void populateType(std::shared_ptr<Node> n) {
    std::istringstream line(n->rule);
    std::string s;
    line >> s;
    if (s == DCL) {
        std::string type = n->children.front()->lexeme; // first child is type
        auto changeNode = n->children.back();
        auto typeIt = kindToType.find(type);
        if (typeIt != kindToType.end()) {
            changeNode->type = typeIt->second;
        }
    } else if (s == NUM) {
        n->type = "int";
    } else {
       for (auto child : n->children) {
            populateType(child);
            std::string type = child->type;
            if (type != "" && isTyped(n)) {
                 n->type = type;
            }
        }
    }
}

bool isSemanticErr(std::map<std::string, Variable> &varTable, Procedure &procedure) {
    // Checks if there is 2 params
    if (procedure.paramTypes.size() < 2) {
        return true;
    }
    // std::cout << procedure.paramTypes.size() << std::endl;
    // Checks if the second param is of type int
    if (procedure.paramTypes.at(1) != INT) {
        return true;
    }
    // for (auto it = varTable.begin(); it != varTable.end(); ++it) {
    //     std::string key = it->first;
    //     Variable v = it->second;
    //     std::string vType = v.type;
    // }
    
    return false;
}

int main() {
    // Reading in terminals for WLP4
    std::istringstream istr(WLP4_TERMINALS);
    std::istream &inn = istr;
    std::vector<std::string> terminals;
    std::string s;
    std::getline(inn, s); // skip .REDUCTIONS header
    while(inn >> s) {
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
        // std::map<std::string, Variable> symTable;
        // populateSymTable(tree ,symTable);
        // semantic err checking

        std::map<std::string, Variable> wainTable = (tables.find("main"))->second;
        Procedure wain = (procedureTable.find("main"))->second;
        if (isSemanticErr(wainTable, wain)) {
            throw std::invalid_argument("Bad params");
        }

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