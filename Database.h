#pragma once

#include "Common.h"

// �����ͺ��̽� ���� �Լ� ����
bool connectDB(const char* dbPath);
void closeDB();
bool createTables();

// ���� ���� ���� �Լ�
bool executeQuery(const char* sql);
bool executeQuery(const std::string& sql);