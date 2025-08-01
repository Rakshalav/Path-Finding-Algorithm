#include "Astar.h"

bool Astar::isValid(Position position) {
	int cols = grid.getDimensions().x; 
	int rows = grid.getDimensions().y; 

	if (position.x >= 0 && position.x < cols && position.y >= 0 && position.y < rows)
		return true;
	return false;
}

bool Astar::isUnblocked(Position position) {
	if (nodes[position.x][position.y].getState() == NodeState::Unblocked)
		return true;
	return false;
}

bool Astar::isDestination(Position position) {
	if (nodes[position.x][position.y].getState() == NodeState::Target)
		return true;
	return false;
}

float Astar::calculateHval(Position currentPos) {
    Position goal(-1, -1);

	for (auto& col : nodes) {
		for (auto& node : col) {
			if (node.getState() == NodeState::Target) {
				goal = node.getWorldPosition(); 
				break;
			}
		}
		if (goal.x != -1) break;
	}

	float h = 0.0f;

	switch (method) {
		case Manhattan_Distance:
            h = static_cast<float>(std::abs(currentPos.x - goal.x) + std::abs(currentPos.y - goal.y));
			break;

		case Diagonal_Distance: {
			int dx = std::abs(currentPos.x - goal.x);
			int dy = std::abs(currentPos.y - goal.y);
			int D = 1;
            float D2 = static_cast<float>(std::sqrt(2));
			h = D * (dx + dy) + (D2 - 2 * D) * std::min(dx, dy);
			break;
		}

		case Euclidean_Distance:
            h = static_cast<float>(std::sqrt(std::pow(currentPos.x - goal.x, 2) + std::pow(currentPos.y - goal.y, 2)));
			break;
	}

	return h;
}

void Astar::clearContainers() {
    openList.clear();
    closedList.clear();
    path.clear();

    for (auto& col : nodes) {
        for (auto& node : col) {
            auto state = node.getState();
            if (state == NodeState::Path || state == NodeState::Visited)
                node.Reset(NodeState::Unblocked);
            else if (state == NodeState::Target)
                node.setParent({ -1, -1 });
        }
    }
}

void Astar::tracePath() {
    bool foundTarget = false;
    Position targetPos;

    for (auto& col : nodes) {
        for (auto& node : col) {
            if (node.getState() == NodeState::Target) {
                targetPos = node.getWorldPosition();
                foundTarget = true;
                break;
            }
        }
        if (foundTarget)
            break;
    }

    Position current = targetPos;
    while (true) {
        Node& node = nodes[current.x][current.y];
        path.push_back(&node);
        Position parent = node.getParent();
        if (nodes[parent.x][parent.y].getState() == NodeState::Source) {
            path.push_back(&nodes[parent.x][parent.y]);
            break;
        }
        current = parent;
    }

    std::reverse(path.begin(), path.end());
    for (auto* node : path) {
        if (node->getState() != NodeState::Source && node->getState() != NodeState::Target) {
            node->setState(NodeState::Path);
            node->changeColor(NodeState::Path);
        }
    }
}

void Astar::resetVisited() {
    if (!closedList.empty()) {
        for (auto& pos : closedList) {
            auto& node = nodes[pos.x][pos.y];
            auto state = node.getState();
            if (state == NodeState::Visited) {
                node.setState(NodeState::Unblocked);
                node.changeColor(NodeState::Unblocked);
            }
        }
    }
}

void Astar::drawVisited() {
    if (!closedList.empty()) {
        for (const auto& pos : closedList) {
             auto& node = nodes[pos.x][pos.y];
             auto state = node.getState();
             if (state != NodeState::Source && state != NodeState::Target && 
                state != NodeState::Blocked && state != NodeState::Path) {
                node.setState(NodeState::Visited);
                node.changeColor(NodeState::Visited);
             }
        }
    }
    return;
}

bool Astar::areEmpty() {
    if (openList.empty() && closedList.empty() && path.empty())
        return true;
    return false;
}

void Astar::searchPath()
{
    if (!areEmpty()) {
        std::cout << "Either of the containers not empty!\n";
        return;
    }

    bool foundDest = false;

    Node* nodeSource = nullptr;
    for (auto& col : nodes) {
        for (auto& node : col) {
            auto state = node.getState();
            if (state == NodeState::Source) {
                nodeSource = &node;
                break;
            }
        }
        if (nodeSource) break;
    }

    nodeSource->setFcost(0);
    nodeSource->setGcost(0);
    nodeSource->setHcost(0);
    float f_Source = nodeSource->getFcost();
    Position gridPos_Source = nodeSource->getWorldPosition();
    Pair Source = std::make_pair(f_Source, gridPos_Source);
    openList.insert(Source);

    while (!openList.empty())
    {
        auto current = *openList.begin();
        openList.erase(openList.begin());
        auto& pos = std::get<1>(current);
        closedList.insert(pos);

        float fnew, gnew, hnew;

        struct Directions {
            Position N = { 0, 1 };
            Position E = { 1, 0 };
            Position S = { 0, -1 };
            Position W = { -1, 0 };
            Position NE = N + E;
            Position SE = S + E;
            Position SW = S + W;
            Position NW = N + W;
        }; Directions D;

        std::vector<Position> directions;

        if (method == Manhattan_Distance)
            directions = { D.N + pos, D.E + pos, D.S + pos, D.W + pos };
        else
            directions = { D.N + pos, D.NE + pos, D.E + pos, D.SE + pos, D.S + pos, D.SW + pos, D.W + pos, D.NW + pos };

        for (auto& direction : directions) {
            if (isValid(direction)) {
                auto& node = nodes[direction.x][direction.y];
                if (isDestination(direction)) {
                    node.setParent(pos);
                    tracePath();
                    foundDest = true;
                    return;
                }
                else if (!closedList.contains(direction) && isUnblocked(direction)) {
                    float cost = (abs(direction.x - pos.x) == 1 && abs(direction.y - pos.y) == 1) ? 1.414f : 1.0f;
                    gnew = nodes[pos.x][pos.y].getGcost() + cost;
                    hnew = calculateHval(direction);
                    fnew = gnew + hnew;

                    if (node.getFcost() == FLT_MAX || node.getFcost() > fnew) {
                        openList.emplace(std::make_pair(fnew, direction));
                        closedList.insert(direction);
                        node.setParent(pos);
                        node.setGcost(gnew);
                        node.setHcost(hnew);
                        node.setFcost(fnew);
                    }
                }
            }
        }
    }
}