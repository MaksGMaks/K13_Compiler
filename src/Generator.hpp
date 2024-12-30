#pragma once

#include <filesystem>
#include <vector>
#include <fstream>
#include <iostream>
#include <map>
#include <list>

#include "constants.hpp"

namespace k_13 {
class Generator {
public:
    Generator() = default;
    ~Generator() = default;

    int createCpp(const std::vector<Keyword> &keywords, const std::string &progName, const std::string &outPath, const std::vector<Literal> &literals_);

private:
    void statement_ch(const std::vector<Keyword> &keywords, std::map<std::string, LexemType> &identifiers, std::ofstream &file);
    void compound_gen(const Keyword &keyword, std::map<std::string, LexemType> &identifiers, std::ofstream &file);
    void assign_gen(const Keyword &keyword, std::map<std::string, LexemType> &identifiers, std::ofstream &file);
    void get_gen(const Keyword &keyword, std::ofstream &file);
    void put_gen(const Keyword &keyword, std::ofstream &file);
    void if_gen(const Keyword &keyword, std::map<std::string, LexemType> &identifiers, std::ofstream &file);
    void for_gen(const Keyword &keyword, std::map<std::string, LexemType> &identifiers, std::ofstream &file);
    void goto_gen(const Keyword &keyword, std::ofstream &file);
    void label_gen(const Keyword &keyword, std::ofstream &file);

    void expression(const std::vector<Lexem> &expressions, std::ofstream &file);
    void str_expression(const std::vector<Lexem> &expressions, std::ofstream &file);

    std::vector<Literal> literals;

};

} // k_13
