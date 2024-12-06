#pragma once

#include <vector>
#include <fstream>
#include <iostream>
#include <map>

#include "constants.hpp"

namespace k_13 {
class SyntaxAnalyzer2 {
public:
    SyntaxAnalyzer2();
    ~SyntaxAnalyzer2() = default;

    int analyze(const std::vector<Lexem> &lexems, const std::vector<UnknownLexem> &unknowns);
private:
    int errors = 0;
    int position = 0;
    std::vector<Lexem> code;
    std::vector<UnknownLexem> unknownLexems;

    std::vector<std::string> subErrors;
    std::vector<std::string> errorMessages;

    void program();

    void program_declaration();
    void compound_statement();
    void program_body();

    void variable_declaration();
    void variable_list();

    void statement();

    void goto_expression();
    void end_goto_expression();
    void if_expression();
    void for_expression();
    
    void get_expression();
    void put_expression();

    void assign_expression();

    bool arithmetic_expression();
    bool term();
    bool factor();

    bool logical_expression();
    bool or_expression();
    bool and_expression();

    bool match(const LexemType expectedType);
};};