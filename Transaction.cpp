#include "Transaction.h"
#include "Database.h"

bool addTransaction(const std::string& type, double amount,
    const std::string& category, const std::string& description) {
    const char* sql = "INSERT INTO transactions (type, amount, category, description) "
        "VALUES (?, ?, ?, ?);";
    sqlite3_stmt* stmt;

    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        std::cout << "Failed to prepare statement: " << sqlite3_errmsg(db) << std::endl;
        return false;
    }

    sqlite3_bind_text(stmt, 1, type.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_double(stmt, 2, amount);
    sqlite3_bind_text(stmt, 3, category.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 4, description.c_str(), -1, SQLITE_STATIC);

    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    return (rc == SQLITE_DONE);
}

std::vector<Transaction> getTransactions(const std::string& whereClause) {
    std::vector<Transaction> transactions;
    std::string sql = "SELECT * FROM transactions ";
    if (!whereClause.empty()) {
        sql += "WHERE " + whereClause;
    }
    sql += " ORDER BY date DESC;";

    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        std::cout << "Failed to fetch transactions: " << sqlite3_errmsg(db) << std::endl;
        return transactions;
    }

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        Transaction t;
        t.id = sqlite3_column_int(stmt, 0);
        t.type = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        t.amount = sqlite3_column_double(stmt, 2);
        t.category = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
        t.description = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));
        t.date = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 5));
        transactions.push_back(t);
    }

    sqlite3_finalize(stmt);
    return transactions;
}

std::vector<MonthlyStats> getMonthlyStats(int months) {
    std::vector<MonthlyStats> stats;
    const char* sql = R"(
        SELECT strftime('%Y-%m', date) as month,
               SUM(CASE WHEN type = 'income' THEN amount ELSE 0 END) as income,
               SUM(CASE WHEN type = 'expense' THEN amount ELSE 0 END) as expense
        FROM transactions
        GROUP BY month
        ORDER BY month DESC
        LIMIT ?;
    )";

    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        std::cout << "Failed to prepare monthly stats: " << sqlite3_errmsg(db) << std::endl;
        return stats;
    }

    sqlite3_bind_int(stmt, 1, months);

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        std::string month = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        double income = sqlite3_column_double(stmt, 1);
        double expense = sqlite3_column_double(stmt, 2);
        stats.push_back(MonthlyStats(month, income, expense));
    }

    sqlite3_finalize(stmt);
    return stats;
}

std::vector<CategoryStats> getCategoryStats(const std::string& type) {
    std::vector<CategoryStats> stats;
    const char* sql = R"(
        SELECT category, SUM(amount) as total
        FROM transactions
        WHERE type = ?
        GROUP BY category
        ORDER BY total DESC;
    )";

    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        std::cout << "Failed to prepare category stats: " << sqlite3_errmsg(db) << std::endl;
        return stats;
    }

    sqlite3_bind_text(stmt, 1, type.c_str(), -1, SQLITE_STATIC);

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        std::string category = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        double amount = sqlite3_column_double(stmt, 1);
        stats.push_back(CategoryStats(category, amount));
    }

    sqlite3_finalize(stmt);
    return stats;
}