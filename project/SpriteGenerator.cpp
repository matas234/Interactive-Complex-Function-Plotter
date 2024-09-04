#include <SFML/Graphics.hpp>
#include <iostream>
#include <algorithm>
#include <complex>
#include <thread>
#include <functional>
#include <algorithm>

#include "SpriteGenerator.h"



using std::string, std::vector;

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
float normalizeAbs(float x, float low_value, float scale)
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



SpriteGenerator::SpriteGenerator(const sf::Vector2u& windowDim,
								std::pair<float, float> center,
								unsigned int imageScaleFactor,
								UserFunctionEvaluator& evaluator)
{

	this->windowDim = windowDim;

	this->center = center;

	this->imageScaleFactor = imageScaleFactor;

	this->evaluator = evaluator;

	this->originalImageDim = sf::Vector2u(windowDim.x / imageScaleFactor, windowDim.y / imageScaleFactor);

	this->texture = new sf::Texture;

}

SpriteGenerator::~SpriteGenerator()
{
	delete texture;

}


void SpriteGenerator::generateSprite(sf::Sprite& finalSprite, float zoom)
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

	finalSprite.setTexture(*texture);
	finalSprite.setScale(imageScaleFactor, imageScaleFactor);
}

void SpriteGenerator::setCenter(const std::pair<float, float>& center)
{
	this->center = center;
}

void SpriteGenerator::setEvaluator(const UserFunctionEvaluator& evaluator)
{
	this->evaluator = evaluator;
}

void SpriteGenerator::drawImageChunk(sf::Image& image, unsigned int startY, unsigned int endY, float zoom)
{
	static const float grid = 0.125f;

	const float temp = 5 / zoom;

	const float x1 = center.first - temp;
	const float x2 = center.first + temp;
	const float y1 = center.second - temp;
	const float y2 = center.second + temp;

	const float c1 = (x2 - x1) / originalImageDim.x;
	const float c2 = (y2 - y1) / originalImageDim.y;

	for (unsigned int curX = 0; curX < originalImageDim.x; ++curX)
	{
		for (unsigned int curY = startY; curY < endY; ++curY)
		{
			complex curCompl(x1 + c1 * curX, y1 + c2 * curY );


			complex imCompl = evaluator.evaluate(curCompl);

			sf::Color color = imageToColour(imCompl, originalImageDim, x1, x2, y1, y2);

			image.setPixel(curX, curY, color);
		}
	}
}
