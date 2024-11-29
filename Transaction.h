#pragma once

#include "Common.h"

struct Transaction {
    int id;
    std::string type;
    double amount;
    std::string category;
    std::string description;
    std::string date;
};

struct MonthlyStats {
    std::string month;
    double income;
    double expense;
    MonthlyStats(const std::string& m, double i, double e)
        : month(m), income(i), expense(e) {}
};

struct CategoryStats {
    std::string category;
    double amount;
    CategoryStats(const std::string& c, double a)
        : category(c), amount(a) {}
};

// 거래 관련 함수 선언
bool addTransaction(const std::string& type, double amount,
    const std::string& category, const std::string& description);
std::vector<Transaction> getTransactions(const std::string& whereClause = "");
std::vector<MonthlyStats> getMonthlyStats(int months = 6);
std::vector<CategoryStats> getCategoryStats(const std::string& type = "expense");