#include "SyntaxAnalyzer2.hpp"

k_13::SyntaxAnalyzer2::SyntaxAnalyzer2() {
    code.clear();
    errorMessages.clear();
}

int k_13::SyntaxAnalyzer2::analyze(const std::vector<Lexem> &lexems, const std::vector<UnknownLexem> &unknowns) {
    std::cout << "[INFO] Starting syntax analysis" << std::endl;
    code = lexems;
    unknownLexems = unknowns;
    position = 0;

    program();
    if(errorMessages.size() > 0) {
        for(auto message : errorMessages) {
            std::cerr << message << std::endl;
        }
        return -1;
    }
    return 0;
}

bool k_13::SyntaxAnalyzer2::match(const LexemType expectedType) {
    if(code[position].type == expectedType) {
        position++;
        return true;
    } 
    return false;
}

void k_13::SyntaxAnalyzer2::program() {
    program_declaration();
    program_body();
}

void k_13::SyntaxAnalyzer2::program_declaration() {
    if(!match(LexemType::PROGRAM)) {
        errorMessages.push_back("\tSyntax error at line " + std::to_string(code[position-1].line) + ": Expected 'program' keyword before program name");
    }
    if(!match(LexemType::IDENTIFIER)) {
        errorMessages.push_back("\tSyntax error at line " + std::to_string(code[position-1].line) + ": Expected program name after 'program' keyword");
    }
    if(!match(LexemType::SEMICOLON)) {
        (position < code.size()) 
        ? errorMessages.push_back("\tSyntax error at line " + std::to_string(code[position-1].line) + ": Missing ';' before statement " + code[position].value)
        : errorMessages.push_back("\tSyntax error at line " + std::to_string(code[position-1].line) + ": Missing ';' after statement " + code[position - 1].value);
    }
}

void k_13::SyntaxAnalyzer2::compound_statement() {
    if(!match(LexemType::START)) {
        errorMessages.push_back("\tSyntax error at line " + std::to_string(code[position-1].line) + ": Expected 'start' keyword before compound statement");
    }
    variable_declaration();
    program_body();
    if(!match(LexemType::FINISH)) {
        errorMessages.push_back("\tSyntax error at line " + std::to_string(code[position-1].line) + ": Expected 'finish' keyword after compound statement");
    }
}

void k_13::SyntaxAnalyzer2::program_body() {
    do {
        statement();
        if(position >= code.size()) {
            break;
        }
    }
    while(code[position].type != LexemType::FINISH && position < code.size());
}

void k_13::SyntaxAnalyzer2::statement() {
    switch(code[position].type) {
        case LexemType::START:
            compound_statement();
            break;
        case LexemType::GET:
            get_expression();
            if(!match(LexemType::SEMICOLON)) {
                (position < code.size()) 
                ? errorMessages.push_back("\tSyntax error at line " + std::to_string(code[position-1].line) + ": Missing ';' before statement " + code[position].value)
                : errorMessages.push_back("\tSyntax error at line " + std::to_string(code[position-1].line) + ": Missing ';' after statement " + code[position - 1].value);
            }
            break;
        case LexemType::PUT:
            put_expression();
            if(!match(LexemType::SEMICOLON)) {
                (position < code.size()) 
                ? errorMessages.push_back("\tSyntax error at line " + std::to_string(code[position-1].line) + ": Missing ';' before statement " + code[position].value)
                : errorMessages.push_back("\tSyntax error at line " + std::to_string(code[position-1].line) + ": Missing ';' after statement " + code[position - 1].value);
            }
            break;
        case LexemType::IF:
            if_expression();
            break;
        case LexemType::GOTO:
            goto_expression();
            if(!match(LexemType::SEMICOLON)) {
                (position < code.size()) 
                ? errorMessages.push_back("\tSyntax error at line " + std::to_string(code[position-1].line) + ": Missing ';' before statement " + code[position].value)
                : errorMessages.push_back("\tSyntax error at line " + std::to_string(code[position-1].line) + ": Missing ';' after statement " + code[position - 1].value);
            }
            break;
        case LexemType::FOR:
            for_expression();
            break;
        case LexemType::IDENTIFIER:
            if(code[position+1].type == LexemType::ASSIGN) {
                assign_expression();
            } else {
                end_goto_expression();
            }
            if(!match(LexemType::SEMICOLON)) {
                (position < code.size()) 
                ? errorMessages.push_back("\tSyntax error at line " + std::to_string(code[position-1].line) + ": Missing ';' before statement " + code[position].value)
                : errorMessages.push_back("\tSyntax error at line " + std::to_string(code[position-1].line) + ": Missing ';' after statement " + code[position - 1].value);
            }
            break;
        case LexemType::FINISH:
            break;
        case LexemType::UNKNOWN:
            errorMessages.push_back("\tSyntax error at line " + std::to_string(code[position].line) + ": Unknown statement " + unknownLexems[std::stoi(code[position].value) - 1].value);
            position++;
            break;
        default:
            errorMessages.push_back("\tSyntax error at line " + std::to_string(code[position].line) + ": Unknown statement " + code[position].value);
            position++;
            break;
    }
}

void k_13::SyntaxAnalyzer2::variable_declaration() {
    if(!match(LexemType::VAR)) {
        errorMessages.push_back("\tSyntax error at line " + std::to_string(code[position-1].line) + ": Expected 'var' keyword before variable segment");
    }
    if(code[position].type == LexemType::INT || code[position].type == LexemType::BOOL || code[position].type == LexemType::STRING)
        variable_list();

    if(!match(LexemType::SEMICOLON)) {
        (position < code.size()) 
        ? errorMessages.push_back("\tSyntax error at line " + std::to_string(code[position-1].line) + ": Missing ';' before statement " + code[position].value)
        : errorMessages.push_back("\tSyntax error at line " + std::to_string(code[position-1].line) + ": Missing ';' after statement " + code[position - 1].value);
    }
}

void k_13::SyntaxAnalyzer2::variable_list() {
    position++;
    if(!match(LexemType::IDENTIFIER)) {
        errorMessages.push_back("\tSyntax error at line " + std::to_string(code[position-1].line) + ": Expected identifier after variable type");
    }
    while(code[position].type == LexemType::COMMA) {
        position++;
        if(code[position].type == LexemType::IDENTIFIER) {
            position++;
        } else {
            if(!(match(LexemType::INT) || match(LexemType::BOOL) || match(LexemType::STRING))) {
                errorMessages.push_back("\tSyntax error at line " + std::to_string(code[position-1].line) + ": Expected variable type before identifier");
            }
            if(!match(LexemType::IDENTIFIER)) {
                errorMessages.push_back("\tSyntax error at line " + std::to_string(code[position-1].line) + ": Expected identifier after variable type");
            }
        }
    }
}

void k_13::SyntaxAnalyzer2::get_expression() {
    position++;
    if(!match(LexemType::LPAREN)) {
        errorMessages.push_back("\tSyntax error at line " + std::to_string(code[position-1].line) + ": Expected '(' before identifier");
    }
    if(!match(LexemType::IDENTIFIER)) {
        errorMessages.push_back("\tSyntax error at line " + std::to_string(code[position-1].line) + ": Expected identifier after 'get' statement");
    }
    if(!match(LexemType::RPAREN)) {
        errorMessages.push_back("\tSyntax error at line " + std::to_string(code[position-1].line) + ": Expected ')' after identifier");
    }
}

void k_13::SyntaxAnalyzer2::put_expression() {
    position++;
    if(!match(LexemType::LPAREN)) {
        errorMessages.push_back("\tSyntax error at line " + std::to_string(code[position-1].line) + ": Expected '(' before identifier");
    }
    if(!match(LexemType::IDENTIFIER)) {
        if(code[position].type == LexemType::STRING_LITERAL && code[position+1].type == LexemType::RPAREN) {
            position++;
        } else {
            int current_position = position;
            subErrors.clear();
            log_arith(LexemType::RPAREN, current_position);
        }
    }
    if(!match(LexemType::RPAREN)) {
        errorMessages.push_back("\tSyntax error at line " + std::to_string(code[position-1].line) + ": Expected ')' after expression");
    }
}

void k_13::SyntaxAnalyzer2::assign_expression() {
    position++;
    if(!match(LexemType::ASSIGN)) {
        errorMessages.push_back("\tSyntax error at line " + std::to_string(code[position-1].line) + ": Expected ':=' after identifier");
    }
    int current_position = position;
    subErrors.clear();
    log_arith(LexemType::SEMICOLON, current_position);
}

void k_13::SyntaxAnalyzer2::goto_expression() {
    position++;
    if(!match(LexemType::IDENTIFIER)) {
        errorMessages.push_back("\tSyntax error at line " + std::to_string(code[position-1].line) + ": Expected identifier after 'goto' statement");
    }
}

void k_13::SyntaxAnalyzer2::end_goto_expression() {
    if(!match(LexemType::IDENTIFIER)) {
        errorMessages.push_back("\tSyntax error at line " + std::to_string(code[position-1].line) + ": Expected identifier after 'end' statement");
    }
}

void k_13::SyntaxAnalyzer2::if_expression() {
    position++;
    if(!match(LexemType::LPAREN)) {
        errorMessages.push_back("\tSyntax error at line " + std::to_string(code[position-1].line) + ": Expected '(' before condition expression");
    }
    int current_position = position;
    subErrors.clear();
    log_arith(LexemType::RPAREN, current_position);
    if(!match(LexemType::RPAREN)) {
        errorMessages.push_back("\tSyntax error at line " + std::to_string(code[position-1].line) + ": Expected ')' after condition expression");
    }
    goto_expression();
    if(!match(LexemType::SEMICOLON)) {
        (position < code.size()) 
        ? errorMessages.push_back("\tSyntax error at line " + std::to_string(code[position-1].line) + ": Missing ';' before statement " + code[position].value)
        : errorMessages.push_back("\tSyntax error at line " + std::to_string(code[position-1].line) + ": Missing ';' after statement " + code[position - 1].value);
    }
    std::string ifErrors = "";
    if(code[position].type != LexemType::START) {
        ifErrors += "\tSyntax error at line " + std::to_string(code[position].line) + ": Unknown statements before 'start' keyword: ";
    }
    while (code[position].type != LexemType::START && position < code.size() - 1) {
        ifErrors += code[position].value + " "; 
        position++;
    }
    if(position == code.size() - 1) {
        errorMessages.push_back("\tSyntax error at line " + std::to_string(code[position].line) + ": Expected 'start' keyword after 'if' statement");
        return;
    }
    if(ifErrors != "") {
        ifErrors += "\n";
        errorMessages.push_back(ifErrors);
    }
    compound_statement();
    goto_expression();
    if(!match(LexemType::SEMICOLON)) {
        (position < code.size()) 
        ? errorMessages.push_back("\tSyntax error at line " + std::to_string(code[position-1].line) + ": Missing ';' before statement " + code[position].value)
        : errorMessages.push_back("\tSyntax error at line " + std::to_string(code[position-1].line) + ": Missing ';' after statement " + code[position - 1].value);
    }
    end_goto_expression();
    if(!match(LexemType::SEMICOLON)) {
        (position < code.size()) 
        ? errorMessages.push_back("\tSyntax error at line " + std::to_string(code[position-1].line) + ": Missing ';' before statement " + code[position].value)
        : errorMessages.push_back("\tSyntax error at line " + std::to_string(code[position-1].line) + ": Missing ';' after statement " + code[position - 1].value);
    }
}
    
void k_13::SyntaxAnalyzer2::for_expression() {
    position++;
    if(!match(LexemType::IDENTIFIER)) {
        errorMessages.push_back("\tSyntax error at line " + std::to_string(code[position-1].line) + ": Expected identifier after 'for' statement");
    }
    int current_position = position;
    subErrors.clear();
    log_arith(LexemType::TO, current_position);
    if(!match(LexemType::TO)) {
        errorMessages.push_back("\tSyntax error at line " + std::to_string(code[position-1].line) + ": Expected 'to' keyword after identifier");
    }
    current_position = position;
    subErrors.clear();
    log_arith(LexemType::NEXT, current_position);
    if(!match(LexemType::NEXT)) {
        errorMessages.push_back("\tSyntax error at line " + std::to_string(code[position-1].line) + ": Expected 'next' keyword after condition expression");
    }
    std::string ifErrors = "";
    if(code[position].type != LexemType::START) {
        ifErrors += "\tSyntax error at line " + std::to_string(code[position].line) + ": Unknown statements before 'start' keyword: ";
    }
    while (code[position].type != LexemType::START && position < code.size() - 1) {
        ifErrors += code[position].value + " "; 
        position++;
    }
    if(position == code.size() - 1) {
        errorMessages.push_back("\tSyntax error at line " + std::to_string(code[position].line) + ": Expected 'start' keyword after 'if' statement");
        return;
    }
    if(ifErrors != "") {
        ifErrors += "\n";
        errorMessages.push_back(ifErrors);
    }
    compound_statement();
}

bool k_13::SyntaxAnalyzer2::arithmetic_expression() {
    bool result = term();
    while(code[position].type == LexemType::ADD || code[position].type == LexemType::SUB) {
        position++;
        result = term();
    }
    return result;
}

bool k_13::SyntaxAnalyzer2::term() {
    bool result = factor();
    while(code[position].type == LexemType::MUL || code[position].type == LexemType::DIV || code[position].type == LexemType::MOD) {
        position++;
        result = factor();
    }
    return result;
}

bool k_13::SyntaxAnalyzer2::factor() {
    bool result = true;
    switch(code[position].type) {
        case LexemType::IDENTIFIER:
        case LexemType::NUMBER:
        case LexemType::TRUE:
        case LexemType::FALSE:
        case LexemType::STRING_LITERAL:
            position++;
            break;
        case LexemType::UNKNOWN:
            result = false;
            subErrors.push_back("\tSyntax error at line " + std::to_string(code[position].line) + ": Unknown statement " + unknownLexems[std::stoi(code[position].value) - 1].value);
            position++;
        break;
        case LexemType::LPAREN:
            position++;    
            if(!arithmetic_expression()) {
                result = false;
                subErrors.push_back("\tSyntax error at line " + std::to_string(code[position].line) + ": Expected expression");
            }
            if(!match(LexemType::RPAREN)) {
                result = false;
                subErrors.push_back("\tSyntax error at line " + std::to_string(code[position-1].line) + ": Expected ')' after expression");
                errors++;
            }
            break;
        default:
            position++;
            result = false;
            subErrors.push_back("\tSyntax error at line " + std::to_string(code[position].line) + ": Expected factor");
            errors++;
            break;
    }
    return result;
}

bool k_13::SyntaxAnalyzer2::logical_expression() {
    int current_position = position;
    bool result = true;
    if(code[position].type == LexemType::NOT) {
        position++;
        if(!match(LexemType::LPAREN)) {
            result = false;
            subErrors.push_back("\tSyntax error at line " + std::to_string(code[position-1].line) + ": Expected '(' before expression");
        }
        current_position = position;
        result = log_arith(LexemType::RPAREN, current_position);
        if(!match(LexemType::RPAREN)) {
            result = false;
            subErrors.push_back("\tSyntax error at line " + std::to_string(code[position-1].line) + ": Expected ')' after expression");
        }
    } else {
        result = or_expression();
    }

    while(code[position].type == LexemType::OR) {
        position++;
        if(code[position].type == LexemType::NOT) {
            position++;
            if(!match(LexemType::LPAREN)) {
                result = false;
                subErrors.push_back("\tSyntax error at line " + std::to_string(code[position-1].line) + ": Expected '(' before expression");
            }
            current_position = position;
            result = log_arith(LexemType::RPAREN, current_position);
            if(!match(LexemType::RPAREN)) {
                result = false;
                subErrors.push_back("\tSyntax error at line " + std::to_string(code[position-1].line) + ": Expected ')' after expression");
            }
        } else {
            result = or_expression();
        }
    }
    return result;
}

bool k_13::SyntaxAnalyzer2::or_expression() {
    bool result = and_expression();
    while(code[position].type == LexemType::AND) {
        position++;
        result = and_expression();
    }
    return result;
}

bool k_13::SyntaxAnalyzer2::and_expression() {
    bool result = true;
    int current_position = position;
    switch (code[position].type) {
    case LexemType::LPAREN:
        position++;
        current_position = position;
        result = log_arith(LexemType::RPAREN, current_position);
        
        if(!match(LexemType::RPAREN)) {
            result = false;
            subErrors.push_back("\tSyntax error at line " + std::to_string(code[position-1].line) + ": Expected ')' after expression");
        }
        break;
    case LexemType::IDENTIFIER:
    case LexemType::NUMBER:
    case LexemType::TRUE:
    case LexemType::FALSE:
    case LexemType::STRING_LITERAL:
        position++;
        if(code[position].type == LexemType::ADD || code[position].type == LexemType::SUB || code[position].type == LexemType::MUL || code[position].type == LexemType::DIV || code[position].type == LexemType::MOD) {
            position++;
            result = term();
        }
        break;
    case LexemType::UNKNOWN:
        result = false;
        subErrors.push_back("\tSyntax error at line " + std::to_string(code[position].line) + ": Unknown statement " + unknownLexems[std::stoi(code[position].value) - 1].value);
        position++;
        break;
    case LexemType::NOT:
        position++;
        if(!match(LexemType::LPAREN)) {
            result = false;
            subErrors.push_back("\tSyntax error at line " + std::to_string(code[position-1].line) + ": Expected '(' before expression");
        }
        current_position = position;
        result = log_arith(LexemType::RPAREN, current_position);
        if(!match(LexemType::RPAREN)) {
            result = false;
            subErrors.push_back("\tSyntax error at line " + std::to_string(code[position-1].line) + ": Expected ')' after expression");
        }
    default:
        position++;
        result = false;
        subErrors.push_back("\tSyntax error at line " + std::to_string(code[position].line) + ": Expected factor");
        break;
    }
    if(code[position].type == LexemType::EQUAL || code[position].type == LexemType::NEQUAL || code[position].type == LexemType::LESS || code[position].type == LexemType::GREATER) {
        position++;
        switch (code[position].type) {
        case LexemType::LPAREN:
            position++;
            current_position = position;
            result = log_arith(LexemType::RPAREN, current_position);
            
            if(!match(LexemType::RPAREN)) {
                result = false;
                subErrors.push_back("\tSyntax error at line " + std::to_string(code[position-1].line) + ": Expected ')' after expression");
            }
            break;
        case LexemType::IDENTIFIER:
        case LexemType::NUMBER:
        case LexemType::TRUE:
        case LexemType::FALSE:
        case LexemType::STRING_LITERAL:
            position++;
            if(code[position].type == LexemType::ADD || code[position].type == LexemType::SUB || code[position].type == LexemType::MUL || code[position].type == LexemType::DIV || code[position].type == LexemType::MOD) {
                position++;
                result = term();
            }
            break;
        case LexemType::UNKNOWN:
            result = false;
            subErrors.push_back("\tSyntax error at line " + std::to_string(code[position].line) + ": Unknown statement " + unknownLexems[std::stoi(code[position].value) - 1].value);
            position++;
            break;
        case LexemType::NOT:
            position++;
            if(!match(LexemType::LPAREN)) {
                result = false;
                subErrors.push_back("\tSyntax error at line " + std::to_string(code[position-1].line) + ": Expected '(' before expression");
            }
            current_position = position;
            result = log_arith(LexemType::RPAREN, current_position);
            if(!match(LexemType::RPAREN)) {
                result = false;
                subErrors.push_back("\tSyntax error at line " + std::to_string(code[position-1].line) + ": Expected ')' after expression");
            }
        default:
            position++;
            result = false;
            subErrors.push_back("\tSyntax error at line " + std::to_string(code[position].line) + ": Expected factor");
            break;
        }
    }
    return result;
}

bool k_13::SyntaxAnalyzer2::log_arith(const LexemType expectedType, const int current_position) {
    bool result = true;
    if(code[position].type == LexemType::IDENTIFIER && code[position+1].type != expectedType) {
        if(!logical_expression()) {
            int subErrorsSize = subErrors.size();
            position = current_position;
            if(!arithmetic_expression()) {
                result = false;
                std::string error = "\tSyntax error at line " + std::to_string(code[position].line) + ": Expected any expression."
                    "\n\tPossible problem for logical:\n";
                for(int i = 0; i < subErrorsSize; i++) {
                    error += "\t" + subErrors[i] + "\n";
                }
                error += "\t Possible problem for arithmetic:\n";
                for(int i = subErrorsSize; i < subErrors.size(); i++) {
                    error += "\t" + subErrors[i] + "\n";
                }
                errorMessages.push_back(error);
            } else if(code[position].type != expectedType) {
                result = false;
                std::string error = "\tSyntax error at line " + std::to_string(code[position].line) + ": Expected any expression."
                    "\n\tPossible problem for logical:\n";
                for(int i = 0; i < subErrorsSize; i++) {
                    error += "\t" + subErrors[i] + "\n";
                }
                error += "\t Possible problem for arithmetic:\n";
                for(int i = subErrorsSize; i < subErrors.size(); i++) {
                    error += "\t" + subErrors[i] + "\n";
                }
                errorMessages.push_back(error);
            }
        } else if(code[position].type != expectedType) {
            int subErrorsSize = subErrors.size();
            position = current_position;
            if(!arithmetic_expression()) {
                result = false;
                std::string error = "\tSyntax error at line " + std::to_string(code[position].line) + ": Expected any expression."
                    "\n\tPossible problem for logical:\n";
                for(int i = 0; i < subErrorsSize; i++) {
                    error += "\t" + subErrors[i] + "\n";
                }
                error += "\t Possible problem for arithmetic:\n";
                for(int i = subErrorsSize; i < subErrors.size(); i++) {
                    error += "\t" + subErrors[i] + "\n";
                }
                errorMessages.push_back(error);
            } else if(code[position].type != expectedType) {
                result = false;
                std::string error = "\tSyntax error at line " + std::to_string(code[position].line) + ": Expected any expression."
                    "\n\tPossible problem for logical:\n";
                for(int i = 0; i < subErrorsSize; i++) {
                    error += "\t" + subErrors[i] + "\n";
                }
                error += "\t Possible problem for arithmetic:\n";
                for(int i = subErrorsSize; i < subErrors.size(); i++) {
                    error += "\t" + subErrors[i] + "\n";
                }
                errorMessages.push_back(error);
            }
        }
    } else if(!match(LexemType::IDENTIFIER)) {
        if(!logical_expression()) {
            int subErrorsSize = subErrors.size();
            position = current_position;
            if(!arithmetic_expression()) {
                result = false;
                std::string error = "\tSyntax error at line " + std::to_string(code[position].line) + ": Expected any expression."
                    "\n\tPossible problem for logical:\n";
                for(int i = 0; i < subErrorsSize; i++) {
                    error += "\t" + subErrors[i] + "\n";
                }
                error += "\t Possible problem for arithmetic:\n";
                for(int i = subErrorsSize; i < subErrors.size(); i++) {
                    error += "\t" + subErrors[i] + "\n";
                }
                errorMessages.push_back(error);
            } else if(code[position].type != expectedType) {
                result = false;
                std::string error = "\tSyntax error at line " + std::to_string(code[position].line) + ": Expected any expression."
                    "\n\tPossible problem for logical:\n";
                for(int i = 0; i < subErrorsSize; i++) {
                    error += "\t" + subErrors[i] + "\n";
                }
                error += "\t Possible problem for arithmetic:\n";
                for(int i = subErrorsSize; i < subErrors.size(); i++) {
                    error += "\t" + subErrors[i] + "\n";
                }
                errorMessages.push_back(error);
            }
        } else if(code[position].type != expectedType) {
            int subErrorsSize = subErrors.size();
            position = current_position;
            if(!arithmetic_expression()) {
                result = false;
                std::string error = "\tSyntax error at line " + std::to_string(code[position].line) + ": Expected any expression."
                    "\n\tPossible problem for logical:\n";
                for(int i = 0; i < subErrorsSize; i++) {
                    error += "\t" + subErrors[i] + "\n";
                }
                error += "\t Possible problem for arithmetic:\n";
                for(int i = subErrorsSize; i < subErrors.size(); i++) {
                    error += "\t" + subErrors[i] + "\n";
                }
                errorMessages.push_back(error);
            } else if(code[position].type != expectedType) {
                result = false;
                std::string error = "\tSyntax error at line " + std::to_string(code[position].line) + ": Expected any expression."
                    "\n\tPossible problem for logical:\n";
                for(int i = 0; i < subErrorsSize; i++) {
                    error += "\t" + subErrors[i] + "\n";
                }
                error += "\t Possible problem for arithmetic:\n";
                for(int i = subErrorsSize; i < subErrors.size(); i++) {
                    error += "\t" + subErrors[i] + "\n";
                }
                errorMessages.push_back(error);
            }
        }
    }
    return result;
}