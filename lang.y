%language "c++"
%locations
%define api.value.type variant

%code requires {
    #include <string>
    #include <sstream>
    #include <tuple>
    #include "../include/expression.h"
    #include "../include/symbol.h"
    #include "../include/semantics.h"
}

%code provides {
    int yylex(yy::parser::semantic_type* yylval, yy::parser::location_type* yylloc);
    extern SymbolContainer* symbol_container;
}

%token T_DBG
%token T_INT
%token T_SEMI
%token T_OPEN
%token T_CLOSE
%token <std::string> T_ID;
%token <std::string> T_INT_LIT
%token T_OPEN_CURLY
%token T_CLOSE_CURLY
%token T_ASSIGN

%type <std::string> statement statements
%type <Expression> expression
%type <std::tuple<Type, size_t>> symbol_type

%%

start:
    statements {
        std::cout << "section .note.GNU-stack\n";

        std::cout << "\nglobal main\n"
                  << "extern dbg_print_int\n";
        
        std::cout << "\nsegment .text\n"
                  << "main:\n"
                  << $1
                  << "add rsp, " << symbol_container->get_current_block_size() << "\n"
                  << "mov rax, 0\n"
                  << "ret\n";
    }
;

symbol_type:
    T_INT {
        $$ = std::make_tuple(INT, 4);
    }
;

statements:
    statement {
        $$ = $1;
    }
|
    statement statements {
        $$ = $1 + $2;
    }
;

statement:
// Block declaration
// { ... }
    T_OPEN_CURLY {
        symbol_container->add_block();
    }
    statements T_CLOSE_CURLY {
        std::stringstream ss;

        ss << $3
           << "add rsp, " << symbol_container->get_current_block_size() << "\n";

        symbol_container->pop_block();

        $$ = ss.str();
    }
// Variable definition
// int x;
|
    symbol_type T_ID T_SEMI {
        if (symbol_container->get_current_block_symbol_table().get_table().count($2) != 0)
            semantic_error(@2.begin.line, "Redeclaration of symbol: " + $2);
        
        Symbol new_symbol($2, std::get<0>($1), symbol_container->get_next_free_pos(), std::get<1>($1));
        symbol_container->add_symbol(new_symbol);
        
        std::stringstream ss;
        ss << "sub rsp, " << new_symbol.get_size() << "\n";

        $$ = ss.str();
    }
// Variable assignment
// x = 10;
|
    T_ID T_ASSIGN expression T_SEMI {
        if (symbol_container->get_current_block_symbol_table().get_table().count($1) == 0)
            semantic_error(@1.begin.line, "Undeclared symbol: " + $1);

        Symbol symbol = symbol_container->get_symbol($1);

        std::stringstream ss;
        ss << $3.get_code()
           << "mov [rbp - " << symbol.get_stack_pos() + symbol.get_size() << "], eax\n";
    
        $$ = ss.str();
    }
// Debug print
|
    T_DBG T_OPEN expression T_CLOSE T_SEMI {
        std::stringstream ss;
        ss << $3.get_code()
           << "mov rdi, rax\n"
           << "call dbg_print_int\n";

        $$ = ss.str();
    }
;

// All expression are evaluated and their result is moved into a register.
expression:
// Integer literal
    T_INT_LIT {
        std::stringstream ss;
        ss << "mov eax, " << $1 << "\n";
        $$ = Expression(INT, ss.str());
    }
// Identifier
|
    T_ID {
        if (symbol_container->get_current_block_symbol_table().get_table().count($1) == 0)
            semantic_error(@1.begin.line, "Undeclared symbol: " + $1);
        
        Symbol symbol = symbol_container->get_symbol($1);

        std::stringstream ss;
        ss << "mov eax, [rbp - " << symbol.get_stack_pos() + symbol.get_size() << "]\n";
        
        $$ = Expression(symbol.get_type(), ss.str());
    }
;

%%
