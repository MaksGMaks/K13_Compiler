#include "SyntaxAnalyzer.hpp"

int k_13::SyntaxAnalyzer::analyze(const std::vector<Lexem> &lexems, const std::vector<UnknownLexem> &unknowns) {
    std::cout << "[INFO] Starting syntax analysis" << std::endl;
    code = lexems;
    unknownLexems = unknowns;
    position = 0;

    program();
    if(errorMessages.size() > 0) {
        for(auto message : errorMessages) {
            std::cerr << message.second.front() << std::endl;
        }
        
        return -1;
    }
    return 0;
}

bool k_13::SyntaxAnalyzer::match(const LexemType expectedType) {
    if(code[position].type == expectedType) {
        position++;
        return true;
    } 
    return false;
}

void k_13::SyntaxAnalyzer::program() {
    program_declaration();
    program_body();
}

void k_13::SyntaxAnalyzer::program_declaration() {
    if(!match(LexemType::PROGRAM)) {
        errorMessages[code[position].line].push_back("\tSyntax error at line " + std::to_string(code[position].line) + ": Expected 'program' keyword before program name");
    }
    if(!match(LexemType::IDENTIFIER)) {
        errorMessages[code[position].line].push_back("\tSyntax error at line " + std::to_string(code[position].line) + ": Expected program name after 'program' keyword");
    }
    programName = code[position-1].value;
    if(!match(LexemType::SEMICOLON)) {
        (position < code.size()) 
        ? errorMessages[code[position-1].line].push_back("\tSyntax error at line " + std::to_string(code[position-1].line) + ": Missing ';' before statement " + code[position].value)
        : errorMessages[code[position-1].line].push_back("\tSyntax error at line " + std::to_string(code[position-1].line) + ": Missing ';' after statement " + code[position - 1].value);
    }
}

void k_13::SyntaxAnalyzer::compound_statement() {
    if(!match(LexemType::START)) {
        errorMessages[code[position].line].push_back("\tSyntax error at line " + std::to_string(code[position].line) + ": Expected 'start' keyword before compound statement");
    }
    for(auto identifier : identifiers) {
        identifiers[identifier.first].push_back(std::make_pair(code[position-1].line, ExpressionType::START));
    }
    for(auto label : labels) {
        labels[label.first].push_back(std::make_pair(code[position-1].line, ExpressionType::START));
    }
    variable_declaration();
    program_body();
    if(!match(LexemType::FINISH)) {
        errorMessages[code[position-1].line].push_back("\tSyntax error at line " + std::to_string(code[position-1].line) + ": Expected 'finish' keyword after compound statement");
    }
    for(auto identifier : identifiers) {
        identifiers[identifier.first].push_back(std::make_pair(code[position-1].line, ExpressionType::FINISH));
    }
    for(auto label : labels) {
        labels[label.first].push_back(std::make_pair(code[position-1].line, ExpressionType::FINISH));
    }
}

void k_13::SyntaxAnalyzer::program_body() {
    do {
        statement();
        if(position >= code.size()) {
            break;
        }
    }
    while(code[position].type != LexemType::FINISH && position < code.size());
}

void k_13::SyntaxAnalyzer::statement() {
    switch(code[position].type) {
        case LexemType::START:
            compound_statement();
            break;
        case LexemType::GET:
            get_expression();
            if(!match(LexemType::SEMICOLON)) {
                (position < code.size()) 
                ? errorMessages[code[position-1].line].push_back("\tSyntax error at line " + std::to_string(code[position-1].line) + ": Missing ';' before statement " + code[position].value)
                : errorMessages[code[position-1].line].push_back("\tSyntax error at line " + std::to_string(code[position-1].line) + ": Missing ';' after statement " + code[position - 1].value);
            }
            break;
        case LexemType::PUT:
            put_expression();
            if(!match(LexemType::SEMICOLON)) {
                (position < code.size()) 
                ? errorMessages[code[position-1].line].push_back("\tSyntax error at line " + std::to_string(code[position-1].line) + ": Missing ';' before statement " + code[position].value)
                : errorMessages[code[position-1].line].push_back("\tSyntax error at line " + std::to_string(code[position-1].line) + ": Missing ';' after statement " + code[position - 1].value);
            }
            break;
        case LexemType::IF:
            if_expression();
            break;
        case LexemType::GOTO:
            goto_expression();
            if(!match(LexemType::SEMICOLON)) {
                (position < code.size()) 
                ? errorMessages[code[position-1].line].push_back("\tSyntax error at line " + std::to_string(code[position-1].line) + ": Missing ';' before statement " + code[position].value)
                : errorMessages[code[position-1].line].push_back("\tSyntax error at line " + std::to_string(code[position-1].line) + ": Missing ';' after statement " + code[position - 1].value);
            }
            break;
        case LexemType::FOR:
            for_expression();
            if(!match(LexemType::SEMICOLON)) {
                (position < code.size()) 
                ? errorMessages[code[position-1].line].push_back("\tSyntax error at line " + std::to_string(code[position-1].line) + ": Missing ';' before statement " + code[position].value)
                : errorMessages[code[position-1].line].push_back("\tSyntax error at line " + std::to_string(code[position-1].line) + ": Missing ';' after statement " + code[position - 1].value);
            }
            break;
        case LexemType::IDENTIFIER:
            if(code[position+1].type == LexemType::ASSIGN) {
                assign_expression();
            } else {
                end_goto_expression();
            }
            if(!match(LexemType::SEMICOLON)) {
                (position < code.size()) 
                ? errorMessages[code[position-1].line].push_back("\tSyntax error at line " + std::to_string(code[position-1].line) + ": Missing ';' before statement " + code[position].value)
                : errorMessages[code[position-1].line].push_back("\tSyntax error at line " + std::to_string(code[position-1].line) + ": Missing ';' after statement " + code[position - 1].value);
            }
            break;
        case LexemType::FINISH:
            break;
        case LexemType::UNKNOWN:
            errorMessages[code[position].line].push_back("\tSyntax error at line " + std::to_string(code[position].line) + ": Unknown statement " + unknownLexems[std::stoi(code[position].value) - 1].value);
            position++;
            break;
        default:
            errorMessages[code[position].line].push_back("\tSyntax error at line " + std::to_string(code[position].line) + ": Unknown statement " + code[position].value);
            position++;
            break;
    }
}

void k_13::SyntaxAnalyzer::variable_declaration() {
    if(!match(LexemType::VAR)) {
        errorMessages[code[position].line].push_back("\tSyntax error at line " + std::to_string(code[position].line) + ": Expected 'var' keyword before variable segment");
    }
    if(code[position].type == LexemType::INT || code[position].type == LexemType::BOOL || code[position].type == LexemType::STRING)
        variable_list();

    if(!match(LexemType::SEMICOLON)) {
        (position < code.size()) 
        ? errorMessages[code[position-1].line].push_back("\tSyntax error at line " + std::to_string(code[position-1].line) + ": Missing ';' before statement " + code[position].value)
        : errorMessages[code[position-1].line].push_back("\tSyntax error at line " + std::to_string(code[position-1].line) + ": Missing ';' after statement " + code[position - 1].value);
    }
}

void k_13::SyntaxAnalyzer::variable_list() {
    LexemType type = code[position].type;
    position++;
    if(!match(LexemType::IDENTIFIER)) {
        errorMessages[code[position].line].push_back("\tSyntax error at line " + std::to_string(code[position].line) + ": Expected identifier after variable type");
    }
    variableTable[code[position-1].value] = type;
    identifiers[code[position-1].value].push_back(std::make_pair(code[position-1].line, ExpressionType::VARIABLE));
    while(code[position].type == LexemType::COMMA) {
        position++;
        if(code[position].type == LexemType::IDENTIFIER) {
            identifiers[code[position].value].push_back(std::make_pair(code[position-1].line, ExpressionType::VARIABLE));
            position++;
        } else {
            if(!(match(LexemType::INT) || match(LexemType::BOOL) || match(LexemType::STRING))) {
                errorMessages[code[position].line].push_back("\tSyntax error at line " + std::to_string(code[position].line) + ": Expected variable type before identifier");
            }
            type = code[position-1].type;
            if(!match(LexemType::IDENTIFIER)) {
                errorMessages[code[position].line].push_back("\tSyntax error at line " + std::to_string(code[position].line) + ": Expected identifier after variable type");
            }
            variableTable[code[position-1].value] = type;
            identifiers[code[position-1].value].push_back(std::make_pair(code[position-1].line, ExpressionType::VARIABLE));
        }
    }
}

void k_13::SyntaxAnalyzer::get_expression() {
    position++;
    if(!match(LexemType::LPAREN)) {
        errorMessages[code[position].line].push_back("\tSyntax error at line " + std::to_string(code[position].line) + ": Expected '(' before identifier");
    }
    if(!match(LexemType::IDENTIFIER)) {
        errorMessages[code[position].line].push_back("\tSyntax error at line " + std::to_string(code[position].line) + ": Expected identifier after 'get' statement");
    }
    identifiers[code[position-1].value].push_back(std::make_pair(code[position-1].line, ExpressionType::INPUT));
    if(!match(LexemType::RPAREN)) {
        errorMessages[code[position].line].push_back("\tSyntax error at line " + std::to_string(code[position].line) + ": Expected ')' after identifier");
    }
}

void k_13::SyntaxAnalyzer::put_expression() {
    position++;
    if(!match(LexemType::LPAREN)) {
        errorMessages[code[position].line].push_back("\tSyntax error at line " + std::to_string(code[position].line) + ": Expected '(' before identifier");
    }
    expression.clear();
    subErrors.clear();
    int current_position = position;
    if(!logical_expression()) {
        expression.clear();
        int subErrorsSize = subErrors.size();
        position = current_position;
        if(!string_expression()) {
            std::string error = "\tSyntax error at line " + std::to_string(code[position].line) + ": Expected identifier or expression after 'put' statement"
                                "\n\tPossible problem for string:\n";
            for(int i = 0; i < subErrorsSize; i++) {
                error += "\t" + subErrors[i] + "\n";
            }
            error += "\tPossible problem for numerical:\n";
            for(int i = subErrorsSize; i < subErrors.size(); i++) {
                error += "\t" + subErrors[i] + "\n";
            }
            errorMessages[code[position].line].push_back(error);
        } else {
            expressions.push_back(std::make_pair(LexemType::STRING, expression));
        }
    } else {
        expressions.push_back(std::make_pair(LexemType::BOOL, expression));
    }
    if(!match(LexemType::RPAREN)) {
        errorMessages[code[position].line].push_back("\tSyntax error at line " + std::to_string(code[position].line) + ": Expected ')' after expression in 'put' statement");
    }
}

void k_13::SyntaxAnalyzer::assign_expression() {
    identifiers[code[position].value].push_back(std::make_pair(code[position-1].line, ExpressionType::ASSIGNMENT));
    position++;
    if(!match(LexemType::ASSIGN)) {
        errorMessages[code[position].line].push_back("\tSyntax error at line " + std::to_string(code[position].line) + ": Expected ':=' after identifier");
    }
    expression.clear();
    int current_position = position;
    subErrors.clear();
    if(!logical_expression()) {
        expression.clear();
        int subErrorsSize = subErrors.size();
        position = current_position;
        if(!string_expression()) {
            std::string error = "\tSyntax error at line " + std::to_string(code[position].line) + ": Expected expression to assign"
                                "\n\tPossible problem for numerical:\n";
            for(int i = 0; i < subErrorsSize; i++) {
                error += "\t" + subErrors[i] + "\n";
            }
            error += "\tPossible problem for string:\n";
            for(int i = subErrorsSize; i < subErrors.size(); i++) {
                error += "\t" + subErrors[i] + "\n";
            }
            errorMessages[code[position].line].push_back(error);
        } else {
            expressions.push_back(std::make_pair(LexemType::STRING, expression));
        }
    } else {
        expressions.push_back(std::make_pair(LexemType::BOOL, expression));
    }
}

void k_13::SyntaxAnalyzer::goto_expression() {
    if(!match(LexemType::GOTO)) {
        errorMessages[code[position].line].push_back("\tSyntax error at line " + std::to_string(code[position].line) + ": Expected 'goto' keyword before identifier");
    }
    if(!match(LexemType::IDENTIFIER)) {
        errorMessages[code[position].line].push_back("\tSyntax error at line " + std::to_string(code[position].line) + ": Expected identifier after 'goto' statement");
    }
    labels[code[position-1].value].push_back(std::make_pair(code[position-1].line, ExpressionType::GOTO));
}

void k_13::SyntaxAnalyzer::end_goto_expression() {
    if(!match(LexemType::IDENTIFIER)) {
        errorMessages[code[position].line].push_back("\tSyntax error at line " + std::to_string(code[position].line) + ": Expected identifier after 'end' statement");
    }
    labels[code[position-1].value].push_back(std::make_pair(code[position-1].line, ExpressionType::LABEL));
}

void k_13::SyntaxAnalyzer::if_expression() {
    position++;
    if(!match(LexemType::LPAREN)) {
        errorMessages[code[position].line].push_back("\tSyntax error at line " + std::to_string(code[position].line) + ": Expected '(' before condition expression");
    }
    int current_position = position;
    expression.clear();
    subErrors.clear();
    if(!logical_expression()) {
        errorMessages[code[position].line].push_back("\tSyntax error at line " + std::to_string(code[position].line) + ": Expected logical expression after '(' statement");
    } else {
        expressions.push_back(std::make_pair(LexemType::BOOL, expression));
    }
    if(!match(LexemType::RPAREN)) {
        errorMessages[code[position].line].push_back("\tSyntax error at line " + std::to_string(code[position].line) + ": Expected ')' after condition expression");
    }
    goto_expression();
    if(!match(LexemType::SEMICOLON)) {
        (position < code.size()) 
        ? errorMessages[code[position-1].line].push_back("\tSyntax error at line " + std::to_string(code[position-1].line) + ": Missing ';' before statement " + code[position].value)
        : errorMessages[code[position-1].line].push_back("\tSyntax error at line " + std::to_string(code[position-1].line) + ": Missing ';' after statement " + code[position - 1].value);
    }
    std::string ifErrors = "";
    int errorLine = code[position].line;
    std::string startLine = std::to_string(code[position-1].line);
    if(code[position].type != LexemType::START) {
        ifErrors += "\tSyntax error at line " + startLine + ": Unknown statements before 'start' keyword: ";
    }
    while (code[position].type != LexemType::START && position < code.size() - 1) {
        ifErrors += code[position].value + " "; 
        position++;
    }
    if(position == code.size() - 1) {
        errorMessages[code[position].line].push_back("\tSyntax error at line " + startLine + ": Expected 'start' keyword after 'if' statement");
        return;
    }
    if(ifErrors != "") {
        ifErrors += "\n";
        errorMessages[errorLine].push_back(ifErrors);
    }
    compound_statement();
    goto_expression();
    if(!match(LexemType::SEMICOLON)) {
        (position < code.size()) 
        ? errorMessages[code[position-1].line].push_back("\tSyntax error at line " + std::to_string(code[position-1].line) + ": Missing ';' before statement " + code[position].value)
        : errorMessages[code[position-1].line].push_back("\tSyntax error at line " + std::to_string(code[position-1].line) + ": Missing ';' after statement " + code[position - 1].value);
    }
    end_goto_expression();
    if(!match(LexemType::SEMICOLON)) {
        (position < code.size()) 
        ? errorMessages[code[position-1].line].push_back("\tSyntax error at line " + std::to_string(code[position-1].line) + ": Missing ';' before statement " + code[position].value)
        : errorMessages[code[position-1].line].push_back("\tSyntax error at line " + std::to_string(code[position-1].line) + ": Missing ';' after statement " + code[position - 1].value);
    }
}
    
void k_13::SyntaxAnalyzer::for_expression() {
    position++;
    if(!match(LexemType::IDENTIFIER)) {
        errorMessages[code[position].line].push_back("\tSyntax error at line " + std::to_string(code[position].line) + ": Expected identifier after 'for' statement");
    }
    std::string forIdentifier = code[position-1].value;
    identifiers[forIdentifier].push_back(std::make_pair(code[position-1].line, ExpressionType::STARTFOR));
    if(!match(LexemType::ASSIGN)) {
        errorMessages[code[position].line].push_back("\tSyntax error at line " + std::to_string(code[position].line) + ": Expected ':=' after identifier");
    }
    expression.clear();
    subErrors.clear();
    if(!arithmetic_expression()) {
        errorMessages[code[position].line].push_back("\tSyntax error at line " + std::to_string(code[position].line) + ": Expected arithmetic expression after ':=' statement");
    } else {
        expressions.push_back(std::make_pair(LexemType::NUMBER, expression));
    }
    if(!match(LexemType::TO)) {
        errorMessages[code[position].line].push_back("\tSyntax error at line " + std::to_string(code[position].line) + ": Expected 'to' keyword after identifier");
    }
    expression.clear();
    subErrors.clear();
    if(!arithmetic_expression()) {
        errorMessages[code[position].line].push_back("\tSyntax error at line " + std::to_string(code[position].line) + ": Expected arithmetic expression after 'to' statement");
    } else {
        expressions.push_back(std::make_pair(LexemType::NUMBER, expression));
    }
    do {
        statement();
        if(position >= code.size()) {
            break;
        }
    }
    while(code[position].type != LexemType::NEXT && position < code.size());
    if(!match(LexemType::NEXT)) {
        errorMessages[code[position].line].push_back("\tSyntax error at line " + std::to_string(code[position].line) + ": Expected 'next' keyword after condition expression");
    }
    if(!match(LexemType::IDENTIFIER)) {
        errorMessages[code[position].line].push_back("\tSyntax error at line " + std::to_string(code[position].line) + ": Expected identifier after 'next' statement");
    } else if(code[position-1].value != forIdentifier) {
        errorMessages[code[position].line].push_back("\tSyntax error at line " + std::to_string(code[position].line) + ": Expected identifier " + forIdentifier + " after 'next' statement");
    }
    identifiers[forIdentifier].push_back(std::make_pair(code[position-1].line, ExpressionType::ENDFOR));

}

bool k_13::SyntaxAnalyzer::arithmetic_expression() {
    bool result = term();
    while(code[position].type == LexemType::ADD || code[position].type == LexemType::SUB) {
        expression.push_back(code[position]);
        position++;
        result &= term();
    }
    return result;
}

bool k_13::SyntaxAnalyzer::term() {
    bool result = factor();
    while(code[position].type == LexemType::MUL || code[position].type == LexemType::DIV || code[position].type == LexemType::MOD) {
        expression.push_back(code[position]);
        position++;
        result &= factor();
    }
    return result;
}

bool k_13::SyntaxAnalyzer::factor() {
    bool result = true;
    switch(code[position].type) {
        case LexemType::IDENTIFIER:
            identifiers[code[position].value].push_back(std::make_pair(code[position-1].line, ExpressionType::EXPRESSION));
        case LexemType::NUMBER:
        case LexemType::TRUE:
        case LexemType::FALSE:
            expression.push_back(code[position]);
            position++;
            break;
        case LexemType::STRING_LITERAL:
            result = false;
            subErrors.push_back("\tSyntax error at line " + std::to_string(code[position].line) + ": Unexpected string literal " + code[position].value);
            position++;
            break;
        case LexemType::UNKNOWN:
            result = false;
            subErrors.push_back("\tSyntax error at line " + std::to_string(code[position].line) + ": Unknown statement " + unknownLexems[std::stoi(code[position].value) - 1].value);
            position++;
            break;
        case LexemType::LPAREN:
            expression.push_back(code[position]);
            position++;    
            if(!logical_expression()) {
                result = false;
                subErrors.push_back("\tSyntax error at line " + std::to_string(code[position].line) + ": Expected expression");
            }
            if(!match(LexemType::RPAREN)) {
                result = false;
                subErrors.push_back("\tSyntax error at line " + std::to_string(code[position].line) + ": Expected ')' after expression");
                errors++;
            } else {
                expression.push_back(code[position-1]);
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

bool k_13::SyntaxAnalyzer::logical_expression() {
    bool result = or_expression();
    while(code[position].type == LexemType::OR) {
        expression.push_back(code[position]);
        position++;
        result &= or_expression();
    }
    return result;
}

bool k_13::SyntaxAnalyzer::or_expression() {
    bool result = and_expression();
    while(code[position].type == LexemType::AND) {
        expression.push_back(code[position]);
        position++;
        result &= and_expression();
    }
    return result;
}

bool k_13::SyntaxAnalyzer::and_expression() {
    bool result = equality_expression();
    while(code[position].type == LexemType::EQUAL || code[position].type == LexemType::NEQUAL) {
        expression.push_back(code[position]);
        position++;
        result &= equality_expression();
    }
    return result;
}

bool k_13::SyntaxAnalyzer::equality_expression() {
    bool result = relational_expression();
    while(code[position].type == LexemType::EQUAL || code[position].type == LexemType::NEQUAL) {
        expression.push_back(code[position]);
        position++;
        result &= relational_expression();
    }
    return result;
}

bool k_13::SyntaxAnalyzer::relational_expression() {
    bool result = true;
    int current_position = position;
    int expressionSize = expression.size();
    if(!arithmetic_expression()) {
        expression.erase(expression.begin() + expressionSize, expression.end());
        int subErrorsSize = subErrors.size();
        position = current_position;
        if(!string_expression()) {
            std::string error = "\tSyntax error at line " + std::to_string(code[position].line) + ": Expected expression"
                                "\n\tPossible problem for numerical:\n";
            for(int i = 0; i < subErrorsSize; i++) {
                error += "\t" + subErrors[i] + "\n";
            }
            error += "\tPossible problem for string:\n";
            for(int i = subErrorsSize; i < subErrors.size(); i++) {
                error += "\t" + subErrors[i] + "\n";
            }
            errorMessages[code[position].line].push_back(error);
            result = false;
        }
    }
    
    while(code[position].type == LexemType::LESS || code[position].type == LexemType::GREATER) {
        expression.push_back(code[position]);
        position++;
        int current_position = position;
        int expressionSize = expression.size();
        if(!arithmetic_expression()) {
            expression.erase(expression.begin() + expressionSize, expression.end());
            int subErrorsSize = subErrors.size();
            position = current_position;
            if(!string_expression()) {
                std::string error = "\tSyntax error at line " + std::to_string(code[position].line) + ": Expected expression"
                                    "\n\tPossible problem for numerical:\n";
                for(int i = 0; i < subErrorsSize; i++) {
                    error += "\t" + subErrors[i] + "\n";
                }
                error += "\tPossible problem for string:\n";
                for(int i = subErrorsSize; i < subErrors.size(); i++) {
                    error += "\t" + subErrors[i] + "\n";
                }
                errorMessages[code[position].line].push_back(error);
                result = false;
            }
        }
    }
    return result;
}

bool k_13::SyntaxAnalyzer::string_expression() {
    bool result = string_factor();
    while(code[position].type == LexemType::ADD) {
        expression.push_back(code[position]);
        position++;
        result &= string_factor();
    }
    return result;
}

bool k_13::SyntaxAnalyzer::string_factor() {
    bool result = true;
    switch (code[position].type) {
    case LexemType::IDENTIFIER:
        identifiers[code[position].value].push_back(std::make_pair(code[position-1].line, ExpressionType::EXPRESSION));
    case LexemType::STRING_LITERAL:
    case LexemType::NUMBER:
    case LexemType::TRUE:
    case LexemType::FALSE:
        expression.push_back(code[position]);
        position++;
        break;
    case LexemType::LPAREN:
        expression.push_back(code[position]);
        position++;
        if(!logical_expression()) {
            result = false;
            subErrors.push_back("\tSyntax error at line " + std::to_string(code[position].line) + ": Expected string expression");
        }
        if(!match(LexemType::RPAREN)) {
            result = false;
            subErrors.push_back("\tSyntax error at line " + std::to_string(code[position].line) + ": Expected ')' after string expression");
            errors++;
        } else {
            expression.push_back(code[position-1]);
        }
        break;
    case LexemType::UNKNOWN:
        result = false;
        subErrors.push_back("\tSyntax error at line " + std::to_string(code[position].line) + ": Unknown statement " + unknownLexems[std::stoi(code[position].value) - 1].value);
        position++;
        break;
    default:
        position++;
        result = false;
        subErrors.push_back("\tSyntax error at line " + std::to_string(code[position].line) + ": Expected factor for string");
        errors++;
        break;
    }
    return result;
}