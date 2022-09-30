/* 
* Owen Shelton
* CSCI 8810 Spring 2011
* Project 2
*/

#include <math.h>
#include "nodes/EdgeDetection.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

RobertsNode::RobertsNode(wxConnexSpace *parent, const wxPoint& pos): 
	NodeBase(parent, wxT("Roberts"), pos, wxSize(100, 95))
{
	result.Create(0, 0);
	image = new ImagePanel(this, wxT("Roberts Operator Applied"), &result, true);
	image->SetSize(10,20, 78, 58);

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

void RobertsNode::onSave(wxCommandEvent& event)
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

void RobertsNode::OnPortDataUpdate(wxConnexPortBase* port)
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

void RobertsNode::apply(wxImage* source)
{
	result.Create(source->GetSize());
	ImageData sourceData(source), resultData(&result);
	
	for (unsigned int i = 0; i < source->GetHeight() - 1; ++i)
	{
		for (unsigned int j = 0; j < source->GetWidth() - 1; ++j)
		{
			FloatPixel value = abs(sourceData.floatAt(i, j) - sourceData.floatAt(i + 1, j + 1)) + 
				abs(sourceData.floatAt(i, j + 1) - sourceData.floatAt(i + 1, j));
			resultData.setFloatAt(value, i, j);
		}
	}
}

/////////////////////////////////////////////////////////////////////////////////////////

LaplacianNode::LaplacianNode(wxConnexSpace *parent, const wxPoint& pos): 
	NodeBase(parent, wxT("Laplacian"), pos, wxSize(100, 95)), zeroCross(false)
{
	result.Create(0, 0);
	image = new ImagePanel(this, wxT("Laplacian Operator Applied"), &result, true);
	image->SetSize(10,20, 78, 58);

	input = new ImagePort(NULL, ImagePort::ALL);
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

void LaplacianNode::addProperties()
{
	if (grid)
	{
		wxBoolProperty* prop = new wxBoolProperty(wxT("Zero Crossings"), wxT("Zero Crossings"), zeroCross);
		prop->SetAttribute("UseCheckbox", 1);
		grid->Append(prop);
	}
}

void LaplacianNode::onPropertyChanged(wxPropertyGridEvent& event)
{
	zeroCross = event.GetPropertyValue().GetBool();
	OnPortDataUpdate(input);	
}

void LaplacianNode::onSave(wxCommandEvent& event)
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

void LaplacianNode::OnPortDataUpdate(wxConnexPortBase* port)
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

void LaplacianNode::apply(wxImage* source)
{
	result.Create(source->GetSize());
	ImageData sourceData(source);
	FloatImageData resultData(&result);
	
	for (unsigned int i = 1; i < source->GetHeight() - 1; ++i)
	{
		for (unsigned int j = 1; j < source->GetWidth() - 1; ++j)
		{
			float value = (sourceData.floatAt(i, j + 1) + sourceData.floatAt(i, j - 1) +
				sourceData.floatAt(i + 1, j) + sourceData.floatAt(i - 1, j)).asGray();
			value += (sourceData.floatAt(i - 1, j - 1) + sourceData.floatAt(i - 1, j + 1) +
				sourceData.floatAt(i + 1, j - 1) + sourceData.floatAt(i + 1, j + 1)).asGray();
			value -= (sourceData.floatAt(i, j) * 8.0f).asGray();

			resultData.setAt(FloatPixel(value), i, j);
		}
	}
	if (!zeroCross)
		resultData.normalize(resultData.getMin(), resultData.getMax());
	else
	{
		wxImage tmp;
		tmp.Create(source->GetSize());
		ImageData tmpData(&tmp);
		
		//Horizontal pass.
		for (unsigned int i = 1; i < tmpData.height - 1; ++i)
		{
			for (unsigned int j = 1; j < tmpData.width - 2; ++j)
			{
				float gray = resultData.at(i, j).r;

				if (abs(gray) < 0.005)
					continue;
				else if (resultData.at(i, j).r < 0  && resultData.at(i, j + 1).r > 0)
					tmpData.at(i, j) = Pixel(255);
				else if (resultData.at(i, j).r > 0 && resultData.at(i, j + 1).r < 0)
					tmpData.at(i, j) = Pixel(255);
				else if (j < tmpData.width - 2 && resultData.at(i, j).r < 0 &&
					abs(resultData.at(i, j + 1).r) < 0.005 &&
					resultData.at(i, j + 2).r > 0)
				{
					tmpData.at(i, j + 1) = Pixel(255);
					++j;
				}
				else if (j < tmpData.width - 2 && resultData.at(i, j).r > 0 &&
					abs(resultData.at(i, j + 1).r) < 0.005 &&
					resultData.at(i, j + 2).r < 0)
				{
					tmpData.at(i, j + 1) = Pixel(255);
					++j;
				}
			}
		}
			
		//Vetical pass.
		for (unsigned int j = 0; j < tmpData.width; ++j)
		{
			for (unsigned int i = 0; i < tmpData.height - 1; ++i)
			{
				if (abs(resultData.at(i, j).r) < 0.005)
					continue;
				else if (resultData.at(i, j).r < 0  && resultData.at(i + 1, j).r > 0)
					tmpData.at(i, j) = Pixel(255);
				else if (resultData.at(i, j).r > 0 && resultData.at(i + 1, j).r < 0)
					tmpData.at(i, j) = Pixel(255);
				else if (i < tmpData.height - 2 && resultData.at(i, j).r < 0 &&
					abs(resultData.at(i + 1, j).r) < 0.005 &&
					resultData.at(i + 2, j).r > 0)
				{
					tmpData.at(i + 1, j) = Pixel(255);
					++i;
				}
				else if (i < tmpData.height - 2 && resultData.at(i, j).r > 0 &&
					abs(resultData.at(i + 1, j).r) < 0.005 &&
					resultData.at(i + 2, j).r < 0)
				{
					tmpData.at(i + 1, j) = Pixel(255);
					++i;
				}
			}
		}
		result = tmp.Copy();
	}
}

/////////////////////////////////////////////////////////////////////////////////////////

SobelNode::SobelNode(wxConnexSpace *parent, const wxPoint& pos): 
	NodeBase(parent, wxT("Sobel"), pos, wxSize(100, 95))
{
	result.Create(0, 0);
	image = new ImagePanel(this, wxT("Sobel Operator Applied"), &result, true);
	image->SetSize(10,20, 78, 58);

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

void SobelNode::onSave(wxCommandEvent& event)
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

void SobelNode::OnPortDataUpdate(wxConnexPortBase* port)
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

void SobelNode::apply(wxImage* source)
{
	result.Create(source->GetSize());
	ImageData sourceData(source), resultData(&result);
	
	FloatPixel mask1Value(0, 0, 0);
	FloatPixel mask2Value(0, 0, 0);

	for (unsigned int i = 1; i < source->GetHeight() - 1; ++i)
	{
		for (unsigned int j = 1; j < source->GetWidth() - 1; ++j)
		{
			mask1Value = sourceData.floatAt(i - 1, j - 1) +  sourceData.floatAt(i - 1, j) * 2.0f +
				sourceData.floatAt(i - 1, j + 1);
			mask1Value += -sourceData.floatAt(i + 1, j - 1) - sourceData.floatAt(i + 1, j) * 2.0f -
				sourceData.floatAt(i + 1, j + 1);
			mask2Value = -sourceData.floatAt(i - 1, j - 1) - sourceData.floatAt(i, j - 1) * 2.0f  -
				sourceData.floatAt(i + 1, j - 1);
			mask2Value += sourceData.floatAt(i - 1, j + 1) + sourceData.floatAt(i, j + 1) * 2.0f  +
				sourceData.floatAt(i + 1, j + 1);
			FloatPixel value = sqrt(pow(mask1Value, 2) + pow(mask2Value, 2));
			
			resultData.setFloatAt(value, i, j);
		}
	}
}

////////////////////////////////////////////////////////////////

PrewittNode::PrewittNode(wxConnexSpace *parent, const wxPoint& pos): 
	NodeBase(parent, wxT("Prewitt"), pos, wxSize(100, 95))
{
	result.Create(0, 0);
	image = new ImagePanel(this, wxT("Prewitt Operator Applied"), &result, true);
	image->SetSize(10,20, 78, 58);

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

void PrewittNode::onSave(wxCommandEvent& event)
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

void PrewittNode::OnPortDataUpdate(wxConnexPortBase* port)
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

void PrewittNode::apply(wxImage* source)
{
	result.Create(0, 0);
	ImageData sourceData(source), resultData(&result);

	FloatPixel mask1Value(0, 0, 0);
	FloatPixel mask2Value(0, 0, 0);

	for (unsigned int i = 1; i < source->GetHeight() - 1; ++i)
	{
		for (unsigned int j = 1; j < source->GetWidth() - 1; ++j)
		{
			mask1Value = sourceData.floatAt(i - 1, j - 1) +  sourceData.floatAt(i - 1, j) +
				sourceData.floatAt(i - 1, j + 1);
			mask1Value += -sourceData.floatAt(i + 1, j - 1) - sourceData.floatAt(i + 1, j) -
				sourceData.floatAt(i + 1, j + 1);
			mask2Value = -sourceData.floatAt(i - 1, j - 1) - sourceData.floatAt(i, j - 1)  -
				sourceData.floatAt(i + 1, j - 1);
			mask2Value += sourceData.floatAt(i - 1, j + 1) + sourceData.floatAt(i, j + 1)  +
				sourceData.floatAt(i + 1, j + 1);
			FloatPixel value = sqrt(pow(mask1Value, 2) + pow(mask2Value, 2));
			
			resultData.setFloatAt(value, i, j);
		}
	}
}

/////////////////////////////////////////////////////

KershNode::KershNode(wxConnexSpace *parent, const wxPoint& pos): 
	NodeBase(parent, wxT("Kersh"), pos, wxSize(100, 95))
{
	result.Create(0, 0);
	image = new ImagePanel(this, wxT("Kersh Operator Applied"), &result, true);
	image->SetSize(10,20, 78, 58);

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

void KershNode::onSave(wxCommandEvent& event)
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

void KershNode::OnPortDataUpdate(wxConnexPortBase* port)
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

void KershNode::apply(wxImage* source)
{
	result.Create(0, 0);
	ImageData sourceData(source), resultData(&result);

	FloatPixel mask1Value(0, 0, 0);
	FloatPixel mask2Value(0, 0, 0);

	for (unsigned int i = 1; i < source->GetHeight() - 1; ++i)
	{
		for (unsigned int j = 1; j < source->GetWidth() - 1; ++j)
		{
			mask1Value = sourceData.floatAt(i - 1, j) +  sourceData.floatAt(i - 1, j + 1) +
				sourceData.floatAt(i, j + 1);
			mask1Value += -sourceData.floatAt(i, j - 1) - sourceData.floatAt(i + 1, j - 1) -
				sourceData.floatAt(i + 1, j);
			mask2Value = -sourceData.floatAt(i + 1, j) - sourceData.floatAt(i + 1, j + 1)  -
				sourceData.floatAt(i, j + 1);
			mask2Value += sourceData.floatAt(i, j - 1) + sourceData.floatAt(i - 1, j - 1)  +
				sourceData.floatAt(i - 1, j);
			FloatPixel value = sqrt(pow(mask1Value, 2) + pow(mask2Value, 2));
			
			resultData.setFloatAt(value, i, j);
		}
	}
}

////////////////////////////////////////////////////////////////////

LOGNode::LOGNode(wxConnexSpace *parent, const wxPoint& pos): 
	NodeBase(parent, wxT("LOG"), pos, wxSize(100, 95)), sigma(1)
{
	result.Create(0, 0);
	image = new ImagePanel(this, wxT("Laplacian of Gaussian Operator Applied"), &result, true);
	image->SetSize(10, 20, 78, 58);

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

void LOGNode::addProperties()
{
	if (grid)
		grid->Append(new wxFloatProperty(wxT("Sigma"), wxT("Sigma"), sigma));
}

void LOGNode::onPropertyChanged(wxPropertyGridEvent& event)
{
	sigma = event.GetProperty()->GetValue().GetDouble();
	OnPortDataUpdate(input);	
}

void LOGNode::onPropertyChanging(wxPropertyGridEvent& event)
{
	
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
		if (event.GetValue().GetInteger() < 1)
		{
			event.SetValidationFailureBehavior(wxPG_VFB_SHOW_MESSAGE);
			event.SetValidationFailureMessage(wxT("At least 1 iteration must be performed."));
			event.Veto();
		}
	}
}

void LOGNode::onSave(wxCommandEvent& event)
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

void LOGNode::OnPortDataUpdate(wxConnexPortBase* port)
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

void LOGNode::apply(wxImage* image)
{
	FloatImageData source(image);
	wxImage tmp = image->Copy();
	wxImage tmp2 = image->Copy();
	FloatImageData result1(&tmp), result2(&tmp2);

	vector<double> x1Weights = calcLapMask();
	vector<double> x2Weights = calcGaussianMask();
	vector<double> y1Weights = x2Weights;
	vector<double> y2Weights = x1Weights;

	unsigned int numWeights = x1Weights.size();

	//pass g11
	for (unsigned int i = 0; i < source.height; ++i)
	{
		for (unsigned int j = numWeights / 2; j < source.width - numWeights / 2; ++j)
		{
			FloatPixel average = 0;
			for (unsigned int k = 0; k < numWeights; ++k)
			{
				average += source.at(i, j + k - numWeights / 2) * x1Weights[k];
			}
			result1.setAt(average, i, j);
		}
	}

	//pass g12
	wxImage tmpResult1;
	tmpResult1.Create(image->GetSize());
	FloatImageData tmpResultData1(&tmpResult1);
	for (unsigned int i = numWeights / 2; i < source.height - numWeights / 2; ++i)
	{
		for (unsigned int j = 0; j < source.width; ++j)
		{
			FloatPixel average = 0;
			for (unsigned int k = 0; k < numWeights; ++k)
			{
				average += result1.at(i + k - numWeights / 2, j) * x2Weights[k];
			}
			tmpResultData1.setAt(average, i, j);
		}
	}
	
	//Pass g21
	for (unsigned int i = 0; i < source.height; ++i)
	{
		for (unsigned int j = numWeights / 2; j < source.width - numWeights / 2; ++j)
		{
			FloatPixel average = 0;
			for (unsigned int k = 0; k < numWeights; ++k)
			{
				average += source.at(i, j + k - numWeights / 2) * y1Weights[k];
			}
			result2.setAt(average, i, j);
		}
	}

	//Pass g22
	wxImage tmpResult2;
	tmpResult2.Create(image->GetSize());
	FloatImageData tmpResultData2(&tmpResult2);
	for (unsigned int i = numWeights / 2; i < source.height - numWeights / 2; ++i)
	{
		for (unsigned int j = 0; j < source.width; ++j)
		{
			FloatPixel average = 0;
			for (unsigned int k = 0; k < numWeights; ++k)
			{
				average += result2.at(i + k - numWeights / 2, j) * y2Weights[k];
			}
			tmpResultData2.setAt(average, i, j);
		}
	}

	result.Create(image->GetSize());
	FloatImageData resultData(&result);
	for (unsigned int i = numWeights / 2; i < source.height - numWeights / 2; ++i)
	{
		for (unsigned int j = numWeights / 2; j < source.width - numWeights / 2; ++j)
		{
			resultData.setAt(FloatPixel((tmpResultData1.at(i, j) + tmpResultData2.at(i, j)).asGray()), i, j);
		}
	}
	
	tmp.Create(image->GetSize());
	ImageData zeroData(&tmp);
	
	//Horizontal pass.
	for (unsigned int i = 1; i < zeroData.height - 1; ++i)
	{
		for (unsigned int j = 1; j < zeroData.width - 2; ++j)
		{
			if (abs(resultData.at(i, j).r) < 0.0005)
				continue;
			else if (resultData.at(i, j).r < 0  && resultData.at(i, j + 1).r > 0)
				zeroData.at(i, j) = Pixel(255);
			else if (resultData.at(i, j).r > 0 && resultData.at(i, j + 1).r < 0)
				zeroData.at(i, j) = Pixel(255);
			else if (j < zeroData.width - 2 && resultData.at(i, j).r < 0 &&
				abs(resultData.at(i, j + 1).r) < 0.005 &&
				resultData.at(i, j + 2).r > 0)
			{
				zeroData.at(i, j + 1) = Pixel(255);
				++j;
			}
			else if (j < zeroData.width - 2 && resultData.at(i, j).r > 0 &&
				abs(resultData.at(i, j + 1).r) < 0.005 &&
				resultData.at(i, j + 2).r < 0)
			{
				zeroData.at(i, j + 1) = Pixel(255);
				++j;
			}
		}
	}
		
	//Vetical pass.
	for (unsigned int j = 0; j < zeroData.width; ++j)
	{
		for (unsigned int i = 0; i < zeroData.height - 1; ++i)
		{
			if (abs(resultData.at(i, j).r) < 0.0005)
				continue;
			else if (resultData.at(i, j).r < 0  && resultData.at(i + 1, j).r > 0)
				zeroData.at(i, j) = Pixel(255);
			else if (resultData.at(i, j).r > 0 && resultData.at(i + 1, j).r < 0)
				zeroData.at(i, j) = Pixel(255);
			else if (i < zeroData.height - 2 && resultData.at(i, j).r < 0 &&
				abs(resultData.at(i + 1, j).r) < 0.005 &&
				resultData.at(i + 2, j).r > 0)
			{
				zeroData.at(i + 1, j) = Pixel(255);
				++i;
			}
			else if (i < zeroData.height - 2 && resultData.at(i, j).r > 0 &&
				abs(resultData.at(i + 1, j).r) < 0.005 &&
				resultData.at(i + 2, j).r < 0)
			{
				zeroData.at(i + 1, j) = Pixel(255);
				++i;
			}
		}
	}
	result = tmp.Copy();
}

#define M_E 2.71828182845904523536

vector<double> LOGNode::calcLapMask()
{
	int numWeights = ceil(8.0 * sigma);
	if (numWeights % 2 == 0)
		++numWeights;
	
	vector<double> weights;
	weights.resize(numWeights);

	for (int x = -numWeights / 2, i = 0; x < numWeights / 2, i < numWeights; ++x, ++i)
	{
		weights[i] = (1.0 - pow((double)x, 2.0) / pow((double)sigma, 2.0)) * pow(M_E, -pow((double)x, 2.0) / (2.0 * pow((double)sigma, 2.0))); 
	}
	return weights;
}

vector<double> LOGNode::calcGaussianMask()
{
	int numWeights = ceil(8.0 * sigma);
	if (numWeights % 2 == 0)
		++numWeights;
	
	vector<double> weights;
	weights.resize(numWeights);

	for (int y = -numWeights / 2, i = 0; y < numWeights / 2, i < numWeights; ++y, ++i)
	{
		weights[i] = pow(M_E, -pow((double)y, 2.0) / (2.0 * pow((double)sigma, 2.0))); 
	}
	return weights;
}

////////////////////////////////////////////////////////

//Constructor for the node.
MultiScaleLOGNode::MultiScaleLOGNode(wxConnexSpace *parent, const wxPoint& pos): 
	NodeBase(parent, wxT("Multi-scale LOG"), pos, wxSize(120, 95)), startSigma(5.0), endSigma(1.0), deltaSigma(0.5), toDisplay(0)
{
	//Create the dummy image used when no results are available.
	dummy.Create(0, 0);
	//Create the image panel where the results are displayed.
	image = new ImagePanel(this, wxT("Multi-scale LOG Operator Applied"), &dummy, true);
	image->SetSize(20, 20, 78, 58);

	//Create the input port.
	input = new ImagePort(NULL, ImagePort::ALL);
	input->m_Placement = wxConnexPortBase::PLACE_LEFT;
	input->SetDirection( wxConnexPortBase::DIR_INPUT);
	input->m_Rect.SetPosition( wxPoint( 0, 45 ) );

	//Create the output port which outputs the lowest sigma value.
	output = new ImagePort(&dummy, ImagePort::BINARY);
	output->m_Placement = wxConnexPortBase::PLACE_RIGHT;
	output->SetDirection( wxConnexPortBase::DIR_OUTPUT );
	output->m_Rect.SetPosition( wxPoint( GetSize().x - 10, 45 ) );

	//Add the ports to the node.
	this->AddCXPort(input);
	this->AddCXPort(output);
}

//Add the properties to the UI property grid, called whenever the
//node is selected by the UI.
void MultiScaleLOGNode::addProperties()
{
	//The grid variable is set whenever the node is selected in the UI.
	if (grid)
	{
		//Add the properties.
		grid->Append(new wxFloatProperty(wxT("Start Sigma"), wxT("sSigma"), startSigma));
		grid->Append(new wxFloatProperty(wxT("End Sigma"), wxT("eSigma"), endSigma));
		grid->Append(new wxFloatProperty(wxT("Delta Sigma"), wxT("dSigma"), deltaSigma));
		//Add the results, if available.
		if (results.size() > 0)
		{
			wxPGChoices choices;
			for (unsigned int i = 0; i < results.size(); ++i)
			{
				//Label is the sigma value and the index of the enum value is used as an
				//index into the results vector.
				choices.Add(wxString::Format("%f", results[i].first), i);
			}
			grid->Append(new wxEnumProperty(wxT("Sigma to Display"), wxT("display"), choices));
			grid->GetProperty("display")->SetValueFromInt(toDisplay);
		}
	}
}

//User changed a property value in the UI.
void MultiScaleLOGNode::onPropertyChanged(wxPropertyGridEvent& event)
{
	//Set variable depending on which property is changed.
	if (event.GetPropertyName() == wxT("sSigma"))
		startSigma = event.GetPropertyValue().GetDouble();
	else if (event.GetPropertyName() == wxT("eSigma"))
		endSigma = event.GetPropertyValue().GetDouble();
	else if (event.GetPropertyName() == wxT("dSigma"))
		deltaSigma = event.GetPropertyValue().GetDouble();
	else
	{
		//Handle when the toDisplay value is changed and change the
		//previewed image.
		toDisplay = event.GetPropertyValue().GetInteger();
		image->Refresh();
		image->setImage(&results[toDisplay].second);
	}

	//Reapply the filter when a property is changed.
	if (event.GetPropertyName() != wxT("display"))
		OnPortDataUpdate(input);	
}

//Perform sanity checking on changed property values, reject changes and post a message as to why.
void MultiScaleLOGNode::onPropertyChanging(wxPropertyGridEvent& event)
{
	//Reject the start sigma value if it is less than equal to 0 or the ending value.
	if (event.GetPropertyName() == wxT("sSigma"))
	{
		if (event.GetValue().GetDouble() <= 0 || event.GetValue().GetDouble() < endSigma)
		{
			event.SetValidationFailureBehavior(wxPG_VFB_SHOW_MESSAGE);
			event.SetValidationFailureMessage(wxT("Start Sigma value must be greater than 0 and greater than End Sigma."));
			event.Veto();
		}
	}
	//Reject the end sigma value if it is less than 0 or greater than the starting value.
	else if (event.GetPropertyName() == wxT("eSigma"))
	{
		if (event.GetValue().GetDouble() < 0 || event.GetValue().GetDouble() > startSigma)
		{
			event.SetValidationFailureBehavior(wxPG_VFB_SHOW_MESSAGE);
			event.SetValidationFailureMessage(wxT("Start Sigma value must be greater than or equal to 0 and less than Start Sigma."));
			event.Veto();
		}
	}
	//Reject the delta sigma value if it is less than or equal to 0.
	else if (event.GetPropertyName() == wxT("dSigma"))
	{
		if (event.GetValue().GetDouble() <= 0)
		{
			event.SetValidationFailureBehavior(wxPG_VFB_SHOW_MESSAGE);
			event.SetValidationFailureMessage(wxT("Delta Sigma value must be greater than 0."));
			event.Veto();
		}
	}
}

//Save the results.
void MultiScaleLOGNode::onSave(wxCommandEvent& event)
{
	//Only save if results are available obviouslty.
	if (results.size() > 0)
	{
		//Show the save file dialog.
		wxFileDialog saveFileDialog(this, _("Save Image"), "", "",
							   "PNG files (*.png )|*.png", wxFD_SAVE);

		if (saveFileDialog.ShowModal() == wxID_CANCEL)
			return;
		else
		{
			//Seperate the file name from the full path.
			wxString basePath = saveFileDialog.GetPath().substr(0, saveFileDialog.GetPath().length() - 4);
			for (unsigned int i = 0; i < results.size(); ++i)
			{
				results[i].second.SaveFile(wxString::Format("%s_%f.png", basePath, results[i].first), wxBITMAP_TYPE_PNG);
			}
		}
	}
}

//Called when the node input is updated or a property changes.
void MultiScaleLOGNode::OnPortDataUpdate(wxConnexPortBase* port)
{
	ImagePort* updated = (ImagePort*)port;
	//Update if the input to the node is valid.
	if (updated && updated->getImage() && updated->getImage()->IsOk())
	{
		//Apply the filter on the input image.
		apply(updated->getImage());
		//Update the output port and the preview panel.
		output->setImage(&results.back().second);
		output->RefreshConnections();
		image->setImage(&results.back().second);
		image->Refresh();
		Refresh();
	}
	else
	{
		//No valid input clear the preview panel and erase the results.
		output->setImage(&dummy);
		results.clear();
		output->RefreshConnections();
		image->setImage(&dummy);
		image->Refresh();
		Refresh();
	}
}

//Apply the filter.
void MultiScaleLOGNode::apply(wxImage* image)
{
	//House cleaning, erase old results and set preview image to a safe value.
	this->image->setImage(&dummy);
	results.clear();
	//Declare data.
	FloatImageData source(image);
	wxImage tmp, tmp2, finalResult;
	vector<double> x1Weights;
	vector<double> x2Weights;
	vector<double> y1Weights;
	vector<double> y2Weights;

	//Set of points found during previous iteration.
	//If this has no entries it will execute LOG for the whole image.
	vector<Coord> oldPoints;
	//Stores points found in the current iteration, will be copied to oldPoints at the end of the iteration.
	vector<Coord> newPoints;

	//Results will be saved every other iteration, starting witht he first.
	bool saveResults = true;
	for (float currentSigma = startSigma; currentSigma >= endSigma; currentSigma -= deltaSigma)
	{
		//Create blank images for intermediate results of G12 and G21. 
		tmp.Create(image->GetSize());
		tmp2.Create(image->GetSize());
		//Create image which holds the final added result.
		finalResult.Create(image->GetSize());
		//Create data structs for accessing the individual pixels
		FloatImageData result1(&tmp), result2(&tmp2), final(&finalResult);

		//Calculate weights.
		x1Weights = calcLapMask(currentSigma);
		x2Weights = calcGaussianMask(currentSigma);
		y1Weights = x2Weights;
		y2Weights = x1Weights;

		//Grab number of weights.
		int numWeights = x1Weights.size();	
		
		//Handle case where some edge pixels have already been found.
		if (oldPoints.size() > 0)
		{
			//reset current points.
			newPoints.clear();
			vector<Coord>::iterator iter;

			//pass g11, horizontal laplacian. 
			for (iter = oldPoints.begin(); iter != oldPoints.end(); ++iter)
			{
				Coord current = *iter;
				//Calculate enough data for all the pixels surrounding the 8 neighbors
				for (int i = -numWeights / 2; i < numWeights / 2 && i + current.row < source.height; ++i)
				{
					if (i + current.row < 0)
						i = 0;
					for (int j = -numWeights / 2; j < numWeights / 2 && j + current.column < source.width; ++j)
					{
						if (j + current.column < 0)
							j = 0;
						FloatPixel average = 0;
						//Calculate the weighted average using the weights generated.
						for (unsigned int k = 0; k < numWeights; ++k)
						{
							if (current.column + j + k - numWeights / 2 < 0 ||
								current.column + j + k - numWeights / 2 >= source.width)
								continue;
							average += source.at(current.row + i, current.column + j + k - numWeights / 2) * x1Weights[k];
						}
						result1.setAt(average, current.row + i, current.column + j);
					}
				}
			}

			//pass g12, vertical gaussian.
			wxImage tmpResult1;
			tmpResult1.Create(image->GetSize());
			FloatImageData tmpResultData1(&tmpResult1);
			for (iter = oldPoints.begin(); iter != oldPoints.end(); ++iter)
			{
				Coord current = *iter;
				for (int i = -2; i < 3; ++i)
				{
					for (int j = -2; j < 3; ++j)
					{
						FloatPixel average = 0;
						//Calculate weighted average using the generated weights.
						for (unsigned int k = 0; k < numWeights; ++k)
						{
							average += result1.at(current.row + i + k - numWeights / 2, current.column + j) * 
								x2Weights[k];
						}
						tmpResultData1.setAt(average, current.row + i, current.column + j);
					}
				}
			}
		
			//pass g21, horizontal gaussian. 
			for (iter = oldPoints.begin(); iter != oldPoints.end(); ++iter)
			{
				Coord current = *iter;
				for (int i = -numWeights / 2; i < numWeights / 2 && i + current.row < source.height; ++i)
				{
					if (i + current.row < 0)
						i = 0;
					for (int j = -numWeights / 2; j < numWeights / 2 && j + current.column < source.width; ++j)
					{
						if (j + current.column < 0)
							j = 0;
						FloatPixel average = 0;
						//Calculate weighted average using the generated weights.
						for (unsigned int k = 0; k < numWeights; ++k)
						{
							if (current.column + j + k - numWeights / 2 < 0 ||
								current.column + j + k - numWeights / 2 >= source.width)
								continue;
							average += source.at(current.row + i, current.column + j + k - numWeights / 2) * y1Weights[k];
						}
						result2.setAt(average, current.row + i, current.column + j);
					}
				}
			}

			//pass g22, vertical laplacian.
			wxImage tmpResult2;
			tmpResult2.Create(image->GetSize());
			FloatImageData tmpResultData2(&tmpResult2);
			for (iter = oldPoints.begin(); iter != oldPoints.end(); ++iter)
			{
				Coord current = *iter;
				for (int i = -2; i < 3; ++i)
				{
					for (int j = -2; j < 3; ++j)
					{
						FloatPixel average = 0;
						//Calculate weighted average using the generated weights.
						for (unsigned int k = 0; k < numWeights; ++k)
						{
							average += result2.at(current.row + i + k - numWeights / 2, current.column + j) * 
								y2Weights[k];
						}
						tmpResultData2.setAt(average, current.row + i, current.column + j);
					}
				}
			}

			//Add the two together seperate convolutions together.
			FloatImageData resultData(&finalResult);
			for (iter = oldPoints.begin(); iter != oldPoints.end(); ++iter)
			{
				Coord current = *iter;
				for (int i = -2; i < 3; ++i)
				{
					for (int j = -2; j < 3; ++j)
					{
						resultData.setAt(FloatPixel((tmpResultData1.at(current.row + i, current.column + j) + 
							tmpResultData2.at(current.row + i, current.column + j))), 
							current.row + i, current.column + j);
					}
				}
			}

			//Prepare data for zero cross detection.
			tmp.Create(image->GetSize());
			ImageData zeroData(&tmp);
			//Horizontal pass.
			for (iter = oldPoints.begin(); iter != oldPoints.end(); ++iter)
			{
				Coord current = *iter;
				for (int i = -1; i < 2; ++i)
				{
					for (int j = -1; j < 2; ++j)
					{
						if (abs(resultData.at(current.row + i, current.column + j).r) < 0.0005)
							continue;
						else if (resultData.at(current.row + i, current.column + j).r < 0  && 
							resultData.at(current.row + i, current.column + j + 1).r > 0)
						{
							if (!foundInVector(Coord(current.row + i, current.column + j), newPoints))
								newPoints.push_back(Coord(current.row + i, current.column + j));
							zeroData.at(current.row + i, current.column + j) = Pixel(255);
						}
						else if (resultData.at(current.row + i, current.column + j).r > 0 && 
							resultData.at(current.row + i, current.column + j + 1).r < 0)
						{
							if (!foundInVector(Coord(current.row + i, current.column + j), newPoints))
								newPoints.push_back(Coord(current.row + i, current.column + j));
							zeroData.at(current.row + i, current.column + j) = Pixel(255);
						}
						else if (abs(resultData.at(current.row + i, current.column + j + 1).r) < 0.005 &&
							resultData.at(current.row + i, current.column + j + 2).r > 0)
						{
							if (!foundInVector(Coord(current.row + i, current.column + j + 1), newPoints))
								newPoints.push_back(Coord(current.row + i, current.column + j + 1));
							zeroData.at(current.row + i, current.column + j + 1) = Pixel(255);
							++j;
						}
						else if (abs(resultData.at(current.row + i, current.column + j + 1).r) < 0.005 &&
							resultData.at(current.row + i, current.column + j + 2).r < 0)
						{
							if (!foundInVector(Coord(current.row + i, current.column + j + 1), newPoints))
								newPoints.push_back(Coord(current.row + i, current.column + j + 1));
							zeroData.at(current.row + i, current.column + j + 1) = Pixel(255);
							++j;
						}
					}
				}
			}
			//Vetical pass.
			for (iter = oldPoints.begin(); iter != oldPoints.end(); ++iter)
			{
				Coord current = *iter;
				for (int i = -1; i < 2; ++i)
				{
					for (int j = -1; j < 2; ++j)
					{
						if (abs(resultData.at(current.row + i, current.column + j).r) < 0.0005)
							continue;
						else if (resultData.at(current.row + i, current.column + j).r < 0  && 
							resultData.at(current.row + i + 1, current.column + j).r > 0)
						{
							if (!foundInVector(Coord(current.row + i, current.column + j), newPoints))
								newPoints.push_back(Coord(current.row + i, current.column + j));
							zeroData.at(current.row + i, current.column + j) = Pixel(255);
						}
						else if (resultData.at(current.row + i, current.column + j).r > 0 && 
							resultData.at(current.row + i + 1, current.column + j).r < 0)
						{
							if (!foundInVector(Coord(current.row + i, current.column + j), newPoints))
								newPoints.push_back(Coord(current.row + i, current.column + j));
							zeroData.at(current.row + i, current.column + j) = Pixel(255);
						}
						else if (abs(resultData.at(current.row + i + 1, current.column + j).r) < 0.005 &&
							resultData.at(current.row + i + 2, current.column + j).r > 0)
						{
							if (!foundInVector(Coord(current.row + i + 1, current.column + j), newPoints))
								newPoints.push_back(Coord(current.row + i + 1, current.column + j));
							zeroData.at(current.row + i + 1, current.column + j) = Pixel(255);
							++i;
						}
						else if (abs(resultData.at(current.row + i + 1, current.column + j).r) < 0.005 &&
							resultData.at(current.row + i + 2, current.column + j).r < 0)
						{
							if (!foundInVector(Coord(current.row + i + 1, current.column + j), newPoints))
								newPoints.push_back(Coord(current.row + i + 1, current.column + j));
							zeroData.at(current.row + i + 1, current.column + j) = Pixel(255);
							++i;
						}
					}
				}
			}
			oldPoints = newPoints;
		}
		//Handle initial case.
		else
		{
			//pass g11, horizontal laplacian.
			for (unsigned int i = 0; i < source.height; ++i)
			{
				for (unsigned int j = numWeights / 2; j < source.width - numWeights / 2; ++j)
				{
					FloatPixel average = 0;
					for (unsigned int k = 0; k < numWeights; ++k)
					{
						average += source.at(i, j + k - numWeights / 2) * x1Weights[k];
					}
					result1.setAt(average, i, j);
				}
			}

			//pass g12, vertical gaussian.
			wxImage tmpResult1;
			tmpResult1.Create(image->GetSize());
			FloatImageData tmpResultData1(&tmpResult1);
			for (unsigned int i = numWeights / 2; i < source.height - numWeights / 2; ++i)
			{
				for (unsigned int j = 0; j < source.width; ++j)
				{
					FloatPixel average = 0;
					for (unsigned int k = 0; k < numWeights; ++k)
					{
						average += result1.at(i + k - numWeights / 2, j) * x2Weights[k];
					}
					tmpResultData1.setAt(average, i, j);
				}
			}
			
			//Pass g21, horizontal gausian.
			for (unsigned int i = 0; i < source.height; ++i)
			{
				for (unsigned int j = numWeights / 2; j < source.width - numWeights / 2; ++j)
				{
					FloatPixel average = 0;
					for (unsigned int k = 0; k < numWeights; ++k)
					{
						average += source.at(i, j + k - numWeights / 2) * y1Weights[k];
					}
					result2.setAt(average, i, j);
				}
			}

			//Pass g22, vertical laplacian.
			wxImage tmpResult2;
			tmpResult2.Create(image->GetSize());
			FloatImageData tmpResultData2(&tmpResult2);
			for (unsigned int i = numWeights / 2; i < source.height - numWeights / 2; ++i)
			{
				for (unsigned int j = 0; j < source.width; ++j)
				{
					FloatPixel average = 0;
					for (unsigned int k = 0; k < numWeights; ++k)
					{
						average += result2.at(i + k - numWeights / 2, j) * y2Weights[k];
					}
					tmpResultData2.setAt(average, i, j);
				}
			}

			//Add the two together.
			FloatImageData resultData(&finalResult);
			for (unsigned int i = numWeights / 2; i < source.height - numWeights / 2; ++i)
			{
				for (unsigned int j = numWeights / 2; j < source.width - numWeights / 2; ++j)
				{
					resultData.setAt(FloatPixel((tmpResultData1.at(i, j) + tmpResultData2.at(i, j)).asGray()), i, j);
				}
			}
			
			//Prepare data fro zero cross detection.
			tmp.Create(image->GetSize());
			ImageData zeroData(&tmp);
			//Horizontal pass.
			for (unsigned int i = 1; i < zeroData.height - 1; ++i)
			{
				for (unsigned int j = 1; j < zeroData.width - 2; ++j)
				{
					if (abs(resultData.at(i, j).r) < 0.0005)
						continue;
					else if (resultData.at(i, j).r < 0  && resultData.at(i, j + 1).r > 0)
					{
						oldPoints.push_back(Coord(i, j));
						zeroData.at(i, j) = Pixel(255);
					}
					else if (resultData.at(i, j).r > 0 && resultData.at(i, j + 1).r < 0)
					{
						oldPoints.push_back(Coord(i, j));
						zeroData.at(i, j) = Pixel(255);
					}
					else if (j < zeroData.width - 2 && resultData.at(i, j).r < 0 &&
						abs(resultData.at(i, j + 1).r) < 0.005 &&
						resultData.at(i, j + 2).r > 0)
					{
						oldPoints.push_back(Coord(i, j + 1));
						zeroData.at(i, j + 1) = Pixel(255);
						++j;
					}
					else if (j < zeroData.width - 2 && resultData.at(i, j).r > 0 &&
						abs(resultData.at(i, j + 1).r) < 0.005 &&
						resultData.at(i, j + 2).r < 0)
					{
						oldPoints.push_back(Coord(i, j + 1));
						zeroData.at(i, j + 1) = Pixel(255);
						++j;
					}
				}
			}
			//Vetical pass.
			for (unsigned int j = 0; j < zeroData.width; ++j)
			{
				for (unsigned int i = 0; i < zeroData.height - 1; ++i)
				{
					if (abs(resultData.at(i, j).r) < 0.0005)
						continue;
					else if (resultData.at(i, j).r < 0  && resultData.at(i + 1, j).r > 0)
					{
						oldPoints.push_back(Coord(i, j));
						zeroData.at(i, j) = Pixel(255);
					}
					else if (resultData.at(i, j).r > 0 && resultData.at(i + 1, j).r < 0)
					{
						oldPoints.push_back(Coord(i, j));
						zeroData.at(i, j) = Pixel(255);
					}
					else if (i < zeroData.height - 2 && resultData.at(i, j).r < 0 &&
						abs(resultData.at(i + 1, j).r) < 0.005 &&
						resultData.at(i + 2, j).r > 0)
					{
						oldPoints.push_back(Coord(i + 1, j));
						zeroData.at(i + 1, j) = Pixel(255);
						++i;
					}
					else if (i < zeroData.height - 2 && resultData.at(i, j).r > 0 &&
						abs(resultData.at(i + 1, j).r) < 0.005 &&
						resultData.at(i + 2, j).r < 0)
					{
						oldPoints.push_back(Coord(i + 1, j));
						zeroData.at(i + 1, j) = Pixel(255);
						++i;
					}
				}
			}
		}
		//Save the result.
		results.push_back(pair<float, wxImage>(currentSigma, tmp.Copy()));
	}
	toDisplay = results.size() - 1;
	//Recalculate choices.
	if (grid)
	{
		wxEnumProperty* prop = (wxEnumProperty*)grid->GetProperty("display");
		wxPGChoices choices;
		for (unsigned int i = 0; i < results.size(); ++i)
		{
			choices.Add(wxString::Format("%f", results[i].first), i);
		}
		prop->SetChoices(choices);
		prop->SetValueFromInt(toDisplay);
	}
}

//Generate the Laplacian/Smoothing combination mask (G1) for a passed sigma value.
vector<double> MultiScaleLOGNode::calcLapMask(const float& currentSigma)
{
	//Calculate number of weights.
	int numWeights = ceil(8.0 * currentSigma);
	if (numWeights % 2 == 0)
		++numWeights;
	
	vector<double> weights;
	weights.resize(numWeights);

	//Calculate weights.
	for (int x = -numWeights / 2, i = 0; x < numWeights / 2, i < numWeights; ++x, ++i)
	{
		weights[i] = (1.0 - pow((double)x, 2.0) / pow((double)currentSigma, 2.0)) * pow(M_E, -pow((double)x, 2.0) / (2.0 * pow((double)currentSigma, 2.0))); 
	}
	return weights;
}

//Generate the Gaussian smoothing mask (G2) for a passed sigma value.
vector<double> MultiScaleLOGNode::calcGaussianMask(const float& currentSigma)
{
	//Calculate number of weights.
	int numWeights = ceil(8.0 * currentSigma);
	if (numWeights % 2 == 0)
		++numWeights;
	
	vector<double> weights;
	weights.resize(numWeights);

	//Calculate weights.
	for (int y = -numWeights / 2, i = 0; y < numWeights / 2, i < numWeights; ++y, ++i)
	{
		weights[i] = pow(M_E, -pow((double)y, 2.0) / (2.0 * pow((double)currentSigma, 2.0))); 
	}
	return weights;
}

//Check if a coordinate is already in the passed vector.
bool MultiScaleLOGNode::foundInVector(const Coord& value, vector<Coord>& list)
{
	for (unsigned int i = 0; i < list.size(); ++i)
	{
		if (value == list[i])
			return true;
	}
	return false;
}