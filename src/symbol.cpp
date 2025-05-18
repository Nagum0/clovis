#include "../include/symbol.h"

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

SymbolTable::SymbolTable() {}

size_t SymbolTable::get_size() const {
    size_t size = 0;

    for (auto it = table.begin(); it != table.end(); ++it)
        size += it->second.get_size();

    return size;
}

std::map<std::string, Symbol> SymbolTable::get_table() const {
    return table;
}

void SymbolTable::add_symbol(Symbol symbol) {
    table[symbol.get_id()] = symbol;
}

SymbolContainer::SymbolContainer() {}

SymbolTable SymbolContainer::get_current_block_symbol_table() const {
    return symbol_tables.back();
}

Symbol SymbolContainer::get_symbol(std::string id) const {
    return get_current_block_symbol_table().get_table()[id];
}

size_t SymbolContainer::get_current_block_size() const {
    return symbol_tables.back().get_size();
}

std::vector<SymbolTable> SymbolContainer::get_all_symbol_tables() const {
    return symbol_tables;
}

size_t SymbolContainer::get_full_size() const {
    size_t size = 0;

    for (auto it = symbol_tables.begin(); it != symbol_tables.end(); ++it)
        size += it->get_size();

    return size;
}

size_t SymbolContainer::get_next_free_pos() const {
    return next_free_pos;
}

void SymbolContainer::add_symbol(Symbol symbol) {
    symbol_tables.back().add_symbol(symbol);
    next_free_pos += symbol.get_size();
}

void SymbolContainer::add_block() {
    symbol_tables.push_back(SymbolTable());
}

void SymbolContainer::pop_block() {
    next_free_pos -= symbol_tables.back().get_size();
    symbol_tables.pop_back();
}
