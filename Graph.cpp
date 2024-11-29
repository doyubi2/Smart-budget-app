#include "Graph.h"
#include <iomanip>
#include <sstream>

Graph::Graph(SDL_Renderer* _renderer, TTF_Font* _font, const SDL_Rect& _bounds)
    : renderer(_renderer), font(_font), bounds(_bounds), maxValue(0), minValue(0) {}

void Graph::addPoint(float x, float y, const std::string& label) {
    points.push_back(GraphPoint(x, y));
    labels.push_back(label);
    updateRange(y);
}

void Graph::clear() {
    points.clear();
    labels.clear();
    maxValue = minValue = 0;
}

void Graph::updateRange(double value) {
    if (points.empty()) {
        maxValue = minValue = value;
    }
    else {
        maxValue = std::max(maxValue, value);
        minValue = std::min(minValue, value);
    }
    maxValue *= 1.1;
}

void Graph::renderAxis() {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

    // X축
    SDL_RenderDrawLine(renderer,
        bounds.x, bounds.y + bounds.h,
        bounds.x + bounds.w, bounds.y + bounds.h);

    // Y축
    SDL_RenderDrawLine(renderer,
        bounds.x, bounds.y,
        bounds.x, bounds.y + bounds.h);

    // Y축 눈금과 값
    int numTicks = 5;
    double valueStep = maxValue / numTicks;
    int pixelStep = bounds.h / numTicks;

    for (int i = 0; i <= numTicks; ++i) {
        int y = bounds.y + bounds.h - i * pixelStep;
        SDL_RenderDrawLine(renderer, bounds.x - 5, y, bounds.x + 5, y);

        std::stringstream ss;
        ss << std::fixed << std::setprecision(0) << (i * valueStep);
        renderText(ss.str(), bounds.x - 50, y - 10, { 0, 0, 0, 255 });
    }

    // 격자 그리기
    SDL_SetRenderDrawColor(renderer, 220, 220, 220, 255);
    for (int i = 1; i < numTicks; ++i) {
        int y = bounds.y + bounds.h - i * pixelStep;
        SDL_RenderDrawLine(renderer, bounds.x, y, bounds.x + bounds.w, y);
    }
}
// ... (이전 코드에 이어서)

LineGraph::LineGraph(SDL_Renderer* renderer, TTF_Font* font, const SDL_Rect& bounds)
    : Graph(renderer, font, bounds) {}

void LineGraph::render() {
    if (points.empty()) return;

    renderAxis();

    float xScale = static_cast<float>(bounds.w) / (points.size() > 1 ? points.size() - 1 : 1);
    float yScale = maxValue > 0 ? static_cast<float>(bounds.h) / maxValue : 0;

    // 수입 선 그리기 (파란색)
    SDL_SetRenderDrawColor(renderer, 41, 128, 185, 255);
    for (size_t i = 1; i < points.size(); i += 2) {
        if (i >= points.size()) break;

        int x1 = bounds.x + static_cast<int>((i - 1) / 2 * xScale);
        int y1 = bounds.y + bounds.h - static_cast<int>(points[i - 1].y * yScale);
        int x2 = bounds.x + static_cast<int>(i / 2 * xScale);
        int y2 = bounds.y + bounds.h - static_cast<int>(points[i].y * yScale);

        SDL_RenderDrawLine(renderer, x1, y1, x2, y2);

        // 데이터 포인트 표시
        SDL_Rect pointRect1 = { x1 - 3, y1 - 3, 6, 6 };
        SDL_RenderFillRect(renderer, &pointRect1);

        if (!labels[i - 1].empty()) {
            renderText(labels[i - 1], x1 - 25, bounds.y + bounds.h + 5, { 0, 0, 0, 255 });
        }
    }

    // 지출 선 그리기 (빨간색)
    SDL_SetRenderDrawColor(renderer, 192, 57, 43, 255);
    for (size_t i = 2; i < points.size(); i += 2) {
        if (i >= points.size()) break;

        int x1 = bounds.x + static_cast<int>((i - 2) / 2 * xScale);
        int y1 = bounds.y + bounds.h - static_cast<int>(points[i - 1].y * yScale);
        int x2 = bounds.x + static_cast<int>((i) / 2 * xScale);
        int y2 = bounds.y + bounds.h - static_cast<int>(points[i].y * yScale);

        SDL_RenderDrawLine(renderer, x1, y1, x2, y2);

        // 데이터 포인트 표시
        SDL_Rect pointRect2 = { x1 - 3, y1 - 3, 6, 6 };
        SDL_RenderFillRect(renderer, &pointRect2);
    }
}

PieChart::PieChart(SDL_Renderer* renderer, TTF_Font* font, SDL_Point center, int radius)
    : Graph(renderer, font, SDL_Rect{ center.x - radius, center.y - radius,
            radius * 2, radius * 2 }),
    center(center), radius(radius), total(0) {}

void PieChart::addSegment(const std::string& label, double value, SDL_Color color) {
    segments.push_back({ label, value });
    total += value;
}

void PieChart::clear() {
    segments.clear();
    total = 0;
}

void PieChart::render() {
    if (segments.empty()) return;

    double startAngle = -M_PI / 2;  // 12시 방향에서 시작

    const SDL_Color colors[] = {
        {41, 128, 185, 255},   // 파란색
        {192, 57, 43, 255},    // 빨간색
        {39, 174, 96, 255},    // 초록색
        {142, 68, 173, 255},   // 보라색
        {243, 156, 18, 255},   // 주황색
        {22, 160, 133, 255},   // 청록색
        {211, 84, 0, 255},     // 진한 주황색
        {52, 73, 94, 255}      // 진한 회색
    };
    const int numColors = sizeof(colors) / sizeof(colors[0]);

    for (size_t i = 0; i < segments.size(); ++i) {
        const auto& segment = segments[i];
        double angle = (segment.second / total) * 2 * M_PI;
        double endAngle = startAngle + angle;

        // 퍼센트 계산
        double percentage = (segment.second / total) * 100;
        std::stringstream ss;
        ss << segment.first << "\n"
            << std::fixed << std::setprecision(1) << percentage << "%\n"
            << std::setprecision(0) << segment.second;

        renderSegment(startAngle, endAngle, colors[i % numColors], ss.str());
        startAngle = endAngle;
    }
}

void PieChart::renderSegment(double startAngle, double endAngle,
    SDL_Color color, const std::string& label) {
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);

    // 부채꼴 그리기
    for (double angle = startAngle; angle < endAngle; angle += 0.01) {
        int x1 = center.x;
        int y1 = center.y;
        int x2 = center.x + static_cast<int>(radius * cos(angle));
        int y2 = center.y + static_cast<int>(radius * sin(angle));
        SDL_RenderDrawLine(renderer, x1, y1, x2, y2);
    }

    // 라벨 위치 계산 및 표시
    double midAngle = (startAngle + endAngle) / 2;
    int labelDistance = radius + 40;
    int labelX = center.x + static_cast<int>(labelDistance * cos(midAngle));
    int labelY = center.y + static_cast<int>(labelDistance * sin(midAngle));

    // 라벨 배경
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 200);
    SDL_Rect labelBg = { labelX - 40, labelY - 30, 80, 60 };
    SDL_RenderFillRect(renderer, &labelBg);

    // 라벨 텍스트
    renderText(label, labelX - 35, labelY - 25, color);
}