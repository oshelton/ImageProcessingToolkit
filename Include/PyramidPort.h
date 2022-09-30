/* 
* Owen Shelton
* CSCI 8810 Spring 2011
* Project 2
*/

#ifndef PYRAMIDPORT_H
#define PYRAMIDPORT_H

#include <wx/image.h>
#include "connex.h"

//Class for handling all Outpu and Input from the Pyramid operator.
class PyramidPort : public wxConnexPortBase
{
protected:
	//Pointer to an array of images (5).
	wxImage* levels;

public:
	PyramidPort(wxImage* levels);
	virtual ~PyramidPort(){}

	//Get the array of images.
	inline wxImage* getLevels() const { return levels; }
	//Set the array of images
	inline void setLevels(wxImage* levels){ this->levels = levels; }

	bool CanTakeAsInput(wxConnexPortBase* const port);

	void OnInputUpdate(wxConnexPortBase *port);

	DECL_PORT_CLASS();
};

#endif