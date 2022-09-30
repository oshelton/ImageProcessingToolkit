/* 
* Owen Shelton
* CSCI 8810 Spring 2011
* Project 2
*/

#ifndef THRESHOLDING_H
#define THRESHOLDING_H

#include "NodeBase.h"

//Node for applying simple thresholding to an image.
class SimpleThresholdingNode : public NodeBase
{
protected:
	unsigned char tValue;
	bool invert;

	//Add the threshold value and invert property to the grid.
	void addProperties();

	//Result of operator.
	wxImage result;

	//Panel on which to display the image.
	ImagePanel* image;

	//Image input.
	ImagePort* input;
	//Binary output.
	ImagePort* output;

public:
	SimpleThresholdingNode(wxConnexSpace *parent, const wxPoint& pos = wxDefaultPosition);

	void onSave(wxCommandEvent& event);
	void OnPortDataUpdate(wxConnexPortBase* port);

	void onPropertyChanged(wxPropertyGridEvent& event);
	void onPropertyChanging(wxPropertyGridEvent& event);
	
	inline wxString getName(){ return wxT("Simple Thresholding"); }

protected:
	void apply(wxImage* image);
};

//Node for applying iterative thresholding to an image.
class IterativeThresholdingNode : public NodeBase
{
protected:
	unsigned int numIterations;
	unsigned char finalT;
	bool invert;

	//Add the invert property and read only properties to the grid.
	void addProperties();

	//Result of operator.
	wxImage result;

	//Panel on which to display the image.
	ImagePanel* image;

	//Image input.
	ImagePort* input;
	//Binary output.
	ImagePort* output;

public:
	IterativeThresholdingNode(wxConnexSpace *parent, const wxPoint& pos = wxDefaultPosition);

	void onSave(wxCommandEvent& event);
	void OnPortDataUpdate(wxConnexPortBase* port);

	void onPropertyChanged(wxPropertyGridEvent& event);
	void onPropertyChanging(wxPropertyGridEvent& event);
	
	inline wxString getName(){ return wxT("Iterative Thresholding"); }

protected:
	void apply(wxImage* image);
};

//Node for applying adaptive thresholding to an image.
class AdaptiveThresholdingNode : public NodeBase
{
protected:
	unsigned int subdivisions;
	bool invert;

	//Add the invert property and read only properties to the grid.
	void addProperties();

	//Result of operator.
	wxImage result;

	//Panel on which to display the image.
	ImagePanel* image;

	//Image input.
	ImagePort* input;
	//Binary output.
	ImagePort* output;

public:
	AdaptiveThresholdingNode(wxConnexSpace *parent, const wxPoint& pos = wxDefaultPosition);

	void onSave(wxCommandEvent& event);
	void OnPortDataUpdate(wxConnexPortBase* port);

	void onPropertyChanged(wxPropertyGridEvent& event);
	void onPropertyChanging(wxPropertyGridEvent& event);
	
	inline wxString getName(){ return wxT("Adaptive Thresholding"); }

protected:
	void apply(wxImage* image);

	void iterativeThresholdRegion(wxImage* image, const wxRect& box);
};

#endif