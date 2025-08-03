#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include "Node.h"
#include <iostream>
#include <optional>
#include <unordered_set>

struct Vector2i_Hash {
    size_t operator()(const sf::Vector2i& p) const {
        return std::hash<int>()(p.x) ^ (std::hash<int>()(p.y) << 1);
    }
};

class Grid {
private:
    int cols, rows;
    float size;
    float guiMarginRight = 100.f;

    Position sourcePos = { -1, -1 };
    Position targetPos = { -1, -1 };

    sf::RenderWindow* window;
    sf::RectangleShape* drawable_area;

    std::vector<std::vector<Node>> nodes;

public:
    Grid(sf::RenderWindow& window, sf::RectangleShape& background);

    void draw();

    void updateColor(Pos mousePos, NodeState state);
    void Reset();

    std::optional<Node> on_mouse_hover(Pos mousePos);

    std::vector<std::vector<Node>>& getNodeData() { return nodes; }
    Position getDimensions();

    void initialize();
    void reinitialize(float newSize, float newMarginRight = 400.f);
};