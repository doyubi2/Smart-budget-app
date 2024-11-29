#pragma execution_character_set("utf-8")
#define SDL_MAIN_HANDLED
#define UNICODE
#include <windows.h>
#include "Common.h"
#include "Database.h"
#include "Transaction.h"
#include "UI.h"
#include "Graph.h"

// 전역 변수 정의
SDL_Window* window = nullptr;
SDL_Renderer* renderer = nullptr;
TTF_Font* font = nullptr;
sqlite3* db = nullptr;

ScreenMode currentScreen = ScreenMode::MAIN;
InputMode currentMode = InputMode::NONE;
std::string inputAmount = "";
std::string inputDescription = "";
std::string composingText = "";
bool isDescriptionFocused = false;
int currentPage = 0;
const int ITEMS_PER_PAGE = 10;

bool init() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cout << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
        return false;
    }

    if (TTF_Init() < 0) {
        std::cout << "TTF could not initialize! TTF_Error: " << TTF_GetError() << std::endl;
        return false;
    }

    window = SDL_CreateWindow("가계부", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
        800, 600, SDL_WINDOW_SHOWN);
    if (window == nullptr) {
        std::cout << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        return false;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == nullptr) {
        std::cout << "Renderer could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        return false;
    }

    // 폰트 로드 시도
    const char* fontPaths[] = {
        "C:\\Windows\\Fonts\\malgun.ttf",    // 맑은 고딕
        "C:\\Windows\\Fonts\\gulim.ttc",     // 굴림
        "C:\\Windows\\Fonts\\batang.ttc"     // 바탕
    };

    for (const char* path : fontPaths) {
        font = TTF_OpenFont(path, 16);
        if (font != nullptr) break;
    }

    if (font == nullptr) {
        std::cout << "Failed to load any font! TTF_Error: " << TTF_GetError() << std::endl;
        return false;
    }

    return connectDB("budget.db");
}

void close() {
    closeDB();

    if (font) {
        TTF_CloseFont(font);
        font = nullptr;
    }

    if (renderer) {
        SDL_DestroyRenderer(renderer);
        renderer = nullptr;
    }

    if (window) {
        SDL_DestroyWindow(window);
        window = nullptr;
    }

    TTF_Quit();
    SDL_Quit();
}

int main(int argc, char* argv[]) {
    if (!init()) {
        MessageBoxA(NULL, "초기화 실패!", "Error", MB_OK | MB_ICONERROR);
        return -1;
    }

    SDL_StartTextInput();
    bool quit = false;
    SDL_Event e;

    while (!quit) {
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                quit = true;
            }
            else if (currentMode != InputMode::NONE) {
                handleInputScreenEvents(e);
            }
            else {
                switch (currentScreen) {
                case ScreenMode::MAIN:
                    if (e.type == SDL_MOUSEMOTION) {
                        int mouseX, mouseY;
                        SDL_GetMouseState(&mouseX, &mouseY);
                        for (auto& button : mainButtons) {
                            button.isHovered = isMouseOverButton(button, mouseX, mouseY);
                        }
                    }
                    else if (e.type == SDL_MOUSEBUTTONDOWN) {
                        int mouseX, mouseY;
                        SDL_GetMouseState(&mouseX, &mouseY);
                        for (size_t i = 0; i < mainButtons.size(); i++) {
                            if (isMouseOverButton(mainButtons[i], mouseX, mouseY)) {
                                switch (i) {
                                case 0:
                                    currentMode = InputMode::INCOME;
                                    break;
                                case 1:
                                    currentMode = InputMode::EXPENSE;
                                    break;
                                case 2:
                                    currentScreen = ScreenMode::TRANSACTIONS;
                                    currentTransactions = getTransactions();
                                    currentPage = 0;
                                    break;
                                case 3:
                                    currentScreen = ScreenMode::STATISTICS;
                                    break;
                                }
                                break;
                            }
                        }
                    }
                    break;
                case ScreenMode::TRANSACTIONS:
                    handleTransactionScreenEvents(e);
                    break;
                case ScreenMode::STATISTICS:
                    handleStatisticsScreenEvents(e);
                    break;
                }
            }
        }

        // 렌더링
        if (currentMode != InputMode::NONE) {
            renderInputScreen();
        }
        else {
            switch (currentScreen) {
            case ScreenMode::MAIN:
                renderMainScreen();
                break;
            case ScreenMode::TRANSACTIONS:
                renderTransactionsScreen();
                break;
            case ScreenMode::STATISTICS:
                renderStatisticsScreen();
                break;
            }
        }

        SDL_RenderPresent(renderer);
    }

    SDL_StopTextInput();
    close();
    return 0;
}