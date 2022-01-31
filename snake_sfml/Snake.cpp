//
//  Snake.cpp
//  snake_sfml
//
//  Created by Dmitry Iv on 30.01.2022.
//  Copyright © 2022 Dmitry Iv. All rights reserved.
//

#include "Headers/Snake.h"
#include <iostream>

// MARK: - SnakeNode

Snake::SnakeNode::SnakeNode(float x, float y) {
    point = sf::Vector2f(x, y);
}

Snake::SnakeNode::~SnakeNode() {
    if (next) {
        delete next;
    }
}

void Snake::SnakeNode::setNewPoint(sf::Vector2f newPoint) {
    prevPoint = point;
    point = newPoint;
}

void Snake::SnakeNode::moveNext() {
    if (next) {
        next->setNewPoint(prevPoint);
        next->moveNext();
    }
}

// MARK: - Snake

Snake::Snake(float fieldMaxX, float fieldMaxY)
: maxX(fieldMaxX)
, maxY(fieldMaxY)
{
    std::random_device randomDevice;
    std::default_random_engine randomEngine(randomDevice());
    std::uniform_int_distribution<uint8_t> xDistribution(4, static_cast<uint8_t>(maxX - 4));
    std::uniform_int_distribution<uint8_t> yDistribution(4, static_cast<uint8_t>(maxY - 4));
    float startX = static_cast<float>(xDistribution(randomEngine));
    float startY = static_cast<float>(yDistribution(randomEngine));
    head = new SnakeNode(startX, startY);
    tail = head;
    makeBigger(startX, startY - 1);
    makeBigger(startX, startY - 2);
}

Snake::~Snake() {
    delete head;
}

void Snake::makeBigger(float x, float y) {
    SnakeNode* newPiece = new SnakeNode(x, y);
    tail->next = newPiece;
    newPiece->prev = tail;
    tail = newPiece;
}

std::vector<sf::Vector2f> Snake::snakePosition() const {
    std::vector<sf::Vector2f> res;
    SnakeNode* node = head;
    while (node) {
        res.push_back(node->point);
        node = node->next;
    }
    
    return res;
}

void Snake::makeMove(SnakeDirection direction) {
    float newX = head->point.x;
    float newY = head->point.y;
    
    switch (direction) {
        case SnakeDirection::Up:
            newY--;
            break;
        case SnakeDirection::Right:
            newX++;
            break;
        case SnakeDirection::Down:
            newY++;
            break;
        case SnakeDirection::Left:
            newX--;
            break;
    }
    
    if (newX < 0 || newX > maxX || newY < 0 || newY > maxY) {
        // check borders
        isDead = true;
    }
    else {
        // check if we bumped into ourselves
        std::vector<sf::Vector2f> nodesPositions = snakePosition();
        sf::Vector2f newPoint(newX, newY);
        if (std::find(nodesPositions.begin(),
                      nodesPositions.end(),
                      newPoint) != nodesPositions.end())
        {
            isDead = true;
        }
        else {
            head->setNewPoint(newPoint);
            head->moveNext();
        }
    }
}

bool Snake::isDirectionAllowed(SnakeDirection direction) const {
    bool res;
    
    switch (direction) {
        case SnakeDirection::Up:
            res = (head->point.y - 1) != head->next->point.y;
            break;
        case SnakeDirection::Right:
            res = (head->point.x + 1) != head->next->point.x;
            break;
        case SnakeDirection::Down:
            res = (head->point.y + 1) != head->next->point.y;
            break;
        case SnakeDirection::Left:
            res = (head->point.x - 1) != head->next->point.x;
            break;
    }
    
    return res;
}

bool Snake::tryEat(sf::Vector2f applePosition) {
    bool res = false;
    
    // Choosing when to add new node depending on what is tail look like
    if (applePosition == head->point) {
        float x = tail->point.x, y = tail->point.y;
        
        // as we call tryEat after makeMove, we should check "prevPoint", not just "point"
        if (tail->prevPoint.y == tail->prev->prevPoint.y) {
            x += (tail->prevPoint.x - tail->prev->prevPoint.x) > 0 ? 1 : -1;
        } else {
            y += (tail->prevPoint.y - tail->prev->prevPoint.y) > 0 ? 1 : -1;
        }
        
        makeBigger(x, y);
        res = true;
    }
    
    return res;
}
