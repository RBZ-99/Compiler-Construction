#ifndef PARSERDEF_H
#define PARSERDEF_H
#include "lexerDef.h"

#define MAX_LENGTH 100
#define NUM_OF_RULES 99
#define NUM_OF_NONTERMINALS 53
typedef enum
{
    PROGRAM,
    MODULEDECLARATIONS,
    MODULEDECLARATION,
    OTHERMODULES,
    DRIVERMODULE,
    NTMODULE,
    MODULEDEF,
    RET,
    INPUT_PLIST,
    INPUT_PLIST_DASH,
    OUTPUT_PLIST,
    OUTPUT_PLIST_DASH,
    DATATYPE,
    TYPE,
    STATEMENTS,
    STATEMENT,
    IOSTMT,
    LITERAL,
    VAR,
    WHICHID,
    SIMPLESTMT,
    ASSIGNMENTSTMT,
    WHICHSTMT,
    LVALUEIDSTMT,
    LVALUEARRSTMT,
    INDEX,
    MODULEREUSESTMT,
    OPTIONAL,
    IDLIST,
    IDLIST_DASH,
    EXPRESSION,
    RESTEXPRESSION,
    RELATIONALEXPR,
    RESTRELATIONALEXPR,
    LOGICALOP,
    RELATIONALOP,
    UNARYARITHMETICEXPR,
    RESTUNARYARITHMETICEXPR,
    NONUNARYARITHMETICEXPR,
    ARITHMETICEXPRDASH,
    TERM,
    TERM_DASH,
    FACTOR,
    ADDSUBOP,
    MULDIVOP,
    DECLARESTMT,
    CONDITIONALSTMT,
    CASESTMT,
    OTHERCASE,
    VALUE,
    DEFAULTSTMT,
    ITERATIVESTMT,
    RANGE
} nonterminal;

char non_terminal_string [][25] = {
    "PROGRAM",
    "MODULEDECLARATIONS",
    "MODULEDECLARATION",
    "OTHERMODULES",
    "DRIVERMODULE",
    "NTMODULE",
    "MODULEDEF",
    "RET",
    "INPUT_PLIST",
    "INPUT_PLIST_DASH",
    "OUTPUT_PLIST",
    "OUTPUT_PLIST_DASH",
    "DATATYPE",
    "TYPE",
    "STATEMENTS",
    "STATEMENT",
    "IOSTMT",
    "LITERAL",
    "VAR",
    "WHICHID",
    "SIMPLESTMT",
    "ASSIGNMENTSTMT",
    "WHICHSTMT",
    "LVALUEIDSTMT",
    "LVALUEARRSTMT",
    "INDEX",
    "MODULEREUSESTMT",
    "OPTIONAL",
    "IDLIST",
    "IDLIST_DASH",
    "EXPRESSION",
    "RESTEXPRESSION",
    "RELATIONALEXPR",
    "RESTRELATIONALEXPR",
    "LOGICALOP",
    "RELATIONALOP",
    "UNARYARITHMETICEXPR",
    "RESTUNARYARITHMETICEXPR",
    "NONUNARYARITHMETICEXPR",
    "ARITHMETICEXPRDASH",
    "TERM",
    "TERM_DASH",
    "FACTOR",
    "ADDSUBOP",
    "MULDIVOP",
    "DECLARESTMT",
    "CONDITIONALSTMT",
    "CASESTMT",
    "OTHERCASE",
    "VALUE",
    "DEFAULTSTMT",
    "ITERATIVESTMT",
    "RANGE"
};

typedef enum {T, NT} type_of_sym;

typedef union
{
    token_name t;
    nonterminal nt;
} symbol;

struct rhsnode
{
    symbol s;
    type_of_sym flag;
    struct rhsnode *next;
};

typedef struct rhsnode rhsnode;
typedef struct rhsnode *rhsnode_ptr;

typedef struct 
{
    nonterminal sym;
    rhsnode_ptr head;
} cell;

typedef cell *grammar_t;
#endif