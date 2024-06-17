#include <SFML/Graphics.hpp>
#include <experimental/filesystem>
#include <iostream>
#include "Version.hpp"
#include <fstream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <cmath>

// Funktionen deklarieren
static void modifyCurrentWorkingDirectory();
void generateTrack(std::vector<sf::RectangleShape> &track, sf::RenderWindow &window);
void handleCarMovement(sf::Sprite &carSprite, float &currentRotation, float &carSpeed, float deltaTime);

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

    float currentRotation = 0.f;
    float carSpeed = 0.f;
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
        handleCarMovement(carSprite, currentRotation, carSpeed, deltaTime);

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
    int segmentCount = 100; // Anzahl der Segmente
    float segmentWidth = 80.f; // Breite der Segmente
    float segmentHeight = 30.f; // Höhe der Segmente

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

void handleCarMovement(sf::Sprite &carSprite, float &currentRotation, float &carSpeed, float deltaTime)
{
    const float acceleration = 200.f; // Beschleunigung des Autos
    const float deceleration = 200.f; // Verzögerung des Autos
    const float maxSpeed = 400.f; // Maximalgeschwindigkeit des Autos
    const float rotationSpeed = 100.f; // Geschwindigkeit der Drehung des Autos

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
        carSpeed += acceleration * deltaTime;
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
        carSpeed -= acceleration * deltaTime;
    else
        carSpeed *= (1 - deceleration * deltaTime / maxSpeed);

    carSpeed = std::max(std::min(carSpeed, maxSpeed), -maxSpeed);

    if (carSpeed != 0.f)
    {
        float turnDirection = 0.f;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
            turnDirection = -1.f;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
            turnDirection = 1.f;

        currentRotation += turnDirection * rotationSpeed * deltaTime * (carSpeed / maxSpeed);
        carSprite.setRotation(currentRotation);
    }

    sf::Vector2f movement(std::cos(currentRotation * 3.14159265 / 180) * carSpeed * deltaTime,
                          std::sin(currentRotation * 3.14159265 / 180) * carSpeed * deltaTime);

    carSprite.move(movement);
}
