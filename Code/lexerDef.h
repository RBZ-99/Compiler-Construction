#ifndef LEXERDEF_H
#define LEXERDEF_H

#define BUFFER_SIZE 20

typedef enum {
    DEF, MODULE, ENDDEF,
    DRIVERDEF, DRIVERENDDEF,
    TAKES, INPUT, 
    SQBO, SQBC,
    RETURNS,
    START, END,
    DECLARE, ID, COLON,
    ARRAY, OF, INTEGER, REAL, BOOLEAN, 
    TRUE, FALSE,
    ASSIGNOP, NUM, RNUM, SEMICOL,
    DRIVER, PROGRAM,
    GET_VALUE, PRINT,
    USE, WITH, PARAMETERS, COMMA,
    FOR, IN, RANGEOP, WHILE,
    SWITCH, BO, BC, CASE, BREAK, DEFAULT,
    PLUS, MINUS,
    MUL, DIV,
    LT, LE, GT, GE, EQ, NE,
    AND, OR,
    LEX_ERROR,      
    END_OF_FILE,   
    DELIM 
} token_name;

typedef struct token {
    token_name name;
    union{
        char *str;
        int num;
        double rnum;
    };
} TOKEN;

typedef enum { false, true } bool;  // boolean type

int state;
int just_retracted;

char buffer[BUFFER_SIZE];
char lexeme[BUFFER_SIZE];

int lexeme_begin;
int forward_ptr;
int line_no;
char terminal_string[][20] = {
    "DEF", "MODULE", "ENDDEF",
    "DRIVERDEF", "DRIVERENDDEF",
    "TAKES", "INPUT", 
    "SQBO", "SQBC",
    "RETURNS",
    "START", "END",
    "DECLARE", "ID", "COLON",
    "ARRAY", "OF", "INTEGER", "REAL", "BOOLEAN", 
    "TRUE", "FALSE",
    "ASSIGNOP", "NUM", "RNUM", "SEMICOL",
    "DRIVER", "PROGRAM",
    "GET_VALUE", "PRINT",
    "USE", "WITH", "PARAMETERS", "COMMA",
    "FOR", "IN", "RANGEOP", "WHILE",
    "SWITCH", "BO", "BC", "CASE", "BREAK", "DEFAULT",
    "PLUS", "MINUS",
    "MUL", "DIV",
    "LT", "LE", "GT", "GE", "EQ", "NE",
    "AND", "OR",
    "LEX_ERROR",    
    "END_OF_FILE",   
    "DELIM"
};
#endif