#include "SyntaxAnalyzer.hpp"

k_13::SyntaxAnalyzer::SyntaxAnalyzer() {
    code.clear();
    errorMessages.clear();
}

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
    variable_declaration();
    program_body();
    if(!match(LexemType::FINISH)) {
        errorMessages[code[position-1].line].push_back("\tSyntax error at line " + std::to_string(code[position-1].line) + ": Expected 'finish' keyword after compound statement");
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
    position++;
    if(!match(LexemType::IDENTIFIER)) {
        errorMessages[code[position].line].push_back("\tSyntax error at line " + std::to_string(code[position].line) + ": Expected identifier after variable type");
    }
    while(code[position].type == LexemType::COMMA) {
        position++;
        if(code[position].type == LexemType::IDENTIFIER) {
            position++;
        } else {
            if(!(match(LexemType::INT) || match(LexemType::BOOL) || match(LexemType::STRING))) {
                errorMessages[code[position].line].push_back("\tSyntax error at line " + std::to_string(code[position].line) + ": Expected variable type before identifier");
            }
            if(!match(LexemType::IDENTIFIER)) {
                errorMessages[code[position].line].push_back("\tSyntax error at line " + std::to_string(code[position].line) + ": Expected identifier after variable type");
            }
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
    if(!match(LexemType::RPAREN)) {
        errorMessages[code[position].line].push_back("\tSyntax error at line " + std::to_string(code[position].line) + ": Expected ')' after identifier");
    }
}

void k_13::SyntaxAnalyzer::put_expression() {
    position++;
    if(!match(LexemType::LPAREN)) {
        errorMessages[code[position].line].push_back("\tSyntax error at line " + std::to_string(code[position].line) + ": Expected '(' before identifier");
    }
    if(!match(LexemType::IDENTIFIER)) {
        if(!logical_expression()) {
            errorMessages[code[position].line].push_back("\tSyntax error at line " + std::to_string(code[position].line) + ": Expected identifier or expression after 'put' statement");
        }
    }
    if(!match(LexemType::RPAREN)) {
        errorMessages[code[position].line].push_back("\tSyntax error at line " + std::to_string(code[position].line) + ": Expected ')' after expression");
    }
}

void k_13::SyntaxAnalyzer::assign_expression() {
    position++;
    if(!match(LexemType::ASSIGN)) {
        errorMessages[code[position].line].push_back("\tSyntax error at line " + std::to_string(code[position].line) + ": Expected ':=' after identifier");
    }
    int current_position = position;
    subErrors.clear();
    if(!logical_expression()) {
        errorMessages[code[position].line].push_back("\tSyntax error at line " + std::to_string(code[position].line) + ": Expected expression after ':=' statement");
    }
}

void k_13::SyntaxAnalyzer::goto_expression() {
    if(!match(LexemType::GOTO)) {
        errorMessages[code[position].line].push_back("\tSyntax error at line " + std::to_string(code[position].line) + ": Expected 'goto' keyword before identifier");
    }
    if(!match(LexemType::IDENTIFIER)) {
        errorMessages[code[position].line].push_back("\tSyntax error at line " + std::to_string(code[position].line) + ": Expected identifier after 'goto' statement");
    }
}

void k_13::SyntaxAnalyzer::end_goto_expression() {
    if(!match(LexemType::IDENTIFIER)) {
        errorMessages[code[position].line].push_back("\tSyntax error at line " + std::to_string(code[position].line) + ": Expected identifier after 'end' statement");
    }
}

void k_13::SyntaxAnalyzer::if_expression() {
    position++;
    if(!match(LexemType::LPAREN)) {
        errorMessages[code[position].line].push_back("\tSyntax error at line " + std::to_string(code[position].line) + ": Expected '(' before condition expression");
    }
    int current_position = position;
    subErrors.clear();
    if(!logical_expression()) {
        errorMessages[code[position].line].push_back("\tSyntax error at line " + std::to_string(code[position].line) + ": Expected logical expression after '(' statement");
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
    if(!match(LexemType::ASSIGN)) {
        errorMessages[code[position].line].push_back("\tSyntax error at line " + std::to_string(code[position].line) + ": Expected ':=' after identifier");
    }
    if(!arithmetic_expression()) {
        errorMessages[code[position].line].push_back("\tSyntax error at line " + std::to_string(code[position].line) + ": Expected arithmetic expression after ':=' statement");
    }
    if(!match(LexemType::TO)) {
        errorMessages[code[position].line].push_back("\tSyntax error at line " + std::to_string(code[position].line) + ": Expected 'to' keyword after identifier");
    }
    if(!arithmetic_expression()) {
        errorMessages[code[position].line].push_back("\tSyntax error at line " + std::to_string(code[position].line) + ": Expected arithmetic expression after 'to' statement");
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
    }
}

bool k_13::SyntaxAnalyzer::arithmetic_expression() {
    bool result = term();
    while(code[position].type == LexemType::ADD || code[position].type == LexemType::SUB) {
        position++;
        result = term();
    }
    return result;
}

bool k_13::SyntaxAnalyzer::term() {
    bool result = factor();
    while(code[position].type == LexemType::MUL || code[position].type == LexemType::DIV || code[position].type == LexemType::MOD) {
        position++;
        result = factor();
    }
    return result;
}

bool k_13::SyntaxAnalyzer::factor() {
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
            if(!logical_expression()) {
                result = false;
                subErrors.push_back("\tSyntax error at line " + std::to_string(code[position].line) + ": Expected expression");
            }
            if(!match(LexemType::RPAREN)) {
                result = false;
                subErrors.push_back("\tSyntax error at line " + std::to_string(code[position].line) + ": Expected ')' after expression");
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

bool k_13::SyntaxAnalyzer::logical_expression() {
    int current_position = position;
    bool result = or_expression();

    while(code[position].type == LexemType::OR) {
        position++;
        result = or_expression();
    }
    return result;
}

bool k_13::SyntaxAnalyzer::or_expression() {
    bool result = and_expression();
    while(code[position].type == LexemType::AND) {
        position++;
        result = and_expression();
    }
    return result;
}

bool k_13::SyntaxAnalyzer::and_expression() {
    bool result = equality_expression();
    while(code[position].type == LexemType::EQUAL || code[position].type == LexemType::NEQUAL) {
        position++;
        result = equality_expression();
    }
    return result;
}

bool k_13::SyntaxAnalyzer::equality_expression() {
    bool result = relational_expression();
    while(code[position].type == LexemType::EQUAL || code[position].type == LexemType::NEQUAL) {
        position++;
        result = relational_expression();
    }
    return result;
}

bool k_13::SyntaxAnalyzer::relational_expression() {
    bool result = arithmetic_expression();
    while(code[position].type == LexemType::LESS || code[position].type == LexemType::GREATER) {
        position++;
        result = arithmetic_expression();
    }
    return result;
}