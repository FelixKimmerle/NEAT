#include <iostream>
#include "Logger/Logger.hpp"
#include "Pool.hpp"
#include <SFML/Graphics.hpp>

int main()
{
    srand(time(0));
    Logger::Log("Seed: " + std::to_string(time(0)), Logger::Information);
    InnovationManager in;
    Genome g2(4, 2, &in);
    Genome g(4, 2, &in);

    for (unsigned int i = 0; i < 10000; i++)
    {
        g.Mutate();
        g2.Mutate();
    }
    
    std::cout << g << std::endl;
    std::cout << g2 << std::endl;

    std::cout << "Distance: " << Genome::Distance(g, g2) << std::endl;

    NN *nn = new NN(&g2);
    auto res = nn->FeedForward({1, 1, 1, 1});
    for (auto &&i : res)
    {
        std::cout << i << std::endl;
    }

    Pool pool(100);
    


    bool dr = false;
    nn->SetSize(1600, 900);
    float scale = 25;
    sf::Clock clock;

    sf::ContextSettings settings;
    settings.antialiasingLevel = 8;
    sf::RenderWindow window(sf::VideoMode(1600, 900), "SFML works!",sf::Style::Default,settings);
    window.setVerticalSyncEnabled(true);

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
            {
                window.close();
            }
            else if (event.type == sf::Event::KeyPressed)
            {
                if (event.key.code == sf::Keyboard::Escape)
                {
                    window.close();
                }
                else if (event.key.code == sf::Keyboard::Space)
                {
                    dr = !dr;
                    nn->DrawGen(dr);
                }
                else if (event.key.code == sf::Keyboard::A)
                {
                    delete nn;
                    Genome g3(4, 2, &in);
                    for (unsigned int i = 0; i < 10000; i++)
                    {
                        g3.Mutate();
                    }
                    g2 = g3;
                    nn = new NN(&g2);
                    nn->SetSize(window.getSize().x,window.getSize().y);
                }
            }
            else if (event.type == sf::Event::Resized)
            {
                window.setView(sf::View(sf::FloatRect(0, 0, event.size.width, event.size.height)));
                nn->SetSize(event.size.width, event.size.height);
            }
            else if (event.type == sf::Event::MouseWheelMoved)
            {
                scale += event.mouseWheel.delta;
                nn->SetScale(scale);
            }
        }

        window.clear();
        window.draw(*nn);
        if (clock.getElapsedTime().asMilliseconds() > 1000)
        {
            nn->FeedForward({rand() / (float)RAND_MAX * 2.f - 1.f, rand() / (float)RAND_MAX * 2.f - 1.f, rand() / (float)RAND_MAX * 2.f - 1.f, rand() / (float)RAND_MAX * 2.f - 1.f});
            clock.restart();
        }
        window.display();
    }
    delete nn;
    return 0;
}