#pragma once

#include "Grid.h"
#include "Node.h"
#include <vector>
#include <queue>
#include <set>
#include <utility>
#include <unordered_set>
#include <iostream>

typedef std::pair<float, Position> Pair; // [F, Position]

// Lowest F cost has highest priority
struct Compare {
	bool operator()(const Pair& a, const Pair& b) const {
		const auto& Fa = std::get<0>(a);
		const auto& posA = std::get<1>(a);
		const auto& Fb = std::get<0>(b);
		const auto& posB = std::get<1>(b);

		if (Fa != Fb)
			return Fa < Fb;  // Lower F-cost has higher priority

		if (posA.x != posB.x)
			return posA.x < posB.x;
		return posA.y < posB.y;
	}
};

enum Method {
	Manhattan_Distance, Diagonal_Distance, Euclidean_Distance, Method_Count
};

class Astar
{
private:
	Grid& grid;

	// containers
	std::vector<std::vector<Node>>& nodes;
	std::set<Pair, Compare> openList;
	std::unordered_set<Position, Vector2i_Hash> closedList;
	std::vector<Node*> path;

	Method method;

	// helper functions
	bool areEmpty();
	bool isValid(Position position); // to check whether the cell exists or not
	bool isUnblocked(Position position); // to check whether the state of the cell is blocked or not
	bool isDestination(Position position); // to find whether we reached our destination
	void tracePath(); // trace path from source to destination
	float calculateHval(Position currentPos);

public:
	Astar(Grid& _grid) : grid(_grid), nodes(_grid.getNodeData()) {}
	void searchPath();
	void clearContainers();
	void resetAstar();

	//setters
	void setMethod(Method newMethod) { method = newMethod; }
};

