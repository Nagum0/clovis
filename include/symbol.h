#pragma once

#include "expression.h"
#include <map>
#include <vector>

class Symbol {
    public:
        Symbol();
        Symbol(std::string id, Type type, size_t stack_pos, size_t size);
        
        // GETTERS
        std::string get_id() const;
        Type get_type() const;
        size_t get_stack_pos() const;
        size_t get_size() const;

    private:
        std::string id;
        Type type;
        size_t stack_pos;
        size_t size;
};

class SymbolTable {
    public:
        SymbolTable();
        
        // GETTERS
        std::map<std::string, Symbol> get_table() const;
        size_t get_size() const;

        // METHODS
        void add_symbol(Symbol symbol);

    private:
        std::map<std::string, Symbol> table;
};

class SymbolContainer {
    public:
        SymbolContainer();
        
        // GETTERS
        SymbolTable get_current_block_symbol_table() const;
        Symbol get_symbol(std::string id) const;
        size_t get_current_block_size() const;
        std::vector<SymbolTable> get_all_symbol_tables() const;
        size_t get_full_size() const;
        size_t get_next_free_pos() const;

        // METHODS
        void add_symbol(Symbol symbol);
        void add_block();
        void pop_block();
        
    private:
        std::vector<SymbolTable> symbol_tables;
        size_t next_free_pos;
};
