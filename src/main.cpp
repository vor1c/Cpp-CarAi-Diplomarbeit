#include <SFML/Graphics.hpp>
#include <experimental/filesystem>
#include <iostream>
#include "Version.hpp"
#include <fstream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <valarray>

// Funktionen deklarieren
static void modifyCurrentWorkingDirectory();
void generateTrack(std::vector<sf::RectangleShape> &track, sf::RenderWindow &window);
void handleCarMovement(sf::Sprite &carSprite, float deltaTime);

int main()
{
    modifyCurrentWorkingDirectory();

    sf::VideoMode desktop = sf::VideoMode::getDesktopMode();
    sf::RenderWindow window(sf::VideoMode(desktop.width, desktop.height), "Car Game", sf::Style::Fullscreen);

    const auto clearColor = sf::Color(234, 240, 206);

    // Rennstrecke generieren
    std::vector<sf::RectangleShape> track;
    generateTrack(track, window);

    // Auto laden
    sf::Texture carTexture;
    if (!carTexture.loadFromFile("resources/32x32-car.png"))
    {
        std::cerr << "Could not load car texture!" << std::endl;
        return -1;
    }
    sf::Sprite carSprite(carTexture);
    carSprite.setOrigin(16, 16); // Mittig setzen
    carSprite.setPosition(window.getSize().x / 2.f, window.getSize().y / 2.f); // Startposition

    sf::Clock clock;

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed || sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
            {
                window.close();
            }
        }

        float deltaTime = clock.restart().asSeconds();

        // Auto Bewegung steuern
        handleCarMovement(carSprite, deltaTime);

        window.clear(clearColor);

        // Rennstrecke zeichnen
        for (const auto &segment : track)
        {
            window.draw(segment);
        }

        window.draw(carSprite);
        window.display();
    }

    return 0;
}

void modifyCurrentWorkingDirectory()
{
    while (!std::experimental::filesystem::exists("resources"))
    {
        std::experimental::filesystem::current_path(std::experimental::filesystem::current_path().parent_path());
    }
    auto cwd = std::experimental::filesystem::current_path();
}

void generateTrack(std::vector<sf::RectangleShape> &track, sf::RenderWindow &window)
{
    std::srand(static_cast<unsigned>(std::time(nullptr)));
    int segmentCount = 50;
    float segmentWidth = 60.f;
    float segmentHeight = 20.f;

    sf::Vector2f position(window.getSize().x / 2.f, window.getSize().y / 2.f);

    for (int i = 0; i < segmentCount; ++i)
    {
        sf::RectangleShape segment(sf::Vector2f(segmentWidth, segmentHeight));
        segment.setFillColor(sf::Color(100, 100, 100));
        segment.setPosition(position);

        track.push_back(segment);

        // Zufällig die Richtung ändern
        int direction = std::rand() % 4;
        if (direction == 0)
            position.x += segmentWidth;
        else if (direction == 1)
            position.y += segmentHeight;
        else if (direction == 2)
            position.x -= segmentWidth;
        else
            position.y -= segmentHeight;
    }
}

void handleCarMovement(sf::Sprite &carSprite, float deltaTime)
{
    float speed = 200.f; // Geschwindigkeit des Autos
    sf::Vector2f movement(0.f, 0.f);

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
        movement.y -= speed * deltaTime;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
        movement.y += speed * deltaTime;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
        movement.x -= speed * deltaTime;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
        movement.x += speed * deltaTime;

    if (movement.x != 0 || movement.y != 0)
    {
        carSprite.move(movement);

        float angle = std::atan2(movement.y, movement.x) * 180 / 3.14159265;
        carSprite.setRotation(angle + 90); // +90 Grad, um das Auto korrekt auszurichten
    }
}
