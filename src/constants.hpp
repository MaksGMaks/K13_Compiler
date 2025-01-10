#pragma once
#include <string>
#include <unordered_map>
#include <map>
#include <optional>

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
        PROGRAM,            // program 0

        START,              // start 1
        FINISH,             // finish 2
        VAR,                // var 3

        IF,                 // if 4
        GOTO,               // goto 5

        FOR,                // for 6
        TO,                 // to 7
        NEXT,               // next 8

        GET,                // get (input) 9
        PUT,                // put (output) 10

        IDENTIFIER,         // identifier 11
        NUMBER,             // number 12
        STRING_LITERAL,     // string literal 13
        TRUE,               // true 14
        FALSE,              // false 15

        STRING,             // string (string type) 16
        INT,                // int16_t (integer type) 17
        BOOL,               // bool (boolean type) 18

        ASSIGN,             // := (assignment) 19
        ADD,                // + (addition) 20
        SUB,                // - (subtraction) 21
        MUL,                // * (multiplication) 22
        DIV,                // / (integer division) 23
        MOD,                // % (modulus) 24


        EQUAL,              // = (equality) 25
        NEQUAL,             // <> (inequality) 26
        LESS,               // le (less than) 27
        GREATER,            // ge (greater than) 28

        LABEL,              // label 29

        AND,                // && (logical AND) 30
        OR,                 // || (logical OR) 31
        NOT,                // !! (logical NOT) 32

        LPAREN,             // ( (left parenthesis) 33
        RPAREN,             // ) (right parenthesis) 34

        SEMICOLON,          // ; (semicolon) 35
        COMMA,              // , (comma) 36

        QUOTES,             // " (string boundaries) 37

        UNKNOWN             // unknown token 38
    };

    enum class ExpressionType {
        ASSIGNMENT,         // assignment 0
        INPUT,              // input 1
        OUTPUT,             // output 2
        GOTO,               // goto 3
        LABEL,              // label 4
        IF,                 // if 5
        STARTFOR,           // start for 6
        ENDFOR,             // end for 7
        START,              // start 8
        FINISH,             // finish 9
        VARIABLE,           // variable 10
        PROGRAM,            // program 11
        EXPRESSION          // expression 12
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

    struct Keyword {
        LexemType keyword{};
        // expression (if, :=, put, for)
        std::vector<Lexem> expression1{};
        std::vector<Lexem> expression2{};

        // goto-label-get
        std::string label{};
        std::string label2{};
        std::string label3{};

        // for, if
        std::vector<Keyword> comp{};

        // comp
        std::vector<Keyword> keywords{};
        std::map<std::string, LexemType> variables{};

    };

    struct constants_k13 {
        std::unordered_map<std::string, LexemType> keywords_k13 = {
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

        std::unordered_map<std::string, LexemType> operators_k13 = {
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

        std::unordered_map<LexemType, std::string> enumToStringLexems = {
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
            {LexemType::UNKNOWN, "UnknownLexem"},
            {LexemType::LABEL, "Label"}
        };

        std::unordered_map<ExpressionType, std::string> enumToStringExpressions = {
            {ExpressionType::ASSIGNMENT, "Assignment"},
            {ExpressionType::INPUT, "Input"},
            {ExpressionType::OUTPUT, "Output"},
            {ExpressionType::GOTO, "Goto"},
            {ExpressionType::LABEL, "Label"},
            {ExpressionType::IF, "If"},
            {ExpressionType::STARTFOR, "StartFor"},
            {ExpressionType::ENDFOR, "EndFor"},
            {ExpressionType::START, "Start"},
            {ExpressionType::FINISH, "Finish"},
            {ExpressionType::VARIABLE, "Variable"},
            {ExpressionType::PROGRAM, "Program"},
            {ExpressionType::EXPRESSION, "Expression"}
        };
    };
}
