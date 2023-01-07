#include <SFML/Graphics.hpp>
#include <iostream>
#include <cstdlib>  // rand()
#include <vector>
#include <string>
#include <time.h>   // srand()
#include <fstream>

enum class Direction { UP, DOWN, LEFT, RIGHT };
enum class Speed { VERYSLOW, SLOW, NORMAL, FAST, VERYFAST };
enum class Action { RESTART, QUIT };
enum class GameState { RUNNING, PAUSE, LOSE };

int count;                   // ate food count
int currentScore;            // current score
//std::string speedStr;
//std::string countStr;
//std::string scoreStr;
sf::Vector2i fieldSize(25, 20);
std::vector<sf::Vector2i> snake = { {3, 4} };
sf::Vector2i food(rand() % fieldSize.x, rand() % fieldSize.y);

void init() {
    snake.clear();
    snake.push_back({3, 4});

    food.x = rand() % fieldSize.x;
    food.y = rand() % fieldSize.y;
    count = 0;
    currentScore = 0;
}

/** 
 * Set 4 different color of food,
 * and each of them has its own score.
 * Green  (152, 251, 152): 1
 * Blue   (30,144,255):    2
 * Violet (238,130,238):   3
 * Orange (255,165,0):     4
 */
//sf::Color foodColor;
//sf::Color foodRandomColor() {
//    int random = rand() % 4 + 1; // random 1-4
//    // we have 4 different color of food
//    switch (random) {
//    case 1:  // green
//        foodColor = sf::Color(152, 251, 152);
//        currentScore += 1;
//        break;
//    case 2:  // blue
//        foodColor = sf::Color(30, 144, 255);
//        currentScore += 2;
//        break;
//    case 3:  // violet
//        foodColor = sf::Color(238, 130, 238);
//        currentScore += 3;
//        break;
//    case 4:  // orange
//        foodColor = sf::Color(255, 165, 0);
//        currentScore += 4;
//        break;
//    }
//    return foodColor;
//}

void renderFood(sf::RenderWindow& window, sf::CircleShape& circle) {
    circle.setRadius(16.f);
    sf::Vector2f circleSize(
        circle.getGlobalBounds().width,
        circle.getGlobalBounds().height);
    sf::Vector2f foodPos(
        food.x * circleSize.x,
        food.y * circleSize.y);
    circle.setPosition(foodPos);
    //circle.setFillColor(foodRandomColor());
    circle.setFillColor(sf::Color(152, 251, 152));
    window.draw(circle);
}

void renderSnake(sf::RenderWindow& window, sf::Sprite & block, sf::Vector2f& size) {
    for (const sf::Vector2i& body : snake) {
        sf::Vector2f bodyPos(
            body.x * size.x,
            body.y * size.y);
        block.setPosition(bodyPos);
        window.draw(block);
    }
}

void SpeedChange(Speed speed, float& frame, float& defaultFrame, std::string& speedStr) {
    switch (speed) {
    case Speed::VERYSLOW:
        frame = defaultFrame * 4;
        speedStr = "Very Slow";
        break;
    case Speed::SLOW:
        frame = defaultFrame * 2;
        speedStr = "Slow";
        break;
    case Speed::NORMAL:
        frame = defaultFrame;
        speedStr = "Normal";
        break;
    case Speed::FAST:
        frame = defaultFrame / 2;
        speedStr = "Fast";
        break;
    case Speed::VERYFAST:
        frame = defaultFrame / 4;
        speedStr = "Very Fast";
        break;
    }
}

int main() {
    srand((unsigned int)time(NULL));

    float win_w, win_h;

    sf::Font font;
    if (!font.loadFromFile("arial.ttf")) {
        std::cout << "arial.ttf is not found." << std::endl;
        return EXIT_FAILURE;
    }

    sf::Text text;       // score
    sf::Text speedText;  // speed
    sf::Text warnText;   // warning
    sf::Text hintText;   // hint
    sf::Text timeText;   // timer

    text.setFont(font);
    text.setString("Score");

    speedText.setFont(font);
    warnText.setFont(font);
    hintText.setFont(font);
    timeText.setFont(font);
    
    hintText.setString("Space key: pause game,\n1-5 key: change speed,\nEsc key: quit game.");
    hintText.setCharacterSize(20);

    std::string speedStr;
    std::string countStr;

    sf::Texture blockTecture;
    if (!blockTecture.loadFromFile("block.png")) {
        std::cout << "block.png is not found." << std::endl;
        return EXIT_FAILURE;
    }

    sf::Sprite block(blockTecture);
    sf::Vector2f blockSize(
        block.getGlobalBounds().width,
        block.getGlobalBounds().height);

    sf::CircleShape circle;

    win_w = fieldSize.x * blockSize.x;
    win_h = fieldSize.y * blockSize.y;
    sf::VideoMode mode(unsigned int(win_w), unsigned int(win_h + 70));
    sf::RenderWindow renderWin(mode, "Snake Game");

    sf::RectangleShape rect(sf::Vector2f(renderWin.getView().getSize().x, (70 - hintText.getLocalBounds().height) / 2));

    Direction direction = Direction::RIGHT;
    Speed speed = Speed::NORMAL;
    Action action = Action::RESTART;
    GameState gameState = GameState::RUNNING;

    sf::Clock clock, timer;
    float elapsedTime = timer.getElapsedTime().asSeconds();
    //int hour = 0, minute = 0, second = 0;
    std::string totalTime;

    bool isDead = false;   // check if snake is dead
    float defaultFrame = 0.28f;  // normal speed
    float frame = 0.f;

    while (renderWin.isOpen()) {
        sf::Event evt;
        if (renderWin.pollEvent(evt)) {
            switch (evt.type) {
            case sf::Event::Closed:
                renderWin.close();
                break;
            case sf::Event::KeyPressed:
                switch (evt.key.code) {
                case sf::Keyboard::Up:
                    direction = Direction::UP;
                    break;
                case sf::Keyboard::Down:
                    direction = Direction::DOWN;
                    break;
                case sf::Keyboard::Left:
                    direction = Direction::LEFT;
                    break;
                case sf::Keyboard::Right:
                    direction = Direction::RIGHT;
                    break;
                case sf::Keyboard::Space:
                    if (gameState == GameState::RUNNING) {
                        gameState = GameState::PAUSE;
                    } else if (gameState == GameState::PAUSE) {
                        gameState = GameState::RUNNING;
                    }
                    break;
                case sf::Keyboard::Escape:
                    action = Action::QUIT;
                    break;
                case sf::Keyboard::Numpad1:
                case sf::Keyboard::Num1:
                    speed = Speed::VERYSLOW;
                    break;
                case sf::Keyboard::Numpad2:
                case sf::Keyboard::Num2:
                    speed = Speed::SLOW;
                    break;
                case sf::Keyboard::Numpad3:
                case sf::Keyboard::Num3:
                    speed = Speed::NORMAL;
                    break;
                case sf::Keyboard::Numpad4:
                case sf::Keyboard::Num4:
                    speed = Speed::FAST;
                    break;
                case sf::Keyboard::Numpad5:
                case sf::Keyboard::Num5:
                    speed = Speed::VERYFAST;
                    break;
                }
                break;
            }
        }
        
        if (clock.getElapsedTime().asSeconds() >= frame) {
            sf::Vector2i head = snake[0];  // snake's head
            // move snake
            switch (direction) {
            case Direction::UP:    head.y--; break;
            case Direction::DOWN:  head.y++; break;
            case Direction::LEFT:  head.x--; break;
            case Direction::RIGHT: head.x++; break;
            default: break;
            }

            if (head.x < 0 || head.x >= fieldSize.x ||
                head.y < 0 || head.y >= fieldSize.y) {
                isDead = true;
            }

            // avoid snake touch itself
            for (unsigned int i = 1; i < snake.size(); i++) {
                if (head == snake[i]) {
                    isDead = true;
                }
            }

            if (!isDead) {
                // check if food is eaten by snake
                if (food == head) {
                    // if snake ate the food, appear a new one at random area
                    food.x = rand() % fieldSize.x;
                    food.y = rand() % fieldSize.y;
                    count++;
                    //circle.setFillColor(foodRandomColor());
                    std::cout << "Eaten: " << count << ", Score:" << currentScore << std::endl;
                } else {
                    snake.pop_back();
                }
                snake.insert(snake.begin(), head);
            }
            clock.restart();
        }

        switch (gameState) {
        case GameState::RUNNING:
            warnText.setString("");
            // draw score
            countStr = std::to_string(count);
            //scoreStr = std::to_string(currentScore);
            text.setString("Score: " + countStr);
            //text.setString("Score: " + scoreStr + "\nEaten: " + countStr);
            text.setCharacterSize(24);
            text.setFillColor(sf::Color(255, 255, 0));
            text.setPosition(10.f, win_h + 10.f);

            speedText.setString("Speed: " + speedStr);
            speedText.setCharacterSize(24);
            speedText.setFillColor(sf::Color(192, 192, 192));
            speedText.setPosition((win_w / 2) - (speedText.getLocalBounds().width / 2), win_h + 10.f);

            hintText.setPosition(win_w - hintText.getLocalBounds().width - 4.f, win_h);
            break;
        case GameState::PAUSE:
            warnText.setString("Game paused, \npress space key to resume.");
            warnText.setCharacterSize(24);
            warnText.setStyle(sf::Text::Bold);
            warnText.setFillColor(sf::Color(0, 180, 255));
            warnText.setPosition(10.f, 10.f);
            clock.restart();
            break;
        case GameState::LOSE:
            isDead = true;
            warnText.setCharacterSize(35);
            warnText.setStyle(sf::Text::Bold);
            warnText.setFillColor(sf::Color(244, 164, 96));
            warnText.setString("Game Over!\nYour score: " + countStr + "\npress any key to try again.");
            warnText.setPosition(50.f, 30.f);
            action = Action::RESTART;
            break;
        }
        
        // clear window
        if (isDead) {
            renderWin.clear(sf::Color(210, 0, 0));
            gameState = GameState::LOSE;
            if (action == Action::RESTART) {
                if (evt.type == sf::Event::KeyPressed) {
                    init();
                    isDead = false;
                    warnText.setString("");
                    clock.restart();
                    timer.restart();
                    direction = Direction::RIGHT;
                    gameState = GameState::RUNNING;
                }
            }
        } else {
            renderWin.clear();
        }

        // if snake isn't dead, press Esc to quit the game
        if (!isDead && action == Action::QUIT) {
            renderWin.close();
        }

        if (!isDead) {
            elapsedTime = timer.getElapsedTime().asSeconds();
        }

        SpeedChange(speed, frame, defaultFrame, speedStr);
        // draw food
        renderFood(renderWin, circle);
        // draw snake
        renderSnake(renderWin, block, blockSize);
        // draw rectangle
        rect.setPosition(sf::Vector2f(0, win_h));
        rect.setFillColor(sf::Color(0, 0, 255));
        renderWin.draw(rect);
        // draw text
        renderWin.draw(text);
        renderWin.draw(speedText);
        renderWin.draw(hintText);
        renderWin.draw(warnText);

        totalTime = std::to_string(elapsedTime);
        timeText.setString("Time: " + totalTime);
        timeText.setCharacterSize(20);
        timeText.setStyle(sf::Text::Bold);
        timeText.setFillColor(sf::Color(0, 250, 154));
        timeText.setPosition(win_w - timeText.getGlobalBounds().width, 10.f);
        renderWin.draw(timeText);

        renderWin.display();
    }
    return EXIT_SUCCESS;
}