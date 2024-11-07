#include "LexicalAnalyzer.hpp"
namespace k_13 {
    LexicalAnalyzer::LexicalAnalyzer() {
        isEndOfFile = false;
    }

    const std::vector<Token> LexicalAnalyzer::getTokens() {
        return tokens;
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
        file.get(ch);

        std::cout << "[LexicalAnalyzer::readFromFile] Start reading file" << std::endl;
        while (1)
        {
            switch (state) {
                case State::START:
                    std::cout << "[LexicalAnalyzer::readFromFile] State START" << std::endl;
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
                    std::cout << "[LexicalAnalyzer::readFromFile] State FINISH" << std::endl;
                    if (!file.eof())
                        state = State::START;
                    else
                        state = State::END_OF_FILE;
                    break;

                case State::END_OF_FILE:
                    std::cout << "[LexicalAnalyzer::readFromFile] State END_OF_FILE" << std::endl;
                    {
                        std::unique_lock<std::mutex> lock(mtx);
                        isEndOfFile = true;
                        getTocken.notify_one();
                    }
                    if(getTockenType.joinable()) {
                        getTockenType.join();
                    }
                    return 0;
                    break;

                case State::LETTER:
                    std::cout << "[LexicalAnalyzer::readFromFile] State LETTER" << std::endl;
                    buffer = ch;
                    file.get(ch);
                    while (ch <= 'z' && ch >= 'a' || ch <= 'Z' && ch >= 'A' || ch <= '9' && ch >= '0') {
                        buffer += ch;
                        file.get(ch);
                    }
                    token = std::make_pair(buffer, line);
                    {
                        std::unique_lock<std::mutex> lock(mtx);
                        inputTokens.push(token);
                        getTocken.notify_one();
                    }
                    state = State::FINISH;
                    break;

                case State::DIGIT:
                    std::cout << "[LexicalAnalyzer::readFromFile] State DIGIT" << std::endl;
                    buffer = ch;
                    file.get(ch);
                    while (ch <= '9' && ch >= '0') {
                        buffer += ch;
                        file.get(ch);
                    }
                    token = std::make_pair(buffer, line);
                    {
                        std::unique_lock<std::mutex> lock(mtx);
                        inputTokens.push(token);
                        getTocken.notify_one();
                    }
                    state = State::FINISH;
                    break;

                case State::S_COMMENT:
                    std::cout << "[LexicalAnalyzer::readFromFile] State S_COMMENT" << std::endl;
                    file.get(ch);
                    if (file.eof())
                        state = State::END_OF_FILE;
                    else if (ch == '$')
                        state = State::COMMENT;
                    else {
                        token = std::make_pair("$", line);
                        {
                            std::unique_lock<std::mutex> lock(mtx);
                            inputTokens.push(token);
                            getTocken.notify_one();
                        }
                    }
                    break;

                case State::COMMENT:
                    std::cout << "[LexicalAnalyzer::readFromFile] State COMMENT" << std::endl;
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
                    std::cout << "[LexicalAnalyzer::readFromFile] State SEPARATORS" << std::endl;
                    if (ch == '\n')
                    line++;
                    file.get(ch);
                    state = State::START;
                    break;

                case State::ANOTHER:
                    std::cout << "[LexicalAnalyzer::readFromFile] State ANOTHER" << std::endl;
                    if(file.eof()) {
                        state = State::END_OF_FILE;
                        break;
                    } 
                    nch = file.peek();
                    if(nch == '\t' || nch == '\n' || nch == ' ' || nch == '$' || nch <= 'z' && nch >= 'a' 
                        || nch <= 'Z' && nch >= 'A' || nch <= '9' && nch >= '0') {
                        state = State::FINISH;
                        token = std::make_pair(std::string(1, ch), line);
                        {
                            std::unique_lock<std::mutex> lock(mtx);
                            inputTokens.push(token);
                            getTocken.notify_one();
                        }
                    }
                    file.get(ch);
                    break;

                default:
                    std::cout << "[LexicalAnalyzer::readFromFile] State default" << std::endl;
                    token = std::make_pair(std::string(1, ch), line);
                    {
                        std::unique_lock<std::mutex> lock(mtx);
                        inputTokens.push(token);
                        getTocken.notify_one();
                    }
                    file.get(ch);
                    state = State::FINISH;
                    break;
            }
        }
    }

    void LexicalAnalyzer::checkToken(int tokenLine, std::string token) {
        for(auto &operator_ : constants.operators_k13) {
                if(token == operator_.first) {
                    tokens.push_back({operator_.second, token, tokenLine});
                    return;
                }
            }

            for(auto &keyword : constants.keywords_k13) {
                if(token == keyword.first) {
                    tokens.push_back({keyword.second, token, tokenLine});
                    return;
                }
            }

            try {
                int number = std::stoi(token);
                if(token.length() > 16) {
                    tokens.push_back({TokenType::UNKNOWN, token, tokenLine});
                } else
                    tokens.push_back({TokenType::NUMBER, token, tokenLine, number});
                
            } catch (const std::exception& e) {
                if(token[0] == '"' && token[token.length() - 1] == '"') {
                    tokens.push_back({TokenType::STRING_LITERAL, token, tokenLine});
                    return;
                } else if(token[0] >= 'a' && token[0] <= 'z' && token.length() <= 6) {
                    tokens.push_back({TokenType::IDENTIFIER, token, tokenLine});
                    return;
                } else {
                    tokens.push_back({TokenType::UNKNOWN, token, tokenLine});
                }
            }
    }

    void LexicalAnalyzer::sortToken() {
        while (!isEndOfFile) {
            {
                std::unique_lock<std::mutex> lock(mtx);
                if(inputTokens.empty()) {
                    getTocken.wait(lock);
                }
            }
            if(isEndOfFile) {
                return;
            }
            std::string token{};
            int tokenLine{};
            {
                std::unique_lock<std::mutex> lock(mtx);
                token = inputTokens.front().first;
                tokenLine = inputTokens.front().second;
                inputTokens.pop();
            }

            checkToken(tokenLine, token);
            
            
        }
    }
}