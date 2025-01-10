#include <iostream>
#include <cstdlib>
#include <filesystem>

#include "LexicalAnalyzer.hpp"
#include "SyntaxAnalyzer.hpp"
#include "SemanticAnalyzer.hpp"
#include "Generator.hpp"

void writeLexems(const std::vector<k_13::Lexem>& lexems, const std::vector<k_13::Literal>& literals,
    const std::vector<k_13::UnknownLexem>& unknownLexems, const std::string& outDir);
void writeIdentifierTable(const std::map<std::string, std::vector<std::pair<int, k_13::ExpressionType>>>& identifiers, const std::string& outDir);
void writeLabelTable(const std::map<std::string, std::list<std::pair<int, k_13::ExpressionType>>>& labels, const std::string& outDir);
void writeVariableTable(const std::map<std::string, k_13::LexemType>& variableTable, const std::string& outDir);
void writeExpressions(const std::list<std::pair<k_13::LexemType, std::vector<k_13::Lexem>>>& expressions, const std::string& outDir);
void writeKeywords(const std::vector<k_13::Keyword>& keywords, const std::string& outDir);

std::string findDistance(const int maxSize, const std::string& lexems);
bool isGppInstalled();

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Error: input path to file for compilation" << std::endl;
        return -1;
    }
    std::string outDir;
    if (argc == 3) {
        outDir = argv[2];
    }
    else {
        std::filesystem::path arg1 = argv[1];
        outDir = (arg1.parent_path() / "build").string();
    }
    std::string path = argv[1];
    k_13::LexicalAnalyzer lexic;
    k_13::SyntaxAnalyzer syntax;
    k_13::SemanticAnalyzer semantic;
    k_13::Generator generator;

    std::string objGenCom = "g++ -c ";
    std::filesystem::path cppPath = outDir;
    std::filesystem::path objPath = outDir;
    std::filesystem::path exePath = outDir;
    std::string exeGenCom = "g++ ";

    int lexicalAnalysStatus = lexic.readFromFile(path);
    int syntaxAnalysStatus;
    int semanticAnalysStatus;
    int generatorStatus;
    switch (lexicalAnalysStatus) {
    case 0:
        std::cout << "[INFO] Done\n";
        writeLexems(lexic.getLexems(), lexic.getLiterals(), lexic.getUnknownLexems(), outDir);
        syntaxAnalysStatus = syntax.analyze(lexic.getLexems(), lexic.getUnknownLexems());
        switch (syntaxAnalysStatus) {
        case 0:
            std::cout << "[INFO] Syntax analysis done" << std::endl;
            writeIdentifierTable(syntax.getIdentifiers(), outDir);
            writeLabelTable(syntax.getLabels(), outDir);
            writeVariableTable(syntax.getVariableTable(), outDir);
            writeExpressions(syntax.getExpressions(), outDir);

            semanticAnalysStatus = semantic.analyze(syntax.getIdentifiers(), syntax.getLabels(), syntax.getVariableTable(), syntax.getExpressions());
            switch (semanticAnalysStatus) {
            case 0:
                std::cout << "[INFO] Semantic analysis done" << std::endl;
                generatorStatus = generator.createCpp(syntax.getKeywords(), syntax.getProgramName(), outDir, lexic.getLiterals());
                switch (generatorStatus) {
                case 0:
                    cppPath /= syntax.getProgramName() + ".cpp";
                    std::cout << "[INFO] Generation completed to " << cppPath.string() << std::endl;
                    if (isGppInstalled()) {
                        objPath /= syntax.getProgramName();
                        exePath /= syntax.getProgramName();
                        objGenCom += cppPath.string() + " -o " + objPath.string();
                        exeGenCom += cppPath.string() + " -o " + exePath.string();
#ifdef _WIN32
                        objGenCom += ".obj";
                        exeGenCom += ".exe";
#else
                        objGenCom += ".o";
#endif
                        std::system(exeGenCom.c_str());
                        std::system(objGenCom.c_str());
                        std::cout << "[INFO] G++ finish" << std::endl;
                    }
                    else {
                        std::cout << "[WARN] g++ (gcc) isn't installed in your system. To generate executive file use any c++ compiler or install g++ and rerun k13 compiler" << std::endl;
                    }
                    break;
                case -1:
                    std::cout << "[ERROR] Generator error. Can't create file" << std::endl;
                    break;
                default:
                    break;
                }
                break;
            case -1:
                std::cout << "[ERROR] Semantic analysis failed" << std::endl;
                break;
            default:
                break;
            }
            break;
        case -1:
            std::cout << "[ERROR] Syntax errors found. Build failed" << std::endl;
            break;
        default:
            break;
        }
        break;
    case -1:
        std::cout << "[ERROR] Wrong file type. Require .k13\n";
        break;
    case -2:
        std::cout << "[ERROR] Can't reach file with path: " << path << std::endl;
        break;
    default:
        break;
    }
    return 0;
}

bool isGppInstalled() {
    int result = std::system("g++ --version");
    return (result == 0);
}

void writeLexems(const std::vector<k_13::Lexem>& lexems, const std::vector<k_13::Literal>& literals,
    const std::vector<k_13::UnknownLexem>& unknownLexems, const std::string& outDir) {
    k_13::constants_k13 constants;

    std::filesystem::path outputFile = outDir;
    if (!std::filesystem::create_directory(outDir)) {
        std::cout << "[WARN] Directory exists. Make sure it's empty" << std::endl;
    }
    outputFile /= "allLexems.txt";

    std::ofstream file(outputFile);
    if (file.is_open()) {
        file << "|----------------------------------------------------------------------------------------|\n";
        file << "|                                     Lexems table                                       |\n";
        file << "|----------------------------------------------------------------------------------------|\n";
        file << "|   line number  |     lexem     |     value     |  lexem code  |     type of lexem      |\n";
        file << "|----------------------------------------------------------------------------------------|\n";
        for (auto lexem : lexems) {
            file << "|\t" << lexem.line << findDistance(14, std::to_string(lexem.line)) << " |\t" << lexem.value << findDistance(10, lexem.value) << " |\t" << lexem.constant << "\t |\t" << static_cast<std::underlying_type_t<k_13::LexemType>>(lexem.type) << " \t|\t";
            file << constants.enumToStringLexems[lexem.type] << findDistance(18, constants.enumToStringLexems[lexem.type]) << " |\n";
            file << "|----------------------------------------------------------------------------------------|\n";
        }

        file << "\n|----------------------------------------------------------------------------------------\n";
        file << "|                          Literals table\n";
        file << "|----------------------------------------------------------------------------------------\n";
        file << "|   literal id   | value \n";
        file << "|----------------------------------------------------------------------------------------\n";
        for (auto literal : literals) {
            file << "|\t" << literal.id << findDistance(10, std::to_string(literal.id)) << " |\t" << literal.value << " \n";
            file << "|----------------------------------------------------------------------------------------\n";
        }

        file << "\n|----------------------------------------------------------------------------------------\n";
        file << "|                      Unknown lexems table\n";
        file << "|----------------------------------------------------------------------------------------\n";
        file << "|   unknown id   | value \n";
        file << "|----------------------------------------------------------------------------------------\n";
        for (auto unknownLexem : unknownLexems) {
            file << "|\t" << unknownLexem.id << findDistance(10, std::to_string(unknownLexem.id)) << " |\t" << unknownLexem.value << " \n";
            file << "|----------------------------------------------------------------------------------------\n";
        }

        file.close();
    }
}

void writeIdentifierTable(const std::map<std::string, std::vector<std::pair<int, k_13::ExpressionType>>> &identifiers, const std::string& outDir) {
    k_13::constants_k13 constants;
    std::filesystem::path outputFile = outDir;
    outputFile /= "allLexems.txt";
    std::ofstream file(outputFile, std::ios::app);
    if (file.is_open()) {
        file << "|----------------------------------------------------------------------------------------|\n";
        file << "|                                 Identifiers usage                                      |\n";
        file << "|----------------------------------------------------------------------------------------|\n";
        file << "|   identifier   |   line number   |   expression type   |\n";
        file << "|----------------------------------------------------------------------------------------|\n";
        for (auto identifier : identifiers) {
            for (auto line : identifier.second) {
                file << "|\t" << identifier.first << findDistance(14, identifier.first) << " |\t" << line.first << findDistance(14, std::to_string(line.first)) << " |\t" << static_cast<std::underlying_type_t<k_13::ExpressionType>>(line.second) << " \t|\t";
                file << constants.enumToStringExpressions[line.second] << findDistance(18, constants.enumToStringExpressions[line.second]) << " |\n";
                file << "|----------------------------------------------------------------------------------------|\n";
            }
        }
        file.close();
    }
}

void writeLabelTable(const std::map<std::string, std::list<std::pair<int, k_13::ExpressionType>>> &labels, const std::string& outDir) {
    k_13::constants_k13 constants;
    std::filesystem::path outputFile = outDir;
    outputFile /= "allLexems.txt";
    std::ofstream file(outputFile, std::ios::app);
    if (file.is_open()) {
        file << "|----------------------------------------------------------------------------------------|\n";
        file << "|                                     Labels usage                                       |\n";
        file << "|----------------------------------------------------------------------------------------|\n";
        file << "|   label   |   line number   |   expression type   |\n";
        file << "|----------------------------------------------------------------------------------------|\n";
        for (auto label : labels) {
            for (auto line : label.second) {
                file << "|\t" << label.first << findDistance(10, label.first) << " |\t" << line.first << findDistance(14, std::to_string(line.first)) << " |\t" << static_cast<std::underlying_type_t<k_13::ExpressionType>>(line.second) << " \t|\t";
                file << constants.enumToStringExpressions[line.second] << findDistance(18, constants.enumToStringExpressions[line.second]) << " |\n";
                file << "|----------------------------------------------------------------------------------------|\n";
            }
        }
        file.close();
    }
}

void writeVariableTable(const std::map<std::string, k_13::LexemType> &variableTable, const std::string& outDir) {
    k_13::constants_k13 constants;
    std::filesystem::path outputFile = outDir;
    outputFile /= "allLexems.txt";
    std::ofstream file(outputFile, std::ios::app);
    if (file.is_open()) {
        file << "|----------------------------------------------------------------------------------------|\n";
        file << "|                                     Variables table                                     |\n";
        file << "|----------------------------------------------------------------------------------------|\n";
        file << "|   variable   |   type   |\n";
        file << "|----------------------------------------------------------------------------------------|\n";
        for (auto variable : variableTable) {
            file << "|\t" << variable.first << findDistance(14, variable.first) << " |\t" << static_cast<std::underlying_type_t<k_13::LexemType>>(variable.second) << " \t|\t";
            file << constants.enumToStringLexems[variable.second] << findDistance(18, constants.enumToStringLexems[variable.second]) << " |\n";
            file << "|----------------------------------------------------------------------------------------|\n";
        }
        file.close();
    }
}

void writeExpressions(const std::list<std::pair<k_13::LexemType, std::vector<k_13::Lexem>>> &expressions, const std::string& outDir) {
    std::filesystem::path outputFile = outDir;
    outputFile /= "allLexems.txt";
    std::ofstream file(outputFile, std::ios::app);
    if (file.is_open()) {
        file << "|----------------------------------------------------------------------------------------|\n";
        file << "|                                     Expressions table                                   |\n";
        file << "|----------------------------------------------------------------------------------------|\n";
        file << "|   expression type   |   expression   \n";
        file << "|----------------------------------------------------------------------------------------|\n";
        for (auto expression : expressions) {
            file << "|\t" << static_cast<std::underlying_type_t<k_13::LexemType>>(expression.first) << findDistance(20, std::to_string(static_cast<std::underlying_type_t<k_13::LexemType>>(expression.first))) << " |\t";
            for (auto lexem : expression.second) {
                file << lexem.value << " ";
            }
            file << "|\n";
            file << "|----------------------------------------------------------------------------------------|\n";
        }
        file.close();
    }
}

std::string findDistance(const int maxSize, const std::string& lexems) {
    int length = maxSize - (lexems.length() - (lexems.length() % 8));
    std::string distance = "";
    int tabs = length / 8;
    for (int i = 0; i < tabs; i++) {
        distance += "\t";
    }

    return distance;
}