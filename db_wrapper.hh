#pragma once
#include <string>
#include <sqlite3.h>

class WordleDataBase {
 public:
    WordleDataBase(std::string);
    ~WordleDataBase();
    std::string getWordle();
 private:
    sqlite3 *db;
};
