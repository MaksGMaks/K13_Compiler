#include "LexicalAnalyzer.hpp"
namespace k_13 {
    LexicalAnalyzer::LexicalAnalyzer() {
        isEndOfFile = false;
        literalId = 0;
        unknownId = 0;
    }

    const std::vector<Lexem> LexicalAnalyzer::getLexems() {
        return lexems;
    }

    const std::vector<Literal> LexicalAnalyzer::getLiterals() {
        return literals;
    }

    const std::vector<UnknownLexem> LexicalAnalyzer::getUnknownLexems() {
        return unknownLexems;
    }

    int LexicalAnalyzer::readFromFile(const std::string &filename) {
        if(filename.find(".k13") == filename.length() - 5) {
            return -1;
        }
        std::ifstream file(filename);
        if(!file.is_open()) {
            return -2;
        }
        
        std::thread getTockenType(&LexicalAnalyzer::sortToken, this);

        State state = State::START;
        isEndOfFile = false;
        char ch, nch{};
        std::string buffer{};
        std::pair<std::string, int> token{};
        int line = 1;
        literalId = 1;
        unknownId = 1;
        
        if(!lexems.empty())
            lexems.clear();
        if(!literals.empty())
            literals.clear();
        if(!unknownLexems.empty())
            unknownLexems.clear();

        file.get(ch);

        std::cout << "[LexicalAnalyzer::readFromFile] Start reading file" << std::endl;
        while (1)
        {
            switch (state) {
                case State::START:
                    if (file.eof())
                        state = State::END_OF_FILE;
                    else if (ch <= 'z' && ch >= 'a' || ch <= 'Z' && ch >= 'A')
                        state = State::LETTER;
                    else if (ch <= '9' && ch >= '0')
                        state = State::DIGIT;
                    else if (ch == ' ' || ch == '\t' || ch == '\n')
                        state = State::SEPARATORS;
                    else if (ch == '$')
                        state = State::S_COMMENT;
                    else
                        state = State::ANOTHER;
                    break;
                    
                case State::FINISH:
                    if (!file.eof())
                        state = State::START;
                    else
                        state = State::END_OF_FILE;
                    break;

                case State::END_OF_FILE:
                    {
                        std::unique_lock<std::mutex> lock(mtx);
                        isEndOfFile = true;
                        getToken.notify_one();
                    }
                    if(getTockenType.joinable()) {
                        getTockenType.join();
                    }
                    return 0;
                    break;

                case State::LETTER:
                    buffer = ch;
                    file.get(ch);
                    while (ch <= 'z' && ch >= 'a' || ch <= 'Z' && ch >= 'A' || ch <= '9' && ch >= '0' || ch == '_') {
                        buffer += ch;
                        file.get(ch);
                        if(file.eof())
                            break;
                    }
                    token = std::make_pair(buffer, line);
                    {
                        std::unique_lock<std::mutex> lock(mtx);
                        inputLexems.push(token);
                        getToken.notify_one();
                    }
                    state = State::FINISH;
                    break;

                case State::DIGIT:
                    buffer = ch;
                    file.get(ch);
                    while (ch <= '9' && ch >= '0') {
                        buffer += ch;
                        file.get(ch);
                        if(file.eof())
                            break;
                    }
                    token = std::make_pair(buffer, line);
                    {
                        std::unique_lock<std::mutex> lock(mtx);
                        inputLexems.push(token);
                        getToken.notify_one();
                    }
                    state = State::FINISH;
                    break;

                case State::S_COMMENT:
                    file.get(ch);
                    if (file.eof())
                        state = State::END_OF_FILE;
                    else if (ch == '$')
                        state = State::COMMENT;
                    else {
                        token = std::make_pair("$", line);
                        {
                            std::unique_lock<std::mutex> lock(mtx);
                            inputLexems.push(token);
                            getToken.notify_one();
                        }
                    }
                    break;

                case State::COMMENT:
                    while (ch != '\n' && !file.eof()) {
                        file.get(ch);
                    }
                    if (file.eof()) {
                        state = State::END_OF_FILE;
                        break;
                    }
                    line++;
                    file.get(ch);
                    state = State::START;
                    break;

                case State::SEPARATORS:
                    if (ch == '\n')
                        line++;
                    file.get(ch);
                    state = State::START;
                    break;

                case State::ANOTHER:
                    if(file.eof()) {
                        state = State::END_OF_FILE;
                        break;
                    } 
                    if(ch == '"') {
                        state = State::STRING;
                        break;
                    }

                    buffer = ch;
                    nch = file.peek();

                    if(ch == ':' && nch == '=') {
                        file.get(ch);
                        buffer += ch;
                    } else if(ch == '<' && nch == '>') {
                        file.get(ch);
                        buffer += ch;
                    } else if(ch == '&' && nch == '&') {
                        file.get(ch);
                        buffer += ch;
                    } else if(ch == '|' && nch == '|') {
                        file.get(ch);
                        buffer += ch;
                    } else if(ch == '!' && nch == '!') {
                        file.get(ch);
                        buffer += ch;
                    }
                    state = State::FINISH;
                    token = std::make_pair(buffer, line);
                    {
                        std::unique_lock<std::mutex> lock(mtx);
                        inputLexems.push(token);
                        getToken.notify_one();
                    }
                    file.get(ch);
                    break;
                    
                case State::STRING:
                    buffer = ch;
                    file.get(ch);
                    while (ch != '"') {
                        buffer += ch;
                        file.get(ch);
                    }
                    buffer += ch;
                    token = std::make_pair(buffer, line);
                    {
                        std::unique_lock<std::mutex> lock(mtx);
                        inputLexems.push(token);
                        getToken.notify_one();
                    }
                    state = State::FINISH;
                    file.get(ch);
                    break;

                default:
                    token = std::make_pair(std::string(1, ch), line);
                    {
                        std::unique_lock<std::mutex> lock(mtx);
                        inputLexems.push(token);
                        getToken.notify_one();
                    }
                    file.get(ch);
                    state = State::FINISH;
                    break;
            }
        }
    }

    void LexicalAnalyzer::checkLexem(int tokenLine, std::string token) {
        for(auto &operator_ : constants.operators_k13) {
                if(token == operator_.first) {
                    lexems.push_back({operator_.second, token, tokenLine});
                    return;
                }
            }

            for(auto &keyword : constants.keywords_k13) {
                if(token == keyword.first) {
                    lexems.push_back({keyword.second, token, tokenLine});
                    return;
                }
            }

            try {
                int number = std::stoi(token);
                if(token.length() > 16) {
                    lexems.push_back({LexemType::UNKNOWN, std::to_string(unknownId), tokenLine});
                    unknownLexems.push_back({unknownId, token});
                    unknownId++;
                } else
                    lexems.push_back({LexemType::NUMBER, token, tokenLine, number});
                
            } catch (const std::exception& e) {
                if(token[0] == '"' && token[token.length() - 1] == '"') {
                    lexems.push_back({LexemType::STRING_LITERAL, std::to_string(literalId), tokenLine});
                    literals.push_back({literalId, token});
                    literalId++;
                    return;
                } else if(token[0] >= 'a' && token[0] <= 'z' && token.length() <= 6) {
                    lexems.push_back({LexemType::IDENTIFIER, token, tokenLine});
                    return;
                } else {
                    lexems.push_back({LexemType::UNKNOWN, std::to_string(unknownId), tokenLine});
                    unknownLexems.push_back({unknownId, token});
                    unknownId++;
                }
            }
    }

    void LexicalAnalyzer::sortToken() {
        while (!isEndOfFile || !inputLexems.empty()) {
            {
                std::unique_lock<std::mutex> lock(mtx);
                if(inputLexems.empty()) {
                    getToken.wait(lock);
                }
            }
            if(isEndOfFile && inputLexems.empty()) {
                return;
            }
            std::string token{};
            int tokenLine{};
            {
                std::unique_lock<std::mutex> lock(mtx);
                token = inputLexems.front().first;
                tokenLine = inputLexems.front().second;
                inputLexems.pop();
            }

            checkLexem(tokenLine, token);
        }
    }
}