#ifndef SPRITE_GENERATOR_H
#define  SPRITE_GENERATOR_H

#include <SFML/Graphics.hpp>
#include <iostream>
#include <algorithm>
#include <complex>
#include <thread>


using std::string, std::vector;

using std::cout, std::endl;

using complex = std::complex<float>;



void HSVtoRGB(float H, float S, float V, float& R, float& G, float& B);


float normalizeAbs(float x, float low_value = 0.3, float scale = 0.3);


sf::Color imageToColour(const complex& c, const sf::Vector2u& windowSize,
						const float x1, const float x2,
						const float y1, const float y2);


sf::Vector2f complexToPoint(const complex& c, const sf::Vector2u& windowDim,
							const float x1, const float x2,
							const float y1, const float y2);


class SpriteGenerator {
public:

	SpriteGenerator(const sf::Vector2u& windowDim, std::pair<float, float> center, unsigned int imageScaleFactor, complex(*funcToMap)(complex));

	~SpriteGenerator();

	void generateSprite(sf::Sprite& finalSprite, float zoom = 1, bool useBlur = false);

	void setCenter(const std::pair<float, float>& center);

private:
	unsigned int imageScaleFactor;
	complex cFunction;

	sf::Vector2u windowDim;
	sf::Vector2u originalImageDim;

	sf::Shader* shader;
	sf::Texture* texture;
	sf::RenderTexture* renderTexture;
	complex(*funcToMap)(complex);

	std::pair<float, float> center;


	void drawImageChunk(sf::Image& image, unsigned int startY, unsigned int endY, float zoom);
};


#endif 