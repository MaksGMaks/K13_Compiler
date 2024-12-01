#include "SyntaxAnalyzer.hpp"

namespace k_13
{
    SyntaxAnalyzer::SyntaxAnalyzer() {
        code.clear();
    }

    int SyntaxAnalyzer::analyze(const std::vector<Lexem> &lexems, const std::vector<UnknownLexem> &unknownLexems) {
        std::cout << "[INFO] Starting syntax analysis" << std::endl;
        code = lexems;
        position = 0;

        for(auto lexem : code) {
            for(auto unknownLexem : unknownLexems) {
                if(lexem.type == LexemType::UNKNOWN && lexem.value == std::to_string(unknownLexem.id)) {
                    std::cerr << "\tSyntax error at line " << lexem.line << ": Unknown statement " << unknownLexem.value << std::endl;
                    errors++;
                }
            }
        }

        program();
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
            std::cerr << "\tSyntax error at line " << code[position-1].line << ": Missing ';' before statement " << code[position].value << std::endl;
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
        }
        while(code[position].type != LexemType::FINISH && position < code.size());
    }

    void SyntaxAnalyzer::statement() {
        switch(code[position].type) {
            case LexemType::START:
                compound_statement();
                break;
            case LexemType::GET:
                get_expression();
                if(!match(LexemType::SEMICOLON)) {
                    std::cerr << "\tSyntax error at line " << code[position-1].line << ": Missing ';' before statement " << code[position].value << std::endl;
                    errors++;
                }
                break;
            case LexemType::PUT:
                put_expression();
                if(!match(LexemType::SEMICOLON)) {
                    std::cerr << "\tSyntax error at line " << code[position-1].line << ": Missing ';' before statement " << code[position].value << std::endl;
                    errors++;
                }
                break;
            case LexemType::IF:
                if_expression();
                break;
            case LexemType::GOTO:
                goto_expression();
                if(!match(LexemType::SEMICOLON)) {
                    std::cerr << "\tSyntax error at line " << code[position-1].line << ": Missing ';' before statement " << code[position].value << std::endl;
                    errors++;
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
                    std::cerr << "\tSyntax error at line " << code[position-1].line << ": Missing ';' before statement " << code[position].value << std::endl;
                    errors++;
                }
                break;
            case LexemType::FINISH:
                break;
            default:
                position++;
                std::cerr << "\tSyntax error at line " << code[position].line << ": Unknown statement " << code[position].value << std::endl;
                errors++;
                break;
        }
    }

    void SyntaxAnalyzer::variable_declaration() {
        if(!match(LexemType::VAR)) {
            std::cerr << "\tSyntax error at line " << code[position-1].line << ": Expected 'var' keyword before variable segment" << std::endl;
            errors++;
        }
        if(code[position].type == LexemType::INT || code[position].type == LexemType::BOOL || code[position].type == LexemType::STRING)
            variable_list();

        if(!match(LexemType::SEMICOLON)) {
            std::cerr << "\tSyntax error at line " << code[position-1].line << ": Missing ';' before statement " << code[position].value << std::endl;
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
        int current_position = position;
        switch(code[position].type) {
            case LexemType::IDENTIFIER:
                position++;
                break;
            case LexemType::STRING_LITERAL:
                position++;
                break;
            default:
                try {
                    logical_expression();
                } catch(const std::exception &e) {
                    position = current_position;
                    try {
                        arithmetic_expression();
                    } catch(const std::exception &e) {
                        std::cerr << "\tSyntax error at line " << code[position].line << ": Expected identifier or string literal after 'put' statement" << std::endl;
                        errors++;
                        break;
                    }
                }
                break;
        }
        if(!match(LexemType::RPAREN)) {
            std::cerr << "\tSyntax error at line " << code[position-1].line << ": Expected ')' after identifier" << std::endl;
            errors++;
        }
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

    void SyntaxAnalyzer::if_expression() {
        match(LexemType::IF);
        if(!match(LexemType::LPAREN)) {
            std::cerr << "\tSyntax error at line " << code[position-1].line << ": Expected '(' before expression" << std::endl;
            errors++;
        }
        try {
            logical_expression();
        } catch(const std::exception &e) {
            std::cerr << "\tSyntax error at line " << code[position].line << ": Expected expression" << std::endl;
            errors++;
        }
        if(!match(LexemType::RPAREN)) {
            std::cerr << "\tSyntax error at line " << code[position-1].line << ": Expected ')' after expression" << std::endl;
            errors++;
        }
        goto_expression();
        if(!match(LexemType::SEMICOLON)) {
            std::cerr << "\tSyntax error at line " << code[position-1].line << ": Missing ';' before statement " << code[position].value << std::endl;
            errors++;
        }
        compound_statement();
        goto_expression();
        if(!match(LexemType::SEMICOLON)) {
            std::cerr << "\tSyntax error at line " << code[position-1].line << ": Missing ';' before statement " << code[position].value << std::endl;
            errors++;
        }
        end_goto_expression();
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
        try {
            arithmetic_expression();
        } catch(const std::exception &e) {
            std::cerr << "\tSyntax error at line " << code[position].line << ": Expected expression" << std::endl;
            errors++;
        }
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
        int current_position = position;
        try {
            logical_expression();
        } catch(const std::exception &e) {
            position = current_position;
            try {
                arithmetic_expression();
            } catch(const std::exception &e) {
                std::cerr << "\tSyntax error at line " << code[position].line << ": Expected expression" << std::endl;
                errors++;
            }
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
                try {
                    arithmetic_expression();
                } catch(const std::exception &e) {
                    std::cerr << "\tSyntax error at line " << code[position].line << ": Expected expression" << std::endl;
                    errors++;
                }
                if(!match(LexemType::RPAREN)) {
                    throw std::exception();
                    std::cerr << "\tSyntax error at line " << code[position-1].line << ": Expected ')' after expression" << std::endl;
                    errors++;
                }
                break;
            default:
                throw std::exception();
                std::cerr << "\tSyntax error at line " << code[position].line << ": Expected factor" << std::endl;
                errors++;
                break;
        }
    }

    void SyntaxAnalyzer::logical_expression() {
        if(!not_expression()) {
            or_expression();
        }
        while(code[position].type == LexemType::OR) {
            position++;
            if(!not_expression()) {
                or_expression();
            }
        }
    }

    void SyntaxAnalyzer::or_expression() {
        and_expression();
        while(code[position].type == LexemType::AND) {
            position++;
            and_expression();
        }
    }

    void SyntaxAnalyzer::and_expression() {
        if(!not_expression()) {
            try {
                logical_expression();
            } catch(const std::exception &e) {
                try {
                    arithmetic_expression();
                } catch(const std::exception &e) {
                    std::cerr << "\tSyntax error at line " << code[position].line << ": Expected expression" << std::endl;
                    errors++;
                }
            }
        }
        switch (code[position].type)
        {
        case LexemType::EQUAL:
            position++;
            break;
        case LexemType::NEQUAL:
            position++;
            break;
        case LexemType::LESS:
            position++;
            break;
        case LexemType::GREATER:
            position++;
            break;
        default:
            throw std::exception();
            std::cerr << "\tSyntax error at line " << code[position].line << ": Expected comparison operator" << std::endl;
            errors++;
            break;
        }
        if(!not_expression()) {
            try {
                logical_expression();
            } catch(const std::exception &e) {
                try {
                    arithmetic_expression();
                } catch(const std::exception &e) {
                    std::cerr << "\tSyntax error at line " << code[position].line << ": Expected expression" << std::endl;
                    errors++;
                }
            }
        }
    }

    bool SyntaxAnalyzer::not_expression() {
        if(code[position].type == LexemType::NOT) {
            if(!match(LexemType::LPAREN)) {
                throw std::exception();
                std::cerr << "\tSyntax error at line " << code[position-1].line << ": Expected '(' before expression" << std::endl;
                errors++;
            }
            try {
                logical_expression();
            } catch(const std::exception &e) {
                try {
                    arithmetic_expression();
                } catch(const std::exception &e) {
                    std::cerr << "\tSyntax error at line " << code[position].line << ": Expected expression" << std::endl;
                    errors++;
                }
            }
            if(!match(LexemType::RPAREN)) {
                throw std::exception();
                std::cerr << "\tSyntax error at line " << code[position-1].line << ": Expected ')' after expression" << std::endl;
                errors++;
            }
            return true;
        }
        return false;
    }
}; // namespace k_13