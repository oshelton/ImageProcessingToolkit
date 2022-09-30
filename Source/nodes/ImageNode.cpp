/* 
* Owen Shelton
* CSCI 8810 Spring 2011
* Project 2
*/

#include "nodes/ImageNode.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

ImageNode::ImageNode(wxConnexSpace *parent, const wxPoint& pos): 
	NodeBase(parent, wxT("Initial Image"), pos, wxSize(100, 95))
{
	loaded.Create(0, 0);
	image = new ImagePanel(this, wxT("Initial Image"), &loaded, true);
	image->SetSize(10,20, 78, 58);

	output = new ImagePort(&loaded, ImagePort::COLOR);
	output->m_Placement = wxConnexPortBase::PLACE_RIGHT;
	output->SetDirection( wxConnexPortBase::DIR_OUTPUT );
	output->m_Rect.SetPosition( wxPoint( GetSize().x - 10, 45 ) );

	this->AddCXPort(output);
}

void ImageNode::addProperties()
{
	if (grid)
		grid->Append(new wxImageFileProperty("Image", "Image", fileName));
}

void ImageNode::onPropertyChanged(wxPropertyGridEvent& event)
{
	fileName = event.GetProperty()->GetValueAsString();
	loaded.LoadFile(fileName, wxBITMAP_TYPE_ANY);
	output->RefreshConnections();
	image->Refresh();
	Refresh();
}