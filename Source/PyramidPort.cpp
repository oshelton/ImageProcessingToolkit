/* 
* Owen Shelton
* CSCI 8810 Spring 2011
* Project 2
*/

#include "PyramidPort.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//Definitions for ImagePort.
PyramidPort::PyramidPort(wxImage* levels)
{
	this->levels = levels;
}

bool PyramidPort::CanTakeAsInput(wxConnexPortBase* const port)
{
	PyramidPort* const castPort = dynamic_cast<PyramidPort* const>(port);
	if (castPort)
		return true;

	return false;
}

void PyramidPort::OnInputUpdate(wxConnexPortBase* port)
{
	if(!IsInput())
		return;

	if(port->GetPortClass()->m_RunTimeID == GetPortClass()->m_RunTimeID)
	{
		levels = ((PyramidPort*)port)->levels;
	}
}

DEF_PORT_CLASS_NO_BASE(PyramidPort)