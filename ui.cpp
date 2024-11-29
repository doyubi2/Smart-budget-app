#include "UI.h"
#include <map>
#include <utility>
#include <algorithm>
#include <cmath>
#include <memory>
#include <string>
#include <vector>
#include <sstream>
#include <iomanip>

// 전역 변수 정의
int selectedCategory = 0;
std::vector<Button> mainButtons;
std::vector<Transaction> currentTransactions;
Transaction* selectedTransaction = nullptr;

bool isMouseOverButton(const Button& button, int mouseX, int mouseY) {
    return mouseX >= button.rect.x && mouseX <= button.rect.x + button.rect.w &&
        mouseY >= button.rect.y && mouseY <= button.rect.y + button.rect.h;
}

void renderButton(const Button& button) {
    if (button.isHovered) {
        SDL_SetRenderDrawColor(renderer, BLUE.r, BLUE.g, BLUE.b, BLUE.a);
    }
    else {
        SDL_SetRenderDrawColor(renderer, GRAY.r, GRAY.g, GRAY.b, GRAY.a);
    }
    SDL_RenderFillRect(renderer, &button.rect);

    SDL_SetRenderDrawColor(renderer, BLACK.r, BLACK.g, BLACK.b, BLACK.a);
    SDL_RenderDrawRect(renderer, &button.rect);

    renderText(button.text, button.rect.x + 10, button.rect.y + 10,
        button.isHovered ? WHITE : BLACK);
}

void renderMainScreen() {
    SDL_SetRenderDrawColor(renderer, WHITE.r, WHITE.g, WHITE.b, WHITE.a);
    SDL_RenderClear(renderer);

    renderText("가계부", 350, 50, BLACK);

    std::vector<std::string> buttonTexts = { "수입 입력", "지출 입력", "거래 내역", "통계" };
    mainButtons.clear();

    for (size_t i = 0; i < buttonTexts.size(); ++i) {
        Button button = { {300, 150 + static_cast<int>(i) * 80, 200, 50},
                        buttonTexts[i], false };
        mainButtons.push_back(button);
        renderButton(button);
    }
}

void renderInputScreen() {
    SDL_SetRenderDrawColor(renderer, WHITE.r, WHITE.g, WHITE.b, WHITE.a);
    SDL_RenderClear(renderer);

    std::string title = (currentMode == InputMode::INCOME) ? "수입 입력" : "지출 입력";
    renderText(title, 350, 50, BLACK);

    SDL_Rect amountRect = { 300, 100, 200, 30 };
    SDL_SetRenderDrawColor(renderer, WHITE.r, WHITE.g, WHITE.b, WHITE.a);
    SDL_RenderFillRect(renderer, &amountRect);
    SDL_SetRenderDrawColor(renderer, BLACK.r, BLACK.g, BLACK.b, BLACK.a);
    SDL_RenderDrawRect(renderer, &amountRect);
    renderText("금액: " + inputAmount, 310, 105, BLACK);

    std::vector<std::string> categories =
        (currentMode == InputMode::INCOME)
        ? std::vector<std::string>{"급여", "상여금", "기타수입"}
    : std::vector<std::string>{ "식비", "교통비", "주거비", "의류비" };

    for (size_t i = 0; i < categories.size(); ++i) {
        SDL_Rect categoryRect = { 300 + static_cast<int>(i) * 110, 180, 100, 30 };
        if (static_cast<int>(i) == selectedCategory) {
            SDL_SetRenderDrawColor(renderer, BLUE.r, BLUE.g, BLUE.b, BLUE.a);
            SDL_RenderFillRect(renderer, &categoryRect);
            renderText(categories[i], 310 + i * 110, 185, WHITE);
        }
        else {
            SDL_SetRenderDrawColor(renderer, GRAY.r, GRAY.g, GRAY.b, GRAY.a);
            SDL_RenderDrawRect(renderer, &categoryRect);
            renderText(categories[i], 310 + i * 110, 185, BLACK);
        }
    }

    SDL_Rect descRect = { 300, 250, 200, 30 };
    SDL_SetRenderDrawColor(renderer, WHITE.r, WHITE.g, WHITE.b, WHITE.a);
    SDL_RenderFillRect(renderer, &descRect);
    SDL_SetRenderDrawColor(renderer, BLACK.r, BLACK.g, BLACK.b, BLACK.a);
    SDL_RenderDrawRect(renderer, &descRect);
    renderText("설명: " + inputDescription + (isDescriptionFocused ? "_" : ""),
        310, 255, BLACK);

    Button saveButton = { {350, 300, 100, 30}, "저장", false };
    renderButton(saveButton);

    Button cancelButton = { {460, 300, 100, 30}, "취소", false };
    renderButton(cancelButton);
}
// ... (이전 코드에 이어서)

void renderTransactionsScreen() {
    SDL_SetRenderDrawColor(renderer, WHITE.r, WHITE.g, WHITE.b, WHITE.a);
    SDL_RenderClear(renderer);

    renderText("거래 내역", 350, 20, BLACK);

    int startIdx = currentPage * ITEMS_PER_PAGE;
    int endIdx = std::min(startIdx + ITEMS_PER_PAGE,
        static_cast<int>(currentTransactions.size()));

    for (int i = startIdx; i < endIdx; ++i) {
        const auto& trans = currentTransactions[i];
        int y = 70 + (i - startIdx) * 40;

        SDL_Rect itemRect = { 50, y, 700, 35 };
        if (selectedTransaction && selectedTransaction->id == trans.id) {
            SDL_SetRenderDrawColor(renderer, BLUE.r, BLUE.g, BLUE.b, 50);
            SDL_RenderFillRect(renderer, &itemRect);
        }

        renderText(trans.date.substr(0, 10), 60, y + 10, BLACK);

        SDL_Color typeColor = (trans.type == "income") ? BLUE : BLACK;
        renderText(trans.type == "income" ? "수입" : "지출", 160, y + 10, typeColor);

        renderText(trans.category, 240, y + 10, BLACK);

        std::stringstream ss;
        ss << std::fixed << std::setprecision(0) << trans.amount;
        renderText(ss.str(), 380, y + 10, typeColor);

        renderText(trans.description, 480, y + 10, BLACK);
    }

    if (!currentTransactions.empty()) {
        int totalPages = (currentTransactions.size() + ITEMS_PER_PAGE - 1) / ITEMS_PER_PAGE;
        std::string pageText = "페이지 " + std::to_string(currentPage + 1) +
            " / " + std::to_string(totalPages);
        renderText(pageText, 350, 500, BLACK);

        if (currentPage > 0) {
            Button prevButton = { {250, 500, 80, 30}, "이전", false };
            renderButton(prevButton);
        }
        if (currentPage < totalPages - 1) {
            Button nextButton = { {450, 500, 80, 30}, "다음", false };
            renderButton(nextButton);
        }
    }

    Button backButton = { {50, 500, 80, 30}, "뒤로", false };
    renderButton(backButton);
}

void renderStatisticsScreen() {
    SDL_SetRenderDrawColor(renderer, WHITE.r, WHITE.g, WHITE.b, WHITE.a);
    SDL_RenderClear(renderer);

    renderText("통계", 350, 20, BLACK);

    static std::unique_ptr<LineGraph> monthlyGraph =
        std::make_unique<LineGraph>(renderer, font, SDL_Rect{ 50, 70, 700, 200 });
    monthlyGraph->clear();

    std::vector<Transaction> transactions = getTransactions();
    std::map<std::string, std::pair<double, double>> monthlyData;

    for (const auto& trans : transactions) {
        std::string monthKey = trans.date.substr(0, 7);
        if (trans.type == "income") {
            monthlyData[monthKey].first += trans.amount;
        }
        else {
            monthlyData[monthKey].second += trans.amount;
        }
    }

    int idx = 0;
    for (const auto& item : monthlyData) {
        monthlyGraph->addPoint(static_cast<float>(idx),
            static_cast<float>(item.second.first),
            item.first);
        monthlyGraph->addPoint(static_cast<float>(idx),
            static_cast<float>(item.second.second),
            "");
        ++idx;
    }
    monthlyGraph->render();

    SDL_SetRenderDrawColor(renderer, BLUE.r, BLUE.g, BLUE.b, BLUE.a);
    SDL_Rect incomeRect = { 50, 280, 20, 20 };
    SDL_RenderFillRect(renderer, &incomeRect);
    renderText("수입", 80, 280, BLACK);

    SDL_SetRenderDrawColor(renderer, BLACK.r, BLACK.g, BLACK.b, BLACK.a);
    SDL_Rect expenseRect = { 150, 280, 20, 20 };
    SDL_RenderFillRect(renderer, &expenseRect);
    renderText("지출", 180, 280, BLACK);

    static std::unique_ptr<PieChart> incomePie =
        std::make_unique<PieChart>(renderer, font, SDL_Point{ 200, 400 }, 80);
    static std::unique_ptr<PieChart> expensePie =
        std::make_unique<PieChart>(renderer, font, SDL_Point{ 600, 400 }, 80);

    incomePie->clear();
    expensePie->clear();

    auto incomeStats = getCategoryStats("income");
    auto expenseStats = getCategoryStats("expense");

    for (const auto& stat : incomeStats) {
        incomePie->addSegment(stat.category, stat.amount, BLUE);
    }

    for (const auto& stat : expenseStats) {
        expensePie->addSegment(stat.category, stat.amount, BLACK);
    }

    renderText("수입 카테고리", 150, 320, BLACK);
    incomePie->render();

    renderText("지출 카테고리", 550, 320, BLACK);
    expensePie->render();

    Button backButton = { {50, 500, 80, 30}, "뒤로", false };
    renderButton(backButton);
}