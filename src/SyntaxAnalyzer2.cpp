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
    std::cout << "End of analyzing" << std::endl;
    if(errorMessages.size() > 0) {
        for(auto message : errorMessages) {
            std::cerr << message;
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
    std::cout << "End of program" << std::endl;
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
    std::cout << "Compound statement" << std::endl;
    if(!match(LexemType::START)) {
        errorMessages.push_back("\tSyntax error at line " + std::to_string(code[position-1].line) + ": Expected 'start' keyword before compound statement");
    }
    variable_declaration();
    program_body();
    // std::cout << "Position: " << position << "\t Line:" << code[position].line << std::endl;
    // std::cout << "Value: " << code[position].value << std::endl;
    if(!match(LexemType::FINISH)) {
        errorMessages.push_back("\tSyntax error at line " + std::to_string(code[position-1].line) + ": Expected 'finish' keyword after compound statement");
    }
    std::cout << "Reached finish" << std::endl;
}

void k_13::SyntaxAnalyzer2::program_body() {
    std::cout << "Program body" << std::endl;
    do {
        std::cout << "Position: " << position << "\t Line:" << code[position].line << std::endl;
        std::cout << "Value: " << code[position].value << std::endl;
        statement();
        // std::cout << "Position: " << position << "\t Line:" << code[position].line << std::endl;
        // std::cout << "Value: " << code[position].value << std::endl;
        if(position >= code.size()) {
            break;
        }
    }
    while(code[position].type != LexemType::FINISH && position < code.size());
    std::cout << "end of program body" << std::endl;
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
            position++;
            errorMessages.push_back("\tSyntax error at line " + std::to_string(code[position].line) + ": Unknown statement " + unknownLexems[std::stoi(code[position].value)].value + "\n");
            break;
        default:
            position++;
            errorMessages.push_back("\tSyntax error at line " + std::to_string(code[position].line) + ": Unknown statement " + code[position].value + "\n");
            break;
    }
}

void k_13::SyntaxAnalyzer2::variable_declaration() {
    if(!match(LexemType::VAR)) {
        errorMessages.push_back("\tSyntax error at line " + std::to_string(code[position-1].line) + ": Expected 'var' keyword before variable segment" + "\n");
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
        errorMessages.push_back("\tSyntax error at line " + std::to_string(code[position-1].line) + ": Expected identifier after variable type" + "\n");
    }
    while(code[position].type == LexemType::COMMA) {
        position++;
        if(code[position].type == LexemType::IDENTIFIER) {
            position++;
        } else {
            if(!(match(LexemType::INT) || match(LexemType::BOOL) || match(LexemType::STRING))) {
                errorMessages.push_back("\tSyntax error at line " + std::to_string(code[position-1].line) + ": Expected variable type before identifier" + "\n");
            }
            if(!match(LexemType::IDENTIFIER)) {
                errorMessages.push_back("\tSyntax error at line " + std::to_string(code[position-1].line) + ": Expected identifier after variable type" + "\n");
            }
        }
    }
}

void k_13::SyntaxAnalyzer2::get_expression() {
    position++;
    if(!match(LexemType::LPAREN)) {
        errorMessages.push_back("\tSyntax error at line " + std::to_string(code[position-1].line) + ": Expected '(' before identifier" + "\n");
    }
    if(!match(LexemType::IDENTIFIER)) {
        errorMessages.push_back("\tSyntax error at line " + std::to_string(code[position-1].line) + ": Expected identifier after 'get' statement" + "\n");
    }
    if(!match(LexemType::RPAREN)) {
        errorMessages.push_back("\tSyntax error at line " + std::to_string(code[position-1].line) + ": Expected ')' after identifier" + "\n");
    }
}

void k_13::SyntaxAnalyzer2::put_expression() {
    position++;
    if(!match(LexemType::LPAREN)) {
        errorMessages.push_back("\tSyntax error at line " + std::to_string(code[position-1].line) + ": Expected '(' before identifier" + "\n");
    }
    if(!match(LexemType::IDENTIFIER)) {
        if(code[position].type == LexemType::STRING_LITERAL && code[position+1].type == LexemType::RPAREN) {
            position++;
        } else {
            int current_position = position;
            subErrors.clear();
            if(!logical_expression() && code[position].type != LexemType::RPAREN) {
                int subErrorsSize = subErrors.size();
                position = current_position;
                if(!arithmetic_expression() && code[position].type != LexemType::RPAREN) {
                    errorMessages.push_back("\tSyntax error at line " + std::to_string(code[position].line) + ": Expected any expression to output."
                        "\n\tPossible problem for logical:" + "\n");
                    for(int i = 0; i < subErrorsSize; i++) {
                        errorMessages.push_back("\t" + subErrors[i]);
                    }
                    errorMessages.push_back("\n\t Possible problem for arithmetic:\n");
                    for(int i = subErrorsSize; i < subErrors.size(); i++) {
                        errorMessages.push_back("\t" + subErrors[i]);
                    }
                }
            }
        }
    }
    if(!match(LexemType::RPAREN)) {
        errorMessages.push_back("\tSyntax error at line " + std::to_string(code[position-1].line) + ": Expected ')' after expression" + "\n");
    }
}

void k_13::SyntaxAnalyzer2::assign_expression() {
    position++;
    if(!match(LexemType::ASSIGN)) {
        errorMessages.push_back("\tSyntax error at line " + std::to_string(code[position-1].line) + ": Expected ':=' after identifier" + "\n");
    }
    int current_position = position;
    subErrors.clear();
    if(!logical_expression() && code[position].type != LexemType::SEMICOLON) {
        int subErrorsSize = subErrors.size();
        position = current_position;
        if(!arithmetic_expression() && code[position].type != LexemType::SEMICOLON) {
            errorMessages.push_back("\tSyntax error at line " + std::to_string(code[position].line) + ": Expected any expression to assign."
                "\n\tPossible problem for logical:" + "\n");
            for(int i = 0; i < subErrorsSize; i++) {
                errorMessages.push_back("\t" + subErrors[i]);
            }
            errorMessages.push_back("\n\t Possible problem for arithmetic:\n");
            for(int i = subErrorsSize; i < subErrors.size(); i++) {
                errorMessages.push_back("\t" + subErrors[i]);
            }
        }
    }
}

void k_13::SyntaxAnalyzer2::goto_expression() {
    position++;
    if(!match(LexemType::IDENTIFIER)) {
        errorMessages.push_back("\tSyntax error at line " + std::to_string(code[position-1].line) + ": Expected identifier after 'goto' statement" + "\n");
    }
}

void k_13::SyntaxAnalyzer2::end_goto_expression() {
    if(!match(LexemType::IDENTIFIER)) {
        errorMessages.push_back("\tSyntax error at line " + std::to_string(code[position-1].line) + ": Expected identifier after 'end' statement" + "\n");
    }
}

void k_13::SyntaxAnalyzer2::if_expression() {
    position++;
    if(!match(LexemType::LPAREN)) {
        errorMessages.push_back("\tSyntax error at line " + std::to_string(code[position-1].line) + ": Expected '(' before condition expression" + "\n");
    }
    if(code[position].type == LexemType::IDENTIFIER && code[position+1].type != LexemType::RPAREN) {
        int current_position = position;
        subErrors.clear();
        if(!logical_expression() && code[position].type != LexemType::RPAREN) {
            int subErrorsSize = subErrors.size();
            position = current_position;
            if(!arithmetic_expression() && code[position].type != LexemType::RPAREN) {
                errorMessages.push_back("\tSyntax error at line " + std::to_string(code[position].line) + ": Expected any expression for condition."
                    "\n\tPossible problem for logical:" + "\n");
                for(int i = 0; i < subErrorsSize; i++) {
                    errorMessages.push_back("\t" + subErrors[i]);
                }
                errorMessages.push_back("\n\t Possible problem for arithmetic:\n");
                for(int i = subErrorsSize; i < subErrors.size(); i++) {
                    errorMessages.push_back("\t" + subErrors[i]);
                }
            }
        }
    } else if(!match(LexemType::IDENTIFIER)) {
        int current_position = position;
        subErrors.clear();
        if(!logical_expression() && code[position].type != LexemType::RPAREN) {
            int subErrorsSize = subErrors.size();
            position = current_position;
            if(!arithmetic_expression() && code[position].type != LexemType::RPAREN) {
                errorMessages.push_back("\tSyntax error at line " + std::to_string(code[position].line) + ": Expected any expression for condition."
                    "\n\tPossible problem for logical:" + "\n");
                for(int i = 0; i < subErrorsSize; i++) {
                    errorMessages.push_back("\t" + subErrors[i]);
                }
                errorMessages.push_back("\n\t Possible problem for arithmetic:\n");
                for(int i = subErrorsSize; i < subErrors.size(); i++) {
                    errorMessages.push_back("\t" + subErrors[i]);
                }
            }
        }
    }
    if(!match(LexemType::RPAREN)) {
        errorMessages.push_back("\tSyntax error at line " + std::to_string(code[position-1].line) + ": Expected ')' after condition expression" + "\n");
    }
    goto_expression();
    if(!match(LexemType::SEMICOLON)) {
        (position < code.size()) 
        ? errorMessages.push_back("\tSyntax error at line " + std::to_string(code[position-1].line) + ": Missing ';' before statement " + code[position].value)
        : errorMessages.push_back("\tSyntax error at line " + std::to_string(code[position-1].line) + ": Missing ';' after statement " + code[position - 1].value);
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
        errorMessages.push_back("\tSyntax error at line " + std::to_string(code[position-1].line) + ": Expected identifier after 'for' statement" + "\n");
    }
    int current_position = position;
    subErrors.clear();
    if(!logical_expression() && code[position].type != LexemType::TO) {
        int subErrorsSize = subErrors.size();
        position = current_position;
        if(!arithmetic_expression() && code[position].type != LexemType::TO) {
            errorMessages.push_back("\tSyntax error at line " + std::to_string(code[position].line) + ": Expected any expression for condition."
                "\n\tPossible problem for logical:" + "\n");
            for(int i = 0; i < subErrorsSize; i++) {
                errorMessages.push_back("\t" + subErrors[i]);
            }
            errorMessages.push_back("\n\t Possible problem for arithmetic:\n");
            for(int i = subErrorsSize; i < subErrors.size(); i++) {
                errorMessages.push_back("\t" + subErrors[i]);
            }
        }
    }
    if(!match(LexemType::TO)) {
        errorMessages.push_back("\tSyntax error at line " + std::to_string(code[position-1].line) + ": Expected 'to' keyword after identifier" + "\n");
    }
    current_position = position;
    subErrors.clear();
    if(!logical_expression() && code[position].type != LexemType::NEXT) {
        int subErrorsSize = subErrors.size();
        position = current_position;
        if(!arithmetic_expression() && code[position].type != LexemType::NEXT) {
            errorMessages.push_back("\tSyntax error at line " + std::to_string(code[position].line) + ": Expected any expression for condition."
                "\n\tPossible problem for logical:" + "\n");
            for(int i = 0; i < subErrorsSize; i++) {
                errorMessages.push_back("\t" + subErrors[i]);
            }
            errorMessages.push_back("\n\t Possible problem for arithmetic:\n");
            for(int i = subErrorsSize; i < subErrors.size(); i++) {
                errorMessages.push_back("\t" + subErrors[i]);
            }
        }
    }
    if(!match(LexemType::NEXT)) {
        errorMessages.push_back("\tSyntax error at line " + std::to_string(code[position-1].line) + ": Expected 'next' keyword after condition expression" + "\n");
    }
    compound_statement();
}

bool k_13::SyntaxAnalyzer2::arithmetic_expression() {
    // std::cout << "Arithmetic expression" << std::endl;
    // std::cout << "Position: " << position << "\t Line:" << std::to_string(code[position].line) << std::endl;
    // std::cout << code[position].value << std::endl;
    bool result = term();
    while(code[position].type == LexemType::ADD || code[position].type == LexemType::SUB) {
        position++;
        result = term();
    }
    return result;
}

bool k_13::SyntaxAnalyzer2::term() {
    // std::cout << "Term" << std::endl;
    // std::cout << "Position: " << position << "\t Line:" << std::to_string(code[position].line) << std::endl;
    // std::cout << code[position].value << std::endl;
    bool result = factor();
    while(code[position].type == LexemType::MUL || code[position].type == LexemType::DIV || code[position].type == LexemType::MOD) {
        position++;
        result = factor();
    }
    return result;
}

bool k_13::SyntaxAnalyzer2::factor() {
    // std::cout << "Factor" << std::endl;
    // std::cout << "Position: " << position << "\t Line:" << std::to_string(code[position].line) << std::endl;
    // std::cout << code[position].value << std::endl;
    bool result = true;
    switch(code[position].type) {
        case LexemType::IDENTIFIER:
        case LexemType::NUMBER:
        case LexemType::TRUE:
        case LexemType::FALSE:
        case LexemType::STRING_LITERAL:
            position++;
            break;
        case LexemType::LPAREN:
            position++;    
            if(!arithmetic_expression()) {
                result = false;
                subErrors.push_back("\tSyntax error at line " + std::to_string(code[position].line) + ": Expected expression" + "\n");
            }
            if(!match(LexemType::RPAREN)) {
                result = false;
                subErrors.push_back("\tSyntax error at line " + std::to_string(code[position-1].line) + ": Expected ')' after expression" + "\n");
                errors++;
            }
            break;
        default:
            position++;
            result = false;
            subErrors.push_back("\tSyntax error at line " + std::to_string(code[position].line) + ": Expected factor" + "\n");
            errors++;
            break;
    }
    return result;
}

bool k_13::SyntaxAnalyzer2::logical_expression() {
    // std::cout << "Logical expression" << std::endl;
    // std::cout << "Position: " << position << "\t Line:" << std::to_string(code[position].line) << std::endl;
    // std::cout << code[position].value << std::endl;
    int current_position = position;
    bool result = true;
    if(code[position].type == LexemType::NOT) {
        position++;
        if(!match(LexemType::LPAREN)) {
            result = false;
            subErrors.push_back("\tSyntax error at line " + std::to_string(code[position-1].line) + ": Expected '(' before expression" + "\n");
        }
        current_position = position;
        if(!logical_expression()) {
            position = current_position;
            if(!arithmetic_expression()) {
                result = false;
                subErrors.push_back("\tSyntax error at line " + std::to_string(code[position].line) + ": Expected logical or arithmatical expression" + "\n");
            }
        }
        if(!match(LexemType::RPAREN)) {
            result = false;
            subErrors.push_back("\tSyntax error at line " + std::to_string(code[position-1].line) + ": Expected ')' after expression" + "\n");
        }
    } else {
        result = or_expression();
    }

    while(code[position].type == LexemType::OR) {
        position++;
        // std::cout << "Logical expression" << std::endl;
        // std::cout << "Position: " << position << "\t Line:" << std::to_string(code[position].line) << std::endl;
        // std::cout << code[position].value << std::endl;
        if(code[position].type == LexemType::NOT) {
            position++;
            if(!match(LexemType::LPAREN)) {
                result = false;
                subErrors.push_back("\tSyntax error at line " + std::to_string(code[position-1].line) + ": Expected '(' before expression" + "\n");
            }
            current_position = position;
            if(!logical_expression()) {
                position = current_position;
                if(!arithmetic_expression()) {
                    result = false;
                    subErrors.push_back("\tSyntax error at line " + std::to_string(code[position].line) + ": Expected logical or arithmatical expression" + "\n");
                }
            }
            if(!match(LexemType::RPAREN)) {
                result = false;
                subErrors.push_back("\tSyntax error at line " + std::to_string(code[position-1].line) + ": Expected ')' after expression" + "\n");
            }
        } else {
            result = or_expression();
        }
    }
    return result;
}

bool k_13::SyntaxAnalyzer2::or_expression() {
    // std::cout << "OR expression" << std::endl;
    // std::cout << "Position: " << position << "\t Line:" << std::to_string(code[position].line) << std::endl;
    // std::cout << code[position].value << std::endl;
    bool result = and_expression();
    while(code[position].type == LexemType::AND) {
        position++;
        // std::cout << "OR expression" << std::endl;
        // std::cout << "Position: " << position << "\t Line:" << std::to_string(code[position].line) << std::endl;
        // std::cout << code[position].value << std::endl;
        result = and_expression();
    }
    return result;
}

bool k_13::SyntaxAnalyzer2::and_expression() {
    // std::cout << "AND expression" << std::endl;
    // std::cout << "Position: " << position << "\t Line:" << std::to_string(code[position].line) << std::endl;
    // std::cout << code[position].value << std::endl;
    bool result = true;
    int current_position = position;
    switch (code[position].type) {
    case LexemType::LPAREN:
        position++;
        current_position = position;
        if(!logical_expression()) {
            position = current_position;
            if(!arithmetic_expression()) {
                result = false;
                subErrors.push_back("\tSyntax error at line " + std::to_string(code[position].line) + ": Expected logical or arithmatical expression" + "\n");
            }
        }
        
        if(!match(LexemType::RPAREN)) {
            result = false;
            subErrors.push_back("\tSyntax error at line " + std::to_string(code[position-1].line) + ": Expected ')' after expression" + "\n");
        }
        break;
    case LexemType::IDENTIFIER:
    case LexemType::NUMBER:
    case LexemType::TRUE:
    case LexemType::FALSE:
    case LexemType::STRING_LITERAL:
        position++;
        break;
    case LexemType::NOT:
        position++;
        if(!match(LexemType::LPAREN)) {
            result = false;
            subErrors.push_back("\tSyntax error at line " + std::to_string(code[position-1].line) + ": Expected '(' before expression" + "\n");
        }
        current_position = position;
        if(!logical_expression()) {
            position = current_position;
            if(!arithmetic_expression()) {
                result = false;
                subErrors.push_back("\tSyntax error at line " + std::to_string(code[position].line) + ": Expected logical or arithmatical expression" + "\n");
            }
        }
        if(!match(LexemType::RPAREN)) {
            result = false;
            subErrors.push_back("\tSyntax error at line " + std::to_string(code[position-1].line) + ": Expected ')' after expression" + "\n");
        }
    default:
        position++;
        result = false;
        subErrors.push_back("\tSyntax error at line " + std::to_string(code[position].line) + ": Expected factor" + "\n");
        break;
    }
    if(code[position].type == LexemType::EQUAL || code[position].type == LexemType::NEQUAL || code[position].type == LexemType::LESS || code[position].type == LexemType::GREATER) {
        position++;
        current_position = position;
        if(!logical_expression()) {
            position = current_position;
            if(!arithmetic_expression()) {
                result = false;
                subErrors.push_back("\tSyntax error at line " + std::to_string(code[position].line) + ": Expected logical or arithmatical expression" + "\n");
            }
        }
    }
    return result;
}
