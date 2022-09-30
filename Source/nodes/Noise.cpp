/* 
* Owen Shelton
* CSCI 8810 Spring 2011
* Project 2
*/

#include <vector>
#include <algorithm>
#include "nodes/Noise.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

PixelNoiseNode::PixelNoiseNode(wxConnexSpace *parent, const wxPoint& pos): 
	NodeBase(parent, wxT("Pixel Noise"), pos, wxSize(100, 95)), pixels(1), grayscale(false)
{
	result.Create(0, 0);
	image = new ImagePanel(this, wxT("After Pixel Noise Added"), &result, true);
	image->SetSize(10, 20, 78, 58);

	input = new ImagePort(NULL, ImagePort::ALL);
	input->m_Placement = wxConnexPortBase::PLACE_LEFT;
	input->SetDirection( wxConnexPortBase::DIR_INPUT);
	input->m_Rect.SetPosition( wxPoint( 0, 45 ) );

	output = new ImagePort(&result, ImagePort::COLOR_GRAY);
	output->m_Placement = wxConnexPortBase::PLACE_RIGHT;
	output->SetDirection( wxConnexPortBase::DIR_OUTPUT );
	output->m_Rect.SetPosition( wxPoint( GetSize().x - 10, 45 ) );

	this->AddCXPort(input);
	this->AddCXPort(output);
}

void PixelNoiseNode::addProperties()
{
	if (grid)
	{
		grid->Append(new wxUIntProperty(wxT("Pixels"), wxT("Pixels"), pixels));
		wxBoolProperty* prop = new wxBoolProperty(wxT("Grayscale"), wxT("Grayscale"), grayscale);
		prop->SetAttribute("UseCheckbox", 1);
		grid->Append(prop);
	}
}

void PixelNoiseNode::onPropertyChanged(wxPropertyGridEvent& event)
{
	if (event.GetPropertyName() == "Pixels")
		pixels = (unsigned int)event.GetProperty()->GetValue().GetInteger();
	else
		grayscale = event.GetProperty()->GetValue().GetBool();
	OnPortDataUpdate(input);	
}

void PixelNoiseNode::onSave(wxCommandEvent& event)
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

void PixelNoiseNode::OnPortDataUpdate(wxConnexPortBase* port)
{
	ImagePort* updated = (ImagePort*)port;
	if (updated && updated->getImage() && updated->getImage()->IsOk())
	{
		apply(updated->getImage());
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

void PixelNoiseNode::apply(wxImage* image)
{
	result = image->Copy();
	ImageData data(&result);
    
	unsigned int randColumn = 0;
	unsigned int randRow = 0;

	for (unsigned int i = 0; i < pixels; ++i)
	{
		randColumn = rand() % image->GetWidth();
		randRow = rand() % image->GetHeight();

		if (grayscale)
			data.at(randRow, randColumn) = Pixel(rand() % 256);
		else
			data.at(randRow, randColumn) = Pixel(rand() % 256,
				rand() % 256,
				rand() % 256);
	}
}

/////////////////////////////////////////////////////

ClusterNoiseNode::ClusterNoiseNode(wxConnexSpace *parent, const wxPoint& pos): 
	NodeBase(parent, wxT("Cluster Noise"), pos, wxSize(100, 95)), clusters(1), grayscale(false)
{
	result.Create(0, 0);
	image = new ImagePanel(this, wxT("After Cluster Noise Added"), &result, true);
	image->SetSize(10, 20, 78, 58);

	input = new ImagePort(NULL, ImagePort::ALL);
	input->m_Placement = wxConnexPortBase::PLACE_LEFT;
	input->SetDirection( wxConnexPortBase::DIR_INPUT);
	input->m_Rect.SetPosition( wxPoint( 0, 45 ) );

	output = new ImagePort(&result, ImagePort::COLOR_GRAY);
	output->m_Placement = wxConnexPortBase::PLACE_RIGHT;
	output->SetDirection( wxConnexPortBase::DIR_OUTPUT );
	output->m_Rect.SetPosition( wxPoint( GetSize().x - 10, 45 ) );

	this->AddCXPort(input);
	this->AddCXPort(output);
}

void ClusterNoiseNode::addProperties()
{
	if (grid)
	{
		grid->Append(new wxUIntProperty(wxT("Clusters"), wxT("Clusters"), clusters));
		wxBoolProperty* prop = new wxBoolProperty(wxT("Grayscale"), wxT("Grayscale"), grayscale);
		prop->SetAttribute("UseCheckbox", 1);
		grid->Append(prop);
	}
}

void ClusterNoiseNode::onPropertyChanged(wxPropertyGridEvent& event)
{
	if (event.GetPropertyName() == "Clusters")
		clusters = (unsigned int)event.GetProperty()->GetValue().GetInteger();
	else
		grayscale = event.GetProperty()->GetValue().GetBool();
	OnPortDataUpdate(input);	
}

void ClusterNoiseNode::onSave(wxCommandEvent& event)
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

void ClusterNoiseNode::OnPortDataUpdate(wxConnexPortBase* port)
{
	ImagePort* updated = (ImagePort*)port;
	if (updated && updated->getImage() && updated->getImage()->IsOk())
	{
		apply(updated->getImage());
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

void ClusterNoiseNode::apply(wxImage* image)
{
	result = image->Copy();
	ImageData data(&result);
    
	unsigned int randColumn = 0;
	unsigned int randRow = 0;

	for (unsigned int i = 0; i < clusters; ++i)
	{
		randColumn = rand() % image->GetWidth();
		randRow = rand() % image->GetHeight();

		Pixel randValue(0);
		if (grayscale)
			randValue = Pixel(rand() % 256); 
		else
			randValue = Pixel(rand() % 256,
				rand() % 256,
				rand() % 256);

		data.at(randRow, randColumn) = randValue;

		if (randColumn > 0)
			data.at(randRow, randColumn - 1) = randValue;
		if (randColumn < image->GetWidth())
			data.at(randRow, randColumn + 1) = randValue;

		if (randRow > 0)
			data.at(randRow - 1, randColumn) = randValue;
		if (randRow < image->GetHeight())
			data.at(randRow + 1, randColumn) = randValue;
	}
}

/////////////////////////////////////////////////////

AdditiveNoiseNode::AdditiveNoiseNode(wxConnexSpace *parent, const wxPoint& pos): 
	NodeBase(parent, wxT("Additive Noise"), pos, wxSize(100, 95)), sigma(1), gain(1)
{
	result.Create(0, 0);
	image = new ImagePanel(this, wxT("After Additive Gaussian Noise Added"), &result, true);
	image->SetSize(10, 20, 78, 58);

	input = new ImagePort(NULL, ImagePort::ALL);
	input->m_Placement = wxConnexPortBase::PLACE_LEFT;
	input->SetDirection( wxConnexPortBase::DIR_INPUT);
	input->m_Rect.SetPosition( wxPoint( 0, 45 ) );

	output = new ImagePort(&result, ImagePort::COLOR_GRAY);
	output->m_Placement = wxConnexPortBase::PLACE_RIGHT;
	output->SetDirection( wxConnexPortBase::DIR_OUTPUT );
	output->m_Rect.SetPosition( wxPoint( GetSize().x - 10, 45 ) );

	this->AddCXPort(input);
	this->AddCXPort(output);
}

void AdditiveNoiseNode::addProperties()
{
	if (grid)
	{
		grid->Append(new wxFloatProperty(wxT("Sigma"), wxT("Sigma"), sigma));
		grid->Append(new wxFloatProperty(wxT("Gain"), wxT("Gain"), gain));
	}
}

void AdditiveNoiseNode::onPropertyChanged(wxPropertyGridEvent& event)
{
	if (event.GetPropertyName() == "Sigma")
		sigma = event.GetProperty()->GetValue().GetDouble();
	else
		gain = event.GetProperty()->GetValue().GetDouble();
	OnPortDataUpdate(input);	
}

void AdditiveNoiseNode::onPropertyChanging(wxPropertyGridEvent& event)
{
	wxString propertyName = event.GetPropertyName();
	wxString value = event.GetPropertyValue().GetString();
	wxString value2 = event.GetValue().GetString();
	if (event.GetPropertyName() == wxT("Sigma"))
	{
		if (event.GetValue().GetDouble() <= 0)
		{
			event.SetValidationFailureBehavior(wxPG_VFB_SHOW_MESSAGE);
			event.SetValidationFailureMessage(wxT("Sigma value must be greater than 0."));
			event.Veto();
		}
	}
	else
	{
		if (event.GetValue().GetDouble() < 0)
		{
			event.SetValidationFailureBehavior(wxPG_VFB_SHOW_MESSAGE);
			event.SetValidationFailureMessage(wxT("Gain must be positive."));
			event.Veto();
		}
	}
}

void AdditiveNoiseNode::onSave(wxCommandEvent& event)
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

void AdditiveNoiseNode::OnPortDataUpdate(wxConnexPortBase* port)
{
	ImagePort* updated = (ImagePort*)port;
	if (updated && updated->getImage() && updated->getImage()->IsOk())
	{
		apply(updated->getImage());
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

void AdditiveNoiseNode::apply(wxImage* image)
{
	result = image->Copy();
	FloatImageData data(&result);
    
	unsigned int randColumn = 0;
	unsigned int randRow = 0;

	for (unsigned int i = 0; i < data.height; ++i)
	{
		for (unsigned int j = 0; j < data.width; ++j)
		{
			FloatPixel tmp = gaussianRandom(sigma, gain);
			data.setAt(data.at(i, j) + tmp, i, j);	
		}
	}
}