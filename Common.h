#pragma once

#include <windows.h>
#include <SDL.h>
#include <SDL_ttf.h>
#include <sqlite3.h>
#include <string>
#include <vector>
#include <memory>
#include <iostream>
#include <cmath>

// 화면 모드 열거형 정의
enum class ScreenMode {
    MAIN,
    TRANSACTIONS,
    STATISTICS
};

// 입력 모드 열거형 정의
enum class InputMode {
    NONE,
    INCOME,
    EXPENSE
};

// 기본 구조체
struct Button {
    SDL_Rect rect;
    std::string text;
    bool isHovered;
};

// 전역 변수 선언
extern SDL_Window* window;
extern SDL_Renderer* renderer;
extern TTF_Font* font;
extern sqlite3* db;

// 전역 상태 변수
extern ScreenMode currentScreen;
extern InputMode currentMode;
extern std::string inputAmount;
extern std::string inputDescription;
extern std::string composingText;
extern bool isDescriptionFocused;
extern int currentPage;
extern const int ITEMS_PER_PAGE;

// 색상 정의
const SDL_Color WHITE = { 255, 255, 255, 255 };
const SDL_Color BLACK = { 0, 0, 0, 255 };
const SDL_Color GRAY = { 200, 200, 200, 255 };
const SDL_Color BLUE = { 0, 120, 255, 255 };

// 공통 함수 선언
bool init();
void close();
void renderText(const std::string& text, int x, int y, SDL_Color color);