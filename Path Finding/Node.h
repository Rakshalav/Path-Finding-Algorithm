#pragma once
#include <SFML/Graphics.hpp>

typedef sf::Vector2i Position;
typedef sf::Vector2f Pos;

enum class NodeState {
	Unblocked, Blocked, Target, Source, Path, Visited
};

struct Attribute {
	NodeState state = NodeState::Unblocked;
	float G = FLT_MAX;
	float H = FLT_MAX;
	float F = FLT_MAX;
};

class Node
{
private:
	sf::RectangleShape node;
	Attribute attribute;
	Pos position;
	Pos size;
	Position parent;
	Position gridPos;

	sf::RenderWindow* window;
	sf::Vector2i renderSize;

public:
	inline static float guiMarginRight = 100.f; // right-side margin

	Node();

	void draw() { window->draw(node); }

	bool contains(Pos point) { return node.getGlobalBounds().contains(point); }
	void changeColor(NodeState state);

	// pos converters
	static Pos WorldtoScreen(Position worldpos, float spacing = 50.0f);
	static Position ScreentoWorld(Pos screenpos, float spacing = 50.0f);

	// setters
	void setPosition(const Pos& pos1, const Position& pos2);
	void setState(NodeState newstate) { attribute.state = newstate; }
	void setParent(const Position& P) { parent = P; }
	void setFcost(float f) { attribute.F = f; }
	void setGcost(float g) { attribute.G = g; }
	void setHcost(float h) { attribute.H = h; }
	void Reset(NodeState state);
	void setSize(Pos Size) { size = Size; node.setSize(Size); }
	void setWindow(sf::RenderWindow& win) { window = &win; }
	void setRenderSize(sf::Vector2i size) { renderSize = size; }

	// getters
	Pos& getPosition() { return position; }
	Attribute& getNodeAttribute() { return attribute; }
	float& getFcost() { return attribute.F; }
	float& getGcost() { return attribute.G; }
	float& getHcost() { return attribute.H; }
	NodeState& getState() { return attribute.state; }
	Position& getParent() { return parent; }
	Position& getWorldPosition() { return gridPos; }
	Pos getSize() { return size; }
};