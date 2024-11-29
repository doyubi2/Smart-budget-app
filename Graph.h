#pragma once
#include "Common.h"
#include <vector>
#include <string>
#include <cmath>

struct GraphPoint {
    float x;
    float y;
    GraphPoint(float _x = 0.0f, float _y = 0.0f) : x(_x), y(_y) {}
};

class Graph {
protected:
    SDL_Renderer* renderer;
    TTF_Font* font;
    SDL_Rect bounds;
    std::vector<GraphPoint> points;
    std::vector<std::string> labels;
    double maxValue;
    double minValue;

public:
    Graph(SDL_Renderer* _renderer, TTF_Font* _font, const SDL_Rect& _bounds);
    virtual ~Graph() = default;

    virtual void addPoint(float x, float y, const std::string& label = "");
    virtual void clear();
    virtual void render() = 0;

protected:
    void updateRange(double value);
    void renderAxis();
};

class LineGraph : public Graph {
public:
    LineGraph(SDL_Renderer* renderer, TTF_Font* font, const SDL_Rect& bounds);
    void render() override;
};

class PieChart : public Graph {
private:
    SDL_Point center;
    int radius;
    std::vector<std::pair<std::string, double>> segments;
    double total;

public:
    PieChart(SDL_Renderer* renderer, TTF_Font* font, SDL_Point center, int radius);
    void addSegment(const std::string& label, double value, SDL_Color color);
    void clear() override;
    void render() override;

private:
    void renderSegment(double startAngle, double endAngle, SDL_Color color, const std::string& label);
};