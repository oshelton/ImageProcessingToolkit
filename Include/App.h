/* 
* Owen Shelton
* CSCI 8810 Spring 2011
* Project 2
*/

#ifndef APP_H
#define APP_H

#include <wx/wx.h>
#include <wx/propgrid/propgrid.h>
#include <wx/propgrid/advprops.h>
#include "connexspace.h"
#include "ImagePanel.h"

enum ID
{
	ID_EDITOR,

	ID_SAVE,

	ID_MENU,

	ID_RESET,
	ID_QUIT,

	ID_AVERAGE,
	ID_MEDIAN,

	ID_ROBERTS,
	ID_LAPLACIAN,
	ID_SOBEL,
	ID_PREWITT,
	ID_KERSH,
	ID_LOG,
	ID_MULTI_LOG,
    
	ID_CONVERT_GREY,
	ID_CONTRAST,

	ID_SIMPLE,
	ID_ITERATIVE,
	ID_ADAPTIVE,
	ID_PTILE,

	ID_HISTOGRAM,
	ID_CCL,

	ID_PYRAMID,
	ID_FIRST_ORDER,
	ID_ZERO_ORDER,

	ID_EMBED_TEXT,
	ID_EMBED_IMAGE,
	ID_EXTRACT_TEXT,
	ID_EXTRACT_IMAGE,

	ID_PIXEL_NOISE,
	ID_CLUSTER_NOISE,
	ID_ADDITIVE_NOISE
};

class App: public wxApp
{
private:
	wxFrame* frame;
	wxMenuBar* menu;

	wxConnexSpace* editorSpace;

	wxButton* saveButton;

	wxPropertyGrid* properties;

public:
	 bool OnInit();

	 void onNodeChanged(wxCommandEvent& event);
	 void onSave(wxCommandEvent& event);

	 void propertyChanged(wxPropertyGridEvent& event);
	 void propertyChanging(wxPropertyGridEvent& event);

	 void onReset(wxCommandEvent& event);
	 void onQuit(wxCommandEvent& event);

	 void onAverage(wxCommandEvent& event);
	 void onMedian(wxCommandEvent& event);

	 void onRoberts(wxCommandEvent& event);
	 void onLaplacian(wxCommandEvent& event);
	 void onSobel(wxCommandEvent& event);
	 void onPrewitt(wxCommandEvent& event);
	 void onKersh(wxCommandEvent& event);
	 void onLOG(wxCommandEvent& event);
	 void onMultiLog(wxCommandEvent& event);

	 void onConvertToGreyscale(wxCommandEvent& event);

	 void onSimpleThreshold(wxCommandEvent& event);
	 void onIterativeThreshold(wxCommandEvent& event);
	 void onAdaptiveThreshold(wxCommandEvent& event);

	 void onCCL(wxCommandEvent& event);

	 void onPyramid(wxCommandEvent& event);
	 void onFirstOrder(wxCommandEvent& event);
	 void onZeroOrder(wxCommandEvent& event);

	 void onEmbedText(wxCommandEvent& event);
	 void onEmbedImage(wxCommandEvent& event);
	 void onExtractText(wxCommandEvent& event);
	 void onExtractImage(wxCommandEvent& event);

	 void onPixelNoise(wxCommandEvent& event);
	 void onClusterNoise(wxCommandEvent& event);
	 void onAdditiveNoise(wxCommandEvent& event);

	 DECLARE_EVENT_TABLE()
};

#endif