/* 
* Owen Shelton
* CSCI 8810 Spring 2011
* Project 2
*/

#ifndef NOISE_H
#define NOISE_H

#include "NodeBase.h"

//Node for applying Average filtering to a image.
class PixelNoiseNode : public NodeBase
{
protected:
	unsigned int pixels;
	bool grayscale;

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
	PixelNoiseNode(wxConnexSpace *parent, const wxPoint& pos = wxDefaultPosition);

	void onSave(wxCommandEvent& event);
	void OnPortDataUpdate(wxConnexPortBase* port);

	void onPropertyChanged(wxPropertyGridEvent& event);
	void onPropertyChanging(wxPropertyGridEvent& event){}
	
	inline wxString getName(){ return wxT("Pixel Nosie"); }

protected:
	void apply(wxImage* image);
};

//Node for applying Median filtering to a image.
class ClusterNoiseNode : public NodeBase
{
protected:
	unsigned int clusters;
	bool grayscale;

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
	ClusterNoiseNode(wxConnexSpace *parent, const wxPoint& pos = wxDefaultPosition);

	void onSave(wxCommandEvent& event);
	void OnPortDataUpdate(wxConnexPortBase* port);

	void onPropertyChanged(wxPropertyGridEvent& event);
	void onPropertyChanging(wxPropertyGridEvent& event){}
	
	inline wxString getName(){ return wxT("Cluster Noise"); }

protected:
	void apply(wxImage* image);
};

//Node for applying simple additive noise based on the Gaussian distribution.
class AdditiveNoiseNode : public NodeBase
{
protected:
	float sigma;
	float gain;

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
	AdditiveNoiseNode(wxConnexSpace *parent, const wxPoint& pos = wxDefaultPosition);

	void onSave(wxCommandEvent& event);
	void OnPortDataUpdate(wxConnexPortBase* port);

	void onPropertyChanged(wxPropertyGridEvent& event);
	void onPropertyChanging(wxPropertyGridEvent& event);
	
	inline wxString getName(){ return wxT("Additive Noise"); }

protected:
	void apply(wxImage* image);
};

#endif