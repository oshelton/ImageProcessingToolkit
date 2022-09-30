/* 
* Owen Shelton
* CSCI 8810 Spring 2011
* Project 2
*/

#ifndef STEGO_PORT_H
#define STEGO_PORT_H

#include <wx/image.h>
#include "connex.h"

//Port class for passing around stego embedded images.
class StegoPort : public wxConnexPortBase
{
public:
	//Enum for handling different type of Stego data.
	enum Type
	{
		TEXT,
		IMAGE
	};

protected:
	//Stego image.
	wxImage* image;
	
	//Number of bits used to hide the data, usually 1.
	unsigned int* bitsUsed;
	//Length of data, in bytes.
	unsigned int* dataLength;

	wxSize* imageDim;

	Type type;
public:

	StegoPort(wxImage* image, unsigned int* bitsUsed, unsigned int* dataLength);
	StegoPort(wxImage* image, unsigned int* bitsUsed, wxSize* dimensions);
	~StegoPort(){}

	inline wxImage* getImage() const { return image; }
	inline void setImage(wxImage* image){ this->image = image; }
	inline const unsigned int& getBitsUsed(){ return *bitsUsed; }
	inline const unsigned int& getDataLength(){ return *dataLength; }
	inline const wxSize& getDimensions(){ return *imageDim; }

	bool CanTakeAsInput(wxConnexPortBase* const port);

	void OnInputUpdate(wxConnexPortBase *port);

	inline const StegoPort::Type& getType() const { return type; }

	DECL_PORT_CLASS();
};

#endif