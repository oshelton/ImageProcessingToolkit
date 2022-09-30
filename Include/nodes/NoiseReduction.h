/* 
* Owen Shelton
* CSCI 8810 Spring 2011
* Project 2
*/

#ifndef NOISE_REDUCTION_H
#define NOISE_REDUCTION_H

#include "NodeBase.h"

//Node for applying Average filtering to an image.
class AveragingNode : public NodeBase
{
protected:
	unsigned int maskSize;
	unsigned int iterations;

	//Add the threshold value property to the grid.
	void addProperties();

	//Image to store the result.
	wxImage result;

	//Panel on which to display the image.
	ImagePanel* image;

	//Greyscale image input.
	ImagePort* input;
	//Greyscale output.
	ImagePort* output;

public:
	AveragingNode(wxConnexSpace *parent, const wxPoint& pos = wxDefaultPosition);

	void onSave(wxCommandEvent& event);
	void OnPortDataUpdate(wxConnexPortBase* port);

	void onPropertyChanged(wxPropertyGridEvent& event);
	void onPropertyChanging(wxPropertyGridEvent& event);
	
	inline wxString getName(){ return wxT("Averaging"); }

protected:
	void apply(wxImage* image);
};

//Node for applying Median filtering to an image.
class MedianNode : public NodeBase
{
protected:
	unsigned int maskSize;
	unsigned int iterations;

	//Add the threshold value property to the grid.
	void addProperties();

	//Image to store the result.
	wxImage result;

	//Panel on which to display the image.
	ImagePanel* image;

	//Greyscale image input.
	ImagePort* input;
	//Greyscale output.
	ImagePort* output;

public:
	MedianNode(wxConnexSpace *parent, const wxPoint& pos = wxDefaultPosition);

	void onSave(wxCommandEvent& event);
	void OnPortDataUpdate(wxConnexPortBase* port);

	void onPropertyChanged(wxPropertyGridEvent& event);
	void onPropertyChanging(wxPropertyGridEvent& event);
	
	inline wxString getName(){ return wxT("Median"); }

protected:
	void apply(wxImage* image);
};

#endif