#include <iostream>

#include "db_wrapper.hh"

WordleDataBase::WordleDataBase(std::string _database_name) {
    const char *database_name = _database_name.c_str();

    int return_code = sqlite3_open(database_name, &db);

    if (return_code != 0) {
        std::cerr << "Error opening database:\n" << sqlite3_errmsg(db) << std::endl;
        throw std::errc::not_connected;
    }
}

WordleDataBase::~WordleDataBase() {
    sqlite3_close(db);
}

int callback(void *pword, int argc, char **argv, char **azColName) {
    if (argc < 1) return 0;
    std::string *word = (std::string *)pword;
    word->append(argv[0]);
    return 0;
}

std::string WordleDataBase::getWordle() {
    char* sql = "SELECT * FROM Wordles LIMIT 1";
    char* zErrMsg = 0;
    std::string word;
    int return_code = sqlite3_exec(db, sql, callback, (void*)&word, &zErrMsg);

    if (return_code != SQLITE_OK) {
      std::cerr << "SQL error: " << zErrMsg << std::endl;
      sqlite3_free(zErrMsg);
      throw std::exception();
    }

    return word;
}
