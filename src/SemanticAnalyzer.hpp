#pragma once
#include <vector>
#include <fstream>
#include <iostream>
#include <map>
#include <list>

#include "constants.hpp"

namespace k_13 {
class SemanticAnalyzer {
public:
    SemanticAnalyzer() = default;
    ~SemanticAnalyzer() = default;

    int analyze(const std::map<std::string, std::vector<std::pair<int, ExpressionType>>> &identifiers
                , const std::map<std::string, std::list<std::pair<int, ExpressionType>>> &labels
                , const std::map<std::string, LexemType> &variableTable
                , const std::list<std::pair<LexemType, std::vector<Lexem>>> &expressions);

private:
    bool checkIdentifiers(const std::map<std::string, std::vector<std::pair<int, ExpressionType>>> &identifiers
                          , const std::map<std::string, std::list<std::pair<int, ExpressionType>>> &labels);
    bool checkLabels(const std::map<std::string, std::list<std::pair<int, ExpressionType>>> &labels);
    bool checkVariables(const std::map<std::string, LexemType> &variableTable, const std::list<std::pair<LexemType, std::vector<Lexem>>> &expressions);

    std::tuple<bool, bool, bool> checkExpression(const std::vector<Lexem> &expression, const std::map<std::string, LexemType> &variableTable);
    void checkVariable(const std::tuple<bool, bool, bool> &varParams, std::vector<std::pair<int, ExpressionType>> &identifiers
                                        , const std::string &identifier);

    std::vector<std::string> errorMessages;
    std::vector<std::string> warnings;
    int pos = 0;
    bool wasDeclared = false;
};};