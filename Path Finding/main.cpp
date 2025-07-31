#include <imgui.h>
#include <imgui-SFML.h>
#include <SFML/Graphics.hpp>
#include "Grid.h"
#include "Astar.h"

static sf::Vector2f getmousePos(sf::RenderWindow& window) {
    sf::Vector2i pixelPos = sf::Mouse::getPosition(window);
    sf::Vector2f worldPos = window.mapPixelToCoords(pixelPos);
    return worldPos;
}

int main()
{
    sf::RenderWindow window(sf::VideoMode(1900, 1000), "Pathfinding", sf::Style::Close);
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

    Astar a_star(grid);

    // slider Method
    static int method = Manhattan_Distance;
    const char* method_names[Method_Count] = { "Manhattan Distance", "Diagonal Distance", "Euclidean Distance" };

    // Node size
    static int nodeSize = 50;

    //debug window
    static bool display_node_data = false;

    sf::Clock deltaClock;
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
                if (event.key.code == sf::Keyboard::Space)
                    grid.Reset();
                else if (event.key.code == sf::Keyboard::Escape)
                    window.close();
            }
        }

        ImGui::SFML::Update(window, deltaClock.restart());

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
                a_star.clearContainers(); // Add: reset states before running
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

            // toggle debug window
            ImGui::SeparatorText("Debug Tools");
            ImGui::Checkbox("Display Node Data", &display_node_data);
            ImGui::Text("Checkbox state: %s", display_node_data ? "ON" : "OFF");
        }
        ImGui::End();

        //output window
        ImGui::SetNextWindowSize(ImVec2(400, 400), ImGuiCond_Always);
        ImGui::SetNextWindowPos(ImVec2(1500, 600), ImGuiCond_Always);

        if (ImGui::Begin("Output", nullptr, ImGuiWindowFlags_NoResize))
        {
            if (display_node_data == true)
            {
                Node* node = grid.on_mouse_hover(mousePos);
                std::cout << "Should Print data\n";

                if (node)
                {
                    std::cout << "Node Data\n";
                    auto name = node->getWorldPosition();
                    auto state = node->getState();

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
                        ImGui::Text("F: %f, G: %f, H: %f", node->getFcost(), node->getGcost(), node->getHcost());
                        ImGui::Text("Parent: (%d, %d)", node->getParent().x, node->getParent().y);
                    }
                    else if (state == NodeState::Target)
                    {
                        ImGui::Text("F: None, G: None, H: None");
                        ImGui::Text("Parent: (%d, %d)", node->getParent().x, node->getParent().y);
                    }
                    else
                    {
                        ImGui::Text("F: None, G: None, H: None");
                        ImGui::Text("Parent: None");
                    }
                }
                else ImGui::Text("No node under mouse.");
            }
        }
        ImGui::End();

        if (method == Manhattan_Distance)
            a_star.setMethod(Manhattan_Distance);
        else if (method == Diagonal_Distance)
            a_star.setMethod(Diagonal_Distance);
        else if (method == Euclidean_Distance)
            a_star.setMethod(Euclidean_Distance);

        window.clear();
        window.draw(backGround);
        grid.draw();
        ImGui::SFML::Render(window);
        window.display();
    }

    ImGui::SFML::Shutdown();
    return 0;
}