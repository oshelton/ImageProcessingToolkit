/* 
* Owen Shelton
* CSCI 8810 Spring 2011
* Project 2
*/

#ifndef DATATYPES_H
#define DATATYPES_H

#include <vector>
#include <wx/image.h>

//Struct for storing a pixel from a 3 channel image (1 byte per channel).
struct Pixel
{
	unsigned char r, g, b;

	Pixel()
	{
		r = g = b = 0;
	}

	Pixel(const unsigned char& all)
	{
		r = g = b = all;
	}

	Pixel(const unsigned char& r, const unsigned char& g, const unsigned char &b)
	{
		this->r = r;
		this->g = g;
		this->b = b;
	}

	inline unsigned char asGray()
	{
		if (r == g && g == b)
			return r;
		else
			return r * .30 + g * .59 + b * .11;
	}

	inline bool operator == (const Pixel& other)
	{
		if (r == other.r &&
			g == other.g &&
			b == other.b)
			return true;
		else
			return false;
	}
};

//Struct used for storing pixels to be used in mathematical operations.
struct FloatPixel
{
	float r, g, b;

	FloatPixel()
	{
		r = g = b = 0;
	}

	FloatPixel(const float& all)
	{
		r = g = b = all;
	}

	FloatPixel(const float& r, const float& g, const float& b)
	{
		this->r = r;
		this->g = g;
		this->b = b;
	}

	FloatPixel(const Pixel& other)
	{
		this->r = other.r;
		this->g = other.g;
		this->b = other.b;
	}

	const FloatPixel& operator = (const Pixel& other);
	FloatPixel operator + (const Pixel& other);
	void operator += (const Pixel& other);
	FloatPixel operator - (const Pixel& other);
	void operator -= (const Pixel& other);
	//Element wise multiplication and division.
	FloatPixel operator * (const Pixel& other);
	void operator *= (const Pixel& other);
	FloatPixel operator / (const Pixel& other);
	void operator /= (const Pixel& other);

	const FloatPixel& operator = (const FloatPixel& other);
	FloatPixel operator + (const FloatPixel& other);
	void operator += (const FloatPixel& other);
	FloatPixel operator - (const FloatPixel& other);
	void operator -= (const FloatPixel& other);
	//Element wise multiplication and division.
	FloatPixel operator * (const FloatPixel& other);
	void operator *= (const FloatPixel& other);
	FloatPixel operator / (const FloatPixel& other);
	void operator /= (const FloatPixel& other);

	//Scalar operators.
	const FloatPixel& operator = (const float& other);
	FloatPixel operator + (const float& other);
	void operator += (const float& other);
	FloatPixel operator - (const float& other);
	void operator -= (const float& other);
	//Element wise multiplication and division.
	FloatPixel operator * (const float& other);
	void operator *= (const float& other);
	FloatPixel operator / (const float& other);
	void operator /= (const float& other);

	inline FloatPixel operator - () const
	{
		return FloatPixel(r * -1, g * -1, b * -1);
	}

	//Comparison operator <, uses gray intensity.
	bool operator < (const FloatPixel& other);
	
	//Convert the FloatPixel to a regular Pixel, truncates and clamps values out of range.
	inline Pixel asPixel() const
	{
		Pixel toReturn(0, 0, 0);

		if (r < 0)
			toReturn.r = 0;
		else if (r > 255)
			toReturn.r = 255;
		else
			toReturn.r = (unsigned char)r;

		if (g < 0)
			toReturn.g = 0;
		else if (g > 255)
			toReturn.g = 255;
		else
			toReturn.g = (unsigned char)g;

		if (b < 0)
			toReturn.b = 0;
		else if (b > 255)
			toReturn.b = 255;
		else
			toReturn.b = (unsigned char)b;

		return toReturn;
	}

	inline float asGray() const
	{
		return r * .30 + g * .59 + b * .11;
	}
};

//Absolute value function for FloatPixels.
FloatPixel abs(const FloatPixel& in);
//Exponential function for FloatPixels.
FloatPixel pow(const FloatPixel& in, double power);
//Square root function for FloatPixels.
FloatPixel sqrt(const FloatPixel& in);
//Floor function for FloatPixels.
FloatPixel floor(const FloatPixel& in);

//Simple class for storing the underlying data in a wxImage.
class ImageData
{
protected:
	unsigned char* data;

public:
	unsigned int width, height;

	ImageData()
	{
		data = NULL;
		width = height = 0;
	}

	ImageData(wxImage* image)
	{
		data = image->GetData();
		width = image->GetWidth();
		height = image->GetHeight();
	}

	inline ImageData& operator = (wxImage* image)
	{
		data = image->GetData();
		width = image->GetWidth();
		height = image->GetHeight();

		return *this;
	}

	inline Pixel& at(const unsigned int& row, const unsigned int& column)
	{
		if (data)
		{
			Pixel* rowPixel = (Pixel*)(data + (row * width * 3));
			return rowPixel[column];
		}
		else
			return Pixel();
	}

	inline FloatPixel floatAt(const unsigned int& row, const unsigned int& column)
	{
		return FloatPixel(at(row, column));
	}

	inline void setFloatAt(const FloatPixel& pixel, const unsigned int& row, const unsigned int& column)
	{
		if (data)
			at(row, column) = pixel.asPixel(); 
	}
};

//Struct for storing image data as floats.
class FloatImageData
{
protected:
	ImageData base;
	std::vector<std::vector<FloatPixel>> pixels;

public:
	//Height and width of image.
	unsigned int height;
	unsigned int width;

	FloatImageData(wxImage* image)
	{
		base = image;
		pixels.resize(base.height);
		for (unsigned int i = 0; i < pixels.size(); ++i)
			pixels[i].resize(base.width);

		for (unsigned int i = 0; i < base.height; ++i)
		{
			for (unsigned int j = 0; j < base.width; ++j)
				pixels[i][j] = base.floatAt(i, j);
		}

		height = base.height;
		width = base.width;
	}

	inline FloatImageData& operator = (wxImage* image)
	{
		base = image;

		pixels.resize(base.height);
		for (unsigned int i = 0; i < pixels.size(); ++i)
			pixels[i].resize(base.width);

		for (unsigned int i = 0; i < base.height; ++i)
		{
			for (unsigned int j = 0; j < base.width; ++j)
				pixels[i][j] = base.floatAt(i, j);
		}

		height = base.height;
		width = base.width;

		return *this;
	}
	
	inline FloatPixel at(const unsigned int& row, const unsigned int& column)
	{
		return pixels[row][column];
	}

	inline void setAt(const FloatPixel& value, const unsigned int& row, const unsigned int& column)
	{
		pixels[row][column] = value;
		base.setFloatAt(value, row, column);
	}

	//Get the minimum pixel values for each channel in the image (RGB).
	FloatPixel getMin();
	//Get the maximum pixel values for each channel in the image (RGB).
	FloatPixel getMax();

	//Normalize the pixels within the passed range to 0-255.
	void normalize(const FloatPixel& bLow, const FloatPixel& bHigh);

protected:
	//Update a pixel value in base, used in normalize.
	void update(const FloatPixel& value, const unsigned int& row, const unsigned int& column);
};

//Simple struct for storing a coordinate pair.
struct Coord
{
	int row, column;

	Coord(){ row = column = 0; }

	Coord(const unsigned int& iRow, const unsigned int& iColumn): row(iRow), column(iColumn){}

	bool operator != (const Coord& other)
	{
		if (row != other.row ||
			column != other.column)
			return true;
		else
			return false;
	}

	bool operator < (const Coord& other)
	{
		if (row < other.row && column < other.column)
			return true;
		else
			return false;
	}
};

bool operator == (const Coord& left, const Coord& right);

//Grab a value from the gaussian distribution at the given x with the passed sigma multiplied by gain.
FloatPixel gaussianRandom(const float& sigma, const float& gain);

#endif