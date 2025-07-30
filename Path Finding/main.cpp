#include <imgui.h>
#include <imgui-SFML.h>
#include <SFML/Graphics.hpp>
#include "Grid.h"
#include "Astar.h"

sf::Vector2f getmousePos(sf::RenderWindow& window) {
    sf::Vector2i pixelPos = sf::Mouse::getPosition(window);
    sf::Vector2f worldPos = window.mapPixelToCoords(pixelPos);
    return worldPos;
}

int main()
{
    sf::RenderWindow window(sf::VideoMode(1900, 1000), "Pathfinding", sf::Style::Close);
    ImGui::SFML::Init(window);

    Grid grid(window);
    grid.reinitialize(50.0f, 400.0f);

    Astar a_star(grid);

    static int method = Manhattan_Distance;
    const char* method_names[Method_Count] = { "Manhattan Distance", "Diagonal Distance", "Eucleadian Distance" };

    sf::Clock deltaClock;
    while (window.isOpen())
    {
        sf::Event event;
        sf::Vector2f mousePos = getmousePos(window);


        while (window.pollEvent(event))
        {
            ImGui::SFML::ProcessEvent(event);
            if (event.type == sf::Event::Closed)
                window.close();

            if (event.type == sf::Event::MouseButtonPressed) {
                if (event.mouseButton.button == sf::Mouse::Left && sf::Keyboard::isKeyPressed(sf::Keyboard::LControl))
                    grid.updateColor(mousePos, NodeState::Source);

                else if (event.mouseButton.button == sf::Mouse::Right && sf::Keyboard::isKeyPressed(sf::Keyboard::LControl))
                    grid.updateColor(mousePos, NodeState::Target);

                else if (event.mouseButton.button == sf::Mouse::Middle)
                    grid.mouseHover(mousePos);
            }

            if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Enter) {
                    a_star.clearContainers();
                    a_star.searchPath(); // Add: reset states before running
                }
                else if (event.key.code == sf::Keyboard::Space)
                    grid.Reset();
                else if (event.key.code == sf::Keyboard::Escape)
                    window.close();
            }
        }

        ImGui::SFML::Update(window, deltaClock.restart());

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


        }
        ImGui::End();

        //output window
        ImGui::SetNextWindowSize(ImVec2(400, 400), ImGuiCond_Always);
        ImGui::SetNextWindowPos(ImVec2(1500, 600), ImGuiCond_Always);

        ImGui::Begin("Output", nullptr, ImGuiWindowFlags_NoResize);
        ImGui::End();

        if (method == Manhattan_Distance)
            a_star.setMethod(Manhattan_Distance);
        else if (method == Diagonal_Distance)
            a_star.setMethod(Diagonal_Distance);
        else if (method == Euclidean_Distance)
            a_star.setMethod(Euclidean_Distance);

        window.clear();
        grid.draw();
        ImGui::SFML::Render(window);
        window.display();
    }

    ImGui::SFML::Shutdown();
    return 0;
}