%language "c++"
%locations
%define api.value.type variant

%code requires {
    #include <string>
    #include <sstream>
    #include <tuple>
    #include <optional>
    #include "../include/expression.h"
    #include "../include/symbol.h"
    #include "../include/semantics.h"
    #include "../include/exceptions.h"
}

%code provides {
    int yylex(yy::parser::semantic_type* yylval, yy::parser::location_type* yylloc);
    extern SymbolTable symbol_table;
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
%token T_BOOL
%token T_TRUE_LIT
%token T_FALSE_LIT

%type <std::string> statement statements
%type <Expression> expression
%type <std::tuple<Type, size_t>> symbol_type

%%

start:
    statements {
        std::cout << "section .note.GNU-stack\n";

        std::cout << "\nglobal main\n"
                  << "extern dbg_print_int, dbg_print_bool\n";
        
        std::cout << "\nsegment .text\n"
                  << "main:\n"
                  << "push rbp\n"
                  << "mov rbp, rsp\n\n"
                  << $1
                  << "add rsp, " << symbol_table.get_top_block_size() << "\n\n"
                  << "mov rsp, rbp\n"
                  << "pop rbp\n"
                  << "mov rax, 0\n"
                  << "ret\n";
    }
;

symbol_type:
    T_INT {
        $$ = std::make_tuple(INT, 4);
    }
|
    T_BOOL {
        $$ = std::make_tuple(BOOL, 1);
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
        symbol_table.push_block();
    }
    statements T_CLOSE_CURLY {
        std::stringstream ss;

        ss << $3
           << "add rsp, " << symbol_table.get_top_block_size() << "\n";

        symbol_table.pop_block();

        $$ = ss.str();
    }
// Variable declaration
|
    symbol_type T_ID T_SEMI {
        Symbol new_symbol($2, std::get<0>($1), symbol_table.get_rsp(), std::get<1>($1));

        try {
            symbol_table.add_symbol(new_symbol);
        }
        catch (const SymbolRedeclarationException& ex) {
            semantic_error(@2.begin.line, ex.what());
        }
        
        std::stringstream ss;
        ss << "sub rsp, " << new_symbol.get_size() << "\n";

        $$ = ss.str();
    }
// Variable assignment
|
    T_ID T_ASSIGN expression T_SEMI {
        std::optional<Symbol> symbol = symbol_table[$1];

        if (!symbol)
            semantic_error(@1.begin.line, "Undeclared symbol " + $1);

        std::stringstream ss;
        ss << $3.get_code()
           << "mov [rbp - " << symbol.value().get_stack_pos() + symbol.value().get_size() << "], " << symbol.value().get_value_register() << "\n";
    
        $$ = ss.str();
    }
// Debug print
|
    T_DBG T_OPEN expression T_CLOSE T_SEMI {
        size_t padding = symbol_table.get_required_rsp_padding();
        symbol_table.add_rsp(padding);

        std::string debug_function = $3.debug_function();

        std::stringstream ss;
        ss << $3.get_code()
           << "mov rdi, rax\n"
           << "sub rsp, " << padding << "\n"
           << "call " << debug_function << "\n"
           << "add rsp, " << padding << "\n";
        
        symbol_table.add_rsp(-padding);

        $$ = ss.str();
    }
;

expression:
// Integer literal
    T_INT_LIT {
        std::stringstream ss;
        ss << "mov eax, " << $1 << "\n";
        $$ = Expression(INT, ss.str());
    }
// Bool literal: true
|
    T_TRUE_LIT {
        $$ = Expression(BOOL, "mov al, 1\n");
    }
// Bool literal: false
|
    T_FALSE_LIT {
        $$ = Expression(BOOL, "mov al, 0\n");
    }
// Identifier
|
    T_ID {
        std::optional<Symbol> symbol = symbol_table[$1];

        if (!symbol)
            semantic_error(@1.begin.line, "Undeclared symbol " + $1);
        

        std::stringstream ss;
        ss << "mov eax, [rbp - " << symbol.value().get_stack_pos() + symbol.value().get_size() << "]\n";
        
        $$ = Expression(symbol.value().get_type(), ss.str());
    }
;

%%
