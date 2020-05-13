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
}

std::array<unsigned int, 2> Renderer::fixPosition(std::array<double, 2> coordinates)
{
    std::array<unsigned int, 2> fixed_coords;
    fixed_coords[0] = round(coordinates[0]);
    fixed_coords[1] = round(coordinates[1]);
    double pixelLength = pow(10, 2);
    fixed_coords[0] /= pixelLength;
    fixed_coords[1] /= pixelLength;
    return std::array<unsigned int, 2> {fixed_coords[0], window->getSize().y-fixed_coords[1]};
}

void Renderer::addMatter(double massParam, double radiusParam, std::array<double, 2> positionParam, std::array<double, 2> velocityParam)
{
    matter.emplace_back(massParam, radiusParam, positionParam, velocityParam);
    for (int i = 0; i < matter.size()-1; i++)
    {
        std::array<double, 2> blank = {0,0}; // for some reason it only works if i initialize it first... type issues?
        forces.emplace_back(blank, matter[i], matter[matter.size()-1]);
        forces[forces.size()-1].updateGravity();
        forces.emplace_back(blank, matter[matter.size()-1], matter[i]);
        forces[forces.size()-1].updateGravity();
    }
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
        std::array<unsigned int,2> loc = fixPosition(matter[i].position);
        canvas.setPixel(loc[0], loc[1], sf::Color(255,255,255,100));
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
        forces[i].applyForce();
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
        std::array<unsigned int, 2> fixedPosition = fixPosition(object.position);
        sf::CircleShape shape (object.radius);
        shape.setPosition(fixedPosition[0]-object.radius, fixedPosition[1]-object.radius);
        window->draw(shape);
    }
}

void Renderer::nextFrame()
{
    updateScene();
    traceObjects();
    drawScene();
}
