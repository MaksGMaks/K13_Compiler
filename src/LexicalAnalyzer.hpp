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
        const std::vector<Token> getTokens();
    private:
        std::queue<std::pair<std::string, int>> inputTokens;
        std::vector<k_13::Token> tokens;

        void checkToken(int tokenLine, std::string token);
        void sortToken();
        std::mutex mtx;

        std::condition_variable getTocken;
        std::atomic<bool> isEndOfFile;

        constants_k13 constants;
    };
}; // namespace k_13

