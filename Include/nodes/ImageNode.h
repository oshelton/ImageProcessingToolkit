/* 
* Owen Shelton
* CSCI 8810 Spring 2011
* Project 2
*/

#ifndef IMAGENODE_H
#define IMAGENODE_H

#include "NodeBase.h"

//Node class for loading and displaying the initial image.
class ImageNode : public NodeBase
{
protected:
	wxString fileName;

	//Add the filename property to the grid.
	void addProperties();

	//Image to store the result.
	wxImage loaded;

	//Panel to display the initial image.
	ImagePanel* image;

	//Port for outputing the image.
	ImagePort* output;


public:
	ImageNode(wxConnexSpace *parent, const wxPoint& pos = wxDefaultPosition);

	//Does nothing for this class.
	void onSave(wxCommandEvent& event){}

	void onPropertyChanged(wxPropertyGridEvent& event);
	void onPropertyChanging(wxPropertyGridEvent& event){}
	
	inline wxString getName(){ return wxT("Image"); }
};

#endif