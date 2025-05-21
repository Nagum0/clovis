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
uint            return parser::token::T_UINT;
bool            return parser::token::T_BOOL;
true            return parser::token::T_TRUE_LIT;
false           return parser::token::T_FALSE_LIT;

";"             return parser::token::T_SEMI;
"("             return parser::token::T_OPEN;
")"             return parser::token::T_CLOSE;
"{"             return parser::token::T_OPEN_CURLY;
"}"             return parser::token::T_CLOSE_CURLY;
"="             return parser::token::T_ASSIGN;
"=="            return parser::token::T_EQUAL;
"not"           return parser::token::T_NOT;
"and"           return parser::token::T_AND;
"or"            return parser::token::T_OR;

({LETTER}|_)+({LETTER}|{DIGIT}|_)*      return parser::token::T_ID;
{DIGIT}+        return parser::token::T_UINT_LIT;

{WS}+           // DO NOTHING

. {
    std::cerr << "Line " << lineno() << ": Unexpected character: '" << YYText() << "'." << std::endl; 
    exit(1);
}

%%
