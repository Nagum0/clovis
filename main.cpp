#include <iostream>
#include <fstream>
#include <cstdlib>
#include "FlexLexer.h"
#include "build/lang.tab.hh"
#include "include/symbol.h"

SymbolTable symbol_table;
yyFlexLexer* lexer;

int yylex(yy::parser::semantic_type* yylval, yy::parser::location_type* yylloc) {
    yylloc->begin.line = lexer->lineno();
    int token = lexer->yylex();
    
    if (token == yy::parser::token::T_UINT 
        || token == yy::parser::token::T_ID
        || token == yy::parser::token::T_UINT_LIT) {
        yylval->build(std::string(lexer->YYText()));
    }

    return token;
}

void yy::parser::error(const location_type& loc, const std::string& msg) {
    std::cerr << "Line " << loc.begin.line << ": " << msg << std::endl;
    exit(1);
}

int main(int argc, char* argv[]) {
    // -- READING THE SOURCE CODE
    std::ifstream in;
    
    if (argc != 2)
        std::cerr << "No input source code.\n";

    std::string source_code_path = argv[1];
    in.open(source_code_path);

    // -- LEXER
    lexer = new yyFlexLexer(&in);

    // -- PARSER
    yy::parser parser;
    parser.parse();
    
    delete lexer;

    return EXIT_SUCCESS;
}
