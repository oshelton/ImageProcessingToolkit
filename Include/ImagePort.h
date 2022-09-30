/* 
* Owen Shelton
* CSCI 8810 Spring 2011
* Project 2
*/

#ifndef IMAGEPORT_H
#define IMAGEPORT_H

#include <wx/image.h>
#include "connex.h"

//Class for handling all image ports.
class ImagePort : public wxConnexPortBase
{
public:
	//Enum for handling different type of images.
	enum Type
	{
		COLOR,
		GRAY,
		COLOR_GRAY,
		BINARY,
		ALL
	};

protected:
	wxImage* image;

	Type type;
public:

	ImagePort(wxImage* image, const ImagePort::Type& type);
	virtual ~ImagePort(){}

	inline wxImage* getImage() const { return image; }
	inline void setImage(wxImage* image){ this->image = image; }

	bool CanTakeAsInput(wxConnexPortBase* const port);

	void OnInputUpdate(wxConnexPortBase *port);

	inline const ImagePort::Type& getType() const { return type; }

	DECL_PORT_CLASS();
};

#endif