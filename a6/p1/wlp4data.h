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
)END";

const std::map<std::string, std::string> kindToType{
    {"INT", "int"},
    {"INT STAR", "int*"},
    {"NULL", "int*"}
};