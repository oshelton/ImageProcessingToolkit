/* 
* Owen Shelton
* CSCI 8810 Spring 2011
* Project 2
*/

#include <bitset>
#include <fstream>
#include "nodes/Stego.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

void EmbedStegoNode::onSave(wxCommandEvent &event)
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

void EmbedStegoNode::OnPortDataUpdate(wxConnexPortBase* port)
{
	ImagePort* updated = (ImagePort*)port;
	if (updated && updated->getImage() && updated->getImage() != NULL && updated->getImage()->IsOk())
	{
		apply(updated->getImage());
		output->RefreshConnections();
		imageOutput->RefreshConnections();
		image->Refresh();
		Refresh();
	}
	else
	{
		result.Create(0, 0);
		output->RefreshConnections();
		imageOutput->RefreshConnections();
		image->Refresh();
		Refresh();
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////

EmbedTextNode::EmbedTextNode(wxConnexSpace *parent, const wxPoint& pos): 
	EmbedStegoNode(parent, wxT("Embed Text"), pos, wxSize(100, 95)), text("Hi!")
{
	result.Create(0, 0);
	image = new ImagePanel(this, wxT("Text Embedded Into Image"), &result, true);
	image->SetSize(10,20, 78, 58);

	input = new ImagePort(NULL, ImagePort::ALL);
	input->m_Placement = wxConnexPortBase::PLACE_LEFT;
	input->SetDirection( wxConnexPortBase::DIR_INPUT);
	input->m_Rect.SetPosition( wxPoint( 0, 45 ) );

	output = new StegoPort(&result, &bitsUsed, &dataLength);
	output->m_Placement = wxConnexPortBase::PLACE_RIGHT;
	output->SetDirection( wxConnexPortBase::DIR_OUTPUT );
	output->m_Rect.SetPosition( wxPoint( GetSize().x - 10, 45 ) );

	imageOutput = new ImagePort(&result, ImagePort::COLOR);
	imageOutput->m_Placement = wxConnexPortBase::PLACE_RIGHT;
	imageOutput->SetDirection( wxConnexPortBase::DIR_OUTPUT);
	imageOutput->m_Rect.SetPosition( wxPoint( GetSize().x - 10, 65 ) );

	this->AddCXPort(input);
	this->AddCXPort(output);
	this->AddCXPort(imageOutput);
}

void EmbedTextNode::addProperties()
{
	if (grid)
	{
		grid->Append(new wxLongStringProperty("Text", "Text", text));
		grid->Append(new wxUIntProperty(wxT("Number Bits"), wxT("Num"), bitsUsed));
	}
}

void EmbedTextNode::onPropertyChanged(wxPropertyGridEvent& event)
{
	if (event.GetPropertyName()== "Text")
		text = event.GetPropertyValue().GetString();
	else
		bitsUsed = (unsigned int)event.GetPropertyValue().GetInteger();
	OnPortDataUpdate(input);	
}

void EmbedTextNode::onPropertyChanging(wxPropertyGridEvent& event)
{
	if (event.GetPropertyName() == "Num")
	{
		unsigned int tmp = (unsigned int)event.GetValue().GetInteger();
		if (tmp > 8 || tmp < 1)
		{
			event.SetValidationFailureBehavior(wxPG_VFB_SHOW_MESSAGE);
			event.SetValidationFailureMessage(wxT("Number Bits value must be between 1 and 8 inclusive."));
			event.Veto();
		}
	}
	else if (event.GetPropertyName() == "Text")
	{
		wxString tmp = event.GetValue().GetString();
		if (tmp.length() < 1)
		{
			event.SetValidationFailureBehavior(wxPG_VFB_SHOW_MESSAGE);
			event.SetValidationFailureMessage(wxT("Embedded text must be at least one character long."));
			event.Veto();
		}
	}
}

void EmbedTextNode::apply(wxImage* image)
{
	result = image->Copy();
	dataLength = text.Length();
	unsigned char* dest = result.GetData();
	unsigned char* data = image->GetData();

	unsigned long bitsWritten = 0;
	unsigned long currentChar = 0;
	unsigned long totalBits = dataLength * 8;
	bool moreData = true;

	for (unsigned int i = 0; i < image->GetHeight() * image->GetWidth() * 3 && moreData; ++i)
	{
		bitset<8> sourceChar(text.at(currentChar));
		bitset<8> currentValue(data[i]);
		for (unsigned int j = 0; j < bitsUsed; ++j)
		{
			if (currentValue[j] != sourceChar[bitsWritten])
				currentValue[j].flip();
			++bitsWritten;
			if (bitsWritten >= 8)
			{
				bitsWritten = 0;
				++currentChar;
				if (currentChar < text.Length())
					sourceChar = bitset<8>(text.at(currentChar));
			}

			if (currentChar * 8 + bitsWritten >= totalBits)
			{
				moreData = false;
				break;
			}
		}
		dest[i] = (unsigned char)currentValue.to_ulong();
	}
}

////////////////////////////////////////////////////////

EmbedImageNode::EmbedImageNode(wxConnexSpace *parent, const wxPoint& pos): 
	EmbedStegoNode(parent, wxT("Embed Image"), pos, wxSize(100, 165)), imageDim(0, 0) 
{
	result.Create(0, 0);
	image = new ImagePanel(this, wxT("Image Embedded Into another Image"), &result, true);
	image->SetSize(10,20, 78, 58);

	loaded.Create(0, 0);
	toEmbed = new ImagePanel(this, wxT("Image to be Embedded."), &loaded, true);
	toEmbed->SetSize(10,90, 78, 58);

	input = new ImagePort(NULL, ImagePort::ALL);
	input->m_Placement = wxConnexPortBase::PLACE_LEFT;
	input->SetDirection( wxConnexPortBase::DIR_INPUT);
	input->m_Rect.SetPosition( wxPoint( 0, 45 ) );

	output = new StegoPort(&result, &bitsUsed, &imageDim);
	output->m_Placement = wxConnexPortBase::PLACE_RIGHT;
	output->SetDirection( wxConnexPortBase::DIR_OUTPUT );
	output->m_Rect.SetPosition( wxPoint( GetSize().x - 10, 45 ) );

	imageOutput = new ImagePort(&loaded, ImagePort::COLOR);
	imageOutput->m_Placement = wxConnexPortBase::PLACE_RIGHT;
	imageOutput->SetDirection( wxConnexPortBase::DIR_OUTPUT);
	imageOutput->m_Rect.SetPosition( wxPoint( GetSize().x - 10, 65 ) );

	this->AddCXPort(input);
	this->AddCXPort(output);
	this->AddCXPort(imageOutput);
}

void EmbedImageNode::addProperties()
{
	if (grid)
	{
		grid->Append(new wxImageFileProperty("Image", "Image", fileName));
		grid->Append(new wxUIntProperty(wxT("Number Bits"), wxT("Num"), bitsUsed));
	}
}

void EmbedImageNode::onPropertyChanged(wxPropertyGridEvent& event)
{
	if (event.GetPropertyName()== "Image")
	{
		fileName = event.GetPropertyValue().GetString();
		loaded.LoadFile(fileName);
		imageDim = toEmbed->getImage()->GetSize();
	}
	else
		bitsUsed = (unsigned int)event.GetPropertyValue().GetInteger();
	OnPortDataUpdate(input);	
}

void EmbedImageNode::onPropertyChanging(wxPropertyGridEvent& event)
{
	if (event.GetPropertyName() == "Num")
	{
		unsigned int tmp = (unsigned int)event.GetValue().GetInteger();
		if (tmp > 8 || tmp < 1)
		{
			event.SetValidationFailureBehavior(wxPG_VFB_SHOW_MESSAGE);
			event.SetValidationFailureMessage(wxT("Number Bits value must be between 1 and 8 inclusive."));
			event.Veto();
		}
	}
}

void EmbedImageNode::OnPortDataUpdate(wxConnexPortBase* port)
{
	ImagePort* updated = (ImagePort*)port;
	if (updated && updated->getImage() && updated->getImage()->IsOk())
	{
		if (loaded.IsOk())
		{
			apply(updated->getImage());
			output->RefreshConnections();
			imageOutput->RefreshConnections();
		}
		else
			result = updated->getImage()->Copy();
	    
		image->Refresh();
		toEmbed->Refresh();
		Refresh();
	}
	else
	{
		result.Create(0, 0);
		output->RefreshConnections();
		imageOutput->RefreshConnections();
		image->Refresh();
		toEmbed->Refresh();
		Refresh();
	}
}

void EmbedImageNode::apply(wxImage* image)
{
	imageDim = loaded.GetSize();
	result = image->Copy();
	unsigned char* dest = result.GetData();

	unsigned char* toHide = toEmbed->getImage()->GetData();
	unsigned char* data = image->GetData();

	unsigned long bitsWritten = 0;
	unsigned long currentByte = 0;
	unsigned long totalBits = imageDim.GetWidth() * imageDim.GetHeight() * 3 * 8;
	bool moreData = true;

	for (unsigned int i = 0; i < image->GetHeight() * image->GetWidth() * 3 && moreData; ++i)
	{
		bitset<8> sourceByte(toHide[currentByte]);
		bitset<8> currentValue(data[i]);
		for (unsigned int j = 0; j < bitsUsed; ++j)
		{
			if (currentValue[j] != sourceByte[bitsWritten])
				currentValue[j].flip();
			++bitsWritten;
			if (bitsWritten >= 8)
			{
				bitsWritten = 0;
				++currentByte;
				if (currentByte < imageDim.GetWidth() * imageDim.GetHeight() * 3)
					sourceByte = bitset<8>(toHide[currentByte]);
			}

			if (currentByte * 8 + bitsWritten >= totalBits)
			{
				moreData = false;
				break;
			}
		}
		dest[i] = (unsigned char)currentValue.to_ulong();
	}
}

////////////////////////////////////////////////////////

ExtractTextNode::ExtractTextNode(wxConnexSpace *parent, const wxPoint& pos): 
	ExtractStegoNode(parent, wxT("Extract Text"), pos, wxSize(100, 50)), extracted("")
{
	input = new StegoPort(NULL, NULL, (unsigned int *)NULL);
	input->m_Placement = wxConnexPortBase::PLACE_LEFT;
	input->SetDirection( wxConnexPortBase::DIR_INPUT);
	input->m_Rect.SetPosition( wxPoint( 0, 20 ) );

	this->AddCXPort(input);
}

void ExtractTextNode::addProperties()
{
	if (grid)
		grid->Append(new wxLongStringProperty("Extracted", "Extracted", extracted));
}

void ExtractTextNode::onSave(wxCommandEvent &event)
{
	if (extracted.Length() > 0)
	{
		wxFileDialog saveFileDialog(this, _("Save Image"), "", "",
							   "TXT files (*.txt )|*.txt", wxFD_SAVE);

		if (saveFileDialog.ShowModal() == wxID_CANCEL)
			return;
		else
		{
			fstream file(((string)saveFileDialog.GetPath()).c_str(), fstream::out);
			file << extracted;
		}
	}
}

void ExtractTextNode::OnPortDataUpdate(wxConnexPortBase* port)
{
	StegoPort* updated = (StegoPort*)port;
	if (updated && updated->getImage() && updated->getImage() != NULL && updated->getImage()->IsOk())
	{
		wxImage tmp = updated->getImage()->Copy();
		extracted = "";
		apply(&tmp);
		if (grid)
			grid->GetProperty("Extracted")->SetValue(extracted);
		Refresh();
	}
	else
	{
		extracted = "";
		if (grid)
			grid->GetProperty("Extracted")->SetValue(extracted);
		Refresh();
	}
}

void ExtractTextNode::apply(wxImage* image)
{
	unsigned int dataLength = input->getDataLength();
	unsigned int bitsUsed = input->getBitsUsed();

	unsigned char* data = image->GetData();

	unsigned long bitsRead = 0;
	unsigned long currentChar = 0;
	unsigned long totalBits = dataLength * 8;

	bool moreData = true;

	bitset<8> readChar(0);

	for (unsigned int i = 0; i < image->GetHeight() * image->GetWidth() * 3 && moreData; ++i)
	{
		bitset<8> currentValue(data[i]);
		for (unsigned int j = 0; j < bitsUsed; ++j)
		{
			readChar.set(bitsRead, currentValue[j]);
			++bitsRead;
			if (bitsRead >= 8)
			{
				bitsRead = 0;
				++currentChar;
				extracted += (char)readChar.to_ulong();
				readChar = bitset<8>(0);
			}

			if (currentChar * 8 + bitsRead >= totalBits)
			{
				moreData = false;
				break;
			}
		}
	}
}

//////////////////////////////////////////////

ExtractImageNode::ExtractImageNode(wxConnexSpace *parent, const wxPoint& pos): 
	ExtractStegoNode(parent, wxT("Extract Image"), pos, wxSize(100, 95))
{
	result.Create(wxSize(0, 0));
	extracted = new ImagePanel(this, wxT("Extracted Image"), &result, true);
	extracted->SetSize(10,20, 78, 58);

	input = new StegoPort(NULL, NULL, (wxSize *)NULL);
	input->m_Placement = wxConnexPortBase::PLACE_LEFT;
	input->SetDirection( wxConnexPortBase::DIR_INPUT);
	input->m_Rect.SetPosition( wxPoint( 0, 45 ) );

	imageOutput = new ImagePort(&result, ImagePort::COLOR);
	imageOutput->m_Placement = wxConnexPortBase::PLACE_RIGHT;
	imageOutput->SetDirection( wxConnexPortBase::DIR_OUTPUT);
	imageOutput->m_Rect.SetPosition( wxPoint( GetSize().x - 10, 45 ) );

	this->AddCXPort(input);
	this->AddCXPort(imageOutput);
}

void ExtractImageNode::onSave(wxCommandEvent &event)
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

void ExtractImageNode::OnPortDataUpdate(wxConnexPortBase* port)
{
	StegoPort* updated = (StegoPort*)port;
	if (updated && updated->getImage() && updated->getImage()->IsOk())
	{
		apply(updated->getImage());
		imageOutput->RefreshConnections();
		extracted->Refresh();
		Refresh();
	}
	else
	{
		result.Create(0, 0);
		imageOutput->RefreshConnections();
		extracted->Refresh();
		Refresh();
	}
}

void ExtractImageNode::apply(wxImage* image)
{
	result.Create(input->getDimensions());
	result.SetRGB(wxRect(input->getDimensions()), 0, 0, 0);
	unsigned char* extractedData = result.GetData();

	unsigned int bitsUsed = input->getBitsUsed();
	wxSize imageDim = input->getDimensions();

	unsigned char* data = image->GetData();

	unsigned long bitsRead = 0;
	unsigned long currentByte = 0;
	unsigned long totalBits = imageDim.GetWidth() * imageDim.GetHeight() * 3 * 8;

	bool moreData = true;

	bitset<8> readByte(0);

	for (unsigned int i = 0; i < image->GetHeight() * image->GetWidth() * 3 && moreData; ++i)
	{
		bitset<8> currentValue(data[i]);
		for (unsigned int j = 0; j < bitsUsed; ++j)
		{
			readByte.set(bitsRead, currentValue[j]);
			++bitsRead;
			if (bitsRead >= 8)
			{
				bitsRead = 0;
				extractedData[currentByte] = (char)readByte.to_ulong();
				++currentByte;
				readByte = bitset<8>(0);
			}

			if (currentByte * 8 + bitsRead >= totalBits)
			{
				moreData = false;
				break;
			}
		}
	}
}