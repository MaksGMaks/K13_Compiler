#include "SyntaxAnalyzer.hpp"

int k_13::SyntaxAnalyzer::analyze(const std::vector<Lexem> &lexems, const std::vector<UnknownLexem> &unknowns) {
    std::cout << "[INFO] Starting syntax analysis" << std::endl;
    code = lexems;
    unknownLexems = unknowns;
    position = 0;

    program();
    if(!errorMessages.empty()) {
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
    keywords = program_body();
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

k_13::Keyword k_13::SyntaxAnalyzer::compound_statement() {
    Keyword compaund;
    if(!match(LexemType::START)) {
        errorMessages[code[position].line].push_back("\tSyntax error at line " + std::to_string(code[position].line) + ": Expected 'start' keyword before compound statement");
    }
    compaund.keyword = LexemType::START;
    for(auto identifier : identifiers) {
        identifiers[identifier.first].push_back(std::make_pair(code[position-1].line, ExpressionType::START));
    }
    for(auto label : labels) {
        labels[label.first].push_back(std::make_pair(code[position-1].line, ExpressionType::START));
    }
    compaund.variables = variable_declaration();
    compaund.keywords = program_body();
    if(!match(LexemType::FINISH)) {
        errorMessages[code[position-1].line].push_back("\tSyntax error at line " + std::to_string(code[position-1].line) + ": Expected 'finish' keyword after compound statement");
    }
    for(auto identifier : identifiers) {
        identifiers[identifier.first].push_back(std::make_pair(code[position-1].line, ExpressionType::FINISH));
    }
    for(auto label : labels) {
        labels[label.first].push_back(std::make_pair(code[position-1].line, ExpressionType::FINISH));
    }
    return compaund;
}

std::vector<k_13::Keyword> k_13::SyntaxAnalyzer::program_body() {
    std::vector<Keyword> keywords_;
    do {
        keywords_.push_back(statement());
        if(position >= code.size()) {
            break;
        }
    }
    while(code[position].type != LexemType::FINISH && position < code.size());
    return keywords_;
}

k_13::Keyword k_13::SyntaxAnalyzer::statement() {
    Keyword statement_key;
    switch(code[position].type) {
        case LexemType::START:
            statement_key = compound_statement();
            break;
        case LexemType::GET:
            statement_key = get_expression();
            if(!match(LexemType::SEMICOLON)) {
                (position < code.size()) 
                ? errorMessages[code[position-1].line].push_back("\tSyntax error at line " + std::to_string(code[position-1].line) + ": Missing ';' before statement " + code[position].value)
                : errorMessages[code[position-1].line].push_back("\tSyntax error at line " + std::to_string(code[position-1].line) + ": Missing ';' after statement " + code[position - 1].value);
            }
            break;
        case LexemType::PUT:
            statement_key = put_expression();
            if(!match(LexemType::SEMICOLON)) {
                (position < code.size()) 
                ? errorMessages[code[position-1].line].push_back("\tSyntax error at line " + std::to_string(code[position-1].line) + ": Missing ';' before statement " + code[position].value)
                : errorMessages[code[position-1].line].push_back("\tSyntax error at line " + std::to_string(code[position-1].line) + ": Missing ';' after statement " + code[position - 1].value);
            }
            break;
        case LexemType::IF:
            statement_key = if_expression();
            break;
        case LexemType::GOTO:
            statement_key = goto_expression();
            if(!match(LexemType::SEMICOLON)) {
                (position < code.size()) 
                ? errorMessages[code[position-1].line].push_back("\tSyntax error at line " + std::to_string(code[position-1].line) + ": Missing ';' before statement " + code[position].value)
                : errorMessages[code[position-1].line].push_back("\tSyntax error at line " + std::to_string(code[position-1].line) + ": Missing ';' after statement " + code[position - 1].value);
            }
            break;
        case LexemType::FOR:
            statement_key = for_expression();
            if(!match(LexemType::SEMICOLON)) {
                (position < code.size()) 
                ? errorMessages[code[position-1].line].push_back("\tSyntax error at line " + std::to_string(code[position-1].line) + ": Missing ';' before statement " + code[position].value)
                : errorMessages[code[position-1].line].push_back("\tSyntax error at line " + std::to_string(code[position-1].line) + ": Missing ';' after statement " + code[position - 1].value);
            }
            break;
        case LexemType::IDENTIFIER:
            if(code[position+1].type == LexemType::ASSIGN) {
                statement_key = assign_expression();
            } else {
                statement_key = end_goto_expression();
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
    return statement_key;
}

std::map<std::string, k_13::LexemType> k_13::SyntaxAnalyzer::variable_declaration() {
    std::map<std::string, LexemType> declaredVariables{};
    if(!match(LexemType::VAR)) {
        errorMessages[code[position].line].push_back("\tSyntax error at line " + std::to_string(code[position].line) + ": Expected 'var' keyword before variable segment");
    }
    if(code[position].type == LexemType::INT || code[position].type == LexemType::BOOL || code[position].type == LexemType::STRING)
        declaredVariables = variable_list();

    if(!match(LexemType::SEMICOLON)) {
        (position < code.size()) 
        ? errorMessages[code[position-1].line].push_back("\tSyntax error at line " + std::to_string(code[position-1].line) + ": Missing ';' before statement " + code[position].value)
        : errorMessages[code[position-1].line].push_back("\tSyntax error at line " + std::to_string(code[position-1].line) + ": Missing ';' after statement " + code[position - 1].value);
    }
    return declaredVariables;
}

std::map<std::string, k_13::LexemType> k_13::SyntaxAnalyzer::variable_list() {
    std::map<std::string, LexemType> declaredVariables;
    LexemType type = code[position].type;
    position++;
    if(!match(LexemType::IDENTIFIER)) {
        errorMessages[code[position].line].push_back("\tSyntax error at line " + std::to_string(code[position].line) + ": Expected identifier after variable type");
    }
    variableTable[code[position-1].value] = type;
    declaredVariables[code[position-1].value] = type;
    identifiers[code[position-1].value].push_back(std::make_pair(code[position-1].line, ExpressionType::VARIABLE));
    while(code[position].type == LexemType::COMMA) {
        position++;
        if(code[position].type == LexemType::IDENTIFIER) {
            variableTable[code[position].value] = type;
            declaredVariables[code[position].value] = type;
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
            declaredVariables[code[position-1].value] = type;
            identifiers[code[position-1].value].push_back(std::make_pair(code[position-1].line, ExpressionType::VARIABLE));
        }
    }
    return declaredVariables;
}

k_13::Keyword k_13::SyntaxAnalyzer::get_expression() {
    Keyword statm;
    statm.keyword = LexemType::GET;
    position++;
    if(!match(LexemType::LPAREN)) {
        errorMessages[code[position].line].push_back("\tSyntax error at line " + std::to_string(code[position].line) + ": Expected '(' before identifier");
    }
    if(!match(LexemType::IDENTIFIER)) {
        errorMessages[code[position].line].push_back("\tSyntax error at line " + std::to_string(code[position].line) + ": Expected identifier after 'get' statement");
    }
    statm.label = code[position-1].value;
    identifiers[code[position-1].value].push_back(std::make_pair(code[position-1].line, ExpressionType::INPUT));
    if(!match(LexemType::RPAREN)) {
        errorMessages[code[position].line].push_back("\tSyntax error at line " + std::to_string(code[position].line) + ": Expected ')' after identifier");
    }
    return statm;
}

k_13::Keyword k_13::SyntaxAnalyzer::put_expression() {
    Keyword statm;
    statm.keyword = LexemType::PUT;
    position++;
    if(!match(LexemType::LPAREN)) {
        errorMessages[code[position].line].push_back("\tSyntax error at line " + std::to_string(code[position].line) + ": Expected '(' before identifier");
    }
    expression.clear();
    subErrors.clear();
    if(!string_expression()) {
        errorMessages[code[position].line].push_back("\tSyntax error at line " + std::to_string(code[position].line) + ": Expected string expression after '(' statement");
    } else {
        expressions.push_back(std::make_pair(LexemType::STRING, expression));
        statm.expression1 = expression;
    }
    if(!match(LexemType::RPAREN)) {
        errorMessages[code[position].line].push_back("\tSyntax error at line " + std::to_string(code[position].line) + ": Expected ')' after expression in 'put' statement");
    }
    return statm;
}

k_13::Keyword k_13::SyntaxAnalyzer::assign_expression() {
    Keyword statm;
    statm.keyword = LexemType::ASSIGN;
    statm.label = code[position].value;
    identifiers[code[position].value].push_back(std::make_pair(code[position-1].line, ExpressionType::ASSIGNMENT));
    std::string identifier = code[position].value;
    position++;
    if(!match(LexemType::ASSIGN)) {
        errorMessages[code[position].line].push_back("\tSyntax error at line " + std::to_string(code[position].line) + ": Expected ':=' after identifier");
    }
    expression.clear();
    subErrors.clear();
    if (variableTable[identifier] == LexemType::STRING) {
        if(!string_expression()) {
            errorMessages[code[position].line].push_back("\tSyntax error at line " + std::to_string(code[position].line) + ": Expected string expression after '(' statement");
        } else {
            expressions.push_back(std::make_pair(LexemType::STRING, expression));
            statm.expression1 = expression;
        }
    } else {
        if(!logical_expression()) {
            errorMessages[code[position].line].push_back("\tSyntax error at line " + std::to_string(code[position].line) + ": Expected logical expression after '(' statement");
        } else {
            expressions.push_back(std::make_pair(LexemType::BOOL, expression));
            statm.expression1 = expression;
        }
    }
    return statm;
}

k_13::Keyword k_13::SyntaxAnalyzer::goto_expression() {
    Keyword statm;
    statm.keyword = LexemType::GOTO;
    if(!match(LexemType::GOTO)) {
        errorMessages[code[position].line].push_back("\tSyntax error at line " + std::to_string(code[position].line) + ": Expected 'goto' keyword before identifier");
    }
    if(!match(LexemType::IDENTIFIER)) {
        errorMessages[code[position].line].push_back("\tSyntax error at line " + std::to_string(code[position].line) + ": Expected identifier after 'goto' statement");
    }
    statm.label = code[position-1].value;
    labels[code[position-1].value].push_back(std::make_pair(code[position-1].line, ExpressionType::GOTO));
    return statm;
}

k_13::Keyword k_13::SyntaxAnalyzer::end_goto_expression() {
    Keyword statm;
    statm.keyword = LexemType::LABEL;
    if(!match(LexemType::IDENTIFIER)) {
        errorMessages[code[position].line].push_back("\tSyntax error at line " + std::to_string(code[position].line) + ": Expected identifier after 'end' statement");
    }
    statm.label = code[position-1].value;
    labels[code[position-1].value].push_back(std::make_pair(code[position-1].line, ExpressionType::LABEL));
    return statm;
}

k_13::Keyword k_13::SyntaxAnalyzer::if_expression() {
    Keyword statm;
    statm.keyword = LexemType::IF;
    position++;
    if(!match(LexemType::LPAREN)) {
        errorMessages[code[position].line].push_back("\tSyntax error at line " + std::to_string(code[position].line) + ": Expected '(' before condition expression");
    }
    expression.clear();
    subErrors.clear();
    if(!logical_expression()) {
        errorMessages[code[position].line].push_back("\tSyntax error at line " + std::to_string(code[position].line) + ": Expected logical expression after '(' statement");
    } else {
        expressions.push_back(std::make_pair(LexemType::BOOL, expression));
        statm.expression1 = expression;
    }
    if(!match(LexemType::RPAREN)) {
        errorMessages[code[position].line].push_back("\tSyntax error at line " + std::to_string(code[position].line) + ": Expected ')' after condition expression");
    }
    Keyword gotoS = goto_expression();
    statm.label = gotoS.label;
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
        return {};
    }
    if(ifErrors != "") {
        ifErrors += "\n";
        errorMessages[errorLine].push_back(ifErrors);
    }
    statm.comp.push_back(compound_statement());
    Keyword gotoS2 = goto_expression();
    statm.label2 = gotoS2.label;
    if(!match(LexemType::SEMICOLON)) {
        (position < code.size()) 
        ? errorMessages[code[position-1].line].push_back("\tSyntax error at line " + std::to_string(code[position-1].line) + ": Missing ';' before statement " + code[position].value)
        : errorMessages[code[position-1].line].push_back("\tSyntax error at line " + std::to_string(code[position-1].line) + ": Missing ';' after statement " + code[position - 1].value);
    }
    Keyword gotoS3 = end_goto_expression();
    statm.label3 = gotoS3.label;
    if(!match(LexemType::SEMICOLON)) {
        (position < code.size()) 
        ? errorMessages[code[position-1].line].push_back("\tSyntax error at line " + std::to_string(code[position-1].line) + ": Missing ';' before statement " + code[position].value)
        : errorMessages[code[position-1].line].push_back("\tSyntax error at line " + std::to_string(code[position-1].line) + ": Missing ';' after statement " + code[position - 1].value);
    }
    return statm;
}
    
k_13::Keyword k_13::SyntaxAnalyzer::for_expression() {
    Keyword statm;
    statm.keyword = LexemType::FOR;
    position++;
    if(!match(LexemType::IDENTIFIER)) {
        errorMessages[code[position].line].push_back("\tSyntax error at line " + std::to_string(code[position].line) + ": Expected identifier after 'for' statement");
    }
    std::string forIdentifier = code[position-1].value;
    statm.label = forIdentifier;
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
        statm.expression1 = expression;
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
        statm.expression2 = expression;
    }
    do {
        statm.keywords.push_back(statement());
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
    return statm;
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
    bool result = logical_term();
    while(code[position].type == LexemType::OR) {
        expression.push_back(code[position]);
        position++;
        result &= logical_term();
    }
    return result;
}

bool k_13::SyntaxAnalyzer::logical_term() {
    bool result = equality_expression();
    while(code[position].type == LexemType::AND) {
        expression.push_back(code[position]);
        position++;
        result &= equality_expression();
    }
    return result;
}

bool k_13::SyntaxAnalyzer::equality_expression() {
    bool result = true;
    if (code[position].type == LexemType::NOT) {
        expression.push_back(code[position]);
        position++;
        if (!match(LexemType::LPAREN)) {
            errorMessages[code[position].line].push_back("\tSyntax error at line " + std::to_string(code[position].line) + ": Expected '(' after NOT operator");
        }
        expression.push_back(code[position-1]);
        result = logical_expression();
        if (!match(LexemType::RPAREN)) {
            errorMessages[code[position].line].push_back("\tSyntax error at line " + std::to_string(code[position].line) + ": Expected ')' after expression");
        }
        expression.push_back(code[position-1]);
    } else {
        result = relational_expression();
    }
    if(code[position].type == LexemType::EQUAL || code[position].type == LexemType::NEQUAL) {
        expression.push_back(code[position]);
        position++;
        if (code[position].type == LexemType::NOT) {
            expression.push_back(code[position]);
            position++;
            if (!match(LexemType::LPAREN)) {
                errorMessages[code[position].line].push_back("\tSyntax error at line " + std::to_string(code[position].line) + ": Expected '(' after NOT operator");
            }
            expression.push_back(code[position-1]);
            result &= logical_expression();
            if (!match(LexemType::RPAREN)) {
                errorMessages[code[position].line].push_back("\tSyntax error at line " + std::to_string(code[position].line) + ": Expected ')' after expression");
            }
            expression.push_back(code[position-1]);
        } else {
            result &= relational_expression();
        }
    }
    return result;
}

bool k_13::SyntaxAnalyzer::relational_expression() {
    bool result = true;
    if (code[position].type != LexemType::STRING_LITERAL) {
        if(!arithmetic_expression()) {
            errorMessages[code[position].line].push_back("\tSyntax error at line " + std::to_string(code[position].line) + ": Expected expression");
            result = false;
        }
    } else {
        expression.push_back(code[position]);
        position++;
    }

    if(code[position].type == LexemType::LESS || code[position].type == LexemType::GREATER) {
        expression.push_back(code[position]);
        position++;
        if (code[position].type != LexemType::STRING_LITERAL) {
            if(!arithmetic_expression()) {
                errorMessages[code[position].line].push_back("\tSyntax error at line " + std::to_string(code[position].line) + ": Expected expression");
                result = false;
            }
        } else {
            expression.push_back(code[position]);
            position++;
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