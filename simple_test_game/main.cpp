#include <iostream>
#include <SFML/Graphics.hpp>
#include <sstream>
#include <string>
#include <fstream>
#include <ranges>
#include <vector>

class Config {
    public:
    Config();
};

class Speed {
public:
    float speedX;
    float speedY;
    Speed();
    Speed(float x, float y) : speedX(x), speedY(y) {}
};

class CircleWithSpeed {

    public:
    float speedX;
    float speedY;
    sf::CircleShape circle;
    CircleWithSpeed();
    CircleWithSpeed(float speedX, float speedY, const sf::CircleShape &circle): speedX(speedX), speedY(speedY), circle(circle) {}

};

class Circle {


public:
    sf::Color color;
    int radius;
    sf::Vector2f position;
    float speedX;
    float speedY;
    std::string name;
    Circle();
    Circle(sf::Color color, int radius, sf::Vector2f position, std::string name, float speedX, float speedY)
        : color(color), radius(radius), position(position), name(name), speedX(speedX), speedY(speedY) {

    }

};

class Player {

};

int main()
{
    std::ifstream file("../config.txt");
    std::string line;
    std::vector<CircleWithSpeed> circles;
    unsigned int windowWidth{800};
    unsigned int windowHeight{600};
    std::string name {""};
    unsigned int r, g, b;
    int fontSize {14};

    while (std::getline(file, line)) {
        std::vector<std::string> windowTokens;
        std::stringstream ss(line);
        std::string token;
        while (std::getline(ss, token, ' ')) {
            windowTokens.push_back(token);
        }
        if (line.rfind("Window", 0) == 0 ) {
            if (windowTokens.size() != 3) {
                std::cout << windowTokens.size() << std::endl;
                exit(-1);
            }
            try {
                windowWidth = std::stoul(windowTokens[1]);
                windowHeight = std::stoul(windowTokens[2]);
            } catch (const std::invalid_argument& e) {
                std::cout << e.what() << std::endl;
                exit(-1);
            }

        } else if (line.rfind("Font", 0) == 0 ) {
            if (windowTokens.size() != 6) {
                std::cout << windowTokens.size() << std::endl;
                exit(-1);
            }
            fontSize = std::stoi(windowTokens[2]);
            r = std::stoul(windowTokens[3]);
            g = std::stoul(windowTokens[4]);
            b = std::stoul(windowTokens[5]);
        } else if (line.rfind("Circle", 0) == 0 ) {
           if (windowTokens.size() != 10) {
               std::cout << windowTokens.size() << std::endl;
               exit(-1);
           }
           Circle circle = Circle(sf::Color(
                                           std::stoi(windowTokens[6]),
                                           std::stoi(windowTokens[7]),
                                           std::stoi(windowTokens[8])
                                       ),
                                       std::stoi(windowTokens[9]),
                                       sf::Vector2f(std::stof(windowTokens[2]), std::stof(windowTokens[3])),
                                       windowTokens[1],
                                       std::stof(windowTokens[4]),
                                       std::stof(windowTokens[5])
           );

            sf::CircleShape ss(static_cast<float>(circle.radius));
            ss.setPosition(circle.position);
            ss.setFillColor(circle.color);
            CircleWithSpeed csp = CircleWithSpeed(circle.speedX, circle.speedY, ss);
            circles.push_back(csp);
        } else {

        }
    }


    auto window = sf::RenderWindow{ {windowWidth, windowHeight }, "My awesome game" };
    window.setFramerateLimit(144);
    // sf::CircleShape shape(50.f);
    // shape.setFillColor(sf::Color(100, 250, 50));
    // float speedX { 1.f };
    // float speedY { 2.f };
    //
    sf::Font myfont;

    if (!myfont.loadFromFile("../fonts/arial.ttf")) {
        std::cerr << "Failed to load font" << std::endl;
        exit(-1);
    }
    // sf::Text text;
    //
    // // select the font
    // text.setFont(myfont); // font is a sf::Font
    //
    // // set the string to display
    // text.setString("Hello world");
    //
    // // set the character size
    // text.setCharacterSize(fontSize); // in pixels, not points!
    //
    // // set the color
    // text.setFillColor(sf::Color(r, g, b, 255));
    //
    // // set the text style
    // text.setStyle(sf::Text::Bold | sf::Text::Underlined);


    while (window.isOpen())
    {
        for (auto event = sf::Event{}; window.pollEvent(event);)
        {
            if (event.type == sf::Event::Closed)
            {
                window.close();
            }

            if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::F5) {
                    for(auto &cc : circles) {
                        cc.speedX *= 2.0;
                        cc.speedY *= 2.0;
                    }
                }
                if (event.key.code == sf::Keyboard::F4) {
                    for(auto &cc : circles) {
                        cc.speedX /= 2.0;
                        cc.speedY /= 2.0;
                    }
                }
            }
        }
        window.clear();

        for(auto &csp : circles) {

        sf::CircleShape &shape = csp.circle;
        float currentPositionX = shape.getPosition().x;
        float currentPositionY = shape.getPosition().y;
        sf::FloatRect localBounds = shape.getLocalBounds();

        float bottomCollisionY = shape.getPosition().y + localBounds.top + localBounds.height;
        float topCollisionY = shape.getPosition().y + localBounds.top;
        float leftCollisionX = shape.getPosition().x + localBounds.left;
        float rightCollisionX = shape.getPosition().x + localBounds.left + localBounds.width;
        if (bottomCollisionY > window.getSize().y) {
            csp.speedY = -csp.speedY;
        }
        if (topCollisionY < 0) {
            csp.speedY = -csp.speedY;
        }

        if (leftCollisionX < 0 || rightCollisionX > window.getSize().x) {
            csp.speedX = -csp.speedX;
        }

        shape.setPosition(shape.getPosition() + sf::Vector2f(csp.speedX, csp.speedY));
        window.draw(shape);

        // text.setOrigin(text.getLocalBounds().getSize() / 2.f + text.getLocalBounds().getPosition());
        // text.setPosition(shape.getPosition() + (shape.getLocalBounds().getSize() / 2.f));
        // window.draw(text);

        }

        window.display();

        // sf::Text text;
        // text.setFont(sf::Font());
        // text.setString("Ownage");
        // text.setStyle(sf::Text::Bold);
        // text.setFillColor(sf::Color::Red);
        // text.setCharacterSize(130);
        // window.draw(text);


    }
}