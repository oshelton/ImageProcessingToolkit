/* 
* Owen Shelton
* CSCI 8810 Spring 2011
* Project 2
*/

#ifndef IMAGE_PANEL_H
#define IMAGE_PANEL_H

#include <wx/wx.h>
#include "connex.h"

//Custom widget class for displaying an Image.
//Comes from the wxWiki.
//http://wiki.wxwidgets.org/An_image_panel
class ImagePanel : public wxPanel
{
protected:
    wxImage* image;
    wxBitmap resized;
    int w, h;
	
	wxFrame* largePreview;
	ImagePanel* largeImage;

	bool previewEnabled;
    
public:
	//Bitmap type is wxBITMAP_TYPE_ANY.
	ImagePanel(wxWindow* parent, const wxString& name, wxImage* pointer, const bool& previewEnabled);

	~ImagePanel();

	void setImage(wxImage* image);
	void calcSize();

	void clearImage();

	inline wxImage* getImage(){ return image; }
    
    void paintEvent(wxPaintEvent & evt);
    void paintNow();
    void OnSize(wxSizeEvent& event);
    void render(wxDC& dc);
	void mouseDown(wxMouseEvent& event);
	void rightDown(wxMouseEvent& event);

	void Refresh(bool eraseBackground = true, const wxRect* rect = (const wxRect*) 0);
    
    DECLARE_EVENT_TABLE()
};

#endif