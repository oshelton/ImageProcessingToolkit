/* 
* Owen Shelton
* CSCI 8810 Spring 2011
* Project 2
*/

#include "nodes/Pyramid.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

PyramidNode::PyramidNode(wxConnexSpace *parent, const wxPoint& pos): 
	NodeBase(parent, wxT("Pyramid"), pos, wxSize(130, 215))
{
	resized.Create(0, 0);

	image = new ImagePanel(this, wxT("Input Image Transformed to 512x512."), &resized, true);
	image->SetMinSize(wxSize(30, 30));

	for (unsigned int i = 0; i < 5; ++i)
	{
		levelArray[i].Create(0, 0);
	}
	levels[0] = new ImagePanel(this, wxT("Level 1, 256x256."), &levelArray[0], true);
	levels[0]->SetMinSize(wxSize(30, 30));
	levels[1] = new ImagePanel(this, wxT("Level 2, 128x128."), &levelArray[1], true);
	levels[1]->SetMinSize(wxSize(30, 30));
	levels[2] = new ImagePanel(this, wxT("Level 3, 64x64."), &levelArray[2], true);
	levels[2]->SetMinSize(wxSize(30, 30));
	levels[3] = new ImagePanel(this, wxT("Level 4, 32x32."), &levelArray[3], true);
	levels[3]->SetMinSize(wxSize(30, 30));
	levels[4] = new ImagePanel(this, wxT("Level 5, 16x16."), &levelArray[4], true);
	levels[4]->SetMinSize(wxSize(30, 30));

	wxBoxSizer* main = new wxBoxSizer(wxVERTICAL);
	wxBoxSizer* horizontal1 = new wxBoxSizer(wxHORIZONTAL);
	wxBoxSizer* vertical11 = new wxBoxSizer(wxVERTICAL);
	wxBoxSizer* vertical12 = new wxBoxSizer(wxVERTICAL);
	wxBoxSizer* horizontal2 = new wxBoxSizer(wxHORIZONTAL);
	wxBoxSizer* vertical21 = new wxBoxSizer(wxVERTICAL);
	wxBoxSizer* vertical22 = new wxBoxSizer(wxVERTICAL);
	wxBoxSizer* verticalBottom = new wxBoxSizer(wxVERTICAL);

	wxStaticText* label;

	main->AddSpacer(18);

	main->Add(image, 0, wxALIGN_CENTER);
	
	label = new wxStaticText(this, wxID_ANY, "256x256");
	label->SetBackgroundColour(wxColour(255, 255, 255));
	vertical11->Add(label, 0, wxALIGN_CENTER, 3);
	vertical11->Add(levels[0], 0, wxALIGN_CENTER);
	horizontal1->Add(vertical11, 1, wxLEFT, 4);

	label = new wxStaticText(this, wxID_ANY, "128x128");
	label->SetBackgroundColour(wxColour(255, 255, 255));
	vertical12->Add(label, 0, wxALIGN_CENTER, 3);
	vertical12->Add(levels[1], 0, wxALIGN_CENTER);
	horizontal1->Add(vertical12, 1, wxRIGHT, 4);

	main->Add(horizontal1, 1, wxEXPAND, 3);

	label = new wxStaticText(this, wxID_ANY, "64x64");
	label->SetBackgroundColour(wxColour(255, 255, 255));
	vertical21->Add(label, 0, wxALIGN_CENTER, 3);
	vertical21->Add(levels[2], 0, wxALIGN_CENTER);
	horizontal2->Add(vertical21, 1, wxLEFT, 4);

	label = new wxStaticText(this, wxID_ANY, "32x32");
	label->SetBackgroundColour(wxColour(255, 255, 255));
	vertical22->Add(label, 0, wxALIGN_CENTER, 3);
	vertical22->Add(levels[3], 0, wxALIGN_CENTER);
	horizontal2->Add(vertical22, 1, wxRIGHT, 4);

	main->Add(horizontal2, 1, wxEXPAND, 3);

	label = new wxStaticText(this, wxID_ANY, "16x16");
	label->SetBackgroundColour(wxColour(255, 255, 255));
	verticalBottom->Add(label, 0, wxALIGN_CENTER, 3);
	verticalBottom->Add(levels[4], 0, wxALIGN_CENTER);

	main->Add(verticalBottom, 1, wxALIGN_CENTER);
	this->SetSizer(main);
	this->Layout();

	input = new ImagePort(NULL, ImagePort::ALL);
	input->m_Placement = wxConnexPortBase::PLACE_LEFT;
	input->SetDirection( wxConnexPortBase::DIR_INPUT);
	input->m_Rect.SetPosition( wxPoint( 0, 25 ) );

	output = new PyramidPort(levelArray);
	output->m_Placement = wxConnexPortBase::PLACE_RIGHT;
	output->SetDirection( wxConnexPortBase::DIR_OUTPUT );
	output->m_Rect.SetPosition( wxPoint( GetSize().x - 10, 25 ) );

	for (unsigned int i = 0; i < 5; ++i)
	{
		levelOutputs[i] = new ImagePort(&levelArray[i], ImagePort::COLOR_GRAY);
		levelOutputs[i]->m_Placement = wxConnexPortBase::PLACE_RIGHT;
		levelOutputs[i]->SetDirection( wxConnexPortBase::DIR_OUTPUT );
		levelOutputs[i]->m_Rect.SetPosition( wxPoint( GetSize().x - 10, 60 + i * 20 ) );
		this->AddCXPort(levelOutputs[i]);
	}

	this->AddCXPort(input);
	this->AddCXPort(output);
}

void PyramidNode::onSave(wxCommandEvent& event)
{
	if (resized.IsOk())
	{
		wxFileDialog saveFileDialog(this, _("Save Image"), "", "",
							   "PNG files (*.png )|*.png", wxFD_SAVE);

		if (saveFileDialog.ShowModal() == wxID_CANCEL)
			return;
		else
		{
			wxString basePath = saveFileDialog.GetPath().substr(0, saveFileDialog.GetPath().length() - 4);
			resized.SaveFile(saveFileDialog.GetPath());
			levelArray[0].SaveFile(basePath + "_256x256.png", wxBITMAP_TYPE_PNG);
			levelArray[1].SaveFile(basePath + "_128x128.png", wxBITMAP_TYPE_PNG);
			levelArray[2].SaveFile(basePath + "_64x64.png", wxBITMAP_TYPE_PNG);
			levelArray[3].SaveFile(basePath + "_32x32.png", wxBITMAP_TYPE_PNG);
			levelArray[4].SaveFile(basePath + "_16x16.png", wxBITMAP_TYPE_PNG);
		}
	}
}

void PyramidNode::OnPortDataUpdate(wxConnexPortBase* port)
{
	ImagePort* updated = (ImagePort*)port;
	if (updated && updated->getImage()->IsOk())
	{
		apply(updated->getImage());
		image->Refresh();
		Refresh();
		output->RefreshConnections();

		for (unsigned int i = 0; i < 5; ++i)
		{
			levels[i]->Refresh();
			levelOutputs[i]->RefreshConnections();
		}
	}
	else
	{
		resized.Create(0, 0);
		image->Refresh();

		for (unsigned int i = 0; i < 5; ++i)
		{
			levelArray[i].Create(0, 0);
			levels[i]->Refresh();
			levelOutputs[i]->RefreshConnections();
		}

		output->RefreshConnections();
		Refresh();
	}
}

void PyramidNode::apply(wxImage* image)
{
	transformInput(image);
	rebuildLevels();
}

void PyramidNode::transformInput(wxImage* image)
{
	resized.Create(512, 512);
	ImageData source(image), dest(&resized);

	for (unsigned int i = 0; i < 512; ++i)
	{
		for (unsigned int j = 0; j < 512; ++j)
		{
			if (i < source.height && j < source.width)
				dest.at(i, j) = source.at(i, j);
			else
				dest.at(i, j) = Pixel();
		}
	}
}

void PyramidNode::rebuildLevels()
{
	ImageData base(&resized), level(&resized);

	for (unsigned int m = 0, n = 2; n <= 32; ++m, n *= 2)
	{
		levelArray[m].Create(wxSize(512 / n, 512 / n));
		level = &levelArray[m];
		for (unsigned int i = 0; i < 512; i += n)
		{
			for (unsigned int j = 0; j < 512; j += n)
			{
				FloatPixel areaAverage;
				for (unsigned int k = 0; k < n; ++k)
				{
					for (unsigned int l = 0; l < n; ++l)
					{
						areaAverage += base.at(i + k, j + l);
					}
				}
				areaAverage /= (n * n);
				level.setFloatAt(areaAverage, i / n, j / n);
			}
		}
	}
}

/////////////////////////////////////////////////////////////////////

FirstOrderNode::FirstOrderNode(wxConnexSpace *parent, const wxPoint& pos): 
	NodeBase(parent, wxT("First Order"), pos, wxSize(130, 195))
{
	for (unsigned int i = 0; i < 5; ++i)
	{
		levelArray[i].Create(0, 0);
	}

	levels[0] = new ImagePanel(this, wxT("Level 1, 256x256."), &levelArray[0], true);
	levels[0]->SetMinSize(wxSize(30, 30));
	levels[1] = new ImagePanel(this, wxT("Level 2, 128x128."), &levelArray[1], true);
	levels[1]->SetMinSize(wxSize(30, 30));
	levels[2] = new ImagePanel(this, wxT("Level 3, 64x64."), &levelArray[2], true);
	levels[2]->SetMinSize(wxSize(30, 30));
	levels[3] = new ImagePanel(this, wxT("Level 4, 32x32."), &levelArray[3], true);
	levels[3]->SetMinSize(wxSize(30, 30));
	levels[4] = new ImagePanel(this, wxT("Level 5, 16x16."), &levelArray[4], true);
	levels[4]->SetMinSize(wxSize(30, 30));

	wxBoxSizer* main = new wxBoxSizer(wxVERTICAL);
	wxBoxSizer* horizontal1 = new wxBoxSizer(wxHORIZONTAL);
	wxBoxSizer* vertical11 = new wxBoxSizer(wxVERTICAL);
	wxBoxSizer* vertical12 = new wxBoxSizer(wxVERTICAL);
	wxBoxSizer* horizontal2 = new wxBoxSizer(wxHORIZONTAL);
	wxBoxSizer* vertical21 = new wxBoxSizer(wxVERTICAL);
	wxBoxSizer* vertical22 = new wxBoxSizer(wxVERTICAL);
	wxBoxSizer* verticalBottom = new wxBoxSizer(wxVERTICAL);

	wxStaticText* label;

	main->AddSpacer(18);
	
	label = new wxStaticText(this, wxID_ANY, "256x256");
	label->SetBackgroundColour(wxColour(255, 255, 255));
	vertical11->Add(label, 0, wxALIGN_CENTER, 3);
	vertical11->Add(levels[0], 0, wxALIGN_CENTER);
	horizontal1->Add(vertical11, 1, wxLEFT, 4);

	label = new wxStaticText(this, wxID_ANY, "128x128");
	label->SetBackgroundColour(wxColour(255, 255, 255));
	vertical12->Add(label, 0, wxALIGN_CENTER, 3);
	vertical12->Add(levels[1], 0, wxALIGN_CENTER);
	horizontal1->Add(vertical12, 1, wxRIGHT, 4);

	main->Add(horizontal1, 1, wxEXPAND, 3);

	label = new wxStaticText(this, wxID_ANY, "64x64");
	label->SetBackgroundColour(wxColour(255, 255, 255));
	vertical21->Add(label, 0, wxALIGN_CENTER, 3);
	vertical21->Add(levels[2], 0, wxALIGN_CENTER);
	horizontal2->Add(vertical21, 1, wxLEFT, 4);

	label = new wxStaticText(this, wxID_ANY, "32x32");
	label->SetBackgroundColour(wxColour(255, 255, 255));
	vertical22->Add(label, 0, wxALIGN_CENTER, 3);
	vertical22->Add(levels[3], 0, wxALIGN_CENTER);
	horizontal2->Add(vertical22, 1, wxRIGHT, 4);

	main->Add(horizontal2, 1, wxEXPAND, 3);

	label = new wxStaticText(this, wxID_ANY, "16x16");
	label->SetBackgroundColour(wxColour(255, 255, 255));
	verticalBottom->Add(label, 0, wxALIGN_CENTER, 3);
	verticalBottom->Add(levels[4], 0, wxALIGN_CENTER);

	main->Add(verticalBottom, 1, wxALIGN_CENTER);
	this->SetSizer(main);
	this->Layout();

	input = new PyramidPort(NULL);
	input->m_Placement = wxConnexPortBase::PLACE_LEFT;
	input->SetDirection( wxConnexPortBase::DIR_INPUT);
	input->m_Rect.SetPosition( wxPoint( 0, 35 ) );

	this->AddCXPort(input);

	for (unsigned int i = 0; i < 5; ++i)
	{
		levelOutputs[i] = new ImagePort(&levelArray[i], ImagePort::COLOR_GRAY);
		levelOutputs[i]->m_Placement = wxConnexPortBase::PLACE_RIGHT;
		levelOutputs[i]->SetDirection( wxConnexPortBase::DIR_OUTPUT );
		levelOutputs[i]->m_Rect.SetPosition( wxPoint( GetSize().x - 10, 50 + i * 20 ) );
		levelOutputs[i]->setImage(&levelArray[i]);
		this->AddCXPort(levelOutputs[i]);
	}
}

void FirstOrderNode::onSave(wxCommandEvent& event)
{
	if (levelArray[0].IsOk())
	{
		wxFileDialog saveFileDialog(this, _("Save Image"), "", "",
							   "PNG files (*.png )|*.png", wxFD_SAVE);

		if (saveFileDialog.ShowModal() == wxID_CANCEL)
			return;
		else
		{
			wxString basePath = saveFileDialog.GetPath().substr(0, saveFileDialog.GetPath().length() - 4);
			levelArray[0].SaveFile(basePath + "_256x256.png", wxBITMAP_TYPE_PNG);
			levelArray[1].SaveFile(basePath + "_128x128.png", wxBITMAP_TYPE_PNG);
			levelArray[2].SaveFile(basePath + "_64x64.png", wxBITMAP_TYPE_PNG);
			levelArray[3].SaveFile(basePath + "_32x32.png", wxBITMAP_TYPE_PNG);
			levelArray[4].SaveFile(basePath + "_16x16.png", wxBITMAP_TYPE_PNG);
		}
	}
}

void FirstOrderNode::OnPortDataUpdate(wxConnexPortBase* port)
{
	PyramidPort* updated = (PyramidPort*)port;
	if (updated && updated->getLevels() && updated->getLevels()[0].IsOk())
	{
		apply(updated->getLevels());

		for (unsigned int i = 0; i < 5; ++i)
		{
			levels[i]->Refresh();
			levelOutputs[i]->RefreshConnections();
		}

		Refresh();
	}
	else
	{
		for (unsigned int i = 0; i < 5; ++i)
		{
			levelArray[i].Create(0, 0);
			levels[i]->Refresh();
			levelOutputs[i]->RefreshConnections();
		}

		Refresh();
	}
}

void FirstOrderNode::apply(wxImage* inputLevels)
{
	wxImage tmp;
	tmp.Create(wxSize(0, 0));
	ImageData source(&tmp), dest(&tmp);

	//Initial pass, copy stretched data.
	for (unsigned int m = 0, n = 2; n <= 32; ++m, n *= 2)
	{
		source = &inputLevels[m];
		levelArray[m].Create(512, 512);
		dest = &levelArray[m];
		for (unsigned int i = 0; i < 512 / n; ++i)
		{
			for (unsigned int j = 0; j < 512 / n; ++j)
			{
				dest.at(i * n, j * n) = source.at(i, j);
			}
		}
	}

	//Horizontal pass.
	for (unsigned int m = 0, n = 2; n <= 32; ++m, n *= 2)
	{
		source = &inputLevels[m];
		dest = &levelArray[m];
		for (unsigned int i = 0; i < 512 / n; ++i)
		{ 
			for (unsigned int j = 0; j < 512 / n - 1; ++j)
			{
				FloatPixel value;
				for (unsigned int k = 1; k < n; ++k)
				{
					value = (source.floatAt(i, j) * (((j * n + n) - (j * n + k)) / ((1.0f * j * n + n) - (j * n))))  +
						(source.floatAt(i, j + 1) * (((j * n + k) - (j * n)) / ((1.0f * j * n + n) - (j * n))));
					dest.setFloatAt(value, i * n, j * n  + k);
				}
			}
		}
	}

	//Vertical pass.
	for (unsigned int m = 0, n = 2; n <= 32; ++m, n *= 2)
	{
		source = &inputLevels[m];
		dest = &levelArray[m];
		for (unsigned int i = 0; i < 512 / n - 1; ++i)
		{ 
			for (unsigned int j = 0; j < 512 / n; ++j)
			{
				FloatPixel value;
				for (unsigned int k = 1; k < n; ++k)
				{
					value = (source.floatAt(i, j) * (((i * n + n) - (i * n + k)) / ((1.0f * i * n + n) - (i * n)))) +
						(source.floatAt(i + 1, j) * (((i * n + k) - (i * n)) / ((1.0f * i * n + n) - (i * n))));
					dest.setFloatAt(value, i * n + k, j * n);
				}
			}
		}
	}

	//Second vertical pass.
	for (unsigned int m = 0, n = 2; n <= 32; ++m, n *= 2)
	{
		source = &inputLevels[m];
		dest = &levelArray[m];
		for (unsigned int i = 0; i < 512 / n - 1; ++i)
		{ 
			for (unsigned int j = 0; j < 512 / n; ++j)
			{
				for (unsigned int k = 1; k < n; ++k)
				{
					for (unsigned int l = 1; l < n; ++l)
					{
						FloatPixel top = dest.floatAt(i * n, j * n);
						FloatPixel bottom = dest.floatAt(i * n + n, j * n);
						float firstFactor = (((1.0f * i * n) - (i * n + k)) / ((1.0f * i * n) - (i * n + n)));
						float secondFactor = (((1.0f * i * n + k) - (i * n + n)) / ((1.0f * i * n) - (i * n + n)));
						FloatPixel value = (bottom * firstFactor) +
							(top * secondFactor);

						dest.setFloatAt(value, i * n + k, j * n + l);
					}
				}
			}
		}
	}

	//Fill in junk space to right by stretching already existing data.
	for (unsigned int m = 0, n = 2; n <= 32; ++m, n *= 2)
	{
		source = &inputLevels[m];
		dest = &levelArray[m];

		for (unsigned int i = 0; i < 512; ++i)
		{
			for (unsigned int j = (512 / n - 1) * n + 1; j < 512; ++j)
			{
				dest.at(i, j) = dest.at(i, (512 / n - 1) * n); 
			}
		}
		levels[m]->Refresh();
	}
}

///////////////////////////////////////////////////////////////////////////////////

ZeroOrderNode::ZeroOrderNode(wxConnexSpace *parent, const wxPoint& pos): 
	NodeBase(parent, wxT("Zero Order"), pos, wxSize(130, 195))
{
	for (unsigned int i = 0; i < 5; ++i)
	{
		levelArray[i].Create(0, 0);
	}

	levels[0] = new ImagePanel(this, wxT("Level 1, 256x256."), &levelArray[0], true);
	levels[0]->SetMinSize(wxSize(30, 30));
	levels[1] = new ImagePanel(this, wxT("Level 2, 128x128."), &levelArray[1], true);
	levels[1]->SetMinSize(wxSize(30, 30));
	levels[2] = new ImagePanel(this, wxT("Level 3, 64x64."), &levelArray[2], true);
	levels[2]->SetMinSize(wxSize(30, 30));
	levels[3] = new ImagePanel(this, wxT("Level 4, 32x32."), &levelArray[3], true);
	levels[3]->SetMinSize(wxSize(30, 30));
	levels[4] = new ImagePanel(this, wxT("Level 5, 16x16."), &levelArray[4], true);
	levels[4]->SetMinSize(wxSize(30, 30));

	wxBoxSizer* main = new wxBoxSizer(wxVERTICAL);
	wxBoxSizer* horizontal1 = new wxBoxSizer(wxHORIZONTAL);
	wxBoxSizer* vertical11 = new wxBoxSizer(wxVERTICAL);
	wxBoxSizer* vertical12 = new wxBoxSizer(wxVERTICAL);
	wxBoxSizer* horizontal2 = new wxBoxSizer(wxHORIZONTAL);
	wxBoxSizer* vertical21 = new wxBoxSizer(wxVERTICAL);
	wxBoxSizer* vertical22 = new wxBoxSizer(wxVERTICAL);
	wxBoxSizer* verticalBottom = new wxBoxSizer(wxVERTICAL);

	wxStaticText* label;

	main->AddSpacer(18);
	
	label = new wxStaticText(this, wxID_ANY, "256x256");
	label->SetBackgroundColour(wxColour(255, 255, 255));
	vertical11->Add(label, 0, wxALIGN_CENTER, 3);
	vertical11->Add(levels[0], 0, wxALIGN_CENTER);
	horizontal1->Add(vertical11, 1, wxLEFT, 4);

	label = new wxStaticText(this, wxID_ANY, "128x128");
	label->SetBackgroundColour(wxColour(255, 255, 255));
	vertical12->Add(label, 0, wxALIGN_CENTER, 3);
	vertical12->Add(levels[1], 0, wxALIGN_CENTER);
	horizontal1->Add(vertical12, 1, wxRIGHT, 4);

	main->Add(horizontal1, 1, wxEXPAND, 3);

	label = new wxStaticText(this, wxID_ANY, "64x64");
	label->SetBackgroundColour(wxColour(255, 255, 255));
	vertical21->Add(label, 0, wxALIGN_CENTER, 3);
	vertical21->Add(levels[2], 0, wxALIGN_CENTER);
	horizontal2->Add(vertical21, 1, wxLEFT, 4);

	label = new wxStaticText(this, wxID_ANY, "32x32");
	label->SetBackgroundColour(wxColour(255, 255, 255));
	vertical22->Add(label, 0, wxALIGN_CENTER, 3);
	vertical22->Add(levels[3], 0, wxALIGN_CENTER);
	horizontal2->Add(vertical22, 1, wxRIGHT, 4);

	main->Add(horizontal2, 1, wxEXPAND, 3);

	label = new wxStaticText(this, wxID_ANY, "16x16");
	label->SetBackgroundColour(wxColour(255, 255, 255));
	verticalBottom->Add(label, 0, wxALIGN_CENTER, 3);
	verticalBottom->Add(levels[4], 0, wxALIGN_CENTER);

	main->Add(verticalBottom, 1, wxALIGN_CENTER);
	this->SetSizer(main);
	this->Layout();

	input = new PyramidPort(NULL);
	input->m_Placement = wxConnexPortBase::PLACE_LEFT;
	input->SetDirection( wxConnexPortBase::DIR_INPUT);
	input->m_Rect.SetPosition( wxPoint( 0, 35 ) );

	this->AddCXPort(input);

	for (unsigned int i = 0; i < 5; ++i)
	{
		levelOutputs[i] = new ImagePort(&levelArray[i], ImagePort::COLOR_GRAY);
		levelOutputs[i]->m_Placement = wxConnexPortBase::PLACE_RIGHT;
		levelOutputs[i]->SetDirection( wxConnexPortBase::DIR_OUTPUT );
		levelOutputs[i]->m_Rect.SetPosition( wxPoint( GetSize().x - 10, 50 + i * 20 ) );
		this->AddCXPort(levelOutputs[i]);
	}
}

void ZeroOrderNode::onSave(wxCommandEvent& event)
{
	if (levelArray[0].IsOk())
	{
		wxFileDialog saveFileDialog(this, _("Save Image"), "", "",
							   "PNG files (*.png )|*.png", wxFD_SAVE);

		if (saveFileDialog.ShowModal() == wxID_CANCEL)
			return;
		else
		{
			wxString basePath = saveFileDialog.GetPath().substr(0, saveFileDialog.GetPath().length() - 4);
			levelArray[0].SaveFile(basePath + "_256x256.png", wxBITMAP_TYPE_PNG);
			levelArray[1].SaveFile(basePath + "_128x128.png", wxBITMAP_TYPE_PNG);
			levelArray[2].SaveFile(basePath + "_64x64.png", wxBITMAP_TYPE_PNG);
			levelArray[3].SaveFile(basePath + "_32x32.png", wxBITMAP_TYPE_PNG);
			levelArray[4].SaveFile(basePath + "_16x16.png", wxBITMAP_TYPE_PNG);
		}
	}
}

void ZeroOrderNode::OnPortDataUpdate(wxConnexPortBase* port)
{
	PyramidPort* updated = (PyramidPort*)port;
	if (updated && updated->getLevels() && updated->getLevels()[0].IsOk())
	{
		apply(updated->getLevels());

		for (unsigned int i = 0; i < 5; ++i)
		{
			levels[i]->Refresh();
			levelOutputs[i]->RefreshConnections();
		}

		Refresh();
	}
	else
	{
		for (unsigned int i = 0; i < 5; ++i)
		{
			levelArray[i].Create(0, 0);
			levels[i]->Refresh();
			levelOutputs[i]->RefreshConnections();
		}

		Refresh();
	}
}

void ZeroOrderNode::apply(wxImage* inputLevels)
{
	wxImage tmp;
	tmp.Create(wxSize(0, 0));
	ImageData source(&tmp), dest(&tmp);

	//Initial pass, copy stretched data.
	for (unsigned int m = 0, n = 2; n <= 32; ++m, n *= 2)
	{
		source = &inputLevels[m];
		dest = &levelArray[m];
		for (unsigned int i = 0; i < 512 / n; ++i)
		{
			for (unsigned int j = 0; j < 512 / n; ++j)
			{
				for (unsigned int k = 0; k < n; ++k)
				{
					for (unsigned int l = 0; l < n; ++l)
					{
						dest.at(i * n + k, j * n + l) = source.at(i, j);
					}
				}
			}
		}
	}
}