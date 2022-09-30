/* 
* Owen Shelton
* CSCI 8810 Spring 2011
* Project 2
*/

#include "nodes/NodeBase.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

NodeBase::NodeBase(wxConnexSpace* parent, const wxString& label, const wxPoint& pos, const wxSize& size):
	wxConnexNode(parent, wxID_ANY, pos, size), saveButton(NULL), grid(NULL), connected(false)
{
	this->SetLabel(label);
}

NodeBase::~NodeBase()
{
	PortList::iterator iter;
	for (iter = m_Ports.begin(); iter != m_Ports.end(); ++iter)
		delete *iter;
}

void NodeBase::connect(wxButton* saveButton, wxPropertyGrid* grid)
{
	this->saveButton = saveButton;
	this->grid = grid;

	addProperties();
	connected = true;
}

void NodeBase::disconnect()
{
	if (connected)
	{
		grid->Clear();
		connected = false;
		saveButton = NULL;
		grid = NULL;
	}
}