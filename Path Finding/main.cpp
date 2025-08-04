#include <imgui.h>
#include <imgui-SFML.h>
#include <SFML/Graphics.hpp>
#include "Grid.h"
#include "Astar.h"

constexpr float FPS = 60.0f;

static sf::Vector2f getmousePos(sf::RenderWindow& window) {
    sf::Vector2i pixelPos = sf::Mouse::getPosition(window);
    sf::Vector2f worldPos = window.mapPixelToCoords(pixelPos);
    return worldPos;
}

static void displayNodeData(Node node)
{
    const auto name = node.gridPos;
    const auto state = node.attribute.state;
    const auto F = node.attribute.F;
    const auto G = node.attribute.G;
    const auto H = node.attribute.H;
    const auto parent = node.parent;

    ImGui::Text("Node: (%d, %d)", name.x, name.y);

    switch (state)
    {
    case NodeState::Unblocked: ImGui::Text("State: Unblocked"); break;
    case NodeState::Blocked: ImGui::Text("State: Blocked"); break;
    case NodeState::Visited: ImGui::Text("State: Visited"); break;
    case NodeState::Target: ImGui::Text("State: Target"); break;
    case NodeState::Source: ImGui::Text("State: Source"); break;
    case NodeState::Path: ImGui::Text("State: Path"); break;
    }

    if (state == NodeState::Path || state == NodeState::Visited)
    {
        ImGui::Text("F: %f, G: %f, H: %f", F, G, H);
        ImGui::Text("Parent: (%d, %d)", parent.x, parent.y);
    }
    else if (state == NodeState::Target)
    {
        ImGui::Text("F: None, G: None, H: None");
        if (parent == sf::Vector2i{ -1, -1 })
            ImGui::Text("Parent: None");
        else
            ImGui::Text("Parent: (%d, %d)", parent.x, parent.y);
    }
    else
    {
        ImGui::Text("F: None, G: None, H: None");
        ImGui::Text("Parent: None");
    }
}

static void printError(Astar& a_star) {
    auto error = a_star.getError();
    switch (error)
    {
    case Unknown:
        ImGui::Text("Error: Unknown!");
        break;
    case NoSourceNode:
        ImGui::Text("Error: No Starting Node selected!");
        break;
    }
}

int main()
{
    sf::RenderWindow window(sf::VideoMode(1900, 1000), "Pathfinding", sf::Style::Close);
    window.setFramerateLimit(65);
    ImGui::SFML::Init(window);

    sf::RectangleShape backGround;
    sf::Vector2f windowSize =
        [&window]() -> sf::Vector2f {
        auto Usize = window.getSize();
        return sf::Vector2f{ static_cast<float>(Usize.x), static_cast<float>(Usize.y) };
        }();

    backGround.setSize({ windowSize.x - Node::guiMarginRight, windowSize.y });
    backGround.setFillColor(sf::Color::White);
    backGround.setPosition({ 0.0f, 0.0f });

    Grid grid(window, backGround);
    grid.initialize();
    Astar a_star(grid);

    // slider Method
    static int method = Manhattan_Distance;
    const char* method_names[Method_Count] = { "Manhattan Distance", "Diagonal Distance", "Euclidean Distance" };

    // Node size
    static int nodeSize = 0;

    //debug window
    bool display_node_data = false;

    static int delayMs = 0;
    static bool wantDelay = false;

    sf::Clock clock;

    while (window.isOpen())
    {
        sf::Event event;

        while (window.pollEvent(event))
        {
            ImGui::SFML::ProcessEvent(event);
            sf::Vector2f mousePos = getmousePos(window);
            if (event.type == sf::Event::Closed)
                window.close();

            if (event.type == sf::Event::MouseButtonPressed) {
                if (event.mouseButton.button == sf::Mouse::Left && sf::Keyboard::isKeyPressed(sf::Keyboard::LControl))
                    grid.updateColor(mousePos, NodeState::Source);

                else if (event.mouseButton.button == sf::Mouse::Right && sf::Keyboard::isKeyPressed(sf::Keyboard::LControl))
                    grid.updateColor(mousePos, NodeState::Target);
            }

            if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Space) {
                    grid.Reset();
                    a_star.clearContainers();
                }

                else if (event.key.code == sf::Keyboard::Escape)
                    window.close();
            }
        }

        ImGui::SFML::Update(window, clock.restart());

        sf::Vector2f mousePos = getmousePos(window);
        if (!sf::Keyboard::isKeyPressed(sf::Keyboard::LControl)) {
            if (sf::Mouse::isButtonPressed(sf::Mouse::Right))
                grid.updateColor(mousePos, NodeState::Blocked);

            else if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
                grid.updateColor(mousePos, NodeState::Unblocked); // Fix: reset states
        }

        // Input window
        ImGui::SetNextWindowSize(ImVec2(400, 600), ImGuiCond_Always);
        ImGui::SetNextWindowPos(ImVec2(1500, 0), ImGuiCond_Always);

        if (ImGui::Begin("Input", nullptr, ImGuiWindowFlags_NoResize))
        {
            // Run algorithm
            ImGui::SeparatorText("Run Algorithm");
            if (ImGui::Button("Start A*")) {
                if (wantDelay)
                    a_star.startSearch(delayMs);
                else
                    a_star.searchPath();
            }

            //Method Slider
            ImGui::SeparatorText("Choose Heuristic Method");
            const char* method_name = (method >= 0 && method < Method_Count) ? method_names[method] : "Unknown";
            ImGui::SliderInt("Method", &method, 0, Method_Count - 1, method_name);

            // Resize node
            ImGui::SeparatorText("Resize Node");
            if (ImGui::SliderInt("Size", &nodeSize, 10, 100))
                grid.reinitialize(static_cast<float>(nodeSize));

            // display node data
            ImGui::SeparatorText("Debug Tools");
            ImGui::Checkbox("Display Node Data", &display_node_data);

            //Miscellaneous
            ImGui::SeparatorText("Miscellaneous");

            ImGui::Checkbox("Want Delay?", &wantDelay);
            if (wantDelay)
                ImGui::SliderInt("Delay", &delayMs, 0, 100);

            if (ImGui::Button("Clear Grid")) {
                grid.Reset();
                a_star.clearContainers();
                a_star.resetAstar();
            }
        }
        ImGui::End();

        //output window
        ImGui::SetNextWindowSize(ImVec2(400, 400), ImGuiCond_Always);
        ImGui::SetNextWindowPos(ImVec2(1500, 600), ImGuiCond_Always);

        ImGui::Begin("Output", nullptr, ImGuiWindowFlags_NoResize);

        printError(a_star);
        if (display_node_data) {
            auto maybenode = grid.on_mouse_hover(mousePos);
            if (maybenode.has_value()) {
                auto node = *maybenode;
                displayNodeData(node);
            }
        }

        ImGui::End();

        if (method == Manhattan_Distance)
            a_star.setMethod(Manhattan_Distance);
        else if (method == Diagonal_Distance)
            a_star.setMethod(Diagonal_Distance);
        else if (method == Euclidean_Distance)
            a_star.setMethod(Euclidean_Distance);

        if (wantDelay) {
            if (a_star.isSearchRunning()) {
                bool finished = a_star.stepSearch();
                if (finished) {
                    a_star.tracePath();
                }
            }
        }

        window.clear();
        window.draw(backGround);
        grid.draw();
        ImGui::SFML::Render(window);
        window.display();
    }

    ImGui::SFML::Shutdown();
    return 0;
}