#include <iostream>
#include <string>
#include <chrono>
#include <memory>
#include <sstream>
#include <vector>
#include <random>

// #include "db_wrapper.hh"
#include "db_structs.hh"
#include "include/sqlite_orm.h"

#define DB_NAME ".words.db"

using std::chrono::duration_cast;
using namespace sqlite_orm;

std::string GREEN_BG  = "\033[37;42m";
std::string YELLOW_BG = "\033[30;43m";
std::string WHITE_BG  = "\033[30;47m";
std::string RESET_BG  = "\033[0m";

bool wordleInvalid(std::string guess) {
    return guess.size() != 5;
}

std::stringstream getColouring(std::string& guess, std::string& actual) {
    // Compute colourings
    // guess X actual adjacency matrix
    int colour_matrix[5][5] = {0};
    // Populate adj matrix
    for (int i = 0; i < 5; ++i) {
        for (int j = 0; j < 5; ++j) {
            if (guess[i] == actual[j]) colour_matrix[i][j] = 1;
        }
    }
    // Zero out incidences to perfect matches
    for (int i = 0; i < 5; ++i) {
        if (colour_matrix[i][i]) {
            for (int j = 0; j < 5; ++j) colour_matrix[j][i] = 0;
            colour_matrix[i][i] = 1;
        }
    }
    // Generate printable
    std::stringstream colourings;
    for (int i = 0; i < 5; ++i) {
        // Check for perfect match
        if (colour_matrix[i][i]) {
            colourings << GREEN_BG;
        } else {
            // Scan for misplaced match
            int j;
            for (j = 0; j < 5; ++j) {
                if (colour_matrix[i][j]) {
                    colourings << YELLOW_BG;
                    // We have "consumed" the match for this guessed letter: null the column
                    for (int k = i + 1; k < 5; ++k) colour_matrix[k][j] = 0;
                    break;
                }
            }
            // If match row is null, set to grey
            if (j == 5) colourings << WHITE_BG;
        }
        colourings << guess[i];
        colourings << RESET_BG;
    }
    return colourings;
}

int main(void) {
    const auto current_unix_time = std::chrono::system_clock::now();

    auto storage = make_storage(DB_NAME,
        make_table("Wordles",
            make_column("word", &Wordles::word, primary_key()),
            make_column("wasUsed", &Wordles::wasUsed)
        ),
        make_table("WordleHistory",
            make_column("startOfUseUnix", &WordleHistory::startOfUseUnix, primary_key()),
            make_column("word", &WordleHistory::word)
        )
    );
    // Get remaining wordle count
    int remaining_words = storage.count<Wordles>(where(c(&Wordles::wasUsed) != 1));

    // Generate (uniformly) random index into Wordle list, and retrieve
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distrib(0, remaining_words - 1);

    auto randomWordle = storage.get_all<Wordles>(where(c(&Wordles::wasUsed) != 1), limit(1, offset(distrib(gen))));

    // If no wordles leave a nice little message
    if (randomWordle.size() == 0) {
        std::cerr << "We seem to have run out of wordles... we'll work on making some new ones\n";
        return 0;
    }

    std::string word_of_day = randomWordle[0].word;

    int turns = 0;
    bool solved = false;
    while (!solved && turns < 6) {
        std::string input;
        std::cout << "Wordle: ";
        std::cin >> input;

        if (input == "q" || input == "exit") return 0;

        if (wordleInvalid(input)) {
            std::cout << "That wasn't five letters...\n";
            continue;
        }
        int word_exists = storage.get_all<Wordles>(where(c(&Wordles::word) == input)).size();
        if (!word_exists) {
            std::cout << "That word is not in our dictionary\n";
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
    if (turns == 6) std::cout << "The word was " << word_of_day << std::endl;
    return 0;
}
