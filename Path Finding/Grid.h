#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include "Node.h"
#include <iostream>

class Grid {
private:
    int cols, rows;
    float size;
    float guiMarginRight = 100.f;

    Position sourcePos = { -1, -1 };
    Position targetPos = { -1, -1 };

    sf::RenderWindow* window;
    std::vector<std::vector<Node>> nodes;

    void printNodeData(Node& node);

public:
    Grid(sf::RenderWindow& window);

    void draw();
    void updateColor(Pos mousePos, NodeState state);
    void Reset();
    void mouseHover(Pos mousePos);

    std::vector<std::vector<Node>>& getNodeData() { return nodes; }
    Position getDimensions();
    void reinitialize(float newSize, float newMarginRight = 100.f);
};