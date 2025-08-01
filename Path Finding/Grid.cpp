#include "Grid.h"

Grid::Grid(sf::RenderWindow& Window, sf::RectangleShape& background) : window(&Window), size(50.f), drawable_area(&background) {
    reinitialize(size, guiMarginRight);
}

Position Grid::getDimensions() {
    int cols = static_cast<int>(drawable_area->getSize().x / size);
    int rows = static_cast<int>(drawable_area->getSize().y / size);
    return { cols, rows };
}

void Grid::reinitialize(float newSize, float newMarginRight) {
    size = newSize;
    guiMarginRight = newMarginRight;
    Node::guiMarginRight = newMarginRight;

    Position dim = getDimensions();
    cols = dim.x;
    rows = dim.y;

    nodes.clear();
    nodes.resize(cols, std::vector<Node>(rows));

    for (int x = 0; x < cols; ++x) {
        for (int y = 0; y < rows; ++y) {
            Position gridPos(x, y);
            auto& node = nodes[x][y];
            node.setWindow(*window);
            node.setPosition(gridPos);
            node.setScreenPos(gridPos, size);
            node.setSize({ size, size });
        }
    }
}

void Grid::draw() {
    for (auto& col : nodes)
        for (auto& node : col)
            node.draw();
}

std::optional<Node> Grid::on_mouse_hover(Pos mousePos) {
    for (auto& col : nodes) {
        for (auto& node : col) {
            if (node.contains(mousePos))
                return node;
        }
    }
    return std::nullopt;
}

void Grid::Reset() {
    for (auto& col : nodes)
        for (auto& node : col) {
            node.setState(NodeState::Unblocked);
            node.Reset(NodeState::Unblocked);
        }
}

void Grid::updateColor(Pos mousePos, NodeState state) {
    for (auto& col : nodes) {
        for (auto& node : col) {
            if (node.contains(mousePos)) {
                Position clickedPos = node.getWorldPosition();
                NodeState current = node.getState();

                if ((state == NodeState::Source && current == NodeState::Target) ||
                    (state == NodeState::Target && current == NodeState::Source)) {
                    return;
                }

                if (state == NodeState::Source) {
                    if (sourcePos != Position(-1, -1)) {
                        Node& prevSource = nodes[sourcePos.x][sourcePos.y];
                        prevSource.setState(NodeState::Unblocked);
                        prevSource.changeColor(NodeState::Unblocked);
                    }

                    sourcePos = clickedPos;
                    node.setState(NodeState::Source);
                    node.changeColor(NodeState::Source);
                }

                else if (state == NodeState::Target) {
                    if (targetPos != Position(-1, -1)) {
                        Node& prevTarget = nodes[targetPos.x][targetPos.y];
                        prevTarget.setState(NodeState::Unblocked);
                        prevTarget.changeColor(NodeState::Unblocked);
                    }

                    targetPos = clickedPos;
                    node.setState(NodeState::Target);
                    node.changeColor(NodeState::Target);
                }

                else {
                    if (current != NodeState::Source && current != NodeState::Target) {
                        node.setState(state);
                        node.changeColor(state);
                    }
                }

                return; 
            }
        }
    }
}
