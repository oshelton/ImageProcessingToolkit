/* 
* Owen Shelton
* CSCI 8810 Spring 2011
* Project 2
*/

#include "ImagePort.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//Definitions for ImagePort.
ImagePort::ImagePort(wxImage* image, const ImagePort::Type& type)
{
	this->image = image;
	this->type = type;
}

bool ImagePort::CanTakeAsInput(wxConnexPortBase* const port)
{
	ImagePort* const castPort = dynamic_cast<ImagePort* const>(port);
	if (castPort)
	{
		if (castPort->getType() == ALL || this->getType() == ALL)
			return true;
		else if (this->getType() == castPort->getType())
			return true;
		else if ((this->getType() == COLOR || this->getType() == GRAY) && castPort->getType() == COLOR_GRAY)
			return true;
	}

	return false;
}

void ImagePort::OnInputUpdate(wxConnexPortBase* port)
{
	if(!IsInput())
		return;

	if(port->GetPortClass()->m_RunTimeID == GetPortClass()->m_RunTimeID)
	{
		image = ((ImagePort*)port)->image;
	}
}

DEF_PORT_CLASS_NO_BASE(ImagePort)