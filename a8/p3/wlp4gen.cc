#include "wlp4data.h"
#include <algorithm>
#include <iostream>
#include <memory>
#include <ostream>
#include <sstream>
#include <string>
#include <vector>


const int NIL_VAL = 1;

// For unique id generation
int uniqueId = 0;
int getUniqueId() {
    ++uniqueId;
    return uniqueId;
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
    std::string type;
    std::vector<std::shared_ptr<Node>> children;

    Node(std::string nodeRule) {
        if (nodeRule.find(":") != std::string::npos) { // case: node has type appended to the end
            type = nodeRule.substr(nodeRule.find(":") + 2); // parse the type
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
    int localVarSize = varTable.localVarSize + varTable.paramSize;
    std::cout << "\n; epilogue: wain" << std::endl;
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

// Pushes reg to the stack, returns offset from $29
void push(int reg) {
    std::string out = "sw $" + std::to_string(reg) + ", -4($30)\nsub $30, $30, $4";
    std::cout << out << std::endl;
}

// Pops reg from the stack, returns offset from $29
void pop(int reg) {
    std::string out = "add $30, $30, $4\nlw $" + std::to_string(reg) + ", -4($30)";
    std::cout << out << std::endl;
}


bool isBinaryOp(std::shared_ptr<Node> n) {
    if (n->children.size() != 3) {
        return false;
    }
    std::string op = n->children.at(1)->getFrom();;
    return op == PLUS || op == MINUS || op == STAR || op == SLASH || op == PCT;
}

bool isComparison(std::shared_ptr<Node> n) {
    if (n->children.size() != 3) {
        return false;
    }
    std::string op = n->children.at(1)->getFrom();;
    return op == EQ || op == NE || op == LT || op == LE || op == GE || op == GT;
}

bool isControl(std::shared_ptr<Node> n) {
    const std::string ifRule = "IF LPAREN test RPAREN LBRACE statements RBRACE ELSE LBRACE statements RBRACE";
    const std::string whileRule = "WHILE LPAREN test RPAREN LBRACE statements RBRACE";
    return n->getTo() == ifRule || n->getTo() == whileRule;
}

bool isPrint(std::shared_ptr<Node> n) {
    const std::string printRule = "PRINTLN LPAREN expr RPAREN SEMI";
    return n->getTo() == printRule;
}

bool isDerefPtr(std::shared_ptr<Node> n) {
    return n->getTo() == "STAR factor";
}

bool isAddressPtr(std::shared_ptr<Node> n) {
    return n->getTo() == "AMP lvalue";
}

void codeBinaryOp(std::shared_ptr<Node> n, SymbolTable &varTable); // declared so code can use it
void codeComparison(std::shared_ptr<Node> n, SymbolTable &varTable); // declared so code can use it
void codeControl(std::shared_ptr<Node> n, SymbolTable &varTable); // declared so code can use it
void codePrint(std::shared_ptr<Node> n, SymbolTable &varTable); // declared so code can use it
void codeDerefPtr(std::shared_ptr<Node> factor, SymbolTable &varTable); // declared so code can use it
void codeAddressPtr(std::shared_ptr<Node> factor, SymbolTable &varTable); // declared so code can use it

void codeProcedure(std::shared_ptr<Node> expr, SymbolTable &varTable); // declared so code can use it


// Gets the code for expr and loads into $3
void code(std::shared_ptr<Node> expr, SymbolTable &varTable) {
    std::string out;
    std::string from = expr->getFrom();
    if (from == NUM) {
        std::string val = expr->getTo();
        out = "lis $3\n.word " + val;
    } else if (from == NIL) {
        out = "add $3, $0, $11 ; NULL is always 1";
    } else if (from == ID) { // get from varTable
        std::string id = expr->getTo();
        if (varTable.contains(id)) { // for variable calls
            Variable v = varTable.find(id);
            out = "lw $3, " + std::to_string(v.offset) + "($29)";
        }
    } else if (expr->getTo() == "ID LPAREN arglist RPAREN" || expr->getTo() == "ID LPAREN RPAREN") {
        codeProcedure(expr, varTable); // when calling a procedure
    } else if (isBinaryOp(expr)) {
        codeBinaryOp(expr, varTable);
    } else if (isComparison(expr)) {
        codeComparison(expr, varTable);
    } else if (isDerefPtr(expr)) {
        codeDerefPtr(expr, varTable);
    } else if (isAddressPtr(expr)) {
        codeAddressPtr(expr, varTable);
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
void addVar(std::shared_ptr<Node> dcl, SymbolTable &varTable, int reg = 3, bool isProcedureParam = false) {
    auto typeNode = dcl->children.front();
    auto idNode = dcl->children.back();
    std::string type = kindToType.find(typeNode->getTo())->second;
    std::string id = idNode->getTo();
    if (!isProcedureParam) {
        push(reg); // pushes variable to stack
    }
    Variable v(id, type);
    varTable.add(v, isProcedureParam);
}


// adds variable dcls to the variable table is generalized for all procedures
void doDcls(std::shared_ptr<Node> dcls, SymbolTable &varTable) {
    std::string from = dcls->getFrom();
    if (dcls->rule != "dcls dcls dcl BECOMES NUM SEMI" && dcls->rule != "dcls dcls dcl BECOMES NULL SEMI") { // validation
        return;
    }

    doDcls(dcls->children.front(), varTable); // recurse on rest of dcls
    std::cout << "; adding var dcl" << std::endl;
    code(dcls->children.at(3), varTable);
    auto dcl = dcls->children.at(1);
    addVar(dcl, varTable);
}

// Gets the first ID node in preorder traversal of n
std::shared_ptr<Node> getIdNode(std::shared_ptr<Node> n) {
    if (n->getFrom() != ID) { // case: lvalue → LPAREN lvalue RPAREN
        if (n->rule == "lvalue LPAREN lvalue RPAREN") {
            return getIdNode(n->children.at(1));
        } else if (n->rule == "lvalue ID" || n->rule == "factor ID") { // case: lvalue → ID
            return getIdNode(n->children.front());
        } else if (n->rule == "lvalue STAR factor") {
            return getIdNode(n->children.at(1));
        }
    }
    return n;
}

void codeStatement(std::shared_ptr<Node> statement, SymbolTable &varTable) {
    std::string to = statement->getTo();
    if (to == "lvalue BECOMES expr SEMI") { // lvalue statement
        std::cout << "; lvalue statement" << std::endl;
        auto lvalueNode = statement->children.front();
        
        if (lvalueNode->rule == "lvalue ID") { // case: lvalue -> ID
            std::cout << "; assignment through id" << std::endl;
            auto idNode = getIdNode(lvalueNode);
            std::string id = idNode->getTo();
            Variable v = varTable.find(id);
            std::string offset = std::to_string(v.offset); // offset of the lvalue
            auto exprNode = statement->children.at(2);
            code(exprNode, varTable);
            std::cout << "sw $3, " + offset + "($29)" << std::endl;
        } else if (lvalueNode->rule == "lvalue STAR factor")  { // case: lvalue -> STAR factor
            std::cout << "; assignment through pointer deref" << std::endl;
            auto exprNode = statement->children.at(2);
            code(exprNode, varTable);
            push(3);
            // we need to get address
            auto factorNode = lvalueNode->children.back();
            auto idNode = factorNode->children.back();
            code(idNode, varTable);
            pop(5);
            std::cout << "sw $5, 0($3)" << std::endl;
        }
    } else if (isControl(statement)) {
        codeControl(statement, varTable);
    } else if (isPrint(statement)) {
        codePrint(statement, varTable);
    }
}

void doStatements(std::shared_ptr<Node> statements, SymbolTable &varTable) {
    if (statements->rule != "statements statements statement") { // validation
        return;
    }
    doStatements(statements->children.front(), varTable);
    auto statement = statements->children.back();
    codeStatement(statement, varTable);
}

void codeBinaryOp(std::shared_ptr<Node> n, SymbolTable &varTable) {
    if (!isBinaryOp(n)) { // validation
        return;
    }
    const std::string ptrType = kindToType.find(INT_STAR)->second;
    const std::string intType = kindToType.find(INT)->second;

    std::cout << "; binary op" << std::endl;
    auto opNode = n->children.at(1);
    std::string op = opNode->getFrom();


    auto leftNode = n->children.front(); // expr
    auto rightNode = n->children.back(); // term
    std::string leftType = leftNode->type;
    std::string rightType = rightNode->type;
    
    // Pointer arithmetic
    if (op == PLUS && leftType == ptrType && rightType == intType) { // case: pointer + int
        std::cout << "; plus op: ptr + int" << std::endl;
        code(leftNode, varTable);
        push(3);
        code(rightNode, varTable);
        std::cout << "mult $3, $4" << std::endl << "mflo $3" << std::endl;
        pop(5);
        std::cout << "add $3, $5, $3" << std::endl;
        return;
    } else if (op == PLUS && leftType == intType && rightType== ptrType) { // case: int + pointer
        std::cout << "; plus op: int + ptr" << std::endl;
        code(rightNode, varTable);
        push(3);
        code(leftNode, varTable);
        std::cout << "mult $3, $4" << std::endl << "mflo $3" << std::endl;
        pop(5);
        std::cout << "add $3, $5, $3" << std::endl;
        return;
    } else if (op == MINUS && leftType == ptrType && rightType == intType) { // case: pointer - int
        std::cout << "; minus op: ptr - int" << std::endl;
        code(leftNode, varTable);
        push(3);
        code(rightNode, varTable);
        std::cout << "mult $3, $4" << std::endl << "mflo $3" << std::endl;
        pop(5);
        std::cout << "sub $3, $5, $3" << std::endl;
        return;
    } else if (op == MINUS && leftType == ptrType && rightType == ptrType) {
        std::cout << "; minus op: ptr - ptr" << std::endl;
        code(leftNode, varTable);
        push(3);
        code(rightNode, varTable);
        pop(5);
        std::cout << "sub $3, $5, $3" << std::endl << "div $3, $4" << std::endl << "mflo $3" << std::endl;
        return;
    }

    // integer only arithmetic
    // getting left and right values into $5, $3, respectively
    code(leftNode, varTable);
    push(3);
    code(rightNode, varTable);
    pop(5);
    std::string out;
    if (op == PLUS) {
        out = "add $3, $5, $3";
    } else if (op == MINUS) {
        out = "sub $3, $5, $3";
    } else if (op == STAR) {
        out = "mult $5, $3\nmflo $3";
    } else if (op == SLASH) {
        out =  "div $5, $3\nmflo $3";
    } else if (op == PCT) {
        out = "div $5, $3\nmfhi $3";
    }
    std::cout << out << std::endl;
}

void codeComparison(std::shared_ptr<Node> n, SymbolTable &varTable) {
    if (!isComparison(n)) {
        return;
    }
    std::cout << "; comparison" << std::endl;
    std::string op = n->children.at(1)->getFrom();
    auto leftNode = n->children.front();
    auto rightNode = n->children.back();
    code(leftNode, varTable);
    push(3);
    code(rightNode, varTable);
    pop(5);
    std::string out;
    if (op == EQ) {
        out = "slt $6, $3, $5\nslt $7, $5, $3\nadd $3, $6, $7\nsub $3, $11, $3";
    } else if (op == NE) {
        out = "slt $6, $3, $5\nslt $7, $5, $3\nadd $3, $6, $7";
    } else if (op == LT) {
        out = "slt $3, $5, $3";
    } else if (op == LE) {
        out = "slt $3, $3, $5\nsub $3, $11, $3";
    } else if (op == GE) {
        out = "slt $3, $5, $3\nsub $3, $11, $3";
    } else if (op == GT) {
        out = "slt $3, $3, $5";
    }
    std::cout << out << std::endl;
}

void codeControl(std::shared_ptr<Node> n, SymbolTable &varTable) {
    std::string uId = std::to_string(getUniqueId()); // unique id for the if labels
    if (!isControl(n)) {
        return;
    } else if (n->children.front()->getFrom() == IF) { // if statement
        // statement → IF LPAREN test RPAREN LBRACE statements RBRACE ELSE LBRACE statements RBRACE
        std::cout << "; if statement" << std::endl;
        auto testNode = n->children.at(2);
        auto stmts1Node = n->children.at(5);
        auto stmts2Node = n->children.at(9);
        code(testNode, varTable);
        std::cout << "beq $3, $0, else" + uId << std::endl;

        doStatements(stmts1Node, varTable);
        std::cout << "beq $0, $0, endif" + uId << std::endl;

        std::cout << "else" + uId + ":" << std::endl;

        doStatements(stmts2Node, varTable);
        std::cout << "endif" + uId + ":" << std::endl;  
    } else if (n->children.front()->getFrom() == WHILE) { // while loops
        // statement → WHILE LPAREN test RPAREN LBRACE statements RBRACE
        std::cout << "; while loop" << std::endl;
        auto testNode = n->children.at(2);
        auto stmtsNode = n->children.at(5);
        std::cout << "loop" + uId + ":" << std::endl;

        code(testNode, varTable);
        std::cout << "beq $3, $0, endWhile" + uId << std::endl;

        doStatements(stmtsNode, varTable);
        std::cout << "beq $0, $0, loop" + uId << std::endl;
        std::cout << "endWhile" + uId + ":" << std::endl;
    }
}

void codePrint(std::shared_ptr<Node> n, SymbolTable &varTable) {
    if (!isPrint(n)) { // validation
        return;
    }
    // statement → PRINTLN LPAREN expr RPAREN SEMI
    auto exprNode = n->children.at(2);
    push(1);
    code(exprNode, varTable);
    std::cout << "add $1, $3, $0" << std::endl;

    push(31);
    std::cout << "lis $5\n.word print\njalr $5" << std::endl;

    pop(31);
    pop(1);
}

void codeDerefPtr(std::shared_ptr<Node> factor, SymbolTable &varTable) {
    auto factor2Node = factor->children.back();
    code(factor2Node, varTable);
    std::cout << "lw $3, 0($3) ; deref ptr" << std::endl;
}

void codeAddressPtr(std::shared_ptr<Node> factor, SymbolTable &varTable) {
    std::string output;
    auto lvalueNode = factor->children.back();
    auto idNode = getIdNode(lvalueNode);
    std::string id = idNode->getTo();
    if (varTable.contains(id)) { // case: lvalue -> ID
        Variable v = varTable.find(id);
        std::string offset = std::to_string(v.offset);
        std::cout << "lis $3\n.word " << offset << "\nadd $3, $3, $29" << std::endl;
    } else { // case: lvalue -> STAR factor
        auto factor2Node = lvalueNode->children.back();
        code(factor2Node, varTable);
    }
}

void populateWainVar(std::shared_ptr<Node> wain, SymbolTable &varTable) {
    int param = 1;
    for (auto child : wain->children) {
        std::string from = child->getFrom();
        if (from == DCL) {
            addVar(child, varTable, param); // exception for param arg here
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

// void codeDcls(std::shared_ptr<Node> dcls, SymbolTable &varTable) {
//     std::string from = dcls->getFrom();
//     if (from == DCL) {
//         addVar(dcls, varTable);
//     }
//     // recurse on all children
//     for (auto child : dcls->children) {
//         codeDcls(child, varTable);
//     }
// }

void doParams(std::shared_ptr<Node> paramlist, SymbolTable &varTable) {
    auto paramsChild = paramlist->children.front();
    if (paramlist->children.size() == 3)  { // recurse on the rest of the params first
        doParams(paramlist->children.back(), varTable);
    }
    if (paramsChild->getFrom() == DCL) { // add param
        auto dcl = paramsChild;
        addVar(dcl, varTable, 3, true);
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
            auto dcls = child;
            doDcls(dcls, varTable);
        } else if (from == "statements") {
            auto statements = child;
            doStatements(statements, varTable);
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
        std::cout << PROLOGUE << std::endl;

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