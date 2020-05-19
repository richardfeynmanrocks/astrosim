//
//  renderer.cpp
//  AstroSim
//
//  Created by David Freifeld on 4/18/20.
//  Copyright © 2020 David Freifeld. All rights reserved.
//

#include "renderer.hpp"

Renderer::Renderer(int speedParam, sf::RenderWindow * windowParam, int pixelParam)
{
    speed=speedParam;
    window=windowParam;
    pixelLength=pixelParam;
    sf::Vector2u size = {window->getSize().x, window->getSize().y};
    canvas.create(size.x, size.y, sf::Color(0, 0, 0));
//    addMatter(pow(10,0), 1, {10000000000000, 0}, {0,0});
}

Eigen::Vector2d Renderer::fixPosition(Eigen::Vector2d coordinates)
{
    std::array<unsigned int, 2> fixed_coords;
    fixed_coords[0] = round(coordinates[0]);
    fixed_coords[1] = round(coordinates[1]);
    double pixelLength = pow(10, 2);
    fixed_coords[0] /= pixelLength;
    fixed_coords[1] /= pixelLength;
    return Eigen::Vector2d {fixed_coords[0], window->getSize().y-fixed_coords[1]};
}

void Renderer::addMatter(double massParam, double radiusParam, Eigen::Vector2d positionParam, Eigen::Vector2d velocityParam)
{
    matter.emplace_back(massParam, radiusParam, positionParam, velocityParam);
}

void Renderer::initializeForces()
{
    for (int i = 0; i < matter.size(); i++)
    {
        for (int j = 0; j < matter.size(); j++)
        {
            if (&forces[j] == &forces[i])
            {
                continue;
            }
            std::array<double, 2> blank = {0,0}; // for some reason it only works if i initialize it first... type issues?
            std::cout << ":/ " << matter[j].mass << " " << matter[i].mass << "\n";
            forces.emplace_back(blank, matter[j], matter[i]);
            forces[forces.size()-1].updateGravity();
            if (forces[forces.size()-1].warn == true)
            {
                warnCount += 1;
            }
            forces.emplace_back(blank, matter[i], matter[j]);
            forces[forces.size()-1].updateGravity();
            if (forces[forces.size()-1].warn == true)
            {
                warnCount += 1;
            }
            forces[forces.size()-1].updateGravity();
        }
    }
}

void Renderer::removeMatter(int index)
{
    for (int i = 0; i < forces.size(); i++)
    {
//        std::cout << forces[i].target << " " << forces[i].source << " " << &matter[index] << std::endl;
        if (forces[i].target == &matter[index] || forces[i].source == &matter[index])
        {
            forces.erase(forces.begin() + i);
        }
    }
    matter.erase(matter.begin()+index);
}

void Renderer::diagnoseForces()
{
    std::cout << "------BEGIN DIAGNOSIS FOR SIZE " << forces.size() << "------\n";
    for (int i = 0; i < matter.size(); i++)
    {
        std::cout << &matter[i] << "(" << matter[i].mass << "): \n";
        std::cout << "\tFORCES IN: \n";
        for (int j = 0; j < forces.size(); j++)
        {
            if (&matter[i] == forces[j].target)
            {
                std::cout << "\t\t" << forces[j].source << "(" << forces[j].source->mass << ") " << forces[j].target << "(" << forces[j].target->mass << ") " << forces[j].components[0] << " " << forces[j].components[1] << std::endl;
            }
        }
        std::cout << "\tFORCES OUT: \n";
        for (int j = 0; j < forces.size(); j++)
        {
            if (&matter[i] == forces[j].source)
            {
                std::cout << "\t\t" << forces[j].source << "(" << forces[j].source->mass << ") " << forces[j].target << "(" << forces[j].target->mass << ") " << forces[j].components[0] << " " << forces[j].components[1] << std::endl;
            }
        }
    }
    std::cout << "------END DIAGNOSIS------\n";
}

void Renderer::findTrajectory(Matter matter)
{
    if (matter.history.size() >= 5)
    {
        Eigen::MatrixXd points(6,6);
        Eigen::MatrixXd answers(6,1);
        for (int i = 0; i < 6; i++)
        {
            std::array<double,2> pos = matter.history[i];
            points.row(i) << pow(pos[0],2), pos[0]*pos[1], pow(pos[1],2), pos[0], pos[1], 1;
            answers.row(i) << pos[1];
        }
        std::cout << "COEFFICIENTS\n";
        std::cout << points << "\n";
        std::cout << "ANSWERS\n";
        std::cout << answers << "\n";
        Eigen::MatrixXd unknowns (6,1);
        unknowns = points.inverse() * answers;
        std::cout << "UNKNOWNS\n";
        std::cout << unknowns << "\n";
    }
}

void Renderer::traceObjects()
{
    for (int i = 0; i<matter.size(); i++)
    {
        Eigen::Vector2d loc = fixPosition(matter[i].position);
        if (loc[0] < window->getSize().x && loc[1] < window->getSize().y)
        {
            canvas.setPixel(loc[0], loc[1], sf::Color(255,255,255,100));
        }
    }
}

// Strange naming convention is a byproduct of an attempt to roll elastic collision and inelastic collision into the same function
// as they're mainly the same type of calculations. You may wonder why they're even in a separate function: it allows me to separately
// work on collision detection and collisions so one can be functional while the other one isn't!
void Renderer::momentumCollision(Matter a, Matter b, int aIndex, bool elastic)
{
    if (elastic == false)
    {
        b.velocity = (a.mass * a.velocity + b.mass * b.velocity)/(a.mass + b.mass);
        b.mass = a.mass + b.mass;
        removeMatter(aIndex);
    }
    else
    {
        // crap this needs some physics that i don't wanna do rn
    }
}

// Code basically taken from https://ericleong.me/research/circle-circle and adapted some. TODO: Learn the linear algebra behind this!
void Renderer::checkCollisions()
{
    for (int i = 0; i<matter.size(); i++)
    {
        for (int j = 0; j<matter.size(); j++)
        {
            if (&matter[i] == &matter[j])
            {
                continue;
            }
            // Calculate closest point to matter[j] on vector of matter[i]
            double a = matter[i].position[1] - matter[i].prevPosition[1];
            double b = matter[i].position[0] - matter[i].prevPosition[0];
            double c1 = (matter[i].prevPosition[1] - matter[i].position[1])*matter[i].position[0] + (matter[i].prevPosition[0] - matter[i].position[0])*matter[i].position[1];
            double c2 = -b*matter[j].position[0] + a*matter[j].position[1];
            double d = a*a + b*b;
            double cx;
            double cy;
            if (d != 0)
            {
                cx = (a*c1 - b*c2)/d;
                cy = (a*c2 - b*c1)/d;
            }
            else
            {
                cx = matter[j].position[0];
                cy = matter[j].position[1];
            }
            // Calculate
            double distSquared = pow(matter[j].position[0] - cx, 2) + pow(matter[j].position[1] - cy, 2);
            std::cout << "coords: " << cx << " " << cy << std::endl;
            std::cout << distSquared << " " << pow((matter[i].radius + matter[j].radius) * pixelLength, 2) << " for planets " << matter[i].mass << " and " << matter[j].mass << std::endl;
            if (distSquared < pow(matter[i].radius + matter[j].radius, 2) && matter[j].mass > matter[i].mass)
            {
                removeMatter(i);
                std::cout << "Collision!\n";
            }
        }
    }
    
}

void Renderer::updateScene()
{
    for (int i = 0; i < matter.size(); i++)
    {
        matter[i].acceleration = {0,0};
    }
    for (int i = 0; i < forces.size(); i++)
    {
        forces[i].updateGravity();
        if (forces[i].warn == true)
        {
            warnCount += 1;
        }
//        std::cout << "COMP " << forces[i].components[0] << " " << forces[i].components[1] << " " << forces[i].target << " (" << forces[i].target->mass << ") " << forces[i].source << " (" << forces[i].source->mass << ")\n";
//        forces[i].applyForce();
        forces[i].target->netForce.components[0] += forces[i].components[0];
        forces[i].target->netForce.components[1] += forces[i].components[1];
    }
    for (int i = 0; i < matter.size(); i++)
    {
        matter[i].updatePosition();
    }
}

void Renderer::drawScene()
{
    sf::Texture backgroundTexture;
    backgroundTexture.loadFromImage(canvas);
    sf::Sprite backgroundSprite;
    backgroundSprite.setTexture(backgroundTexture);
    window->draw(backgroundSprite);
    for (Matter object : matter)
    {
        object.screenPosition = fixPosition(object.position);
        sf::CircleShape shape (object.radius);
        shape.setPosition(object.screenPosition[0]-object.radius, object.screenPosition[1]-object.radius);
        window->draw(shape);
    }
}


void Renderer::nextFrame()
{
    updateScene();
    // diagnoseForces();
    checkCollisions();
    traceObjects();
    drawScene();
}
