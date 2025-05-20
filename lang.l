%option noyywrap c++ yylineno

%{
    #include <iostream>
    #include <cstdlib>
    #include "lang.tab.hh"

    using namespace yy;
%}

DIGIT  		[0-9]
WS          [ \t\n]
LETTER      [a-zA-Z]

%%

__dbg__         return parser::token::T_DBG;
int             return parser::token::T_INT;

";"             return parser::token::T_SEMI;
"("             return parser::token::T_OPEN;
")"             return parser::token::T_CLOSE;
"{"             return parser::token::T_OPEN_CURLY;
"}"             return parser::token::T_CLOSE_CURLY;
"="             return parser::token::T_ASSIGN;

({LETTER}|_)+({LETTER}|{DIGIT}|_)*      return parser::token::T_ID;
{DIGIT}+        return parser::token::T_INT_LIT;

{WS}+           // DO NOTHING

. {
    std::cerr << "Line " << lineno() << ": Unexpected character: '" << YYText() << "'." << std::endl; 
    exit(1);
}

%%
