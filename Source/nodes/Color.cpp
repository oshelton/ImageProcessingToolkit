/* 
* Owen Shelton
* CSCI 8810 Spring 2011
* Project 2
*/

#include "nodes/Color.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

ToGreyscaleNode::ToGreyscaleNode(wxConnexSpace *parent, const wxPoint& pos): 
	NodeBase(parent, wxT("RGB -> Gray"), pos, wxSize(100, 95))
{
	result.Create(0, 0);
	image = new ImagePanel(this, wxT("Converted to Greyscale"), &result, true);
	image->SetSize(10,20, 78, 58);

	input = new ImagePort(NULL, ImagePort::COLOR);
	input->m_Placement = wxConnexPortBase::PLACE_LEFT;
	input->SetDirection( wxConnexPortBase::DIR_INPUT);
	input->m_Rect.SetPosition( wxPoint( 0, 45 ) );

	output = new ImagePort(&result, ImagePort::GRAY);
	output->m_Placement = wxConnexPortBase::PLACE_RIGHT;
	output->SetDirection( wxConnexPortBase::DIR_OUTPUT );
	output->m_Rect.SetPosition( wxPoint( GetSize().x - 10, 45 ) );

	this->AddCXPort(input);
	this->AddCXPort(output);
}

void ToGreyscaleNode::onSave(wxCommandEvent& event)
{
	if (result.IsOk())
	{
		wxFileDialog saveFileDialog(this, _("Save Image"), "", "",
							   "PNG files (*.png )|*.png", wxFD_SAVE);

		if (saveFileDialog.ShowModal() == wxID_CANCEL)
			return;
		else
			result.SaveFile(saveFileDialog.GetPath(), wxBITMAP_TYPE_PNG);
	}
}

void ToGreyscaleNode::OnPortDataUpdate(wxConnexPortBase* port)
{
	ImagePort* updated = (ImagePort*)port;
	if (updated && updated->getImage() && updated->getImage()->IsOk())
	{
		result = updated->getImage()->ConvertToGreyscale();
		output->RefreshConnections();
		image->Refresh();
		Refresh();
	}
	else
	{
		result.Create(0, 0);
		output->RefreshConnections();
		image->Refresh();
		Refresh();
	}
}