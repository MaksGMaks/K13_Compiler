#pragma once
#include <string>
#include <unordered_map>
#include <map>

namespace k_13 {
    enum class State {
        START,              // початок виділення чергової лексеми
        FINISH,             // кінець виділення чергової лексеми
        LETTER,             // опрацювання слів (ключові слова і ідентифікатори)
        DIGIT,              // опрацювання цифри
        SEPARATORS,         // видалення пробілів, символів табуляції і переходу на новий рядок
        ANOTHER,            // опрацювання інших символів
        END_OF_FILE,        // кінець файлу
        S_COMMENT,          // початок коментаря
        COMMENT,            // видалення коментаря
        STRING              // опрацювання рядка
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

        IDENTIFIER,         // ідентифікатор
        NUMBER,             // число
        STRING_LITERAL,     // рядок
        TRUE,               // true
        FALSE,              // false

        STRING,             // string (рядок)
        INT,                // int16_t (ціле число)
        BOOL,               // bool (логічний тип)

        ASSIGN,             // := (присвоєння)
        ADD,                // + (додавання)
        SUB,                // - (віднімання)
        MUL,                // * (множення)
        DIV,                // / (цілочисельне ділення)
        MOD,                // % (остача від ділення)

        EQUAL,              // = (рівність)
        NEQUAL,             // <> (нерівність)
        LESS,               // le (менше)
        GREATER,            // ge (більше)

        AND,                // && (логічне І)
        OR,                 // || (логічне АБО)
        NOT,                // !! (логічне НІ)

        LPAREN,             // ( (ліва дужка)
        RPAREN,             // ) (права дужка)

        SEMICOLON,          // ; (крапка з комою)
        COMMA,              // , (кома)

        QUOTES,             // " (межі рядка)

        UNKNOWN             // невідомий токен
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
            {"int", LexemType::INT},
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