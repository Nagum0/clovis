#include <iostream>
#include <fstream>
#include <cstdlib>
#include "FlexLexer.h"
#include "build/lang.tab.hh"
#include "include/symbol.h"

SymbolContainer* symbol_container;
yyFlexLexer* lexer;

int yylex(yy::parser::semantic_type* yylval, yy::parser::location_type* yylloc) {
    yylloc->begin.line = lexer->lineno();
    int token = lexer->yylex();
    
    if (token == yy::parser::token::T_INT 
        || token == yy::parser::token::T_ID
        || token == yy::parser::token::T_INT_LIT) {
        yylval->build(std::string(lexer->YYText()));
    }

    return token;
}

void yy::parser::error(const location_type& loc, const std::string& msg) {
    std::cerr << "Line " << loc.begin.line << ": " << msg << std::endl;
    exit(1);
}

int main(int argc, char* argv[]) {
    // -- INIT SYMBOL CONTAINER
    symbol_container = new SymbolContainer();
    symbol_container->add_block();

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
    delete symbol_container;

    return EXIT_SUCCESS;
}
