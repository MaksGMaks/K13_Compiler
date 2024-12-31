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
    // <програма> = "program" <ідентифікатор> ";" <складений_оператор>
    void program();

    // <програма> = "program" <ідентифікатор> ";"
    void program_declaration();
    // <складений_оператор> = "start" <змінні> <тіло> "finish"
    Keyword compound_statement();
    // <тіло> = <оператор> {";" | <оператор>}
    std::vector<Keyword> program_body();

    // <змінні> = "var" <список_змінних> ";"
    std::map<std::string, LexemType> variable_declaration();
    // <список_змінних> = <тип> <ідентифікатор> {"," <тип> <ідентифікатор> | <ідентифікатор>} | NULL
    std::map<std::string, LexemType> variable_list();

    // <оператор> = <складений_оператор> | <умовний_оператор> | <перехід> | <точка_переходу> | <цикл> | <присвоєння> | <ввід> | <вивід>
    Keyword statement();

    // <перехід> = "goto" <ідентифікатор> ";"
    Keyword goto_expression();
    // <точка_переходу> = <ідентифікатор> ";"
    Keyword end_goto_expression();
    // <умовний_оператор> = "if" "(" <логічний_вираз> ")" <складений_оператор> <перехід> <точка_переходу> <перехід> <точка_переходу>
    Keyword if_expression();
    // <цикл> = "for" <ідентифікатор> ":=" <арифметичний_вираз> "to" <арифметичний_вираз> <тіло> "next" <ідентифікатор> ";"
    Keyword for_expression();

    // <ввід> = "get" "(" <ідентифікатор> ")" ";"
    Keyword get_expression();
    // <вивід> = "put" "(" <рядковий_вираз> ")" ";"
    Keyword put_expression();

    // <присвоєння> = <ідентифікатор> ":=" [<рядковий_вираз> | <логічний_вираз>] ";"
    Keyword assign_expression();

    // <логічний_вираз> = <логічний_терм> {"||" <логічний_терм>}
    bool logical_expression();
    // <логічний_терм> = <вираз_порівняння> {"&&" <вираз_порівняння>}
    bool logical_term();
    // <вираз_порівняння> = ["!!(" <логічний_вираз> ")" | <вираз_відношення>] ["=" | "<>"] ["!!(" <логічний_вираз> ")" | <вираз_відношення>]
    bool equality_expression();
    // <вираз_відношення> = [ <арифметичний_вираз> | <рядковий_літерал> ] ["le" | "ge"] [ <арифметичний_вираз> | <рядковий_літерал> ]
    bool relational_expression();

    // <арифметичний_вираз> = <терм> { ["+" | "-"] <терм> }
    bool arithmetic_expression();
    // <терм> = <фактор> { ["*" | "/" | "%"] <фактор> }
    bool term();
    // <фактор> = <число> | <булівський_тип> | <ідентифікатор> | "(" <логічний_вираз> ")"
    bool factor();

    // <рядковий_вираз> = <рядковий_фактор> {"+" <рядковий_фактор>}
    bool string_expression();
    // <рядковий_фактор> = <рядковий_літерал> | <фактор>
    bool string_factor();

    bool match(const LexemType expectedType);
};};