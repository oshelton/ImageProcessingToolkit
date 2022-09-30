/* 
* Owen Shelton
* CSCI 8810 Spring 2011
* Project 2
*/

#include "nodes/ImageNode.h"
#include "nodes/Color.h"
#include "nodes/EdgeDetection.h"
#include "nodes/Thresholding.h"
#include "nodes/NoiseReduction.h"
#include "nodes/Noise.h"
#include "nodes/Statistics.h"
#include "nodes/Pyramid.h"
#include "nodes/Stego.h"
#include "App.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

BEGIN_EVENT_TABLE(App, wxApp)
	EVT_COMMAND(ID_EDITOR, wxEVT_NODE_CHANGED, App::onNodeChanged)
	EVT_PG_CHANGED(-1, App::propertyChanged)
    EVT_PG_CHANGING(-1, App::propertyChanging)
	EVT_BUTTON(ID_SAVE, App::onSave)
	EVT_MENU(ID_RESET, App::onReset)
	EVT_MENU(ID_QUIT, App::onQuit)
	EVT_MENU(ID_AVERAGE, App::onAverage)
	EVT_MENU(ID_MEDIAN, App::onMedian)
	EVT_MENU(ID_ROBERTS, App::onRoberts)
	EVT_MENU(ID_LAPLACIAN, App::onLaplacian)
	EVT_MENU(ID_SOBEL, App::onSobel)
	EVT_MENU(ID_PREWITT, App::onPrewitt)
	EVT_MENU(ID_KERSH, App::onKersh)
	EVT_MENU(ID_LOG, App::onLOG)
	EVT_MENU(ID_MULTI_LOG, App::onMultiLog)
	EVT_MENU(ID_CONVERT_GREY, App::onConvertToGreyscale)
	EVT_MENU(ID_SIMPLE, App::onSimpleThreshold)
	EVT_MENU(ID_ITERATIVE, App::onIterativeThreshold)
	EVT_MENU(ID_ADAPTIVE, App::onAdaptiveThreshold)
	EVT_MENU(ID_CCL, App::onCCL)
	EVT_MENU(ID_PYRAMID, App::onPyramid)
	EVT_MENU(ID_FIRST_ORDER, App::onFirstOrder)
	EVT_MENU(ID_ZERO_ORDER, App::onZeroOrder)
	EVT_MENU(ID_EMBED_TEXT, App::onEmbedText)
	EVT_MENU(ID_EMBED_IMAGE, App::onEmbedImage)
	EVT_MENU(ID_EXTRACT_TEXT, App::onExtractText)
	EVT_MENU(ID_EXTRACT_IMAGE, App::onExtractImage)
	EVT_MENU(ID_PIXEL_NOISE, App::onPixelNoise)
	EVT_MENU(ID_CLUSTER_NOISE, App::onClusterNoise)
	EVT_MENU(ID_ADDITIVE_NOISE, App::onAdditiveNoise)
END_EVENT_TABLE()


bool App::OnInit()
{
	wxInitAllImageHandlers();
	frame = new wxFrame(NULL, wxID_ANY, "Image Editor", wxPoint(50,50), wxSize(800,600));
	frame->Show();

	menu = new wxMenuBar;
	menu->SetId(ID_MENU);

	wxMenu* system = new wxMenu();
	system->Append(ID_RESET, wxT("R&eset Editor"), wxT("Remove all the nodes from the editor and start from scratch."));
	system->AppendSeparator();
	system->Append(ID_QUIT, wxT("E&xit"), wxT("Exit the program."));

	wxMenu* masks = new wxMenu();

	wxMenu* reduction = new wxMenu();
	reduction->Append(ID_AVERAGE, wxT("&Average"), wxT("Create a new Average Filter Operator."));
	reduction->Append(ID_MEDIAN, wxT("&Median"), wxT("Create a new Median Filter Operator."));
	masks->AppendSubMenu(reduction, wxT("&Noise Reduction"));

	wxMenu* edgeDetection = new wxMenu;
	edgeDetection->Append(ID_ROBERTS, wxT("&Roberts"), wxT("Create a new Roberts Operator."));
	edgeDetection->Append(ID_LAPLACIAN, wxT("&Laplacian"), wxT("Create a new Laplacian Operator."));
	edgeDetection->Append(ID_SOBEL, wxT("&Sobel"), wxT("Create a new Sobel Operator."));
	edgeDetection->Append(ID_PREWITT, wxT("&Prewitt"), wxT("Create a new Prewitt Operator."));
	edgeDetection->Append(ID_KERSH, wxT("&Kersh"), wxT("Create a new Kersh Operator."));
	edgeDetection->Append(ID_LOG, wxT("&LOG"), wxT("Create a new Laplacian of Gaussians Operator."));
	edgeDetection->Append(ID_MULTI_LOG, wxT("&Multi-Scale LOG"), wxT("Create a new Multi-scale LOG Operator."));
	masks->AppendSubMenu(edgeDetection, wxT("&Edge Detection"));

	wxMenu* color = new wxMenu();
	//color->Append(ID_CONTRAST, wxT("&Linear Contrast Enhancement"), wxT("Create a new Linear Contrast Enhancement Operator."));
	color->Append(ID_CONVERT_GREY, wxT("&Convert To Grescale"), wxT("Create a new RGB to Greyscale Operator."));

	wxMenu* thresholding = new wxMenu();
	thresholding->Append(ID_SIMPLE, wxT("&Simple"), wxT("Create a new Simple Thresholding Operator."));
	thresholding->Append(ID_ITERATIVE, wxT("&Iterative"), wxT("Create a new Iterative Thresholding Operator."));
	thresholding->Append(ID_ADAPTIVE, wxT("&Adaptive"), wxT("Create a new Adaptive Thresholding Operator."));
	//thresholding->Append(ID_PTILE, wxT("&P Tile"), wxT("Create a new P Tile Thresholding Operator."));

	wxMenu* statistics = new wxMenu();
	//statistics->Append(ID_HISTOGRAM, wxT("&Histogram"), wxT("Create a new Histogram Operator."));
	statistics->Append(ID_CCL, wxT("&Connected Component Labeling"), wxT("Create a new CCL Operator."));

	wxMenu* misc = new wxMenu();

	wxMenu* pyramid = new wxMenu();
	pyramid->Append(ID_PYRAMID, wxT("&Pyramid Generator"), wxT("Create a new 5-level Pyramid Operator."));
	pyramid->Append(ID_FIRST_ORDER, wxT("&First Order Hold Scheme"), wxT("Create a new First Order Hold Scheme Pyramid Operator."));
	pyramid->Append(ID_ZERO_ORDER, wxT("&Zero Order Hold Scheme"), wxT("Create a new Zero Order Hold Scheme Pyramid Operator."));
	misc->AppendSubMenu(pyramid, "&Pyramid");

	wxMenu* stego = new wxMenu();
	stego->Append(ID_EMBED_TEXT, wxT("&Embed Text"), wxT("Create a new text embedding Stego Operator."));
	stego->Append(ID_EMBED_IMAGE, wxT("E&mbed Image"), wxT("Create a new image embedding Stego Operator."));
	stego->Append(ID_EXTRACT_TEXT, wxT("E&xtract Text"), wxT("Create a new text extracting Stego Operator."));
	stego->Append(ID_EXTRACT_IMAGE, wxT("Ex&tract Image"), wxT("Create a new image extracting Stego Operator."));
	misc->AppendSubMenu(stego, "&Steganography");
	
	wxMenu* noise = new wxMenu();
	noise->Append(ID_PIXEL_NOISE, wxT("&Pixel Noise"), wxT("Create a new Noise Operator that adds single pixels."));
	noise->Append(ID_CLUSTER_NOISE, wxT("&Cluster Noise"), wxT("Create a new Noise Operator that adds NS5 noise."));
	noise->Append(ID_ADDITIVE_NOISE, wxT("&Additive Noise"), wxT("Create a new Additive Gaussian Noise Operator that adds NS5 noise."));

	menu->Append(system, wxT("&System"));
	menu->Append(masks, wxT("&Masks"));
	menu->Append(color, "&Color");
	menu->Append(thresholding, wxT("&Thresholding"));
	menu->Append(statistics, wxT("&Statistics"));
	menu->Append(misc, wxT("Misc"));
	menu->Append(noise, wxT("N&oise"));

	frame->SetMenuBar(menu);

	editorSpace = new wxConnexSpace(frame, ID_EDITOR);
	editorSpace->AddNode(new ImageNode(editorSpace, wxPoint(10, 250)));
	editorSpace->SetBackgroundColour(wxColour(100, 100, 100));

	wxBoxSizer* splitter = new wxBoxSizer(wxHORIZONTAL);
	splitter->Add(editorSpace, 1, wxEXPAND);

	wxPanel* infoWindow = new wxPanel(frame);
	infoWindow->SetMinSize(wxSize(230, 480));
	wxBoxSizer* infoSizer = new wxBoxSizer(wxVERTICAL);
	infoWindow->SetSizer(infoSizer);
	saveButton = new wxButton(infoWindow, ID_SAVE, "Save Result", wxDefaultPosition, wxSize(96, 25));
	infoSizer->Add(saveButton, 0, wxALL | wxALIGN_CENTRE | wxTOP, 2);
	properties = new wxPropertyGrid(infoWindow);
	properties->SetMinSize(wxSize(230, 450));
	infoSizer->Add(properties, 0, wxGROW | wxALIGN_BOTTOM | wxALL, 2);
	
	saveButton->Disable();

	splitter->Add(infoWindow, 0, wxEXPAND);

	frame->SetSizer(splitter);

	frame->CreateStatusBar(1);

	return true;
}

void App::onNodeChanged(wxCommandEvent& event)
{
	wxConnexSpace* space = static_cast<wxConnexSpace*>(event.GetEventObject());
	if (space->GetFocusNode() != NULL)
	{
		space->GetFocusNode()->connect(saveButton, properties);
		saveButton->Enable();
		properties->Refresh();
	}
	else
	{
		saveButton->Disable();
	}
}

void App::onSave(wxCommandEvent& event)
{
	if (editorSpace->GetFocusNode() != NULL)
		editorSpace->GetFocusNode()->onSave(event);
}

void App::propertyChanged(wxPropertyGridEvent& event)
{
	if (editorSpace->GetFocusNode() != NULL)
		editorSpace->GetFocusNode()->onPropertyChanged(event);
}

void App::propertyChanging(wxPropertyGridEvent& event)
{
	if (editorSpace->GetFocusNode() != NULL)
		editorSpace->GetFocusNode()->onPropertyChanging(event);
}

void App::onReset(wxCommandEvent& event)
{
	wxConnexSpace::NodeList::iterator iter;
	for (iter = editorSpace->m_NodeList.begin(); iter != editorSpace->m_NodeList.end(); ++iter)
	{
		delete *iter;
	}
	saveButton->Disable();
	properties->Clear();
	editorSpace->m_NodeList.clear();
	editorSpace->Refresh();
	editorSpace->AddNode(new ImageNode(editorSpace,wxPoint(10, 250)));
}

void App::onQuit(wxCommandEvent& event)
{
	frame->Close(true);
}

void App::onAverage(wxCommandEvent& event)
{
	editorSpace->AddNode(new AveragingNode(editorSpace));
}

void App::onMedian(wxCommandEvent& event)
{
	editorSpace->AddNode(new MedianNode(editorSpace));
}

void App::onRoberts(wxCommandEvent& event)
{
	editorSpace->AddNode(new RobertsNode(editorSpace));
}

void App::onLaplacian(wxCommandEvent& event)
{
	editorSpace->AddNode(new LaplacianNode(editorSpace));
}

void App::onSobel(wxCommandEvent& event)
{
	editorSpace->AddNode(new SobelNode(editorSpace));
}

void App::onPrewitt(wxCommandEvent& event)
{
	editorSpace->AddNode(new PrewittNode(editorSpace));
}

void App::onKersh(wxCommandEvent& event)
{
	editorSpace->AddNode(new KershNode(editorSpace));
}

void App::onLOG(wxCommandEvent& event)
{
	editorSpace->AddNode(new LOGNode(editorSpace));
}

void App::onMultiLog(wxCommandEvent& event)
{
	editorSpace->AddNode(new MultiScaleLOGNode(editorSpace));
}

void App::onConvertToGreyscale(wxCommandEvent& event)
{
	editorSpace->AddNode(new ToGreyscaleNode(editorSpace));
}

void App::onSimpleThreshold(wxCommandEvent& event)
{
	editorSpace->AddNode(new SimpleThresholdingNode(editorSpace));
}

void App::onIterativeThreshold(wxCommandEvent& event)
{
	editorSpace->AddNode(new IterativeThresholdingNode(editorSpace));
}

void App::onAdaptiveThreshold(wxCommandEvent& event)
{
	editorSpace->AddNode(new AdaptiveThresholdingNode(editorSpace));
}

void App::onCCL(wxCommandEvent& event)
{
	editorSpace->AddNode(new CCLNode(editorSpace));
}

void App::onPyramid(wxCommandEvent& event)
{
	editorSpace->AddNode(new PyramidNode(editorSpace));
}

void App::onFirstOrder(wxCommandEvent& event)
{
	editorSpace->AddNode(new FirstOrderNode(editorSpace));
}

void App::onZeroOrder(wxCommandEvent& event)
{
	editorSpace->AddNode(new ZeroOrderNode(editorSpace));
}

void App::onEmbedText(wxCommandEvent &event)
{
	editorSpace->AddNode(new EmbedTextNode(editorSpace));
}

void App::onEmbedImage(wxCommandEvent &event)
{
	editorSpace->AddNode(new EmbedImageNode(editorSpace));
}

void App::onExtractText(wxCommandEvent& event)
{
	editorSpace->AddNode(new ExtractTextNode(editorSpace));
}

void App::onExtractImage(wxCommandEvent& event)
{
	editorSpace->AddNode(new ExtractImageNode(editorSpace));
}

void App::onPixelNoise(wxCommandEvent& event)
{
	editorSpace->AddNode(new PixelNoiseNode(editorSpace));
}

void App::onClusterNoise(wxCommandEvent& event)
{
	editorSpace->AddNode(new ClusterNoiseNode(editorSpace));
}

void App::onAdditiveNoise(wxCommandEvent& event)
{
	editorSpace->AddNode(new AdditiveNoiseNode(editorSpace));
}