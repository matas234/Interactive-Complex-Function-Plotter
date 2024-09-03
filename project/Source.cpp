#include <SFML/Graphics.hpp>
#include <iostream>
#include <algorithm>
#include <complex>
#include <thread>
#include <functional>

#include "SpriteGenerator.h"
#include "UserFunctionEvaluator.h"
#include <chrono>


using std::string, std::vector;
using complex = std::complex<float>;


int main()
{
	string inputFunction;
	std::cout << "Input Function:\n";
	std::cin >> inputFunction;

	UserFunctionEvaluator evaluator(inputFunction);



	// setting up the window
	const int WIDTH = 1000;
	const int HEIGHT = 1000;
	unsigned int imageScaleFactor = 1;

	// scaling factors
	float textureZoomFactor = 1;
	float textureZoomFactorScaling = 1.1;

	sf::Vector2u windowDim(WIDTH, HEIGHT);
	sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "Complex Function Plotter");


	// creating initial sprite
	std::pair<float, float> center = { 0, 0 };
	sf::Sprite finalSprite;
	SpriteGenerator spriteGenerator(windowDim, center, imageScaleFactor, evaluator);
	spriteGenerator.generateSprite(finalSprite);


	// defining the view
	sf::View view(sf::FloatRect(0, 0, WIDTH, HEIGHT));
	window.setView(view);
	window.draw(finalSprite);


	sf::Vector2i lastMousePosition;
	bool isDragging = false;
	bool isZooming = false;
	sf::Clock zoomClock;  
	float zoomTimeout = 0.2f;  

	//std::cout << view.getSize().x << " " << view.getSize().y << " " << sprite.getGlobalBounds().width << std::endl;



	sf::Event event;
	while (window.isOpen())
	{
		while (window.pollEvent(event))
		{
			switch (event.type)
			{            
				case sf::Event::Closed:
					window.close();
					break;


					// zooming
				case sf::Event::MouseWheelScrolled:
					isZooming = true;
					zoomClock.restart();
					if (event.mouseWheelScroll.delta > 0)
					{
						view.zoom(0.9);
						textureZoomFactor *= 1.11;
					}
					else
					{
						view.zoom(1.1);
						textureZoomFactor /= textureZoomFactorScaling;
					}

					break;


				case sf::Event::MouseButtonPressed:
					if (event.mouseButton.button == sf::Mouse::Left)
					{
						isDragging = true;
					}
					break;


				case sf::Event::MouseButtonReleased:
					if (event.mouseButton.button == sf::Mouse::Left)
					{
						isDragging = false;
						
						auto start = std::chrono::high_resolution_clock::now();

						float deltaX = ((view.getCenter().x / (float)WIDTH) - 0.5f) * 10 / textureZoomFactor;
						float deltaY = ((0.5f - view.getCenter().y / (float)HEIGHT)) * 10 / textureZoomFactor;
						center.first += deltaX;
						center.second += deltaY;

						spriteGenerator.setCenter(center);
						spriteGenerator.generateSprite(finalSprite, textureZoomFactor);

						view.reset(sf::FloatRect(0, 0, WIDTH, HEIGHT));

						auto end = std::chrono::high_resolution_clock::now();
						auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

						cout << "Rerender took: " << duration << " milliseconds\n";
					}
					break;


				case sf::Event::MouseMoved:
					if (isDragging)
					{
						sf::Vector2i currentMousePos = sf::Mouse::getPosition();
						sf::Vector2f delta = window.mapPixelToCoords(lastMousePosition) - window.mapPixelToCoords(currentMousePos);

						lastMousePosition = currentMousePos;
						view.move(delta);
					}
					else
					{
						lastMousePosition = sf::Mouse::getPosition();
					}
					break;

				// reset view
				case sf::Event::KeyPressed:
					if (event.key.code == sf::Keyboard::C)
					{
						view.setCenter(finalSprite.getGlobalBounds().width / 2, finalSprite.getGlobalBounds().height / 2);
						textureZoomFactor = 1.0f;
						center = std::pair<float, float>(0, 0);
						spriteGenerator.setCenter(center);
						spriteGenerator.generateSprite(finalSprite, textureZoomFactor);
					}
					break;
			}
		}
		if (isZooming && zoomClock.getElapsedTime().asSeconds() > zoomTimeout && !isDragging)
		{
			isZooming = false;

			spriteGenerator.generateSprite(finalSprite, textureZoomFactor);

			view.reset(sf::FloatRect(0,0, WIDTH, HEIGHT));
		}

		window.setView(view);
		window.clear();
		window.draw(finalSprite);
		window.display();
	}
	return 0;
}




