#pragma once
#include <string>

struct Wordles {
   std::string word;
   int wasUsed;
};

struct WordleHistory {
   int startOfUseUnix;
   std::string word;
};
