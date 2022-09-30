/* 
* Owen Shelton
* CSCI 8820 Spring 2011
* Assignment 1
*/

#ifndef COMPONENTS_H
#define COMPONENTS_H

#include <vector>
#include "connex.h"
#include "DataTypes.h"

//Class representing a connected component.
class ConnectedComponent
{
private:
	//Label for this specific component.
	unsigned int label;

	//Area (number of pixels) for the component.
	unsigned long area;
	//Centroid coordinates (row, column).
	Coord centroid;
	//Bounding box coordinates (row, column).
	Coord bottomLeft;
	Coord bottomRight;
	Coord topLeft;
	Coord topRight;

public:
	//Constructor, calculates the area as well.
	ConnectedComponent(const unsigned int& label, std::vector<std::vector<unsigned int>>& data);

	inline const unsigned long& getArea()
	{
		return area;
	}

	inline void setLabel(const unsigned int& label)
	{
		this->label = label;
	}

	inline const unsigned short& getLabel()
	{
		return label;
	}

	inline const Coord& getCentroid()
	{
		return centroid;
	}

	inline const Coord& getBottomLeft()
	{
		return bottomLeft;
	}

	inline const Coord& getBottomRight()
	{
		return bottomRight;
	}

	inline const Coord& getTopLeft()
	{
		return topLeft;
	}

	inline const Coord& getTopRight()
	{
		return topRight;
	}
};

//Function that calculates the connected components in a binary image.
std::vector<ConnectedComponent*> calculateConnectedComponents(ImageData* image, const unsigned int& sizeThreshold);

#endif