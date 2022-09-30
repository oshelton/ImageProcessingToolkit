/* 
* Owen Shelton
* CSCI 8810 Spring 2011
* Project 2
*/

#include <vector>
#include <algorithm>
#include "nodes/NoiseReduction.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

AveragingNode::AveragingNode(wxConnexSpace *parent, const wxPoint& pos): 
	NodeBase(parent, wxT("Averaging"), pos, wxSize(100, 95)), maskSize(3), iterations(1)
{
	result.Create(wxSize(0, 0));
	image = new ImagePanel(this, wxT("Averaging Filter Operator Applied"), &result, true);
	image->SetSize(10, 20, 78, 58);

	input = new ImagePort(NULL, ImagePort::ALL);
	input->m_Placement = wxConnexPortBase::PLACE_LEFT;
	input->SetDirection( wxConnexPortBase::DIR_INPUT);
	input->m_Rect.SetPosition( wxPoint( 0, 45 ) );

	output = new ImagePort(&result, ImagePort::COLOR);
	output->m_Placement = wxConnexPortBase::PLACE_RIGHT;
	output->SetDirection( wxConnexPortBase::DIR_OUTPUT );
	output->m_Rect.SetPosition( wxPoint( GetSize().x - 10, 45 ) );

	this->AddCXPort(input);
	this->AddCXPort(output);
}

void AveragingNode::addProperties()
{
	if (grid)
	{
		grid->Append(new wxUIntProperty(wxT("Mask Size"), wxT("Size"), maskSize));
		grid->Append(new wxUIntProperty(wxT("Iterations"), wxT("Iterations"), iterations));
	}
}

void AveragingNode::onPropertyChanged(wxPropertyGridEvent& event)
{
	if (event.GetPropertyName() == wxT("Size"))
		maskSize = (unsigned int)event.GetProperty()->GetValue().GetInteger();
	else
		iterations = (unsigned int)event.GetProperty()->GetValue().GetInteger();
	OnPortDataUpdate(input);	
}

void AveragingNode::onPropertyChanging(wxPropertyGridEvent& event)
{
	
	if (event.GetPropertyName() == wxT("Size"))
	{
		if (event.GetValue().GetInteger() < 1 ||
			event.GetValue().GetInteger() % 2 != 1)
		{
			event.SetValidationFailureBehavior(wxPG_VFB_SHOW_MESSAGE);
			event.SetValidationFailureMessage(wxT("Mask Size value must be greater than 0 and odd."));
			event.Veto();
		}
	}
	else
	{
		if (event.GetValue().GetInteger() < 1)
		{
			event.SetValidationFailureBehavior(wxPG_VFB_SHOW_MESSAGE);
			event.SetValidationFailureMessage(wxT("At least 1 iteration must be performed."));
			event.Veto();
		}
	}
}

void AveragingNode::onSave(wxCommandEvent& event)
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

void AveragingNode::OnPortDataUpdate(wxConnexPortBase* port)
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

void AveragingNode::apply(wxImage* image)
{
	result = image->Copy();
	wxImage tmp = image->Copy();
	ImageData source(&tmp), resultData(&result);

	int halfMask = maskSize / 2;
	unsigned int imageHeight = image->GetHeight() - halfMask;
	unsigned int imageWidth = image->GetWidth() - halfMask;
	float maskSquared =  maskSize * maskSize * 1.0f;

	for (unsigned int i = 0; i < iterations; ++i)
	{
		source = &tmp;
		for (unsigned int i = halfMask; i < imageHeight; ++i)
		{
			for (unsigned int j = halfMask; j < imageWidth; ++j)
			{
				FloatPixel sum(0, 0, 0);
				for (int k = 0 - halfMask; k <= halfMask; ++k)
				{
					for (int l = 0 - halfMask; l <= halfMask; ++l)
						sum += source.at(i + k, j + l);
				}

				resultData.setFloatAt(floor(sum / maskSquared + 0.5), i, j);
			}
		}
		tmp = result.Copy();
	}
}

/////////////////////////////////////////////////////

MedianNode::MedianNode(wxConnexSpace *parent, const wxPoint& pos): 
	NodeBase(parent, wxT("Median"), pos, wxSize(100, 95)), maskSize(3), iterations(1)
{
	result.Create(0, 0);
	image = new ImagePanel(this, wxT("Median Filter Operator Applied"), &result, true);
	image->SetSize(10, 20, 78, 58);

	input = new ImagePort(NULL, ImagePort::ALL);
	input->m_Placement = wxConnexPortBase::PLACE_LEFT;
	input->SetDirection( wxConnexPortBase::DIR_INPUT);
	input->m_Rect.SetPosition( wxPoint( 0, 45 ) );

	output = new ImagePort(&result, ImagePort::COLOR);
	output->m_Placement = wxConnexPortBase::PLACE_RIGHT;
	output->SetDirection( wxConnexPortBase::DIR_OUTPUT );
	output->m_Rect.SetPosition( wxPoint( GetSize().x - 10, 45 ) );

	this->AddCXPort(input);
	this->AddCXPort(output);
}

void MedianNode::addProperties()
{
	if (grid)
	{
		grid->Append(new wxUIntProperty(wxT("Mask Size"), wxT("Size"), maskSize));
		grid->Append(new wxUIntProperty(wxT("Iterations"), wxT("Iterations"), iterations));
	}
}

void MedianNode::onPropertyChanged(wxPropertyGridEvent& event)
{
	if (event.GetPropertyName() == wxT("Size"))
		maskSize = (unsigned int)event.GetProperty()->GetValue().GetInteger();
	else
		iterations = (unsigned int)event.GetProperty()->GetValue().GetInteger();
	OnPortDataUpdate(m_Ports.back());	
}

void MedianNode::onPropertyChanging(wxPropertyGridEvent& event)
{
	
	if (event.GetPropertyName() == wxT("Size"))
	{
		if (event.GetValue().GetInteger() < 1 ||
			event.GetValue().GetInteger() % 2 != 1)
		{
			event.SetValidationFailureBehavior(wxPG_VFB_SHOW_MESSAGE);
			event.SetValidationFailureMessage(wxT("Mask Size value must be greater than 0 and odd."));
			event.Veto();
		}
	}
	else
	{
		if (event.GetValue().GetInteger() < 1)
		{
			event.SetValidationFailureBehavior(wxPG_VFB_SHOW_MESSAGE);
			event.SetValidationFailureMessage(wxT("At least 1 iteration must be performed."));
			event.Veto();
		}
	}
}

void MedianNode::onSave(wxCommandEvent& event)
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

void MedianNode::OnPortDataUpdate(wxConnexPortBase* port)
{
	ImagePort* updated = (ImagePort*)port;
	if (updated && updated->getImage()->IsOk())
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

void MedianNode::apply(wxImage* image)
{
	result = image->Copy();
	wxImage tmp = image->Copy();
	ImageData source(&tmp), resultData(&result);

	int halfMask = maskSize / 2;
	unsigned int imageHeight = image->GetHeight() - halfMask;
	unsigned int imageWidth = image->GetWidth() - halfMask;

	vector<FloatPixel> pixels;
	pixels.reserve(maskSize * maskSize);
	pixels.resize(maskSize * maskSize);
	unsigned int count = 0;

	for (unsigned int i = 0; i < iterations; ++i)
	{
		source = &tmp;
		for (unsigned int i = halfMask; i < imageHeight; ++i)
		{
			for (unsigned int j = halfMask; j < imageWidth; ++j)
			{
				for (int k = 0 - halfMask; k <= halfMask; ++k)
				{
					for (int l = 0 - halfMask; l <= halfMask; ++l, ++count)
						pixels[count] = source.at(i + k, j + l);
				}
				count = 0;
				sort(pixels.begin(), pixels.end());
				resultData.setFloatAt(pixels[halfMask], i, j);
			}
		}
		tmp = result.Copy();
	}
}