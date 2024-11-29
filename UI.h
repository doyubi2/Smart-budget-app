#pragma once
#include "Common.h"
#include "Transaction.h"
#include "Graph.h"

// ���� ���� ����
extern int selectedCategory;
extern std::vector<Button> mainButtons;
extern std::vector<Transaction> currentTransactions;
extern Transaction* selectedTransaction;

// ��ư ���� �Լ�
bool isMouseOverButton(const Button& button, int mouseX, int mouseY);
void renderButton(const Button& button);

// ȭ�� ������ �Լ�
void renderMainScreen();
void renderInputScreen();
void renderTransactionsScreen();
void renderStatisticsScreen();

// �̺�Ʈ ó�� �Լ�
void handleMainScreenEvents(SDL_Event& e);
void handleInputScreenEvents(SDL_Event& e);
void handleTransactionScreenEvents(SDL_Event& e);
void handleStatisticsScreenEvents(SDL_Event& e);