#include "Node.h"

Node::Node() {
	node.setSize({ 50.f, 50.f });
	node.setOutlineColor(sf::Color::Black);
	node.setOutlineThickness(1);
	node.setFillColor(sf::Color::White);
	position = { 0.f, 0.f };
	gridPos = { 0, 0 };
	parent = { -1, -1 };
}

void Node::changeColor(NodeState state) {
	switch (state)
	{
	case NodeState::Unblocked:
		node.setFillColor(sf::Color::White);
		break;
	case NodeState::Blocked:
		node.setFillColor(sf::Color(156, 156, 156));
		break;
	case NodeState::Target:
		node.setFillColor(sf::Color::Red);
		break;
	case NodeState::Source:
		node.setFillColor(sf::Color::Green);
		break;
	case NodeState::Path:
		node.setFillColor(sf::Color::Yellow);
		break;
	case NodeState::Visited:
		node.setFillColor(sf::Color(139, 208, 250));
		break;
	}
}

void Node::setScreenPos(Position gridPos, float spacing) {
	position = { gridPos.x * spacing, gridPos.y * spacing };
	node.setPosition(position);	
}

void Node::Reset(NodeState state) {
	setFcost(FLT_MAX);
	setGcost(FLT_MAX);
	setHcost(FLT_MAX);
	setParent({ -1, -1 });
	changeColor(state);
}