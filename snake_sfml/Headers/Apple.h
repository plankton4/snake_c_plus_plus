//
//  Apple.h
//  snake_sfml
//
//  Created by Dmitry Iv on 30.01.2022.
//  Copyright © 2022 Dmitry Iv. All rights reserved.
//

#ifndef Apple_h
#define Apple_h

#include <random>
#include <SFML/System/Vector2.hpp>

class Apple
{
public:
    Apple(std::vector<sf::Vector2f> snakePosition, float fieldMaxX, float fieldMaxY);
    ~Apple() {};
    void generateNewPosition(std::vector<sf::Vector2f> snakePosition);
    
    sf::Vector2f position = { -1.0, -1.0 }; // because we need not valid position at start
    
private:
    std::random_device randomDevice;
    std::default_random_engine randomEngine;
    std::uniform_int_distribution<uint8_t> xDistribution;
    std::uniform_int_distribution<uint8_t> yDistribution;
};

#endif /* Apple_h */
