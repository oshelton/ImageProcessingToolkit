/* 
* Owen Shelton
* CSCI 8810 Spring 2011
* Project 2
*/

#ifndef STATISTICS_H
#define STATISTICS_H

#include <vector>
#include "NodeBase.h"
#include "ConnectedComponent.h"

//Node for CCL to an image.
class CCLNode : public NodeBase
{
protected:
	unsigned int sizeThreshold;

	std::vector<ConnectedComponent*> components;

	//Add the threshold value property to the grid.
	void addProperties();

	//Result of the operation.
	wxImage result;

	//Panel on which to display the image.
	ImagePanel* image;

	//Greyscale image input.
	ImagePort* input;

public:
	CCLNode(wxConnexSpace *parent, const wxPoint& pos = wxDefaultPosition);

	void onSave(wxCommandEvent& event);
	void OnPortDataUpdate(wxConnexPortBase* port);

	void onPropertyChanged(wxPropertyGridEvent& event);
	void onPropertyChanging(wxPropertyGridEvent& event);
	
	inline wxString getName(){ return wxT("CCL"); }

protected:
	void apply(wxImage* image);
};

#endif