/* 
* Owen Shelton
* CSCI 8810 Spring 2011
* Project 2
*/

#ifndef COLOR_H
#define COLOR_H

#include "NodeBase.h"

//Node for converting an RGB image to Greyscale
class ToGreyscaleNode : public NodeBase
{
protected:
	//This node doesn't have any properties.
	void addProperties(){}

	//Image to store the result.
	wxImage result;

	//Panel on which to display the image.
	ImagePanel* image;

	//RGB (or greyscale image input).
	ImagePort* input;
	//Greyscale output.
	ImagePort* output;

public:
	ToGreyscaleNode(wxConnexSpace *parent, const wxPoint& pos = wxDefaultPosition);

	void onSave(wxCommandEvent& event);
	void OnPortDataUpdate(wxConnexPortBase* port);

	void onPropertyChanged(wxPropertyGridEvent& event){}
	void onPropertyChanging(wxPropertyGridEvent& event){}
	
	inline wxString getName(){ return wxT("RGB -> Gray"); }
};

#endif