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
        COMMENT             // видалення коментаря
    };

    enum class TokenType {
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
        CONST,              // const (константа)

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

    struct Token {
        TokenType type{};
        std::string value{};
        int line{};
        int constant{};
    };

    struct constants_k13
    {
        std::map<std::string, TokenType> keywords_k13 = {
            {"program", TokenType::PROGRAM},
            {"start", TokenType::START},
            {"finish", TokenType::FINISH},
            {"var", TokenType::VAR},
            {"if", TokenType::IF},
            {"goto", TokenType::GOTO},
            {"for", TokenType::FOR},
            {"to", TokenType::TO},
            {"next", TokenType::NEXT},
            {"get", TokenType::GET},
            {"put", TokenType::PUT},
            {"string", TokenType::STRING},
            {"int16_t", TokenType::INT},
            {"bool", TokenType::BOOL},
            {"true", TokenType::TRUE},
            {"false", TokenType::FALSE},
            {"const", TokenType::CONST},
            {"le", TokenType::LESS},
            {"ge", TokenType::GREATER}
        };

        std::map<std::string, TokenType> operators_k13 = {
            {":=", TokenType::ASSIGN},
            {"+", TokenType::ADD},
            {"-", TokenType::SUB},
            {"*", TokenType::MUL},
            {"/", TokenType::DIV},
            {"%", TokenType::MOD},
            {"=", TokenType::EQUAL},
            {"<>", TokenType::NEQUAL},
            {"&&", TokenType::AND},
            {"||", TokenType::OR},
            {"!!", TokenType::NOT}
        };
    };
    

}