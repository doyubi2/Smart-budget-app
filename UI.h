#pragma once
#include "Common.h"
#include "Transaction.h"
#include "Graph.h"

// 전역 변수 선언
extern int selectedCategory;
extern std::vector<Button> mainButtons;
extern std::vector<Transaction> currentTransactions;
extern Transaction* selectedTransaction;

// 버튼 관련 함수
bool isMouseOverButton(const Button& button, int mouseX, int mouseY);
void renderButton(const Button& button);

// 화면 렌더링 함수
void renderMainScreen();
void renderInputScreen();
void renderTransactionsScreen();
void renderStatisticsScreen();

// 이벤트 처리 함수
void handleMainScreenEvents(SDL_Event& e);
void handleInputScreenEvents(SDL_Event& e);
void handleTransactionScreenEvents(SDL_Event& e);
void handleStatisticsScreenEvents(SDL_Event& e);