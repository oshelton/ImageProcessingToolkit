/* 
* Owen Shelton
* CSCI 8810 Spring 2011
* Project 2
*/

#include "StegoPort.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//Definitions for StegoPort.
StegoPort::StegoPort(wxImage* image, unsigned int* bitsUsed, unsigned int* dataLength)
{
	this->image = image;
	this->bitsUsed = bitsUsed;
	this->dataLength = dataLength;
	this->type = StegoPort::TEXT;
	imageDim  = NULL;
}

StegoPort::StegoPort(wxImage* image, unsigned int* bitsUsed, wxSize* dimensions)
{
	this->image = image;
	this->bitsUsed = bitsUsed;
	this->imageDim = dimensions;
	this->type = StegoPort::IMAGE;
	dataLength = NULL;
}

bool StegoPort::CanTakeAsInput(wxConnexPortBase* const port)
{
	StegoPort* const castPort = dynamic_cast<StegoPort* const>(port);
	if (castPort)
	{
		if (this->getType() == castPort->getType())
			return true;
	}

	return false;
}

void StegoPort::OnInputUpdate(wxConnexPortBase* port)
{
	if(!IsInput())
		return;

	if(port->GetPortClass()->m_RunTimeID == GetPortClass()->m_RunTimeID)
	{
		StegoPort* const castPort = dynamic_cast<StegoPort* const>(port);
		image = castPort->image;
		bitsUsed = castPort->bitsUsed;
		dataLength = castPort->dataLength;
		imageDim = castPort->imageDim;
	}
}

DEF_PORT_CLASS_NO_BASE(StegoPort)

