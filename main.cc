#include <iostream>
#include <string>
#include <chrono>
#include <memory>
#include <sstream>
#include <map>

#include <sqlite3.h>

#include "db_wrapper.hh"

#define DB_NAME "words.db"

using std::chrono::duration_cast;

std::string GREEN_BG  = "\033[37;42m";
std::string YELLOW_BG = "\033[30;43m";
std::string WHITE_BG  = "\033[30;47m";
std::string RESET_BG  = "\033[0m";

bool wordleInvalid(std::string guess) {
    return guess.size() != 5;
}

std::stringstream getColouring(std::string guess, std::string actual) {
    std::stringstream colourings;
    for (int i = 0; i < 5; ++i) {
        if (guess[i] == actual[i]) {
            colourings << GREEN_BG;
        } else if (actual.find(guess[i]) != actual.npos) {
            colourings << YELLOW_BG;
        } else {
            colourings << WHITE_BG;
        }
        colourings << guess[i];
        colourings << RESET_BG;
    }
    return colourings;
}

int main(void) {
    const auto current_unix_time = std::chrono::system_clock::now();

    std::unique_ptr<WordleDataBase> WordleDB(new WordleDataBase(DB_NAME));
    if (WordleDB == nullptr) {
        std::cerr << "Failed to connect to the database\n";
        return 0;
    }
    std::string word_of_day = WordleDB->getWordle();
    
    int turns = 0;
    bool solved = false;
    while (!solved && turns < 6) {
        std::string input;
        std::cout << "Wordle: ";
        std::cin >> input;

        if (wordleInvalid(input)) {
            std::cout << "That wasn't five letters...\n";
            continue;
        }
        ++turns;

        std::stringstream colouring = getColouring(input, word_of_day);
        std::cout << colouring.str() << std::endl;
        if (input == word_of_day) {
            std::cout << "🎉🎉🎉 in " << turns << " turns!\n";
            break;
        }
    }
    return 0;
}
