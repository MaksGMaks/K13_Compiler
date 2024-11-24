#pragma once

#include <vector>
#include <fstream>
#include <iostream>
#include <map>

#include "constants.hpp"

namespace k_13
{
    class SyntaxAnalyzer {
    public:
        SyntaxAnalyzer();
        ~SyntaxAnalyzer() = default;

        int analyze(const std::vector<Lexem> &lexems, const std::vector<UnknownLexem> &unknownLexems);
    private:
        int errors = 0;
        int position = 0;
        std::vector<Lexem> code;

        void program();

        void program_declaration();
        void compound_statement();
        void block_statement();
        void program_body();

        void variable_declaration();
        void variable_list();

        void statement();

        void end_goto_expression();
        void if_expression();
        void goto_expression();
        void for_expression();
        
        void assign_expression();

        void arithmetic_expression();
        void term();
        void factor();

        void logical_expression();
        void logical_term();
        void logical_factor();

        void conditional_expression();
        void and_expression();
        void compare_expression();

        void get_expression();
        void put_expression();
        
        bool match(const LexemType expectedType);
    };
};