/* 
* Owen Shelton
* CSCI 8810 Spring 2011
* Project 2
*/

#ifndef PYRAMID_H
#define PYRAMID_H

#include "NodeBase.h"

//Node generates a 5 level pyramid from a 512x512 image.
//If the image is larger than 512x512, the image will be croped from the top-left to 512x512.
//If the image is smaller than 512x512 it will be padded with 0's.
class PyramidNode : public NodeBase
{
protected:
	//No properties for this Node.
	void addProperties(){}

	//Image resized to 512x512.
	wxImage resized;

	//Base input image, cropped/expanded to 512x512.
	ImagePanel* image;

	//Levels, 0 = 256x256, 4 = 16x16.
	wxImage levelArray[5];
	ImagePanel* levels[5];

	//Greyscale image input.
	ImagePort* input;

	//Output port.
	PyramidPort* output;

	//Image outpus for the seperate levels.
	ImagePort* levelOutputs[5];

public:
	PyramidNode(wxConnexSpace *parent, const wxPoint& pos = wxDefaultPosition);

	void onSave(wxCommandEvent& event);
	void OnPortDataUpdate(wxConnexPortBase* port);

	//Node has no properties.
	void onPropertyChanged(wxPropertyGridEvent& event){}
	void onPropertyChanging(wxPropertyGridEvent& event){}
	
	inline wxString getName(){ return wxT("Pyramid"); }

protected:
	void apply(wxImage* image);

	//Called in apply, pads/crops the input image as needed.
	void transformInput(wxImage* image);
	//Called by apply, rebuilds the levels.
	void rebuildLevels();
};

//Node which expands the outut of the Pyramid Node using the First order hold scheme.
class FirstOrderNode: public NodeBase
{
protected:
	//No properties for this Node.
	void addProperties(){}

	//Levels, 0 = 256x256, 4 = 16x16.
	wxImage levelArray[5];
	ImagePanel* levels[5];

	//Greyscale image input.
	PyramidPort* input;

	//Image outpus for the seperate levels.
	ImagePort* levelOutputs[5];

public:
	FirstOrderNode(wxConnexSpace *parent, const wxPoint& pos = wxDefaultPosition);

	void onSave(wxCommandEvent& event);
	void OnPortDataUpdate(wxConnexPortBase* port);

	//Node has no properties.
	void onPropertyChanged(wxPropertyGridEvent& event){}
	void onPropertyChanging(wxPropertyGridEvent& event){}
	
	inline wxString getName(){ return wxT("First Order"); }

protected:
	void apply(wxImage* inputLevels);
};

//Node which expands the outut of the Pyramid Node using the Zero order hold scheme.
class ZeroOrderNode: public NodeBase
{
protected:
	//No properties for this Node.
	void addProperties(){}

	//Levels, 0 = 256x256, 4 = 16x16.
	wxImage levelArray[5];
	ImagePanel* levels[5];

	//Greyscale image input.
	PyramidPort* input;

	//Image outpus for the seperate levels.
	ImagePort* levelOutputs[5];

public:
ZeroOrderNode(wxConnexSpace *parent, const wxPoint& pos = wxDefaultPosition);

	void onSave(wxCommandEvent& event);
	void OnPortDataUpdate(wxConnexPortBase* port);

	//Node has no properties.
	void onPropertyChanged(wxPropertyGridEvent& event){}
	void onPropertyChanging(wxPropertyGridEvent& event){}
	
	inline wxString getName(){ return wxT("First Order"); }

protected:
	void apply(wxImage* inputLevels);
};

#endif