#pragma once
#include <string>
#include <unordered_map>
#include <map>

namespace k_13 {
    enum class State {
        START,              // start of extracting the next lexeme
        FINISH,             // end of extracting the next lexeme
        LETTER,             // processing words (keywords and identifiers)
        DIGIT,              // processing digits
        SEPARATORS,         // removing spaces, tab characters, and new line characters
        ANOTHER,            // processing other characters
        END_OF_FILE,        // end of file
        S_COMMENT,          // start of comment
        COMMENT,            // removing comment
        STRING              // processing string
    };

    enum class LexemType {
        PROGRAM,            // program

        START,              // start
        FINISH,             // finish
        VAR,                // var

        IF,                 // if
        GOTO,               // goto
        
        FOR,                // for
        TO,                 // to
        NEXT,               // next
        
        GET,                // get (input)
        PUT,                // put (output)

        IDENTIFIER,         // identifier
        NUMBER,             // number
        STRING_LITERAL,     // string literal
        TRUE,               // true
        FALSE,              // false

        STRING,             // string (string type)
        INT,                // int16_t (integer type)
        BOOL,               // bool (boolean type)

        ASSIGN,             // := (assignment)
        ADD,                // + (addition)
        SUB,                // - (subtraction)
        MUL,                // * (multiplication)
        DIV,                // / (integer division)
        MOD,                // % (modulus)

        EQUAL,              // = (equality)
        NEQUAL,             // <> (inequality)
        LESS,               // le (less than)
        GREATER,            // ge (greater than)

        AND,                // && (logical AND)
        OR,                 // || (logical OR)
        NOT,                // !! (logical NOT)

        LPAREN,             // ( (left parenthesis)
        RPAREN,             // ) (right parenthesis)

        SEMICOLON,          // ; (semicolon)
        COMMA,              // , (comma)

        QUOTES,             // " (string boundaries)

        UNKNOWN             // unknown token
    };

    struct Lexem {
        LexemType type{};
        std::string value{};
        int line{};
        int constant{};
    };

    struct Literal {
        int id{};
        std::string value{};
    };

    struct UnknownLexem {
        int id{};
        std::string value{};
    };

    struct constants_k13
    {
        std::map<std::string, LexemType> keywords_k13 = {
            {"program", LexemType::PROGRAM},
            {"start", LexemType::START},
            {"finish", LexemType::FINISH},
            {"var", LexemType::VAR},
            {"if", LexemType::IF},
            {"goto", LexemType::GOTO},
            {"for", LexemType::FOR},
            {"to", LexemType::TO},
            {"next", LexemType::NEXT},
            {"get", LexemType::GET},
            {"put", LexemType::PUT},
            {"string", LexemType::STRING},
            {"int16_t", LexemType::INT},
            {"bool", LexemType::BOOL},
            {"true", LexemType::TRUE},
            {"false", LexemType::FALSE},
            {"le", LexemType::LESS},
            {"ge", LexemType::GREATER}
        };

        std::map<std::string, LexemType> operators_k13 = {
            {":=", LexemType::ASSIGN},
            {"+", LexemType::ADD},
            {"-", LexemType::SUB},
            {"*", LexemType::MUL},
            {"/", LexemType::DIV},
            {"%", LexemType::MOD},
            {"=", LexemType::EQUAL},
            {"<>", LexemType::NEQUAL},
            {"&&", LexemType::AND},
            {"||", LexemType::OR},
            {"!!", LexemType::NOT},
            {"(", LexemType::LPAREN},
            {")", LexemType::RPAREN},
            {";", LexemType::SEMICOLON},
            {",", LexemType::COMMA},
            {"\"", LexemType::QUOTES}
        };

        std::map<LexemType, std::string> enumToStringLexems = {
            {LexemType::PROGRAM, "ProgramKeyword"},
            {LexemType::START, "StartOperator"},
            {LexemType::FINISH, "FinishOperator"},
            {LexemType::VAR, "VarKeyword"},
            {LexemType::IF, "IfKeyword"},
            {LexemType::GOTO, "GotoKeyword"},
            {LexemType::FOR, "ForKeyword"},
            {LexemType::TO, "ToKeyword"},
            {LexemType::NEXT, "NextKeyword"},
            {LexemType::GET, "GetKeyword"},
            {LexemType::PUT, "PutKeyword"},
            {LexemType::IDENTIFIER, "Identifier"},
            {LexemType::NUMBER, "Number"},
            {LexemType::STRING_LITERAL, "StringLiteral"},
            {LexemType::TRUE, "TrueConstant"},
            {LexemType::FALSE, "FalseConstant"},
            {LexemType::STRING, "StringType"},
            {LexemType::INT, "IntType"},
            {LexemType::BOOL, "BoolType"},
            {LexemType::ASSIGN, "AssignOperator"},
            {LexemType::ADD, "AddOperator"},
            {LexemType::SUB, "SubOperator"},
            {LexemType::MUL, "MulOperator"},
            {LexemType::DIV, "DivOperator"},
            {LexemType::MOD, "ModOperator"},
            {LexemType::EQUAL, "EqualOperator"},
            {LexemType::NEQUAL, "NotEqualOperator"},
            {LexemType::LESS, "LessOperator"},
            {LexemType::GREATER, "GreaterOperator"},
            {LexemType::AND, "AndOperator"},
            {LexemType::OR, "OrOperator"},
            {LexemType::NOT, "NotOperator"},
            {LexemType::LPAREN, "LeftParenthesis"},
            {LexemType::RPAREN, "RightParenthesis"},
            {LexemType::SEMICOLON, "Semicolon"},
            {LexemType::COMMA, "Comma"},
            {LexemType::QUOTES, "Quotes"},
            {LexemType::UNKNOWN, "UnknownLexem"}
        };
    };
    

}