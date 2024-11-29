#include "Database.h"

bool connectDB(const char* dbPath) {
    int rc = sqlite3_open(dbPath, &db);
    if (rc) {
        std::cout << "Can't open database: " << sqlite3_errmsg(db) << std::endl;
        return false;
    }
    return createTables();
}

void closeDB() {
    if (db) {
        sqlite3_close(db);
        db = nullptr;
    }
}

bool createTables() {
    const char* sql =
        "CREATE TABLE IF NOT EXISTS transactions ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "type TEXT NOT NULL,"
        "amount REAL NOT NULL,"
        "category TEXT NOT NULL,"
        "description TEXT,"
        "date DATETIME DEFAULT CURRENT_TIMESTAMP"
        ");";

    char* errMsg = 0;
    int rc = sqlite3_exec(db, sql, 0, 0, &errMsg);
    if (rc != SQLITE_OK) {
        std::cout << "SQL error: " << errMsg << std::endl;
        sqlite3_free(errMsg);
        return false;
    }
    return true;
}

bool executeQuery(const char* sql) {
    char* errMsg = 0;
    int rc = sqlite3_exec(db, sql, 0, 0, &errMsg);
    if (rc != SQLITE_OK) {
        std::cout << "SQL error: " << errMsg << std::endl;
        sqlite3_free(errMsg);
        return false;
    }
    return true;
}

bool executeQuery(const std::string& sql) {
    return executeQuery(sql.c_str());
}