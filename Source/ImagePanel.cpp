/* 
* Owen Shelton
* CSCI 8810 Spring 2011
* Project 2
*/

#include <wx/file.h>
#include "ImagePanel.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

ImagePanel::ImagePanel(wxWindow* parent, const wxString& name, wxImage* pointer, const bool& previewEnabled) :
wxPanel(parent)
{
	image = pointer;
    w = -1;
	h = -1;
	this->previewEnabled = previewEnabled;

	if (previewEnabled)
	{
		if (image && image->IsOk())
			largePreview = new wxFrame(NULL, wxID_ANY, name, wxDefaultPosition, 
				wxSize(image->GetWidth(), image->GetHeight()), wxCAPTION | wxRESIZE_BORDER);
		else
			largePreview = new wxFrame(NULL, wxID_ANY, name, wxDefaultPosition, 
				wxSize(0, 0), wxCAPTION | wxRESIZE_BORDER);
		wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
		largePreview->SetSizer(sizer);
		largeImage = new ImagePanel(largePreview, name, pointer, false);
		sizer->Add(largeImage, 1, wxEXPAND);
	}
	else
	{
		largePreview = NULL;
		largeImage = NULL;
	}
}

void ImagePanel::setImage(wxImage* image)
{
	this->image = image;

	w = -1;
	h = -1;
	
	if (largePreview && largeImage)
	{
		if (image && image->IsOk())
			largePreview->SetSize(0, 0, image->GetWidth(), image->GetHeight());
		largeImage->setImage(this->image);
	}
}

void ImagePanel::calcSize()
{
	w = -1;
	h = -1;

	if (largePreview && largeImage && image && image->IsOk())
		largePreview->SetSize(0, 0, image->GetWidth(), image->GetHeight());
}

void ImagePanel::clearImage()
{
	if (image)
		image = NULL;

	if (largePreview && largeImage)
		largeImage->clearImage();
}

ImagePanel::~ImagePanel()
{
	if (largePreview && largeImage)
	{
		largePreview->Close(true);
	}
}
 
/*
 * Called by the system of by wxWidgets when the panel needs
 * to be redrawn. You can also trigger this call by
 * calling Refresh()/Update().
 */
void ImagePanel::paintEvent(wxPaintEvent & evt)
{
    // depending on your system you may need to look at double-buffered dcs
    wxPaintDC dc(this);
    render(dc);
}
 
/*
 * Alternatively, you can use a clientDC to paint on the panel
 * at any time. Using this generally does not free you from
 * catching paint events, since it is possible that e.g. the window
 * manager throws away your drawing when the window comes to the
 * background, and expects you will redraw it when the window comes
 * back (by sending a paint event).
 */
void ImagePanel::paintNow()
{
    // depending on your system you may need to look at double-buffered dcs
    wxClientDC dc(this);
    render(dc);
}
 
/*
 * Here we do the actual rendering. I put it in a separate
 * method so that it can work no matter what type of DC
 * (e.g. wxPaintDC or wxClientDC) is used.
 */
void ImagePanel::render(wxDC&  dc)
{
	if (image && image->IsOk())
	{
		int neww, newh;
		dc.GetSize( &neww, &newh );
	    
		if( neww != w || newh != h)
		{
			resized = wxBitmap( image->Scale( neww, newh) );
			w = neww;
			h = newh;
			dc.DrawBitmap( resized, 0, 0, false );
		}else{
			dc.DrawBitmap( resized, 0, 0, false );
		}
	}
}
 
/*
 * Here we call refresh to tell the panel to draw itself again.
 * So when the user resizes the image panel the image should be resized too.
 */
void ImagePanel::OnSize(wxSizeEvent& event){
    Refresh();
    //skip the event.
    event.Skip();
}

void ImagePanel::mouseDown(wxMouseEvent& event)
{
	if (previewEnabled && image && image->IsOk())
	{
		if (largePreview->IsShown())
			largePreview->Hide();
		else
			largePreview->Show();
	}
}

void ImagePanel::rightDown(wxMouseEvent& event)
{
	if (previewEnabled && image && image->IsOk())
		largePreview->SetSize(image->GetWidth(), image->GetHeight());
}

void ImagePanel::Refresh(bool eraseBackground, const wxRect* rect)
{
	calcSize();
	if (largePreview && (!image || !image->IsOk()))
		largePreview->Hide();
	if (largeImage)
		largeImage->Refresh();
	wxPanel::Refresh(eraseBackground, rect);
    paintEvent(wxPaintEvent());
}

BEGIN_EVENT_TABLE(ImagePanel, wxPanel) 
// catch paint events
EVT_PAINT(ImagePanel::paintEvent)
//Size event
EVT_SIZE(ImagePanel::OnSize)
EVT_LEFT_DOWN(ImagePanel::mouseDown)
EVT_RIGHT_DOWN(ImagePanel::rightDown)
END_EVENT_TABLE()