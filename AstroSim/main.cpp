//
// Disclaimer:
// ----------
//
// This code will work only if you selected window, graphics and audio.
//
// Note that the "Run Script" build phase will copy the required frameworks
// or dylibs to your application bundle so you can execute it on any OS X
// computer.
//
// Your resource files (images, sounds, fonts, ...) are also copied to your
// application bundle. To get the path to these resources, use the helper
// function `resourcePath()` from ResourcePath.hpp
//

#include <SFML/Graphics.hpp>
// Here is a small helper for you! Have a look.
#include "ResourcePath.hpp"
#include "main.hpp"


int main(int, char const**, char* argv[])
{
    system("rm /Users/davfrei/AstroSim/logs/debug.txt");
    plog::init(plog::debug, "/Users/davfrei/AstroSim/logs/debug.txt", 5e+6, 1);
    // Create the main window
    sf::RenderWindow window(sf::VideoMode(2560, 1600), "AstroSim");
    
    // Set the Icon
    sf::Image icon;
    if (!icon.loadFromFile(resourcePath() + "icon.png")) {
        return EXIT_FAILURE;
    }
    window.setIcon(icon.getSize().x, icon.getSize().y, icon.getPixelsPtr());
    
    // run the program as long as the window is open
    
    sf::RenderWindow * windowPointer = &window;
    Renderer renderer(0, windowPointer, pow(10, 2));
    Rocket rocket (pow(10,6), {10,30}, {45000, 100000}, {2000,0}, 10000, 1000);
    renderer.rockets.push_back(rocket);
//    renderer.addMatter(pow(10,18), 11, {40000, 100100}, {0,5});
//    renderer.addMatter(pow(10,10), 2, {45000, 100000}, {20,0});
//    renderer.addStar(pow(10,20), {60000, 100000}, {0,0}, 20);
    
    // stress test!
//    renderer.addMatter(pow(10,1), 3, {60000, 125000}, {-800,0});
//    renderer.addMatter(pow(10,1), 5, {60000, 116000}, {-700,0});
//    renderer.addMatter(pow(10,1), 10, {60000, 115000}, {-1000,0});

    // "control" system that demonstrates that forces between other objects still work
//    renderer.addMatter(pow(10,17), 10, {200000, 120000}, {0,0});
//    renderer.addMatter(pow(10,1), 1, {200000, 110000}, {10,20});
    
    // show that collisions are a thing
//    renderer.addMatter(pow(10,1), 1, {40000, 90000}, {0,40});
    
    renderer.initializeForces();
    while (window.isOpen())
    {
        // check all the window's events that were triggered since the last iteration of the loop
        sf::Event event;
        while (window.pollEvent(event))
        {
            // "close requested" event: we close the window
            if (event.type == sf::Event::Closed)
                window.close();
        }
        
        //         clear the window with black color
        window.clear();
        
        //         draw everything here...
        //         window.draw(...);
        
        renderer.nextFrame();
        window.display();
    }
    PLOG_INFO << "CLOSED EVENT RECEIEVED. Error Count: " << renderer.errCount << " Warn Count: " << renderer.warnCount;
    return EXIT_SUCCESS;
}
