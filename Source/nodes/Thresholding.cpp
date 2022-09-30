/* 
* Owen Shelton
* CSCI 8810 Spring 2011
* Project 2
*/

#include "connexspace.h"
#include "nodes/Thresholding.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

SimpleThresholdingNode::SimpleThresholdingNode(wxConnexSpace *parent, const wxPoint& pos): 
	NodeBase(parent, wxT("Simple Thr."), pos, wxSize(100, 95)), tValue(127), invert(false)
{
	result.Create(wxSize(0, 0));
	image = new ImagePanel(this, wxT("Simple Thresholding Operator Applied"), &result, true);
	image->SetSize(10,20, 78, 58);

	input = new ImagePort(NULL, ImagePort::ALL);
	input->m_Placement = wxConnexPortBase::PLACE_LEFT;
	input->SetDirection( wxConnexPortBase::DIR_INPUT);
	input->m_Rect.SetPosition( wxPoint( 0, 45 ) );

	output = new ImagePort(&result, ImagePort::BINARY);
	output->m_Placement = wxConnexPortBase::PLACE_RIGHT;
	output->SetDirection( wxConnexPortBase::DIR_OUTPUT );
	output->m_Rect.SetPosition( wxPoint( GetSize().x - 10, 45 ) );

	this->AddCXPort(input);
	this->AddCXPort(output);
}

void SimpleThresholdingNode::addProperties()
{
	if (grid)
	{
		grid->Append(new wxUIntProperty(wxT("Threshold"), wxT("Threshold"), tValue));
		wxBoolProperty* prop = new wxBoolProperty(wxT("Invert"), wxT("Invert"), invert);
		prop->SetAttribute("UseCheckbox", 1);
		grid->Append(prop);
	}
}

void SimpleThresholdingNode::onPropertyChanged(wxPropertyGridEvent& event)
{
	if (event.GetPropertyName()== "Threshold")
		tValue = (unsigned char)event.GetPropertyValue().GetInteger();
	else
		invert = event.GetPropertyValue().GetBool();
	OnPortDataUpdate(input);	
}

void SimpleThresholdingNode::onPropertyChanging(wxPropertyGridEvent& event)
{
	long tmp = event.GetValue().GetInteger();
	if (tmp > 255)
	{
		event.SetValidationFailureBehavior(wxPG_VFB_SHOW_MESSAGE);
		event.SetValidationFailureMessage(wxT("Threshold value must be between 0 and 255 inclusive."));
		event.Veto();
	}
}

void SimpleThresholdingNode::onSave(wxCommandEvent& event)
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

void SimpleThresholdingNode::OnPortDataUpdate(wxConnexPortBase* port)
{
	ImagePort* updated = (ImagePort*)port;
	if (updated && updated->getImage() && updated->getImage() != NULL && updated->getImage()->IsOk())
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

void SimpleThresholdingNode::apply(wxImage* image)
{
	ImageData data(image), resultData(&result);
	for (unsigned int i = 0; i < image->GetHeight(); ++i)
	{
		for (unsigned int j = 0; j < image->GetWidth(); ++j)
		{
			if (data.floatAt(i, j).asGray() < tValue)
				resultData.at(i, j) = Pixel(0, 0, 0);
			else
				resultData.at(i, j) = Pixel(255, 255, 255);
		}
	}

	if (invert)
	{
		for (unsigned int i = 0; i < image->GetHeight(); ++i)
		{
			for (unsigned int j = 0; j < image->GetWidth(); ++j)
			{
				if (resultData.at(i, j).asGray() == 255)
					resultData.at(i, j) = Pixel(0, 0, 0);
				else
					resultData.at(i, j) = Pixel(255, 255, 255);
			}
		}
	}
}

////////////////////////////////////////////////////////////////////

IterativeThresholdingNode::IterativeThresholdingNode(wxConnexSpace *parent, const wxPoint& pos): 
NodeBase(parent, wxT("Iterative Thr."), pos, wxSize(100, 95)), numIterations(0), finalT(0), invert(false)
{
	result.Create(wxSize(0, 0));
	image = new ImagePanel(this, wxT("Iterative Thresholding Operator Applied"), &result, true);
	image->SetSize(10,20, 78, 58);

	input = new ImagePort(NULL, ImagePort::ALL);
	input->m_Placement = wxConnexPortBase::PLACE_LEFT;
	input->SetDirection( wxConnexPortBase::DIR_INPUT);
	input->m_Rect.SetPosition( wxPoint( 0, 45 ) );

	output = new ImagePort(&result, ImagePort::BINARY);
	output->m_Placement = wxConnexPortBase::PLACE_RIGHT;
	output->SetDirection( wxConnexPortBase::DIR_OUTPUT );
	output->m_Rect.SetPosition( wxPoint( GetSize().x - 10, 45 ) );

	this->AddCXPort(input);
	this->AddCXPort(output);
}

void IterativeThresholdingNode::addProperties()
{
	if (grid)
	{
		wxBoolProperty* prop = new wxBoolProperty(wxT("Invert"), wxT("Invert"), invert);
		prop->SetAttribute("UseCheckbox", 1);
		grid->Append(prop);
		if (numIterations > 0)
		{
			wxPGId mainId = grid->Append(new wxPropertyCategory(wxT("Results")));
			grid->AppendIn(mainId, new wxUIntProperty(wxT("Number Iterations"), wxT("Iterations"), numIterations));
			grid->AppendIn(mainId, new wxUIntProperty(wxT("Final T"), wxT("Final"), finalT));
		}
	}
}

void IterativeThresholdingNode::onPropertyChanged(wxPropertyGridEvent& event)
{
	invert = event.GetPropertyValue().GetBool();
	OnPortDataUpdate(input);	
}

void IterativeThresholdingNode::onPropertyChanging(wxPropertyGridEvent& event)
{
	event.Veto();
}

void IterativeThresholdingNode::onSave(wxCommandEvent& event)
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

void IterativeThresholdingNode::OnPortDataUpdate(wxConnexPortBase* port)
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
		numIterations = 0;
		finalT = 0;
		output->RefreshConnections();
		image->Refresh();
		Refresh();
	}
}

void IterativeThresholdingNode::apply(wxImage* image)
{
	result.Create(image->GetSize());
	ImageData data(image), resultData(&result);
	float threshold = 0;
	unsigned long tmp = 0;
	//Calculate initial threshold value.
	for (unsigned int i = 0; i < data.height; ++i)
	{
		for (unsigned int j = 0; j < data.width; ++j)
		{
			tmp += data.at(i, j).asGray();
		}
	}
	threshold = tmp / (data.width * data.height);
    
	//Old threshold value;
	unsigned char old = 0;
	
	//Repeats until the difference between the calculated threshold and 
	//the previous one is less than or equal to 2.
	while (true)
	{
		wxImage tmp = image->Copy();
		ImageData tmpData(&tmp);
		//Threshold the image using the previous (or initial) threshold value.
		for (unsigned int i = 0; i < tmpData.height; ++i)
		{
			for (unsigned int j = 0; j < tmpData.width; ++j)
			{
				if (tmpData.at(i, j).asGray() < threshold)
					tmpData.at(i, j) = Pixel(0, 0, 0);
				else
					tmpData.at(i, j) = Pixel(255, 255, 255);
			}
		}

		++numIterations;
		finalT = threshold;

		//Old value becomes the current threshold value.
		old = threshold;
		//Setup.
		unsigned long whitePixels = 0;
		unsigned long whiteAverage = 0;
		unsigned long blackPixels = 0;
		unsigned long blackAverage = 0;

		//Calculate pixel coverage.
		for (unsigned int i = 0; i < tmpData.height; ++i)
		{
			for (unsigned int j = 0; j < tmpData.width; ++j)
			{
				if (tmpData.at(i, j).asGray() == 255)
				{
					++whitePixels;
					whiteAverage += data.at(i, j).asGray();
				}
				else
				{
					++blackPixels;
					blackAverage += data.at(i, j).asGray();
				}
			}
		}

		//recalculate threshold as a modified version of the weighted average.
		threshold = ((whiteAverage * 1.0 / whitePixels) + (blackAverage * 1.0 / blackPixels)) / 2.0;
	
		//Stop case, reapply changes to original image.
		if (abs(threshold - old) <= 1)
		{
			for (unsigned int i = 0; i < data.height; ++i)
			{
				for (unsigned int j = 0; j < data.width; ++j)
				{
					resultData.at(i, j) = tmpData.at(i, j);
				}
			}
			break;
		}
	}

	if (invert)
	{
		for (unsigned int i = 0; i < image->GetHeight(); ++i)
		{
			for (unsigned int j = 0; j < image->GetWidth(); ++j)
			{
				if (resultData.at(i, j).asGray() == 255)
					resultData.at(i, j) = Pixel(0, 0, 0);
				else
					resultData.at(i, j) = Pixel(255, 255, 255);
			}
		}
	}

	wxConnexSpace* space = (wxConnexSpace*)this->GetParent();
	if (space->GetFocusNode() == this && grid)
	{
		grid->Clear();
		addProperties();
	}
}

///////////////////////////////////////////////////////////////

AdaptiveThresholdingNode::AdaptiveThresholdingNode(wxConnexSpace *parent, const wxPoint& pos): 
NodeBase(parent, wxT("Adaptive Thr."), pos, wxSize(100, 95)), subdivisions(2), invert(false)
{
	result.Create(wxSize(0, 0));
	image = new ImagePanel(this, wxT("Adaptive Thresholding Operator Applied"), &result, true);
	image->SetSize(10,20, 78, 58);

	input = new ImagePort(NULL, ImagePort::ALL);
	input->m_Placement = wxConnexPortBase::PLACE_LEFT;
	input->SetDirection( wxConnexPortBase::DIR_INPUT);
	input->m_Rect.SetPosition( wxPoint( 0, 45 ) );

	output = new ImagePort(&result, ImagePort::BINARY);
	output->m_Placement = wxConnexPortBase::PLACE_RIGHT;
	output->SetDirection( wxConnexPortBase::DIR_OUTPUT );
	output->m_Rect.SetPosition( wxPoint( GetSize().x - 10, 45 ) );

	this->AddCXPort(input);
	this->AddCXPort(output);
}

void AdaptiveThresholdingNode::addProperties()
{
	if (grid)
	{
		grid->Append(new wxUIntProperty(wxT("Number Subdivisions"), wxT("Subdivisions"), subdivisions));
		wxBoolProperty* prop = new wxBoolProperty(wxT("Invert"), wxT("Invert"), invert);
		prop->SetAttribute("UseCheckbox", 1);
		grid->Append(prop);
	}
}

void AdaptiveThresholdingNode::onPropertyChanged(wxPropertyGridEvent& event)
{
	if (event.GetPropertyName() == "Subdivisions")
		subdivisions = event.GetPropertyValue().GetInteger();
	else
		invert = event.GetPropertyValue().GetBool();
	OnPortDataUpdate(input);	
}

void AdaptiveThresholdingNode::onPropertyChanging(wxPropertyGridEvent& event)
{
	if (event.GetPropertyName() == "Subdivisions")
	{
		if (event.GetValue().GetInteger() == 0 || event.GetValue().GetInteger() % 2 != 0)
		{
			event.SetValidationFailureBehavior(wxPG_VFB_SHOW_MESSAGE);
			event.SetValidationFailureMessage(wxT("Number of subdivisions bust be even and greater than 0."));
			event.Veto();
		}
	}
}

void AdaptiveThresholdingNode::onSave(wxCommandEvent& event)
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

void AdaptiveThresholdingNode::OnPortDataUpdate(wxConnexPortBase* port)
{
	ImagePort* updated = (ImagePort*)port;
	if (updated && updated->getImage() && updated->getImage() != NULL && updated->getImage()->IsOk())
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

void AdaptiveThresholdingNode::apply(wxImage* image)
{
	result.Create(image->GetSize());
	for (unsigned int i = 0; i < subdivisions; ++i)
	{
		for (unsigned int j = 0; j < subdivisions; ++j)
		{
			iterativeThresholdRegion(image, wxRect(1.0 * j / subdivisions * image->GetWidth(),
				1.0 * i / subdivisions * image->GetHeight(),
				1.0 * image->GetWidth() / subdivisions,
				1.0 * image->GetHeight() / subdivisions));
		}
	}
}

void AdaptiveThresholdingNode::iterativeThresholdRegion(wxImage* image, const wxRect& box)
{
	ImageData data(image), resultData(&result);
	float threshold = 0;
	unsigned long tmp = 0;
	//Calculate initial threshold value.
	for (unsigned int i = box.GetTop(); i < box.GetBottom(); ++i)
	{
		for (unsigned int j = box.GetLeft(); j < box.GetRight(); ++j)
		{
			tmp += data.at(i, j).asGray();
		}
	}
	threshold = tmp / (box.GetWidth() * box.GetHeight());

	unsigned char old = 0;
	
	//Repeats until the difference between the calculated threshold and 
	//the previous one is less than or equal to 2.
	while (true)
	{
		wxImage tmp = image->GetSubImage(box).Copy();
		ImageData tmpData(&tmp);
		//Threshold the image using the previous (or initial) threshold value.
		for (unsigned int i = 0; i < resultData.height; ++i)
		{
			for (unsigned int j = 0; j < resultData.width; ++j)
			{
				if (tmpData.at(i, j).asGray() < threshold)
					tmpData.at(i, j) = Pixel(0, 0, 0);
				else
					tmpData.at(i, j) = Pixel(255, 255, 255);
			}
		}

		//Setup.
		unsigned long whitePixels = 0;
		unsigned long whiteAverage = 0;
		unsigned long blackPixels = 0;
		unsigned long blackAverage = 0;

		old = threshold;

		//Calculate pixel coverage.
		for (unsigned int i = 0; i < tmpData.height; ++i)
		{
			for (unsigned int j = 0; j < tmpData.width; ++j)
			{
				if (tmpData.at(i, j).asGray() == 255)
				{
					++whitePixels;
					whiteAverage += data.at(i + box.GetTop(), j + box.GetLeft()).asGray();
				}
				else
				{
					++blackPixels;
					blackAverage += data.at(i + box.GetTop(), j + box.GetLeft()).asGray();
				}
			}
		}

		//recalculate threshold as a modified version of the weighted average.
		threshold = ((whiteAverage * 1.0 / whitePixels) + (blackAverage * 1.0 / blackPixels)) / 2.0;
		//Stop case, reapply changes to original image.
		if (abs(threshold - old) <= 1)
		{
			for (unsigned int i = 0; i < resultData.height; ++i)
			{
				for (unsigned int j = 0; j < resultData.width; ++j)
				{
					resultData.at(i + box.GetTop(), j + box.GetLeft()) = resultData.at(i, j);
				}
			}
			break;
		}
	}

	if (invert)
	{
		for (unsigned int i = box.GetTop(); i < box.GetBottom(); ++i)
		{
			for (unsigned int j = box.GetLeft(); j < box.GetRight(); ++j)
			{
				if (resultData.at(i + box.GetTop(), j + box.GetLeft()).asGray() == 255)
					resultData.at(i + box.GetTop(), j + box.GetLeft()) = Pixel(0, 0, 0);
				else
					resultData.at(i + box.GetTop(), j + box.GetLeft()) = Pixel(255, 255, 255);
			}
		}
	}
}