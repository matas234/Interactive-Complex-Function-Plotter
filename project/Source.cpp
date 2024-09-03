#include <SFML/Graphics.hpp>
#include <iostream>
#include <algorithm>
#include <complex>
#include <thread>

using std::string;
using std::sqrt;
using std::vector;
using complex = std::complex<float>;

// converts HSV colour format to RGB.
void HSVtoRGB(float H, float S, float V, float& R, float& G, float& B)
{
	float C = V * S;
	float X = C * (1 - fabs(fmod(H / 60.0, 2) - 1));
	float m = V - C;

	float rPrime, gPrime, bPrime;

	if (H >= 0 && H < 60)
	{
		rPrime = C;
		gPrime = X;
		bPrime = 0;
	}
	else if (H >= 60 && H < 120)
	{
		rPrime = X;
		gPrime = C;
		bPrime = 0;
	}
	else if (H >= 120 && H < 180)
	{
		rPrime = 0;
		gPrime = C;
		bPrime = X;
	}
	else if (H >= 180 && H < 240)
	{
		rPrime = 0;
		gPrime = X;
		bPrime = C;
	}
	else if (H >= 240 && H < 300)
	{
		rPrime = X;
		gPrime = 0;
		bPrime = C;
	}
	else
	{
		rPrime = C;
		gPrime = 0;
		bPrime = X;
	}

	R = (rPrime + m) * 255;
	G = (gPrime + m) * 255;
	B = (bPrime + m) * 255;
}



// normalized the abs value of a complex number.
float normalizeAbs(float x, float low_value = 0.3, float scale = 0.3)
{
	return low_value + (1.0 - low_value) / (1.0 + scale * x);
}



// maps the image of a complex number under a function to a colour to be drawn on the screen.
sf::Color imageToColour(const complex& c, const sf::Vector2u& windowSize,
	const float x1, const float x2,
	const float y1, const float y2)
{

	// if a pole then return pure white
	if (std::isinf(c.real()) or std::isinf(c.imag()))
	{
		return sf::Color(255, 255, 255, 255);
	}

	float hue = std::fmod((std::arg(c) * 360 / 6.28318530718) + 360.0, 360.0);

	float R, G, B;
	HSVtoRGB(hue, normalizeAbs(std::abs(c)), 1.0f, R, G, B);

	return sf::Color(R, G, B, 255);


}



// maps a complex number to a point on the screen
sf::Vector2f complexToPoint(const complex& c, const sf::Vector2u& windowDim,
	const float x1, const float x2,
	const float y1, const float y2)
{

	float x_c = ((c.real() - x1) / (x2 - x1)) * windowDim.x;
	float y_c = windowDim.y - ((c.imag() - y1) / (y2 - y1)) * windowDim.y;

	return sf::Vector2f(x_c, y_c);

}



class SpriteGenerator {
public:
	unsigned int imageScaleFactor;
	complex cFunction;

	sf::Vector2u windowDim;
	sf::Vector2u originalImageDim;

	sf::Shader* shader;
	sf::Texture* texture;
	sf::RenderTexture* renderTexture;
	complex (*funcToMap)(complex);

	std::pair<float, float> center;



	SpriteGenerator(const sf::Vector2u& windowDim, std::pair<float, float> center, unsigned int imageScaleFactor, complex (*funcToMap)(complex))
		: windowDim(windowDim), center(center), imageScaleFactor(imageScaleFactor), funcToMap(funcToMap)
	{

		originalImageDim = sf::Vector2u(windowDim.x / imageScaleFactor, windowDim.y / imageScaleFactor);

		texture = new sf::Texture;
		renderTexture = new sf::RenderTexture;
		shader = new sf::Shader;

		shader->loadFromFile("BlurShader.frag", sf::Shader::Fragment);
		shader->setUniform("resolution", sf::Vector2f(windowDim.x, windowDim.y));
		shader->setUniform("radius", 2.0f); 
		shader->setUniform("sigmaColor", 30.0f);
		shader->setUniform("sigmaSpace", 5.0f);
	}

	~SpriteGenerator()
	{
		delete texture;
		delete renderTexture;
		delete shader;
	}


	void generateSprite(sf::Sprite& finalSprite, float zoom = 1, bool useBlur = false)
	{

		// Generating the image
		sf::Image image;
		image.create(originalImageDim.x, originalImageDim.y);


		unsigned int numThreads = std::thread::hardware_concurrency();
		vector<std::thread> threads;

		unsigned int chunkHeight = originalImageDim.y / numThreads;

		for (unsigned int i = 0; i < numThreads; i++)
		{
			unsigned int startY = i * chunkHeight;
			unsigned int endY = (i == numThreads - 1) ? originalImageDim.y : (i + 1) * chunkHeight;
			threads.emplace_back(&SpriteGenerator::drawImageChunk, this, std::ref(image), startY, endY, zoom);
		}

		for (auto& thread : threads)
		{
			thread.join();
		}

		//// turning the image into a texture
		texture->loadFromImage(image);


		//// turning the texture into a sprite and scaling it by the imageScaleFactor
		sf::Sprite sprite;
		sprite.setTexture(*texture);
		sprite.setScale(imageScaleFactor, imageScaleFactor);


		renderTexture->create(windowDim.x, windowDim.y);
		if (useBlur)
		{
			shader->setUniform("texture", *texture);
			renderTexture->draw(sprite, shader);
		}
		else
		{
			renderTexture->draw(sprite);
		}

		sf::Image(renderTexture->getTexture().copyToImage()).saveToFile("output2.png");

		finalSprite = sf::Sprite(renderTexture->getTexture());
	}

	void setCenter(const std::pair<float, float>& center)
	{
		this->center = center;
	}

private:
	void drawImageChunk(sf::Image& image, unsigned int startY, unsigned int endY, float zoom)
	{
		const float temp = 5 / zoom;
			
		const float x1 = center.first - temp;
		const float x2 = center.first + temp;
		const float y1 = center.second - temp;
		const float y2 = center.second + temp;

		for (unsigned int curX = 0; curX < originalImageDim.x; ++curX)
		{
			for (unsigned int curY = startY; curY < endY; ++curY)
			{
				complex curCompl(x1 + (x2 - x1) * curX / originalImageDim.x, y1 + (y2 - y1) * curY / originalImageDim.y);


				complex imCompl = funcToMap(curCompl);
				//complex imCompl = (curCompl * (curCompl + complex(2.0f, 1.0f)) * (curCompl + complex(1.0f, 1.0f)));

				sf::Color color = imageToColour(imCompl, originalImageDim, x1, x2, y1, y2);

				image.setPixel(curX, curY, color);
			}
		}
	}
};


int main756()
{
	auto cFunction = [](complex z) -> complex { 
		return std::cos(z);
	};

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
	SpriteGenerator spriteGenerator(windowDim, center, imageScaleFactor, cFunction);
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

						float deltaX = ((view.getCenter().x / (float)WIDTH) - 0.5f) * 10 / textureZoomFactor;
						float deltaY = ((0.5f - view.getCenter().y / (float)HEIGHT)) * 10 / textureZoomFactor;
						center.first += deltaX;
						center.second += deltaY;

						spriteGenerator.setCenter(center);
						spriteGenerator.generateSprite(finalSprite, textureZoomFactor);

						view.reset(sf::FloatRect(0, 0, WIDTH, HEIGHT));
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




