#pragma once

#include "Common.h"

// 데이터베이스 관련 함수 선언
bool connectDB(const char* dbPath);
void closeDB();
bool createTables();

// 쿼리 실행 헬퍼 함수
bool executeQuery(const char* sql);
bool executeQuery(const std::string& sql);