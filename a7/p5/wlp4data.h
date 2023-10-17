#include <map>
#include <string>

const std::string PROLOGUE = R"END(; start prologue
.import print
lis $4 ; $4 will always hold 4
.word 4
lis $10 ; $10 will always hold address for print
.word print
lis $11 ; $1 will always hold 1
.word 1
sub $29, $30, $4 ; setup frame pointer
; end prologue
)END";

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
IF
ELSE
WHILE
PRINTLN
DELETE
EQ
NE
LT
LE
GE
GT
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
const std::string EQ = "EQ";
const std::string NE = "NE";
const std::string LT = "LT";
const std::string LE = "LE";
const std::string GT = "GT";
const std::string GE = "GE";
const std::string IF = "IF";
const std::string WHILE = "WHILE";

const std::string MAIN = "main";
const std::string PROCEDURES = "procedures";
const std::string PROCEDURE = "procedure";
const std::string DCL = "dcl";
const std::string DCLS = "dcls";
const std::string EXPR = "expr";
const std::string TERM = "term";
const std::string FACTOR = "factor";
const std::string LVALUE = "lvalue";
const std::string STATEMENTS = "statements";
const std::string STATEMENT = "statement";

const std::map<std::string, std::string> kindToType{
    {NUM, "int"},
    {INT, "int"},
    {INT_STAR, "int*"},
    {NIL, "int*"}};