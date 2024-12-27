#include "SemanticAnalyzer.hpp"

int k_13::SemanticAnalyzer::analyze(const std::map<std::string, std::list<std::pair<int, ExpressionType>>> &identifiers
, const std::map<std::string, std::list<std::pair<int, ExpressionType>>> &labels
, const std::map<std::string, LexemType> &variableTable
, const std::list<std::pair<LexemType, std::vector<Lexem>>> &expressions) {
    bool identifiersChecked = checkIdentifiers(identifiers, labels);
    bool labelsChecked = checkLabels(labels);
    bool expressionsChecked = checkVariables(variableTable, expressions);
    if (identifiersChecked && labelsChecked && expressionsChecked) {
        return 0;
    }
    return -1;
}

bool k_13::SemanticAnalyzer::checkIdentifiers(const std::map<std::string, std::list<std::pair<int, ExpressionType>>> &identifiers
, const std::map<std::string, std::list<std::pair<int, ExpressionType>>> &labels) {
    errorMessages.clear();
    warnings.clear();
    for(auto identifier : identifiers) {
        if(labels.find(identifier.first) != labels.end()) {
            errorMessages.push_back("\tSemantic error at line " + std::to_string(identifier.second.begin()->first) + ": Identifier " + identifier.first + " is a label");
        } else {
            bool isInitialized = false, isDeclared = false, isUsed = false, isFor = false;
            for(auto expression : identifier.second) {
                switch(expression.second) {
                case ExpressionType::START:
                    break;
                case ExpressionType::FINISH:
                    isDeclared = false;
                    isInitialized = false;
                    isUsed = false;
                    break;
                case ExpressionType::ASSIGNMENT:
                case ExpressionType::INPUT:
                    if(!isDeclared) {
                        errorMessages.push_back("\tSemantic error at line " + std::to_string(expression.first) + ": Identifier " + identifier.first + " is not declared");
                    }
                    if(isFor) {
                        warnings.push_back("\tWarning at line " + std::to_string(expression.first) + ": Identifier " + identifier.first + " is used in for loop. Possible undefined behavior");
                    }
                    isInitialized = true;
                    break;
                case ExpressionType::STARTFOR:
                    if(isDeclared) {
                        warnings.push_back("\tWarning at line " + std::to_string(expression.first) + ": Identifier " + identifier.first + " is already declared. Possible undefined behavior");
                    }
                    isFor = true;
                    break;
                case ExpressionType::ENDFOR:
                    isFor = false;
                    break;
                case ExpressionType::VARIABLE:
                    if(isDeclared) {
                        errorMessages.push_back("\tSemantic error at line " + std::to_string(expression.first) + ": Identifier " + identifier.first + " is already declared");
                    } else if(isFor) {
                        errorMessages.push_back("\tWarning at line " + std::to_string(expression.first) + ": Identifier " + identifier.first + " is used in for loop. Redeclaration is unacceptable"); 
                    }
                    isDeclared = true;
                    break;
                case ExpressionType::IF:
                case ExpressionType::EXPRESSION:    
                case ExpressionType::OUTPUT:
                    if(!isDeclared) {
                        errorMessages.push_back("\tSemantic error at line " + std::to_string(expression.first) + ": Identifier " + identifier.first + " is not declared");
                    } else if(!isInitialized) {
                        errorMessages.push_back("\tSemantic error at line  " + std::to_string(expression.first) + ": Identifier " + identifier.first + " is not initialized");
                    }
                    if(isFor) {
                        warnings.push_back("\tWarning at line " + std::to_string(expression.first) + ": Identifier " + identifier.first + " is used in for loop. Possible undefined behavior");
                    }
                    isUsed = true;
                    break;
                default:
                    break;
                }
            }
            if(!isDeclared || !isInitialized) {
                errorMessages.push_back("\tSemantic error at line " + std::to_string(identifier.second.begin()->first) + ": Identifier " + identifier.first + " is not declared or initialized");
            }
            if(!isUsed) {
                warnings.push_back("\tWarning at line " + std::to_string(identifier.second.begin()->first) + ": Identifier " + identifier.first + " is not used");
            }
        }
    }
    if(errorMessages.empty()) {
        return true;
    } else {
        for (auto error : errorMessages) {
            std::cout << error << std::endl;
        }
        for (auto warning : warnings) {
            std::cout << warning << std::endl;
        }
        return false;
    }
}

bool k_13::SemanticAnalyzer::checkLabels(const std::map<std::string, std::list<std::pair<int, ExpressionType>>> &labels) {
    errorMessages.clear();
    warnings.clear();
    for(auto label : labels) {
        bool isDeclared = false, isUsed = false;
        int depth = 0, labelDepth = 0, labelPosition = 0;
        std::vector<std::pair<int, int>> categorizedStatements;
        std::unordered_map<int, int> lastPositionAtDepth;
        lastPositionAtDepth[0] = 0;
        for(auto expression : label.second) {
            switch(expression.second) {
            case ExpressionType::START:
                depth++;
                if(lastPositionAtDepth.find(depth) == lastPositionAtDepth.end()) {
                    lastPositionAtDepth[depth]++;
                } else {
                    lastPositionAtDepth[depth] = 0;
                }
                break;
            case ExpressionType::FINISH:
                depth--;
                if(lastPositionAtDepth.find(depth) == lastPositionAtDepth.begin()) {
                    lastPositionAtDepth[depth]++;
                } else {
                    lastPositionAtDepth[depth] = 0;
                }
                break;
            case ExpressionType::LABEL:
                if(isDeclared) {
                    errorMessages.push_back("\tSemantic error at line " + std::to_string(expression.first) + ": Identifier " + label.first + " is already declared");
                }
                labelDepth = depth;
                labelPosition = lastPositionAtDepth[depth];
                isDeclared = true;
            case ExpressionType::GOTO:
                categorizedStatements.emplace_back(depth, lastPositionAtDepth[depth]);
            default:
                break;
            }  
        }
        if(!isDeclared) {
            errorMessages.push_back("\tSemantic error at line " + std::to_string(label.second.begin()->first) + ": Label " + label.first + " is not declared");
        } else {
            for(auto expression : categorizedStatements) {
                if(expression.first < labelDepth || (expression.first == labelDepth && expression.second != labelPosition)) {
                    errorMessages.push_back("\tSemantic error at line " + std::to_string(expression.first) + ": Label " + label.first + " is used out of scope");
                }
            }
        }
    }
    if(errorMessages.empty()) {
        return true;
    } else {
        for (auto error : errorMessages) {
            std::cout << error << std::endl;
        }
        for (auto warning : warnings) {
            std::cout << warning << std::endl;
        }
        return false;
    }
}

bool k_13::SemanticAnalyzer::checkVariables(const std::map<std::string, LexemType> &variableTable, const std::list<std::pair<LexemType, std::vector<Lexem>>> &expressions) {
    errorMessages.clear();
    warnings.clear();
    for(auto expression : expressions) {
        pos = 0;
        bool concatOp = true, hasString = false, hasComp = false, concatOpB = true, hasStringB = false;
        LexemType type = expression.first;
        switch (type) {
        case LexemType::INT:
            for(auto lexem : expression.second) {
                if(lexem.type == LexemType::IDENTIFIER) {
                    if(variableTable.at(lexem.value) == LexemType::STRING) {
                        errorMessages.push_back("\tSemantic error at line " + std::to_string(lexem.line) + ": Identifier " + lexem.value + " is declared as string, using as int is unacceptable");
                    } 
                }
            }
            break;
        case LexemType::BOOL:
            for(auto lexem : expression.second) {
                if(lexem.type == LexemType::STRING || lexem.type == LexemType::STRING_LITERAL) {
                    hasString = true;
                    if(!concatOp) {
                        errorMessages.push_back("\tSemantic error at line " + std::to_string(lexem.line) + ": In boolean expression string is used with non-concatenation operator"); 
                    }
                } else if(lexem.type == LexemType::AND || lexem.type == LexemType::OR) {
                    if(hasString != hasStringB) {
                        errorMessages.push_back("\tSemantic error at line " + std::to_string(lexem.line) + ": In boolean expression both operands must be one type");
                    } else if(hasString && !concatOp && !concatOpB) {
                        errorMessages.push_back("\tSemantic error at line " + std::to_string(lexem.line) + ": In boolean expression string is used with non-concatenation operator"); 
                    }
                    concatOp = true;
                    hasString = false;
                    hasComp = false;
                } else if(lexem.type == LexemType::LESS || lexem.type == LexemType::GREATER 
                          || lexem.type == LexemType::EQUAL || lexem.type == LexemType::NEQUAL) {
                    concatOpB = concatOp;
                    hasStringB = hasString;
                    concatOp = true;
                    hasString = false;
                    hasComp = true;
                } else if(lexem.type == LexemType::RPAREN) {
                    if(hasString != hasStringB) {
                        errorMessages.push_back("\tSemantic error at line " + std::to_string(lexem.line) + ": In boolean expression both operands must be one type");
                    } else if(hasString && !concatOp && !concatOpB) {
                        errorMessages.push_back("\tSemantic error at line " + std::to_string(lexem.line) + ": In boolean expression string is used with non-concatenation operator"); 
                    }
                    concatOp = true;
                    hasString = false;
                    hasComp = false;
                } else if(lexem.type == LexemType::SUB || lexem.type == LexemType::MUL || lexem.type == LexemType::DIV || lexem.type == LexemType::MOD) {
                    concatOp = false;
                } else if(lexem.type == LexemType::LPAREN) {
                    auto res = checkExpression(expression.second, variableTable);
                    if(!std::get<0>(res)) {
                        concatOp &= std::get<1>(res);
                        hasString |= std::get<2>(res);
                    }
                }
                pos++;
            }

            break;
        case LexemType::STRING:
            break;
        default:
            break;
        }
    }
    if(errorMessages.empty()) {
        return true;
    } else {
        for (auto error : errorMessages) {
            std::cout << error << std::endl;
        }
        for (auto warning : warnings) {
            std::cout << warning << std::endl;
        }
        return false;
    }
}

std::tuple<bool, bool, bool> k_13::SemanticAnalyzer::checkExpression(const std::vector<Lexem> &expression, const std::map<std::string, LexemType> &variableTable) {
    bool result = false, concatOp = true, hasString = false, hasComp = false, concatOpB = true, hasStringB = false;
    while(expression[pos].type != LexemType::RPAREN) {
        if(expression[pos].type == LexemType::STRING || expression[pos].type == LexemType::STRING_LITERAL) {
            hasString = true;
            if(!concatOp) {
                errorMessages.push_back("\tSemantic error at line " + std::to_string(expression[pos].line) + ": In boolean expression string is used with non-concatenation operator"); 
            }
        } else if(expression[pos].type == LexemType::AND || expression[pos].type == LexemType::OR) {
            if(hasString != hasStringB) {
                errorMessages.push_back("\tSemantic error at line " + std::to_string(expression[pos].line) + ": In boolean expression both operands must be one type");
            } else if(hasString && !concatOp && !concatOpB) {
                errorMessages.push_back("\tSemantic error at line " + std::to_string(expression[pos].line) + ": In boolean expression string is used with non-concatenation operator"); 
            }
            concatOp = true;
            hasString = false;
            hasComp = false;
        } else if(expression[pos].type == LexemType::LESS || expression[pos].type == LexemType::GREATER 
                    || expression[pos].type == LexemType::EQUAL || expression[pos].type == LexemType::NEQUAL) {
            concatOpB = concatOp;
            hasStringB = hasString;
            concatOp = true;
            hasString = false;
            hasComp = true;
        } else if(expression[pos].type == LexemType::RPAREN) {
            if(hasString != hasStringB) {
                errorMessages.push_back("\tSemantic error at line " + std::to_string(expression[pos].line) + ": In boolean expression both operands must be one type");
            } else if(hasString && !concatOp && !concatOpB) {
                errorMessages.push_back("\tSemantic error at line " + std::to_string(expression[pos].line) + ": In boolean expression string is used with non-concatenation operator"); 
            }
            concatOp = true;
            hasString = false;
            hasComp = false;
        } else if(expression[pos].type == LexemType::SUB || expression[pos].type == LexemType::MUL || expression[pos].type == LexemType::DIV || expression[pos].type == LexemType::MOD) {
            concatOp = false;
        } else if(expression[pos].type == LexemType::LPAREN) {
            auto res = checkExpression(expression, variableTable);
            if(!std::get<0>(res)) {
                concatOp &= std::get<1>(res);
                hasString |= std::get<2>(res);
            }
        }
        pos++;
    }
    return std::make_tuple(result, concatOp, hasString);
}