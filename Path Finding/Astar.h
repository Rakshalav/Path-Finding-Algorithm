#pragma once

#include "Grid.h"
#include "Node.h"
#include <vector>
#include <queue>
#include <set>
#include <utility>
#include <unordered_set>
#include <iostream>
#include <chrono>
#include "imgui.h"

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

enum Error {
	NoError, Unknown, NoSourceNode
};

class Astar
{
private:
	Grid& grid;

	// containers
	std::vector<std::vector<Node>>& nodes;
	std::set<Pair, Compare> openList;
	std::unordered_set<Position, Vector2i_Hash> closedList;

	Method method;
	Error error;

	bool isRunning = false;
	std::chrono::steady_clock::time_point lastStepTime;
	int delayMs = 0;

	// helper functions
	bool areEmpty();
	bool isValid(Position position); 
	bool isUnblocked(Position position); 
	bool isDestination(Position position); 
	float calculateHval(Position currentPos);

public:
	Astar(Grid& _grid) : grid(_grid), nodes(_grid.getNodeData()), error(NoError) {}
	void clearContainers();
	void resetAstar();
	void searchPath();

	void startSearch(int delay);   
	bool stepSearch();            
	bool isSearchRunning() const { return isRunning; }
	void tracePath();

	Error getError() { return error; }

	//setters
	void setMethod(Method newMethod) { method = newMethod; }
};

