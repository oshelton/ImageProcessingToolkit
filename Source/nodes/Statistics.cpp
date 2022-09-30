/* 
* Owen Shelton
* CSCI 8810 Spring 2011
* Project 2
*/

#include "connexspace.h"
#include "nodes/Statistics.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CCLNode::CCLNode(wxConnexSpace *parent, const wxPoint& pos): 
	NodeBase(parent, wxT("CCL"), pos, wxSize(100, 95)), sizeThreshold(0)
{
	result.Create(wxSize(0, 0));
	image = new ImagePanel(this, wxT("Connected Component Labeling Operator Applied."), &result, true);
	image->SetSize(10,20, 78, 58);

	input = new ImagePort(NULL, ImagePort::BINARY);
	input->m_Placement = wxConnexPortBase::PLACE_LEFT;
	input->SetDirection( wxConnexPortBase::DIR_INPUT);
	input->m_Rect.SetPosition( wxPoint( 0, 45 ) );

	this->AddCXPort(input);
}

void CCLNode::addProperties()
{
	if (grid)
	{
		grid->Append(new wxUIntProperty(wxT("Size Threhsold"), wxT("Size"), sizeThreshold));
		if (components.size() > 0)
		{
			wxPGId mainId = grid->Append(new wxPropertyCategory(wxT("Components")));
			grid->AppendIn(mainId, new wxUIntProperty(wxT("Count"), wxT("Count"), components.size()));
			
			float average = 0;
			for (unsigned int i = 0; i < components.size(); ++i)
				average += components[i]->getArea();
			average /= components.size();
			grid->AppendIn(mainId, new wxFloatProperty(wxT("Average Area"), wxT("Average Area"), average));

			wxPGId comId;
			for (unsigned int i = 0; i < components.size(); ++i)
			{
				comId = grid->AppendIn(mainId, new wxUIntProperty(wxT("Label"), wxString::Format("Label%d", components[i]->getLabel()), 
					components[i]->getLabel()));
				grid->AppendIn(comId, new wxUIntProperty(wxT("Area"), wxT("Area"), components[i]->getArea()));
				grid->AppendIn(comId, new wxStringProperty(wxT("Centroid"), wxT("Centroid"),
					wxString::Format("(%d, %d)", components[i]->getCentroid().row, components[i]->getCentroid().column)));
				wxPGId boxId = grid->AppendIn(comId, new wxStringProperty(wxT("Bounding Box"), wxT("bBox"),
					wxT("<composed>")));
				grid->AppendIn(boxId, new wxStringProperty(wxT("Top Left"), wxT("tLeft"),
					wxString::Format("(%d, %d)", components[i]->getTopLeft().row, components[i]->getTopLeft().column)));
				grid->AppendIn(boxId, new wxStringProperty(wxT("Top Right"), wxT("tRight"),
					wxString::Format("(%d, %d)", components[i]->getTopRight().row, components[i]->getTopRight().column)));
				grid->AppendIn(boxId, new wxStringProperty(wxT("Bottom Left"), wxT("bLeft"),
					wxString::Format("(%d, %d)", components[i]->getBottomLeft().row, components[i]->getBottomLeft().column)));
				grid->AppendIn(boxId, new wxStringProperty(wxT("Bottom Right"), wxT("bRight"),
					wxString::Format("(%d, %d)", components[i]->getBottomRight().row, components[i]->getBottomRight().column)));
			}
			grid->CollapseAll();
		}
	}
}

void CCLNode::onPropertyChanged(wxPropertyGridEvent& event)
{
	sizeThreshold = (unsigned int)event.GetProperty()->GetValue().GetInteger();
	OnPortDataUpdate(input);	
}

void CCLNode::onPropertyChanging(wxPropertyGridEvent& event)
{
	if (event.GetPropertyName() != "Size")
		event.Veto();
}

void CCLNode::onSave(wxCommandEvent& event)
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

void CCLNode::OnPortDataUpdate(wxConnexPortBase* port)
{
	ImagePort* updated = (ImagePort*)port;
	if (updated && updated->getImage() && updated->getImage()->IsOk())
	{
		for (unsigned int i = 0; i < components.size(); ++i)
			delete components[i];
		components.clear();

		apply(updated->getImage());
		image->Refresh();
		Refresh();
	}
	else
	{
		for (unsigned int i = 0; i < components.size(); ++i)
			delete components[i];
		components.clear();

		result.Create(0, 0);
		Refresh();
	}
}

void CCLNode::apply(wxImage* image)
{
	result = image->Copy();
	ImageData data(&result);
	components = calculateConnectedComponents(&data, sizeThreshold);

	wxConnexSpace* space = (wxConnexSpace*)this->GetParent();
	if (space->GetFocusNode() == this && grid)
	{
		grid->Clear();
		addProperties();
	}
}