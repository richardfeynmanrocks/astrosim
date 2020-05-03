//
//  matter.cpp
//  AstroSim
//
//  Created by David Freifeld on 4/18/20.
//  Copyright © 2020 David Freifeld. All rights reserved.
//

#include "matter.hpp"
#include "force.hpp"

Matter::Matter(double massParam, double radiusParam, std::array<double, 2> positionParam, std::array<double, 2> velocityParam)
{
    mass = massParam;
    radius = radiusParam;
    position = positionParam;
    velocity = velocityParam;
    shape.setRadius(radius);
    shape.setFillColor(sf::Color::Red);
}

void Matter::updatePosition()
{
    netForce.components = {0,0};
    netForce.target = this;
    for (Force &force : forces)
    {
        netForce.components[0] += force.components[0];
        netForce.components[1] += force.components[1];
    }
    
    acceleration[0] = netForce.components[0]/mass;
    acceleration[1] = netForce.components[1]/mass;
    
    velocity[0] += acceleration[0];
    velocity[1] += acceleration[1];
    position[0] += velocity[0];
    position[1] += velocity[1];
}
