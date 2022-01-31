#include <iostream>
#include <chrono>
#include <random>

#include <SFML/Graphics.hpp>
#include "Headers/ResourcePath.hpp"
#include "Headers/Snake.h"
#include "Headers/Apple.h"

const short int SCORE_LABEL_HEIGHT = 40;
const uint8_t CELL_SIZE = 20;
const uint8_t CELL_BORDER_WIDTH = 0;
const uint8_t ROWS = 15;
const uint8_t COLUMNS = 15;
const uint8_t SCREEN_SCALE = 3;
const double SECONDS_PER_FRAME = 0.016666; // 60 frames per second
const uint8_t START_SNAKE_SLOWNESS = 20; // lower value makes snake faster
const uint8_t HEAD_FLASH_INTERVAL = 50;
const uint8_t HEAD_FLASH_DURATION = 15;
const uint8_t TAIL_VISIBLE_DURATION = 60;
const uint8_t EATEN_APPLES_TO_INCREASE_SPEED = 3;
const sf::Color COLOR_CELL = sf::Color(75, 75, 75);
const sf::Color COLOR_SNAKE = sf::Color(85, 115, 195);
const sf::Color COLOR_SNAKE_HEAD = sf::Color(115, 145, 225);
const sf::Color COLOR_SNAKE_DEAD = sf::Color(40, 40, 40);
const sf::Color COLOR_APPLE = sf::Color(185, 3, 29);
const sf::Color COLOR_APPLE_DEAD = sf::Color(90, 5, 20);

sf::Font font;
Snake* snake = new Snake(static_cast<float>(COLUMNS - 1) , static_cast<float>(ROWS - 1));
Apple apple(snake->snakePosition(), static_cast<float>(COLUMNS - 1) , static_cast<float>(ROWS - 1));
std::vector<std::vector<uint8_t>> field(COLUMNS, std::vector<uint8_t>(ROWS));
bool isGameOver = false;
uint8_t timeToMoveSnake = 0;
uint8_t timeToHeadFlash = 0;
uint8_t tailVisibleDuration = 0;
uint8_t headFlashDuration = HEAD_FLASH_DURATION;
uint8_t currentSnakeSlowness = START_SNAKE_SLOWNESS;
SnakeDirection snakeDirection = SnakeDirection::Down;
short int eatenApples = 0;

void resetGame() {
    isGameOver = false;
    eatenApples = 0;
    timeToMoveSnake = 0;
    currentSnakeSlowness = START_SNAKE_SLOWNESS;
    snakeDirection = SnakeDirection::Down;
    
    delete snake;
    snake = new Snake(static_cast<float>(COLUMNS - 1) , static_cast<float>(ROWS - 1));
    apple.generateNewPosition(snake->snakePosition());
}

// MARK: - processInput
void processInput(sf::RenderWindow& window) {
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            window.close();
        }
        
        if (event.type == sf::Event::KeyPressed) {
            SnakeDirection newDirection = snakeDirection;
            
            switch (event.key.code) {
                case sf::Keyboard::Escape:
                    window.close();
                case sf::Keyboard::Enter:
                    if (isGameOver) {
                        resetGame();
                        return;
                    }
                    break;
                case sf::Keyboard::Up:
                    newDirection = SnakeDirection::Up;
                    break;
                case sf::Keyboard::Right:
                    newDirection = SnakeDirection::Right;
                    break;
                case sf::Keyboard::Down:
                    newDirection = SnakeDirection::Down;
                    break;
                case sf::Keyboard::Left:
                    newDirection = SnakeDirection::Left;
                    break;
                default:
                    break;
            }
            
            if (!isGameOver && snake->isDirectionAllowed(newDirection)) {
                snakeDirection = newDirection;
            }
        }
    }
}

// MARK: - updateGameState
void updateGameState() {
    timeToMoveSnake++;
    if (timeToMoveSnake == currentSnakeSlowness) {
        snake->makeMove(snakeDirection);
        timeToMoveSnake = 0;
        tailVisibleDuration = TAIL_VISIBLE_DURATION;
        
        if (snake->isDead) {
            isGameOver = true;
        } else {
            if (snake->tryEat(apple.position)) {
                eatenApples++;
                apple.generateNewPosition(snake->snakePosition());

                if (eatenApples % EATEN_APPLES_TO_INCREASE_SPEED == 0) {
                    currentSnakeSlowness--;
                }
            }
        }
    }
}

// MARK: - draw
void draw(sf::RenderWindow& window) {
    window.clear();
    
    sf::RectangleShape cell(sf::Vector2f(CELL_SIZE - CELL_BORDER_WIDTH,
                                         CELL_SIZE - CELL_BORDER_WIDTH));
    
    // Drawing the field
    for (uint8_t col = 0; col < COLUMNS; col++) {
        for (uint8_t row = 0; row < ROWS; row++) {
            cell.setPosition(static_cast<float>(CELL_SIZE * col), static_cast<float>(CELL_SIZE * row));
            cell.setFillColor(COLOR_CELL);

            window.draw(cell);
        }
    }
   
    // Drawing the apple
    cell.setPosition(sf::Vector2f(apple.position.x * CELL_SIZE, apple.position.y * CELL_SIZE));
    cell.setFillColor(isGameOver ? COLOR_APPLE_DEAD : COLOR_APPLE);
    window.draw(cell);
    
    // Drawing the snake
    std::vector<sf::Vector2f> snakePosition = snake->snakePosition();
    for (int i = 0; i < snakePosition.size(); i++) {
        const sf::Vector2f& pos = snakePosition[i];
        
        cell.setPosition(sf::Vector2f(pos.x * CELL_SIZE, pos.y * CELL_SIZE));
        
        sf::Color snakeColor;
        if (isGameOver) {
            snakeColor = COLOR_SNAKE_DEAD;
        } else if (i == 0) {
            // to distinguish head from the body
            if (timeToHeadFlash == HEAD_FLASH_INTERVAL) {
                snakeColor = COLOR_SNAKE_HEAD;
                timeToHeadFlash = 0;
                headFlashDuration = HEAD_FLASH_DURATION;
            } else if (headFlashDuration > 0) {
                snakeColor = COLOR_SNAKE_HEAD;
                headFlashDuration--;
            } else {
                snakeColor = COLOR_SNAKE;
                timeToHeadFlash++;
            }
        } else if (i == snakePosition.size() - 1) {
            // to make snake moving a little smoothier
            snakeColor = COLOR_SNAKE;
            if (tailVisibleDuration > 0) {
                snakeColor.a = 255 * (static_cast<double>(tailVisibleDuration) / TAIL_VISIBLE_DURATION);
                tailVisibleDuration--;
            }
        } else {
            snakeColor = COLOR_SNAKE;
        }
        
        cell.setFillColor(snakeColor);
        window.draw(cell);
    }
    
    // Drawing the score
    sf::Text text("Score : " + std::to_string(eatenApples) , font);
    text.setCharacterSize(15);
    text.setPosition(CELL_SIZE, ROWS * CELL_SIZE + SCORE_LABEL_HEIGHT * 0.2);
    text.setFillColor(sf::Color::White);
    window.draw(text);
    
    if (isGameOver) {
        // Drawing "Game Over" sign
        sf::Text gameOverText("YOU DIED" , font);
        gameOverText.setCharacterSize(40);
        
        sf::FloatRect textRect = gameOverText.getLocalBounds();
        
        gameOverText.setOrigin(textRect.left + textRect.width / 2.0,
                               textRect.top  + textRect.height / 2.0);
        gameOverText.setPosition(sf::Vector2f(window.getView().getSize().x / 2.0,
                                              window.getView().getSize().y / 2.3));
        gameOverText.setFillColor(sf::Color(149, 13, 13));
        window.draw(gameOverText);
    }
    
    // Update the window
    window.display();
}

// MARK: - main
int main() {
    // Create the main window
    sf::RenderWindow window(sf::VideoMode(COLUMNS * CELL_SIZE * SCREEN_SCALE - CELL_BORDER_WIDTH
                                          , (ROWS * CELL_SIZE + SCORE_LABEL_HEIGHT) * SCREEN_SCALE - CELL_BORDER_WIDTH)
                            , "SNAKE"
                            , sf::Style::Close);
    window.setView(sf::View(sf::FloatRect(0,
                                          0,
                                          CELL_SIZE * COLUMNS - CELL_BORDER_WIDTH,
                                          CELL_SIZE * ROWS - CELL_BORDER_WIDTH + SCORE_LABEL_HEIGHT)));
    
    font.loadFromFile(resourcePath() + "sansation.ttf");
    
    auto previousTime = std::chrono::steady_clock::now();
    double lag = 0.0;
    
    // Start the game loop
    while (window.isOpen()) {
        auto currentTime = std::chrono::steady_clock::now();
        std::chrono::duration<double> elapsedTime = currentTime - previousTime;
        
        previousTime = currentTime;
        lag += elapsedTime.count();
        
        while (lag >= SECONDS_PER_FRAME) {
            // Process events
            processInput(window);
            updateGameState();
            
            lag -= SECONDS_PER_FRAME;
            if (lag < SECONDS_PER_FRAME) {
                draw(window);
            }
        }
    }
}
