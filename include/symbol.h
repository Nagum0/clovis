/*
 *  This file contains the definitions for working
 *  with symbols and blocks in the compiler.
 * */

#pragma once

#include "expression.h"
#include <map>
#include <vector>

/*
 *  A Symbol currently represents variables.
 *  This class holds the symbols information such as id, type, stack position and size.
 * */
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

/*
 *  A SymbolTable stores multiple defined symbols.
 *  The SymbolTable is made up of a stack of blocks.
 *  Each block internally holds its own table (std::map<std::string, Symbol>) which maps
 *  from the symbol's id to its Symbol class.
 * */
class SymbolTable {
    public:
        /*
         *  Initializes the class and adds a beginning block to the stack.
         * */
        SymbolTable();
        
        std::vector<std::map<std::string, Symbol>> get_blocks();

        /*
         *  @return The deepest block's symbol table.
         * */      
        std::map<std::string, Symbol> get_top_block();

        size_t get_rsp();
        
        /*
         *  @return The deepest blocks size in bytes.
         * */
        size_t get_top_block_size();

        /*
         *  @return The amount of padding (bytes) needed for 16 byte alignment of the rsp.
         * */
        size_t get_required_rsp_padding();
        
        /*
         *  Pushes a new empty block onto the stack.
         * */
        void push_block();

        /*
         *  Pops the top block off the stack and adjusts the rsp.
         * */
        void pop_block();

        /*
         *  Adds a new symbol to the top block's symbol table and adjusts the rsp.
         * */
        void add_symbol(Symbol symbol);

        /*
         *  Adds n value to the rsp.
         * */
        void add_rsp(size_t n);

    private:
        size_t rsp;
        std::vector<std::map<std::string, Symbol>> blocks;
};
