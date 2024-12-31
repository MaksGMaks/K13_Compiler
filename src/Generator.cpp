#include "Generator.hpp"

int k_13::Generator::createCpp(const std::vector<Keyword> &keywords, const std::string &progName, const std::string &outPath, const std::vector<Literal> &literals_) {
    std::filesystem::path outputFile = outPath;
    outputFile /= (progName + ".cpp");
    std::ofstream file(outputFile);

    if(!file.is_open()) {
        return -1;
    }
    literals = literals_;
    std::map<std::string, LexemType> identifiers;
    file << "#include <iostream>\n"
            "#include <string>\n"
            "#include <sstream>\n\n"
            "int main()";
    statement_ch(keywords, identifiers, file);
    file.close();
    return 0;
}

void k_13::Generator::statement_ch(const std::vector<Keyword> &keywords, std::map<std::string, LexemType> &identifiers, std::ofstream &file) {
    for (auto keyword : keywords) {
        switch (keyword.keyword) {
        case LexemType::START:
            compound_gen(keyword, identifiers, file);
            break;
        case LexemType::ASSIGN:
            assign_gen(keyword, identifiers, file);
            break;
        case LexemType::GET:
            get_gen(keyword, file);
            break;
        case LexemType::PUT:
            put_gen(keyword, file);
            break;
        case LexemType::GOTO:
            goto_gen(keyword, file);
            break;
        case LexemType::LABEL:
            label_gen(keyword, file);
            break;
        case LexemType::IF:
            if_gen(keyword, identifiers, file);
            break;
        case LexemType::FOR:
            for_gen(keyword, identifiers, file);
            break;
        }
    }
}

void k_13::Generator::compound_gen(const Keyword &keyword, std::map<std::string, LexemType> &identifiers, std::ofstream &file) {
    std::map<std::string, LexemType> identifiers_comp = identifiers;
    file << "{\n";
    for (auto var : keyword.variables) {
        switch (var.second) {
            case LexemType::BOOL:
                file << "bool " << var.first << ";\n";
                identifiers_comp[var.first] = LexemType::BOOL;
                break;
            case LexemType::INT:
                file << "int16_t " << var.first << ";\n";
                identifiers_comp[var.first] = LexemType::INT;
                break;
            case LexemType::STRING:
                file << "std::stringstream " << var.first << "_ss;\n";
                file << "std::string " << var.first << ";\n";
                identifiers_comp[var.first] = LexemType::STRING;
                break;
            default:
                break;
        }
    }
    statement_ch(keyword.keywords, identifiers_comp, file);
    file << "}\n";
}

void k_13::Generator::assign_gen(const Keyword &keyword, std::map<std::string, LexemType> &identifiers, std::ofstream &file) {
    switch (identifiers.at(keyword.label)) {
    case LexemType::INT:
        file << keyword.label << " = ";
        expression(keyword.expression1, file);
        file << ";\n";
        break;
    case LexemType::BOOL:
        file << keyword.label << " = ";
        expression(keyword.expression1, file);
        file << ";\n";
        break;
    case LexemType::STRING:
        file << keyword.label << "_ss";
        str_expression(keyword.expression1, file);
        file << ";\n";
        file << keyword.label << " = " << keyword.label << "_ss.str();\n";
        file << keyword.label << "_ss.str(\"\");\n";
        file << keyword.label << "_ss.clear();\n";
        break;
    default:
        break;
    }
}

void k_13::Generator::get_gen(const Keyword &keyword, std::ofstream &file) {
    file << "std::cin >> " << keyword.label << ";\n";
}

void k_13::Generator::put_gen(const Keyword &keyword, std::ofstream &file) {
    file << "std::cout";
    str_expression(keyword.expression1, file);
    file << " << std::endl;\n";
}

void k_13::Generator::if_gen(const Keyword &keyword, std::map<std::string, LexemType> &identifiers, std::ofstream &file) {
    file << "if (";
    expression(keyword.expression1, file);
    file << ") goto " << keyword.label << ";\n";
    compound_gen(keyword.comp.front(), identifiers, file);
    file << "goto " << keyword.label2 << ";\n";
    file << keyword.label3 << ":\n";
}
// need table of declared vars
void k_13::Generator::for_gen(const Keyword &keyword, std::map<std::string, LexemType> &identifiers, std::ofstream &file) {
    file << "for (";
    if (identifiers.find(keyword.label) != identifiers.end())
        file << keyword.label << "=";
    else
        file << "int16_t " << keyword.label << "=";
    expression(keyword.expression1, file);
    file  << "; " << keyword.label << "<";
    expression(keyword.expression2, file);
    file << "; " << keyword.label << "++) ";
    compound_gen(keyword, identifiers, file);
}

void k_13::Generator::goto_gen(const Keyword &keyword, std::ofstream &file) {
    file << "goto " << keyword.label << ";\n";
}

void k_13::Generator::label_gen(const Keyword &keyword, std::ofstream &file) {
    file << keyword.label << ":\n";
}

void k_13::Generator::str_expression(const std::vector<Lexem> &expressions, std::ofstream &file) {
    bool hasLP = false;
    int depth = 0;
    for (auto exp : expressions) {
        switch (exp.type) {
        case LexemType::LPAREN:
            if (!hasLP)
                file << "<< ";
            hasLP = true;
            depth++;
            file << "(";
            break;
        case LexemType::RPAREN:
            depth--;
            (depth > 0) ? hasLP = true : hasLP = false;
            file << ")";
            break;
        case LexemType::NUMBER:
            if (!hasLP)
                file << "<< ";
            file << exp.constant;
            break;
        case LexemType::IDENTIFIER:
            if (!hasLP)
                file << "<< ";
            file << exp.value;
            break;
        case LexemType::STRING_LITERAL:
            if (!hasLP)
                file << "<< ";
            file << literals[std::stoi(exp.value)-1].value;
            break;
        case LexemType::TRUE:
            file << "true";
            break;
        case LexemType::FALSE:
            file << "false";
            break;
        case LexemType::ADD:
            if (hasLP)
                file << "+";
            break;
        case LexemType::SUB:
            file << "-";
            break;
        case LexemType::MUL:
            file << "*";
            break;
        case LexemType::DIV:
            file << "/";
            break;
        case LexemType::MOD:
            file << "%";
            break;
        case LexemType::AND:
            file << "&&";
            break;
        case LexemType::OR:
            file << "||";
            break;
        case LexemType::NOT:
            file << "!";
            break;
        case LexemType::EQUAL:
            file << "==";
            break;
        case LexemType::NEQUAL:
            file << "!=";
            break;
        case LexemType::LESS:
            file << "<";
            break;
        case LexemType::GREATER:
            file << ">";
            break;
        default:
            break;
        }
    }
}

void k_13::Generator::expression(const std::vector<Lexem> &expressions, std::ofstream &file) {
    for (auto exp : expressions) {
        switch (exp.type) {
        case LexemType::LPAREN:
            file << "(";
            break;
        case LexemType::RPAREN:
            file << ")";
            break;
        case LexemType::NUMBER:
            file << exp.constant;
            break;
        case LexemType::IDENTIFIER:
            file << exp.value;
            break;
        case LexemType::STRING_LITERAL:
            file << literals[std::stoi(exp.value)-1].value;
            break;
        case LexemType::TRUE:
            file << "true";
            break;
        case LexemType::FALSE:
            file << "false";
            break;
        case LexemType::ADD:
            file << "+";
            break;
        case LexemType::SUB:
            file << "-";
            break;
        case LexemType::MUL:
            file << "*";
            break;
        case LexemType::DIV:
            file << "/";
            break;
        case LexemType::MOD:
            file << "%";
            break;
        case LexemType::AND:
            file << "&&";
            break;
        case LexemType::OR:
            file << "||";
            break;
        case LexemType::NOT:
            file << "!";
            break;
        case LexemType::EQUAL:
            file << "==";
            break;
        case LexemType::NEQUAL:
            file << "!=";
            break;
        case LexemType::LESS:
            file << "<";
            break;
        case LexemType::GREATER:
            file << ">";
            break;
        }
    }
}