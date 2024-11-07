#include <iostream>
#include "LexicalAnalyzer.hpp"
#include <filesystem>

void writeTokens(const std::vector<k_13::Token> &tokens);

int main(int argc, char *argv[]) {
    if (argc != 2) {
        std::cerr << "Error: input path to file for compilation" << std::endl;
        return -1;
    }
    std::string path = argv[1];
    k_13::LexicalAnalyzer lexic;
    
    int lexicalAnalysStatus = lexic.readFromFile(path);
    switch (lexicalAnalysStatus)
    {
    case 0:
        std::cout << "Done\n";
        writeTokens(lexic.getTokens());
        break;
    case -1:
        std::cout << "Wrong file type. Require .k13\n";
        break;
    case -2:
        std::cout << "Can't reach file with path: " << path << std::endl;
        break;
    default:
        break;
    }
    return 0;
}

void writeTokens(const std::vector<k_13::Token> &tokens) {
    k_13::constants_k13 constants;
    std::filesystem::path outputFile = std::filesystem::current_path() / "allLexems.txt";
    std::ofstream file(outputFile);
    if(file.is_open()) {
        file << "|---------------------------------------------------------------------|\n";
        file << "| line number | token | value | token code | type of token |\n";
        file << "|---------------------------------------------------------------------|\n";
        for(auto token : tokens) {
            file << "| " << token.line << " | " << token.value << " | " << token.constant << " | " << static_cast<std::underlying_type_t<k_13::TokenType>>(token.type) << " | " ;
            for(auto &enumToString : constants.enumToStringTokens) {
                if(token.type == enumToString.first) {
                    file << enumToString.second << " |\n";
                    break;
                }
            }
            file << "|---------------------------------------------------------------------|\n";
        }
    }
}