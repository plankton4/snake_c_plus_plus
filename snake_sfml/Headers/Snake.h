//
//  Snake.h
//  snake_sfml
//
//  Created by Dmitry Iv on 30.01.2022.
//  Copyright © 2022 Dmitry Iv. All rights reserved.
//

#ifndef Snake_h
#define Snake_h

#include <vector>
#include <random>
#include <SFML/System/Vector2.hpp>

enum SnakeDirection {
    Up, Right, Down, Left
};

class Snake
{
public:
    Snake(float fieldMaxX, float fieldMaxY);
    ~Snake();
    
    void makeBigger(float x, float y);
    std::vector<sf::Vector2f> snakePosition() const;
    void makeMove(SnakeDirection);
    bool isDirectionAllowed(SnakeDirection) const;
    bool tryEat(sf::Vector2f applePosition);
    
    bool isDead = false;
    
private:
    struct SnakeNode {
        SnakeNode(float x = 0.0, float y = 0.0);
        ~SnakeNode();
        
        void setNewPoint(sf::Vector2f);
        void moveNext();
        
        sf::Vector2f point = { 0.0, 0.0 };
        sf::Vector2f prevPoint = { 0.0, 0.0 };
        SnakeNode* next = nullptr;
        SnakeNode* prev = nullptr;
    };
    
    SnakeNode* head = nullptr;
    SnakeNode* tail = nullptr;
    float maxX = 0.0;
    float maxY = 0.0;
};

#endif /* Snake_h */
