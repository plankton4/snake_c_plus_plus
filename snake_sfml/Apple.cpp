//
//  Apple.cpp
//  snake_sfml
//
//  Created by Dmitry Iv on 30.01.2022.
//  Copyright © 2022 Dmitry Iv. All rights reserved.
//

#include "Headers/Apple.h"
#include <iostream>

Apple::Apple(std::vector<sf::Vector2f> snakePosition, float fieldMaxX, float fieldMaxY)
: randomEngine(randomDevice())
, xDistribution(0, static_cast<uint8_t>(fieldMaxX))
, yDistribution(0, static_cast<uint8_t>(fieldMaxY))
{
    generateNewPosition(snakePosition);
}

void Apple::generateNewPosition(std::vector<sf::Vector2f> snakePosition) {
    float x = 0.0, y = 0.0;
    sf::Vector2f newPosition;
    
    std::vector<sf::Vector2f> forbiddenPoints;
    forbiddenPoints.push_back(position);
    
    // to not conflict with snake
    for (sf::Vector2f& pos: snakePosition) {
        forbiddenPoints.push_back(pos);
    }
    
    do {
        x = static_cast<float>(xDistribution(randomEngine));
        y = static_cast<float>(yDistribution(randomEngine));
        newPosition = sf::Vector2f(x, y);
    } while (std::find(forbiddenPoints.begin(),
                       forbiddenPoints.end(),
                       newPosition) != forbiddenPoints.end());

    position = newPosition;
}
