#include "SyntaxAnalyzer.hpp"

namespace k_13
{
    SyntaxAnalyzer::SyntaxAnalyzer() {
        code.clear();
    }

    int SyntaxAnalyzer::analyze(const std::vector<Lexem> &lexems, const std::vector<UnknownLexem> &unknownLexems) {
        std::cout << "[INFO] Starting syntax analysis" << std::endl;
        code = lexems;
        unknown = unknownLexems;
        position = 0;

        program();
        std::cout << std::endl;
        if(errors > 0) {
            return -1;
        }
        return 0;
    }

    bool SyntaxAnalyzer::match(const LexemType expectedType) {
        if(code[position].type == expectedType) {
            position++;
            return true;
        } 
        return false;
    }

    void SyntaxAnalyzer::program() {
        program_declaration();
        program_body();
    }

    void SyntaxAnalyzer::program_declaration() {
        if(!match(LexemType::PROGRAM)) {
            std::cerr << "\tSyntax error at line " << code[position-1].line << ": Expected 'program' keyword before program name" << std::endl;
            errors++;
        }
        if(!match(LexemType::IDENTIFIER)) {
            std::cerr << "\tSyntax error at line " << code[position-1].line << ": Expected program name after 'program' keyword" << std::endl;
            errors++;
        }
        if(!match(LexemType::SEMICOLON)) {
            if(code[position].type == LexemType::UNKNOWN) {
                std::cerr << "\tSyntax error at line " << code[position].line << ": Missing ';' before statement " << unknown[std::stoi(code[position].value)].value << std::endl;
            } else {
                std::cerr << "\tSyntax error at line " << code[position].line << ": Missing ';' before statement " << code[position].value << std::endl;
            }
            errors++;
        }
    }

    void SyntaxAnalyzer::compound_statement() {
        if(!match(LexemType::START)) {
            std::cerr << "\tSyntax error at line " << code[position-1].line << ": Expected 'start' keyword before program body" << std::endl;
            errors++;
        }
        variable_declaration();
        program_body();
        if(!match(LexemType::FINISH)) {
            std::cerr << "\tSyntax error at line " << code[position-1].line << ": Expected 'finish' keyword after program body" << std::endl;
            errors++;
        }
    }

    void SyntaxAnalyzer::program_body() {
        do {
            statement();
            if(!(code[position-1].type == LexemType::FINISH)) {    
                if(!match(LexemType::SEMICOLON)) {
                    if(code[position].type == LexemType::UNKNOWN) {
                        std::cerr << "\tSyntax error at line " << code[position].line << ": Missing ';' before statement " << unknown[std::stoi(code[position].value)].value << std::endl;
                    } else {
                        std::cerr << "\tSyntax error at line " << code[position].line << ": Missing ';' before statement " << code[position].value << std::endl;
                    }
                    errors++;
                }
            }
            if(position >= code.size() - 1) {
                break;
            }
        }
        while(code[position].type != LexemType::FINISH);
    }

    void SyntaxAnalyzer::variable_declaration() {
        if(!match(LexemType::VAR)) {
            std::cerr << "\tSyntax error at line " << code[position-1].line << ": Expected 'var' keyword before variable segment" << std::endl;
            errors++;
        }
        if(code[position].type == LexemType::INT || code[position].type == LexemType::BOOL || code[position].type == LexemType::STRING)
            variable_list();

        if(!match(LexemType::SEMICOLON)) {
            if(code[position].type == LexemType::UNKNOWN) {
                std::cerr << "\tSyntax error at line " << code[position].line << ": Missing ';' before statement " << unknown[std::stoi(code[position].value)].value << std::endl;
            } else {
                std::cerr << "\tSyntax error at line " << code[position].line << ": Missing ';' before statement " << code[position].value << std::endl;
            }
            errors++;
        }
    }

    void SyntaxAnalyzer::variable_list() {
        position++;
        if(!match(LexemType::IDENTIFIER)) {
            std::cerr << "\tSyntax error at line " << code[position-1].line << ": Expected identifier after variable type" << std::endl;
            errors++;
        }
        while(code[position].type == LexemType::COMMA) {
            position++;
            if(!(match(LexemType::INT) || match(LexemType::BOOL) || match(LexemType::STRING))) {
                std::cerr << "\tSyntax error at line " << code[position-1].line << ": Expected variable type before identifier" << std::endl;
                errors++;
            }
            if(!match(LexemType::IDENTIFIER)) {
                std::cerr << "\tSyntax error at line " << code[position-1].line << ": Expected identifier after variable type" << std::endl;
                errors++;
            }
        }
    }

    void SyntaxAnalyzer::statement() {
        switch(code[position].type) {
            case LexemType::START:
                compound_statement();
                break;
            case LexemType::GET:
                get_expression();
                break;
            case LexemType::PUT:
                put_expression();
                break;
            case LexemType::IF:
                if_expression();
                break;
            case LexemType::GOTO:
                goto_expression();
                break;
            case LexemType::FOR:
                for_expression();
                break;
            case LexemType::IDENTIFIER:
                if(code[position+1].type == LexemType::ASSIGN) {
                    assign_expression();
                } else if(code[position+1].type == LexemType::START) {
                    block_statement();
                } else {
                    end_goto_expression();
                }
                break;
            default:
                if(code[position].type == LexemType::UNKNOWN) {
                    std::cerr << "\tSyntax error at line " << code[position].line << ": Unknown statement " << unknown[std::stoi(code[position].value)].value << std::endl;
                } else {
                    std::cerr << "\tSyntax error at line " << code[position].line << ": Unknown statement " << code[position].value << std::endl;
                }
                errors++;
                position++;
                break;
        }
    }

    void SyntaxAnalyzer::block_statement() {
        match(LexemType::IDENTIFIER);
        compound_statement();
    }

    void SyntaxAnalyzer::end_goto_expression() {
        match(LexemType::IDENTIFIER);
    }

    void SyntaxAnalyzer::goto_expression() {
        if(!match(LexemType::GOTO)) {
            std::cerr << "\tSyntax error at line " << code[position-1].line << ": Expected 'goto' keyword before identifier" << std::endl;
            errors++;
        }
        if(!match(LexemType::IDENTIFIER)) {
            std::cerr << "\tSyntax error at line " << code[position-1].line << ": Expected identifier after 'goto' statement" << std::endl;
            errors++;
        }
    }

    void SyntaxAnalyzer::goto_statement() {
        match(LexemType::GOTO);
        if(!match(LexemType::IDENTIFIER)) {
            std::cerr << "\tSyntax error at line " << code[position-1].line << ": Expected identifier after 'goto' statement" << std::endl;
            errors++;
        }
    }

    void SyntaxAnalyzer::if_expression() {
        match(LexemType::IF);
        if(!match(LexemType::LPAREN)) {
            std::cerr << "\tSyntax error at line " << code[position-1].line << ": Expected '(' before expression" << std::endl;
            errors++;
        }
        logical_expression();
        if(!match(LexemType::RPAREN)) {
            std::cerr << "\tSyntax error at line " << code[position-1].line << ": Expected ')' after expression" << std::endl;
            errors++;
        }
        goto_expression();
        if(!match(LexemType::SEMICOLON)) {
            if(code[position].type == LexemType::UNKNOWN) {
                std::cerr << "\tSyntax error at line " << code[position].line << ": Missing ';' before statement " << unknown[std::stoi(code[position].value)].value << std::endl;
            } else {
                std::cerr << "\tSyntax error at line " << code[position].line << ": Missing ';' before statement " << code[position].value << std::endl;
            }
            errors++;
        }
        goto_expression();
    }

    void SyntaxAnalyzer::for_expression() {
        match(LexemType::FOR);
        if(!match(LexemType::IDENTIFIER)) {
            std::cerr << "\tSyntax error at line " << code[position-1].line << ": Expected identifier after 'for' statement" << std::endl;
            errors++;
        }
        if(!match(LexemType::TO)) {
            std::cerr << "\tSyntax error at line " << code[position-1].line << ": Expected 'to' keyword after identifier" << std::endl;
            errors++;
        }
        arithmetic_expression();
        if(!match(LexemType::NEXT)) {
            std::cerr << "\tSyntax error at line " << code[position-1].line << ": Expected 'next' keyword after expression" << std::endl;
            errors++;
        }
        compound_statement();
    }

    void SyntaxAnalyzer::assign_expression() {
        match(LexemType::IDENTIFIER);
        if(!match(LexemType::ASSIGN)) {
            std::cerr << "\tSyntax error at line " << code[position-1].line << ": Expected ':=' after identifier" << std::endl;
            errors++;
        }
        arithmetic_expression();
    }

    void SyntaxAnalyzer::get_expression() {
        match(LexemType::GET);
        if(!match(LexemType::LPAREN)) {
            std::cerr << "\tSyntax error at line " << code[position-1].line << ": Expected '(' before identifier" << std::endl;
            errors++;
        }
        if(!match(LexemType::IDENTIFIER)) {
            std::cerr << "\tSyntax error at line " << code[position-1].line << ": Expected identifier after 'get' statement" << std::endl;
            errors++;
        }
        if(!match(LexemType::RPAREN)) {
            std::cerr << "\tSyntax error at line " << code[position-1].line << ": Expected ')' after identifier" << std::endl;
            errors++;
        }
    }

    void SyntaxAnalyzer::put_expression() {
        match(LexemType::PUT);
        if(!match(LexemType::LPAREN)) {
            std::cerr << "\tSyntax error at line " << code[position-1].line << ": Expected '(' before identifier" << std::endl;
            errors++;
        }
        if(!match(LexemType::IDENTIFIER)) {
            std::cerr << "\tSyntax error at line " << code[position-1].line << ": Expected identifier after 'put' statement" << std::endl;
            errors++;
        }
        if(!match(LexemType::RPAREN)) {
            std::cerr << "\tSyntax error at line " << code[position-1].line << ": Expected ')' after identifier" << std::endl;
            errors++;
        }
    }

    void SyntaxAnalyzer::arithmetic_expression() {
        term();
        while(code[position].type == LexemType::ADD || code[position].type == LexemType::SUB) {
            position++;
            term();
        }
    }

    void SyntaxAnalyzer::term() {
        factor();
        while(code[position].type == LexemType::MUL || code[position].type == LexemType::DIV || code[position].type == LexemType::MOD) {
            position++;
            factor();
        }
    }

    void SyntaxAnalyzer::factor() {
        switch(code[position].type) {
            case LexemType::IDENTIFIER:
                position++;
                break;
            case LexemType::NUMBER:
                position++;
                break;
            case LexemType::BOOL:
                position++;
                break;
            case LexemType::TRUE:
                position++;
                break;
            case LexemType::FALSE:
                position++;
                break;
            case LexemType::STRING_LITERAL:
                position++;
                break;
            case LexemType::LPAREN:
                position++;
                arithmetic_expression();
                if(!match(LexemType::RPAREN)) {
                    std::cerr << "\tSyntax error at line " << code[position-1].line << ": Expected ')' after expression" << std::endl;
                    errors++;
                }
                break;
            default:
                std::cerr << "\tSyntax error at line " << code[position].line << ": Expected factor" << std::endl;
                break;
        }
    }

    void SyntaxAnalyzer::logical_expression() {
        if(code[position].type == LexemType::NOT) {
            position++;
            if(!match(LexemType::LPAREN)) {
                std::cerr << "\tSyntax error at line " << code[position-1].line << ": Expected '(' before expression" << std::endl;
                errors++;
            }
            logical_expression();
            if(!match(LexemType::RPAREN)) {
                std::cerr << "\tSyntax error at line " << code[position-1].line << ": Expected ')' after expression" << std::endl;
                errors++;
            }
        } else {
            and_expression();
        }
        if(code[position].type == LexemType::EQUAL || code[position].type == LexemType::NEQUAL) {
            position++;
            if(code[position].type == LexemType::TRUE || code[position].type == LexemType::FALSE) {
                position++;
            } else {
                if(code[position].type == LexemType::NOT) {
                    position++;
                    logical_expression();
                } else {
                    if(!match(LexemType::LPAREN)) {
                        std::cerr << "\tSyntax error at line " << code[position-1].line << ": Expected '(' before expression" << std::endl;
                        errors++;
                    }
                    logical_expression();
                    if(!match(LexemType::RPAREN)) {
                        std::cerr << "\tSyntax error at line " << code[position-1].line << ": Expected ')' after expression" << std::endl;
                        errors++;
                    }
                }
            }
        }
        while(code[position].type == LexemType::OR) {
            position++;
            if(code[position].type == LexemType::NOT) {
                position++;
                if(!match(LexemType::LPAREN)) {
                    std::cerr << "\tSyntax error at line " << code[position-1].line << ": Expected '(' before expression" << std::endl;
                    errors++;
                }
                logical_expression();
                if(!match(LexemType::RPAREN)) {
                    std::cerr << "\tSyntax error at line " << code[position-1].line << ": Expected ')' after expression" << std::endl;
                    errors++;
                }
            } else {
                and_expression();
            }
        }
    }

    void SyntaxAnalyzer::and_expression() {
        compare_expression();
        while(code[position].type == LexemType::AND) {
            position++;
            compare_expression();
        }
    }

    void SyntaxAnalyzer::compare_expression() {
        switch(code[position].type) {
            case LexemType::EQUAL:
                position++;
                if(code[position].type == LexemType::TRUE || code[position].type == LexemType::FALSE) {
                    position++;
                } else {
                    logical_expression();
                }
                break;
            case LexemType::NEQUAL:
                position++;
                if(code[position].type == LexemType::TRUE || code[position].type == LexemType::FALSE) {
                    position++;
                } else {
                    logical_expression();
                }
                break;
            case LexemType::LPAREN:
                position++;
                logical_expression();
                if(!match(LexemType::RPAREN)) {
                    std::cerr << "\tSyntax error at line " << code[position-1].line << ": Expected ')' after expression" << std::endl;
                    errors++;
                }
                break;
            default:
                arithmetic_expression();
                if(!(match(LexemType::EQUAL) || match(LexemType::NEQUAL) || match(LexemType::LESS) || match(LexemType::GREATER))) {
                    std::cerr << "\tSyntax error at line " << code[position-1].line << ": Expected comparison operator" << std::endl;
                }
                arithmetic_expression();
                break;
        }
    }
}; // namespace k_13