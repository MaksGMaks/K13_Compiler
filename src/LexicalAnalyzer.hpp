#pragma once

#include <iostream>
#include <fstream>
#include <queue>
#include <atomic>
#include <condition_variable>
#include <mutex>
#include <thread>
#include <vector>

#include "constants.hpp"

namespace k_13
{
    class LexicalAnalyzer {
    public:
        LexicalAnalyzer();
        ~LexicalAnalyzer() = default;
    
        int readFromFile(const std::string &filename);

        const std::vector<Lexem> getLexems();
        const std::vector<Literal> getLiterals();
        const std::vector<UnknownLexem> getUnknownLexems();
    private:
        std::queue<std::pair<std::string, int>> inputLexems;

        std::vector<Lexem> lexems;
        std::vector<Literal> literals;
        std::vector<UnknownLexem> unknownLexems;

        void checkLexem(int tokenLine, std::string token);
        void sortToken();
        std::mutex mtx;

        std::condition_variable getToken;
        std::atomic<bool> isEndOfFile;

        int literalId;
        int unknownId;

        constants_k13 constants;
    };
}; // namespace k_13

