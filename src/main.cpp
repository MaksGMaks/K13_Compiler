#include <iostream>
#include "LexicalAnalyzer.hpp"

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