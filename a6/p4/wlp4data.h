#include <string>
#include <map>
const std::string WLP4_TERMINALS = R"END(.TERMINALS
BOF
INT
WAIN
LPAREN
RPAREN
COMMA
LBRACE
RBRACE
RETURN
SEMI
STAR
ID
NUM
NULL
SEMI
EOF
BECOMES
PLUS
MINUS
STAR
SLASH
PCT
AMP
STAR
NEW
LBRACK
RBRACK
FACTOR
)END";


const std::string EMPTY = ".EMPTY";
const std::string ID = "ID";
const std::string INT = "INT";
const std::string INT_STAR = "INT STAR";
const std::string NUM = "NUM";
const std::string NIL = "NULL";

const std::string PLUS = "PLUS";
const std::string MINUS = "MINUS";
const std::string STAR = "STAR";
const std::string SLASH = "SLASH";
const std::string PCT = "PCT";
const std::string AMP = "AMP";

const std::string MAIN = "main";
const std::string PROCEDURES = "procedures";
const std::string PROCEDURE = "procedure";
const std::string DCL = "dcl";
const std::string DCLS = "dcls";
const std::string EXPR = "expr";
const std::string TERM = "term";
const std::string FACTOR = "factor";
const std::string LVALUE = "lvalue";

const std::map<std::string, std::string> kindToType{
    {NUM, "int"},
    {INT, "int"},
    {INT_STAR, "int*"},
    {NIL, "int*"}
};