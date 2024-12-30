#pragma once

#include <vector>
#include <fstream>
#include <iostream>
#include <map>
#include <list>

#include "constants.hpp"

namespace k_13 {
class SyntaxAnalyzer {
public:
    SyntaxAnalyzer() = default;
    ~SyntaxAnalyzer() = default;

    int analyze(const std::vector<Lexem> &lexems, const std::vector<UnknownLexem> &unknowns);
    std::map<std::string, std::vector<std::pair<int, ExpressionType>>> getIdentifiers() { return identifiers; }
    std::map<std::string, std::list<std::pair<int, ExpressionType>>> getLabels() { return labels; }
    std::map<std::string, LexemType> getVariableTable() { return variableTable; }
    std::list<std::pair<LexemType, std::vector<Lexem>>> getExpressions() { return expressions; }
    std::vector<Keyword> getKeywords() {return keywords;};
    std::string getProgramName() { return programName; }
private:
    int errors = 0;
    int position = 0;
    std::vector<Lexem> code;
    std::vector<UnknownLexem> unknownLexems;

    std::vector<std::string> subErrors;
    std::map<int, std::vector<std::string>> errorMessages;

    std::map<std::string, std::vector<std::pair<int, ExpressionType>>> identifiers;
    std::map<std::string, std::list<std::pair<int, ExpressionType>>> labels;
    std::map<std::string, LexemType> variableTable;
    std::list<std::pair<LexemType, std::vector<Lexem>>> expressions;
    std::string programName;

    std::vector<Keyword> keywords;
    std::vector<Lexem> expression;

    void program();

    void program_declaration();
    Keyword compound_statement();
    std::vector<Keyword> program_body();

    std::map<std::string, LexemType> variable_declaration();
    std::map<std::string, LexemType> variable_list();

    Keyword statement();

    Keyword goto_expression();
    Keyword end_goto_expression();
    Keyword if_expression();
    Keyword for_expression();
    
    Keyword get_expression();
    Keyword put_expression();

    Keyword assign_expression();

    bool arithmetic_expression();
    bool term();
    bool factor();

    bool logical_expression();
    bool or_expression();
    bool and_expression();
    bool equality_expression();
    bool relational_expression();

    bool string_expression();
    bool string_factor();

    bool match(const LexemType expectedType);
};};