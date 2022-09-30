/* 
* Owen Shelton
* CSCI 8810 Spring 2011
* Project 2
*/

#ifndef EDGEDETECTION_H
#define EDGEDETECTION_H

#include "NodeBase.h"

//Node for applying the Roberts operator to an image.
class RobertsNode : public NodeBase
{
protected:
	//No properties.
	void addProperties(){}

	//Image to store the result.
	wxImage result;

	//Panel on which to display the image.
	ImagePanel* image;

	//Greyscale image input.
	ImagePort* input;
	//Greyscale output.
	ImagePort* output;

public:
	RobertsNode(wxConnexSpace *parent, const wxPoint& pos = wxDefaultPosition);

	void onSave(wxCommandEvent& event);
	void OnPortDataUpdate(wxConnexPortBase* port);

	void onPropertyChanged(wxPropertyGridEvent& event){}
	void onPropertyChanging(wxPropertyGridEvent& event){}
	
	inline wxString getName(){ return wxT("Roberts"); }

protected:
	void apply(wxImage* source);
};

//Node for applying the Laplacian operator to an image.
class LaplacianNode : public NodeBase
{
protected:
	bool zeroCross;

	//No properties.
	void addProperties();

	//Image to store the result.
	wxImage result;

	//Panel on which to display the image.
	ImagePanel* image;

	//Greyscale image input.
	ImagePort* input;
	//Greyscale output.
	ImagePort* output;

public:
	LaplacianNode(wxConnexSpace *parent, const wxPoint& pos = wxDefaultPosition);

	void onSave(wxCommandEvent& event);
	void OnPortDataUpdate(wxConnexPortBase* port);

	void onPropertyChanged(wxPropertyGridEvent& event);
	void onPropertyChanging(wxPropertyGridEvent& event){}
	
	inline wxString getName(){ return wxT("Laplacian"); }

protected:
	void apply(wxImage* source);
};

//Node for applying the Sobel operator to an image.
class SobelNode : public NodeBase
{
protected:
	//No properties.
	void addProperties(){}

	//Image to store the result.
	wxImage result;

	//Panel on which to display the image.
	ImagePanel* image;

	//Greyscale image input.
	ImagePort* input;
	//Greyscale output.
	ImagePort* output;

public:
	SobelNode(wxConnexSpace *parent, const wxPoint& pos = wxDefaultPosition);

	void onSave(wxCommandEvent& event);
	void OnPortDataUpdate(wxConnexPortBase* port);

	void onPropertyChanged(wxPropertyGridEvent& event){}
	void onPropertyChanging(wxPropertyGridEvent& event){}
	
	inline wxString getName(){ return wxT("Sobel"); }

protected:
	void apply(wxImage* source);
};

//Node for applying the Prewitt operator to an image.
class PrewittNode : public NodeBase
{
protected:
	//No properties.
	void addProperties(){}

	//Image to store the result.
	wxImage result;

	//Panel on which to display the image.
	ImagePanel* image;

	//Greyscale image input.
	ImagePort* input;
	//Greyscale output.
	ImagePort* output;

public:
	PrewittNode(wxConnexSpace *parent, const wxPoint& pos = wxDefaultPosition);

	void onSave(wxCommandEvent& event);
	void OnPortDataUpdate(wxConnexPortBase* port);

	void onPropertyChanged(wxPropertyGridEvent& event){}
	void onPropertyChanging(wxPropertyGridEvent& event){}
	
	inline wxString getName(){ return wxT("Prewitt"); }

protected:
	void apply(wxImage* source);
};

//Node for applying the Kersh operator to an image.
class KershNode : public NodeBase
{
protected:
	//No properties.
	void addProperties(){}

	//Image to store the result.
	wxImage result;

	//Panel on which to display the image.
	ImagePanel* image;

	//Greyscale image input.
	ImagePort* input;
	//Greyscale output.
	ImagePort* output;

public:
	KershNode(wxConnexSpace *parent, const wxPoint& pos = wxDefaultPosition);

	void onSave(wxCommandEvent& event);
	void OnPortDataUpdate(wxConnexPortBase* port);

	void onPropertyChanged(wxPropertyGridEvent& event){}
	void onPropertyChanging(wxPropertyGridEvent& event){}
	
	inline wxString getName(){ return wxT("Kersh"); }

protected:
	void apply(wxImage* source);
};

//Node for applying LOG (Laplacian of Gaussian) to an image.
class LOGNode : public NodeBase
{
protected:
	float sigma;

	//Add the threshold value property to the grid.
	void addProperties();

	//Image to store the result.
	wxImage result;

	//Panel on which to display the image.
	ImagePanel* image;

	//Greyscale image input.
	ImagePort* input;
	//Greyscale output.
	ImagePort* output;

public:
	LOGNode(wxConnexSpace *parent, const wxPoint& pos = wxDefaultPosition);

	void onSave(wxCommandEvent& event);
	void OnPortDataUpdate(wxConnexPortBase* port);

	void onPropertyChanged(wxPropertyGridEvent& event);
	void onPropertyChanging(wxPropertyGridEvent& event);
	
	inline wxString getName(){ return wxT("LOG"); }

protected:
	void apply(wxImage* image);

	std::vector<double> calcLapMask();
	std::vector<double> calcGaussianMask();
};

//Node for applying Multi-scale LOG (Laplacian of Gaussian) to an image.
class MultiScaleLOGNode : public NodeBase
{
protected:
	//Control variables.
	float startSigma;
	float endSigma;
	float deltaSigma;
	//Variable used to control which sigma value result to display.
	unsigned int toDisplay;

	//Add the control variables and toDisplay to the property grid for user interaction.
	void addProperties();

	//Dummy image used as a placeholder when no other information is available.
	wxImage dummy;
	//Vector that holds the results, first pair argument is the sigma value of the result.
	std::vector<std::pair<float, wxImage>> results;

	//Panel on which to display the image.
	ImagePanel* image;

	//All image input.
	ImagePort* input;
	//Binary output, outputs the final result.
	ImagePort* output;

public:
	//Constructs a new Operator.
	MultiScaleLOGNode(wxConnexSpace *parent, const wxPoint& pos = wxDefaultPosition);

	//Save the results (all of them) to file.
	void onSave(wxCommandEvent& event);
	//Called when a node connected to the input port changes.
	void OnPortDataUpdate(wxConnexPortBase* port);

	//Called when the user updates a property of the Node in the UI.
	void onPropertyChanged(wxPropertyGridEvent& event);
	//Performs value checking when the user changes a property value in the UI.
	void onPropertyChanging(wxPropertyGridEvent& event);
	
	//Get the name of the Node.
	inline wxString getName(){ return wxT("Multi-scale LOG"); }

protected:
	//Apply the multi-scale LOG to the image, called when a property changes or
	//when the data from the iput port changes (and is valid).
	void apply(wxImage* image);

	//Generate the Laplacian/Smoothing combination mask (G1) for a passed sigma value.
	std::vector<double> calcLapMask(const float& currentSigma);
	//Generate the Gaussian smoothing mask (G2) for a passed sigma value.
	std::vector<double> calcGaussianMask(const float& currentSigma);

	//Check is a coord is already in the vector.
	bool foundInVector(const Coord& value, std::vector<Coord>& list);
};

#endif