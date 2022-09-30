/* 
* Owen Shelton
* CSCI 8810 Spring 2011
* Project 2
*/

#include "DataTypes.h"

#define M_E 2.71828182845904523536

using namespace std;

const FloatPixel& FloatPixel::operator = (const Pixel& other)
{
	r = other.r;
	g = other.g;
	b = other.b;

	return *this;
}

FloatPixel FloatPixel::operator + (const Pixel& other)
{
	return FloatPixel(r + other.r, g + other.g, b + other.b);
}

void FloatPixel::operator += (const Pixel& other)
{
	r += other.r;
	g += other.g;
	b += other.b;
}

FloatPixel FloatPixel::operator - (const Pixel& other)
{
	return FloatPixel(r - other.r, g - other.g, b - other.b);
}

void FloatPixel::operator -= (const Pixel& other)
{
	r -= other.r;
	g -= other.g;
	b -= other.b;
}

FloatPixel FloatPixel::operator * (const Pixel& other)
{
	return FloatPixel(r * other.r, g * other.g, b * other.b);
}

void FloatPixel::operator *= (const Pixel& other)
{
	r *= other.r;
	g *= other.g;
	b *= other.b;
}

FloatPixel FloatPixel::operator / (const Pixel& other)
{
	return FloatPixel(r / other.r, g / other.g, b / other.b);
}

void FloatPixel::operator /= (const Pixel& other)
{
	r /= other.r;
	g /= other.g;
	b /= other.b;
}

//////////////////////////////////////

const FloatPixel& FloatPixel::operator = (const FloatPixel& other)
{
	r = other.r;
	g = other.g;
	b = other.b;

	return *this;
}

FloatPixel FloatPixel::operator + (const FloatPixel& other)
{
	return FloatPixel(r + other.r, g + other.g, b + other.b);
}

void FloatPixel::operator += (const FloatPixel& other)
{
	r += other.r;
	g += other.g;
	b += other.b;
}

FloatPixel FloatPixel::operator - (const FloatPixel& other)
{
	return FloatPixel(r - other.r, g - other.g, b - other.b);
}

void FloatPixel::operator -= (const FloatPixel& other)
{
	r -= other.r;
	g -= other.g;
	b -= other.b;
}

FloatPixel FloatPixel::operator * (const FloatPixel& other)
{
	return FloatPixel(r * other.r, g * other.g, b * other.b);
}

void FloatPixel::operator *= (const FloatPixel& other)
{
	r *= other.r;
	g *= other.g;
	b *= other.b;
}

FloatPixel FloatPixel::operator / (const FloatPixel& other)
{
	return FloatPixel(r / other.r, g / other.g, b / other.b);
}

void FloatPixel::operator /= (const FloatPixel& other)
{
	r /= other.r;
	g /= other.g;
	b /= other.b;
}

bool FloatPixel::operator < (const FloatPixel& other)
{
	if (this->asGray() < other.asGray())
		return true;
	else
		return false;
}

////////////////////////////////////

const FloatPixel& FloatPixel::operator = (const float& other)
{
	r = other;
	g = other;
	b = other;

	return *this;
}

FloatPixel FloatPixel::operator + (const float& other)
{
	return FloatPixel(r + other, g + other, b + other);
}

void FloatPixel::operator += (const float& other)
{
	r += other;
	g += other;
	b += other;
}

FloatPixel FloatPixel::operator - (const float& other)
{
	return FloatPixel(r - other, g - other, b - other);
}

void FloatPixel::operator -= (const float& other)
{
	r -= other;
	g -= other;
	b -= other;
}

FloatPixel FloatPixel::operator * (const float& other)
{
	return FloatPixel(r * other, g * other, b * other);
}

void FloatPixel::operator *= (const float& other)
{
	r *= other;
	g *= other;
	b *= other;
}

FloatPixel FloatPixel::operator / (const float& other)
{
	return FloatPixel(r / other, g / other, b / other);
}

void FloatPixel::operator /= (const float& other)
{
	r /= other;
	g /= other;
	b /= other;
}

///////////////////////////////////////////////////////////////

FloatPixel abs(const FloatPixel& in)
{
	FloatPixel ret(in);
	if (ret.r < 0)
		ret.r *= -1;
	if (ret.g < 0)
		ret.g *= -1;
	if (ret.b < 0)
		ret.b *= -1;

	return ret;
}

FloatPixel pow(const FloatPixel& in, double power)
{
	return FloatPixel(pow((float)in.r, (float)power), pow((float)in.g, (float)power),
		pow((float)in.b, (float)power));
}

FloatPixel sqrt(const FloatPixel& in)
{
	return FloatPixel(sqrt(in.r), sqrt(in.g), sqrt(in.b));
}

FloatPixel floor(const FloatPixel& in)
{
	return FloatPixel(floor(in.r), floor(in.g), floor(in.b));
}

///////////////////////////////////////////////////////////////////

FloatPixel FloatImageData::getMin()
{
	FloatPixel min = pixels[0][0];
	for (unsigned int i = 0; i < pixels.size(); ++i)
	{
		for (unsigned int j = 0; j < pixels[i].size(); ++j)
		{
			FloatPixel current   = pixels[i][j];
			
			if (current.r < min.r)
				min.r = current.r;
			if (current.g < min.g)
				min.g = current.g;
			if (current.b < min.b)
				min.b = current.b;
		}
	}
	return min;
}

FloatPixel FloatImageData::getMax()
{
	FloatPixel max = pixels[0][0];
	for (unsigned int i = 0; i < pixels.size(); ++i)
	{
		for (unsigned int j = 0; j < pixels[i].size(); ++j)
		{
			FloatPixel current   = pixels[i][j];
			
			if (current.r > max.r)
				max.r = current.r;
			if (current.g > max.g)
				max.g = current.g;
			if (current.b > max.b)
				max.b = current.b;
		}
	}
	return max;
}

void FloatImageData::normalize(const FloatPixel& bLow, const FloatPixel& bHigh)
{
	for (unsigned int i = 0; i < pixels.size(); ++i)
	{
		for (unsigned int j = 0; j < pixels[i].size(); ++j)
		{
			if (pixels[i][j].r < bLow.r)
				pixels[i][j].r = 0;
			else if (pixels[i][j].r > bHigh.r)
				pixels[i][j].r = 255;
			else
				pixels[i][j].r = (pixels[i][j].r - bLow.r) / (bHigh.r - bLow.r * 1.0) * 255.0;

			if (pixels[i][j].g < bLow.g)
				pixels[i][j].g = 0;
			else if (pixels[i][j].g > bHigh.g)
				pixels[i][j].g = 255;
			else
				pixels[i][j].g = (pixels[i][j].g - bLow.g) / (bHigh.g - bLow.g * 1.0) * 255.0;

			if (pixels[i][j].b < bLow.b)
				pixels[i][j].b = 0;
			else if (pixels[i][j].b > bHigh.b)
				pixels[i][j].b = 255;
			else
				pixels[i][j].b = (pixels[i][j].b - bLow.b) / (bHigh.b - bLow.b * 1.0) * 255.0;

			update(pixels[i][j], i, j);
		}
	}
}

void FloatImageData::update(const FloatPixel& value, const unsigned int& row, const unsigned int& column)
{
	base.setFloatAt(value, row, column);
}

///////////////////////////////////////////////////////////////////////

bool operator == (const Coord& left, const Coord& right)
{
	return left.row == right.row && left.column == right.column;
}

FloatPixel gaussianRandom(const float& sigma, const float& gain)
{
	float randBase = rand() * 1.0 / RAND_MAX;
	float x = 8 * sigma * randBase - 4 * sigma;
	float value = 0;
	if (x > sigma)
		return FloatPixel(value);
	else
	{
		value = 1.0 / sqrt(2 * M_PI * (sigma * sigma));
		value *= pow((double)M_E, (double)-(x * x / (2.0 * pow((double)sigma, 2.0)))) * gain;
	}
	return FloatPixel(value);
}