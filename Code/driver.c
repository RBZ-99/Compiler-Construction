#include "lexer.h"
#include "lexerDef.h"

int main()
{
    FILE *source = fopen("test.txt", "r");
    FILE *token_file = fopen("tokens.txt", "w");
    init();
    getStream(source);
    TOKEN t;
    while(1){
        t = getNextToken(source);
        if(t.name == END_OF_FILE){
            break;
        }
        else
        {
            if(t.name == LEX_ERROR)
            {
                lexError("",source);
            }
            else
            {
                if(t.name != DELIM)
                {
                    fprintf(token_file, "%s ", terminal_string[t.name] ); 
                }
                else
                {
                    if(t.num == '\n')
                    {
                        fprintf(token_file, "\n");
                    }
                }
            }        
        }
    }   // end of while
}   // end of main