#include "../include/symbol.h"
#include "../include/exceptions.h"
#include <optional>
#include <iostream>

/*
 *  Symbol class definitions.
 * */
Symbol::Symbol() {}

Symbol::Symbol(std::string id, Type type, size_t stack_pos, size_t size) {
    this->id = id;
    this->type = type;
    this->stack_pos = stack_pos;
    this->size = size;
}

std::string Symbol::get_id() const {
    return id;
}

Type Symbol::get_type() const {
    return type;
}

size_t Symbol::get_stack_pos() const {
    return stack_pos;
}

size_t Symbol::get_size() const {
    return size;
}

/*
 *  SymbolTable class definitions. 
 * */
SymbolTable::SymbolTable() {
    rsp = 0;
    blocks.push_back(std::map<std::string, Symbol>());
}

std::vector<std::map<std::string, Symbol>> SymbolTable::get_blocks() {
    return blocks;
}

std::map<std::string, Symbol> SymbolTable::get_top_block() {
    return blocks.back();
}

size_t SymbolTable::get_rsp() {
    return rsp;
}

size_t SymbolTable::get_top_block_size() {
    size_t size = 0;
    auto top_block = blocks.back();

    for (auto entry : top_block)
        size += entry.second.get_size();

    return size;
}

// TODO:
size_t SymbolTable::get_required_rsp_padding() {
    return 0;
}

std::optional<Symbol> SymbolTable::operator[](std::string id) {
    std::optional<Symbol> val;

    for (auto it = blocks.rbegin(); it != blocks.rend(); ++it)
        if (it->count(id) > 0)
            val = it->at(id);

    return val;
}

bool SymbolTable::contains(std::string id) {
    for (auto it = blocks.rbegin(); it != blocks.rend(); ++it)
        if (it->count(id) > 0)
            return true;
    
    return false;
}

void SymbolTable::push_block() {
    blocks.push_back(std::map<std::string, Symbol>());
}

void SymbolTable::pop_block() {
    rsp -= get_top_block_size();
    blocks.pop_back();
}

void SymbolTable::add_symbol(Symbol symbol) {
    auto& top_block = blocks.back();

    if (top_block.count(symbol.get_id()) > 0)
        throw SymbolRedeclarationException("Symbol: " + symbol.get_id() + " is already declared in this scope");

    top_block[symbol.get_id()] = symbol;
    rsp += symbol.get_size();
}

void SymbolTable::add_rsp(size_t n) {
    rsp += n;
}
