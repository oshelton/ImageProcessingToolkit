/* 
* Owen Shelton
* CSCI 8820 Spring 2011
* Assignment 1
*/

#include <algorithm>
#include <math.h>
#include "ConnectedComponent.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace std;

ConnectedComponent::ConnectedComponent(const unsigned int& label, vector<vector<unsigned int>>& data): area(0)
{
	this->label = label;

	//Variables to help determine bounding box.
	unsigned int minRow = 0;
	unsigned int maxRow = 0;
	unsigned int minColumn = 0;
	unsigned int maxColumn = 0;

	//Calculates area, centroid, and bounding box.
	for (unsigned int i = 0; i < data.size(); ++i)
	{
		for (unsigned int j = 0; j < data[i].size(); ++j)
		{
			if (data[i][j] == label)
			{
				//Handle bounding box calculations.
				if (area == 0)
				{
					minRow = i;
					maxRow = i;
					minColumn = j;
					maxColumn = j;
				}
				else
				{
					if (i >= maxRow)
						maxRow = i;
					else if (i < minRow)
						minRow = i;

					if (j >= maxColumn)
						maxColumn = j;
					else if (j < minColumn)
						minColumn = j;
				}

				//Increase area.
				++area;

				//Centroid calculations.
				centroid.row += i;
				centroid.column += j;
			}
		}
	}

	centroid.row = centroid.row / area;
	centroid.column = centroid.column / area;

	//Fill bounding box values.
	bottomLeft.row = maxRow;
	bottomLeft.column = minColumn;

	bottomRight.row = maxRow;
	bottomRight.column = maxColumn;

	topLeft.row = minRow;
	topLeft.column = minColumn;

	topRight.row = minRow;
	topRight.column = maxColumn;
}

////////////////////////////////////////////

//Special function used to search the pairs of equivalent labels and compose them into a single list. 
//Note: Is recursive.
vector<unsigned int> findAllRelatedTo(const pair<unsigned int, unsigned int>& source, 
											vector<pair<unsigned int, unsigned int>>& eqv)
{
	vector<unsigned int> toReturn;
	vector<pair<unsigned int, unsigned int>> toProcess;
	toReturn.push_back(source.first);
	//If the second component is 0, then the label in source.first has no equivalencies to consider.
	if (source.second != 0)
	{
		toReturn.push_back(source.second);
		bool erased = false;

		//Finds and adds any pairs that the source label is used to a list for further processing.
		vector<pair<unsigned int, unsigned int>>::iterator iter;
		for (iter = eqv.begin(); iter != eqv.end();)
		{
			erased = false;

			if ((*iter).second == source.first)
			{
				toProcess.push_back(*iter);
				iter = eqv.erase(iter);
				erased = true;
			}

			else if (iter != eqv.end() && (*iter).first == source.first)
			{
				toProcess.push_back(*iter);
				iter = eqv.erase(iter);
				erased = true;
			}

			if (iter != eqv.end() && (*iter).second == source.second)
			{
				toProcess.push_back(*iter);
				iter = eqv.erase(iter);
				erased = true;
			}
			else if (iter != eqv.end() && (*iter).first == source.second)
			{
				toProcess.push_back(*iter);
				iter = eqv.erase(iter);
				erased = true;
			}

			if (!erased)
				++iter;
		}

		//Process other equivalance pairs that involve source.
		//Recursively calls the function, results in a tree structure of calls, ensuring all possible equivalence relations are checked.
		vector<unsigned int> parts;
		for (unsigned int i = 0; i < toProcess.size(); ++i)
		{
			parts = findAllRelatedTo(toProcess[i], eqv);
			for (unsigned int j = 0; j < parts.size(); ++j)
				toReturn.push_back(parts[j]);
		}
	}

	return toReturn;
}

vector<ConnectedComponent*> calculateConnectedComponents(ImageData* image, const unsigned int& sizeThreshold)
{
	//Vector to store the results.
	vector<ConnectedComponent*> results;
	//Takes care of the current label.
	unsigned int label = 0;

	//Shortcuts just to shave a few function calls.
	unsigned int width = image->width;
	unsigned int height = image->height;

	//Vector for storing equivalence pairs.
	//When a specific label has no equivalences, it will be paired with 0.
	//All lebals when initially used will be paired wit 0, that pair will be erased once
	//a real equivalent label is found.
	vector<pair<unsigned int, unsigned int>> equivalents;

	//Vector for storing the underlying in processing data.
	//Needed to get around the fact that in-place processing of the image would be restricted to 
	//using 255 labels (doesn't include black).
	vector<vector<unsigned int>> data;

	//Resize data so that it is the same size as the image + 2 rows and columns for padding.
	data.resize(image->height);
	for (unsigned int i = 0; i < data.size(); ++i)
		data[i].resize(image->width);

	//Iterate over pixels in image.
	for (unsigned int i = 0; i < height; ++i)
	{
		for (unsigned int j = 0; j < width; ++j)
		{
			//Special case where top-left pixel is white.
			if (i == 0 && j == 0 && image->at(i, j).asGray() == 255)
			{
				++label;
				data[i][j] = label;
				equivalents.push_back(pair<unsigned int, unsigned int>(label, 0));
			}
			//Special case for first row.
			else if (i == 0 && j != 0  && image->at(i, j).asGray() == 255)
			{
				//Pixel to the left has been labeled.
				if (image->at(i, j - 1).asGray() != 0)
					data[i][j] = data[i][j - 1];
				else
				{
					++label;
					data[i][j] = label;
					equivalents.push_back(pair<unsigned int, unsigned int>(label, 0));
				}
			}
			//Special case for first column.
			else if (i != 0 && j == 0 && image->at(i, j).asGray() == 255)
			{
				//Pixel above has been labeled.
				if (image->at(i - 1, j).asGray() != 0)
					data[i][j] = data[i - 1][j];
				else
				{
					++label;
					data[i][j] = label;
					equivalents.push_back(pair<unsigned int, unsigned int>(label, 0));
				}
			}
			//General case.
			else if (i != 0 && j != 0 && image->at(i, j).asGray() == 255)
			{
				//Only if both the pixel to the left and above are labeled.
				if (data[i - 1][j] != 0 && data[i][j - 1] != 0)
				{
					//Easy case, both left and above pixel are the same.
					if (data[i - 1][j] == data[i][j - 1])
						data[i][j] = data[i - 1][j];
					//Not so easy case.
					else
					{
						//Assign current pixel label of above pixel.
						data[i][j] = data[i - 1][j];

						//Check if an equivalence pair already exists for the current label and the label to the left.
						vector<pair<unsigned int, unsigned int>>::iterator result;
						result = find(equivalents.begin(), equivalents.end(),
									  pair<unsigned int, unsigned int>(data[i][j - 1], data[i][j]));
						if (result == equivalents.end())
						{
							result = find(equivalents.begin(), equivalents.end(),
									  pair<unsigned int, unsigned int>(data[i][j], data[i][j - 1]));
						}

						//If no equivalence pair has been added.
						if (result == equivalents.end())
						{
							//Add a new equivalence pair.
							equivalents.push_back(pair<unsigned int, unsigned int>(data[i][j - 1], data[i][j]));

							//Following checks if either label involved has been paired with 0, and erases it if so.
							result = find(equivalents.begin(), equivalents.end(), pair<unsigned int, unsigned int>(data[i][j], 0));
							if (result != equivalents.end())
								equivalents.erase(result);

							result = find(equivalents.begin(), equivalents.end(), pair<unsigned int, unsigned int>(data[i][j - 1], 0));
							if (result != equivalents.end())
								equivalents.erase(result);
						}
					}
				}
				//Special case, above pixel is black, left is labeled.
				else if (data[i - 1][j] == 0 && data[i][j - 1] != 0)
					data[i][j] = data[i][j - 1];
				//Special case, left pixel is black, above is labeled.
				else if (data[i - 1][j] != 0 && data[i][j - 1] == 0)
					data[i][j] = data[i - 1][j];
				//Neither the pixel to the left or above are labeled.
				else
				{
					++label;
					data[i][j] = label;
					equivalents.push_back(pair<unsigned int, unsigned int>(label, 0));
				}
			}
		}
	}

	//Vector that holds the seperate components and the labels that they use.
	vector<vector<unsigned int>> parsedEqv;

	//While some equivalence relations have not been processed.
	while (equivalents.size() > 0)
	{
		//Generate the component that the last equivalence relation is a part of.
		pair<unsigned int, unsigned int> current = equivalents.back();
		equivalents.pop_back();

		//Find all equivalent labels of current.
		parsedEqv.push_back(findAllRelatedTo(current, equivalents));
		sort(parsedEqv.back().begin(), parsedEqv.back().end());
		//Erase any duplicates (they can occur).
		parsedEqv.back().erase(unique(parsedEqv.back().begin(), parsedEqv.back().end()), parsedEqv.back().end());
	}

	//Reassign component labels based on number of equivalent lists generated.
	for (unsigned int i = 0; i < parsedEqv.size(); ++i)
	{
		for (unsigned int j = 0; j < parsedEqv[i].size(); ++j)
		{
			for (unsigned int k = 0; k < height; ++k)
			{
				for (unsigned int l = 0; l < width; ++l)
				{	
					//Assign merged label.
					if (image->at(k, l).asGray() == 255 && data[k][l] == parsedEqv[i][j])
					{
						data[k][l] = i + 1;
						image->at(k, l) = Pixel(128);
					}
				}
			}
		}
	}
	//Finally!  Create an instance of ConnectedComponent for further processing.
	for (unsigned int i = 0; i < parsedEqv.size(); ++i)
		results.push_back(new ConnectedComponent(i + 1, data));

	vector<ConnectedComponent*>::iterator iter;
	for (iter = results.begin(); iter != results.end();)
	{
		if ((*iter)->getArea() < sizeThreshold)
		{
			for (unsigned int i = 0; i < height; ++i)
			{
				for (unsigned int j = 0; j < width; ++j)
				{
					if (data[i][j] == (*iter)->getLabel())
					{
						data[i][j] = 0;
						image->at(i, j) = Pixel();
					}
				}
			}
			iter = results.erase(iter);
		}
		else
			++iter;
	}

	//Generate spacing for shades so they aren't all adjacent.
	unsigned int spacing = 206.0 / results.size();

	for (unsigned int k = 0; k < results.size(); ++k)
	{
		for (unsigned int i = 0; i < height; ++i)
		{
			for (unsigned int j = 0; j < width; ++j)
			{
				if (data[i][j] == results[k]->getLabel())
					image->at(i, j) = Pixel(spacing * k + 49);
			}
		}
		results[k]->setLabel(spacing * k + 49);
	}

	return results;
}