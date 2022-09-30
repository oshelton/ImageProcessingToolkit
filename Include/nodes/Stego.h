/* 
* Owen Shelton
* CSCI 8810 Spring 2011
* Project 2
*/

#ifndef STEGO_H
#define STEGO_H

#include "StegoPort.h"
#include "NodeBase.h"

//Abstract class which serves as a base for other Stego embedding nodes.
class EmbedStegoNode: public NodeBase
{
protected:
	//Result of embedding operation.
	wxImage result;

	//Panel on which to display the image (after embedding).
	ImagePanel* image;

	//Image input.
	ImagePort* input;

	//Special output to 
	StegoPort* output;
	//Regular output for other nodes.
	ImagePort* imageOutput;

	//Number of least signifigant bits used.
	unsigned int bitsUsed;
	//Number of bytes in data.
	unsigned int dataLength;

public:
	EmbedStegoNode(wxConnexSpace* parent, const wxString& label, const wxPoint& pos, const wxSize& size): 
	  NodeBase(parent, label, pos, size), bitsUsed(1), dataLength(0){}
	virtual ~EmbedStegoNode(){}

	void onSave(wxCommandEvent& event);
	virtual void OnPortDataUpdate(wxConnexPortBase* port);

protected:
	virtual void apply(wxImage* image) = 0;
};

//Node for embedding text into an image.
class EmbedTextNode: public EmbedStegoNode
{
protected:
	//Text to be embedded.
	wxString text;

	//Add the text value and bitsUsed property to the grid.
	void addProperties();

public:
	EmbedTextNode(wxConnexSpace *parent, const wxPoint& pos = wxDefaultPosition);

	void onPropertyChanged(wxPropertyGridEvent& event);
	void onPropertyChanging(wxPropertyGridEvent& event);
	
	inline wxString getName(){ return wxT("Embed Text"); }

protected:
	void apply(wxImage* image);
};


//Node for embedding an image into another image.
class EmbedImageNode: public EmbedStegoNode
{
protected:
	//Image to be embeded.
	wxImage loaded;
	//ImagePanel for viewing the image being embedded.
	ImagePanel* toEmbed;

	//File name of image.
	wxString fileName;
	//Dimensions of the image being embedded.
	wxSize imageDim;

	//Add the file name and bitsUsed property to the grid.
	void addProperties();

public:
	EmbedImageNode(wxConnexSpace *parent, const wxPoint& pos = wxDefaultPosition);

	void onPropertyChanged(wxPropertyGridEvent& event);
	void onPropertyChanging(wxPropertyGridEvent& event);

	void OnPortDataUpdate(wxConnexPortBase* port);
	
	inline wxString getName(){ return wxT("Embed Image"); }

protected:
	void apply(wxImage* image);
};


//Abstract class which serves as a base for other Stego extracting nodes.
class ExtractStegoNode: public NodeBase
{
protected:
	//Image input.
	StegoPort* input;

public:
	ExtractStegoNode(wxConnexSpace* parent, const wxString& label, const wxPoint& pos, const wxSize& size): 
	  NodeBase(parent, label, pos, size){}
	virtual ~ExtractStegoNode(){}

protected:
	virtual void apply(wxImage* image) = 0;
};

//Node for extracting text from an image.
class ExtractTextNode: public ExtractStegoNode
{
protected:
	wxString extracted;

	//Add the extracted text to the grid.
	void addProperties();

public:
	ExtractTextNode(wxConnexSpace *parent, const wxPoint& pos = wxDefaultPosition);

	void onPropertyChanged(wxPropertyGridEvent& event){}
	void onPropertyChanging(wxPropertyGridEvent& event){ event.Veto(); }

	void onSave(wxCommandEvent& event);
	void OnPortDataUpdate(wxConnexPortBase* port);
	
	inline wxString getName(){ return wxT("Extract Text"); }

protected:
	void apply(wxImage* image);
};

//Node for extracting an image from another image.
class ExtractImageNode: public ExtractStegoNode
{
protected:
	//Result of operator.
	wxImage result;
	//ImagePanel for viewing the image extracted.
	ImagePanel* extracted;

	//Regular output for other nodes.
	ImagePort* imageOutput;

	//No properties for this node.
	void addProperties(){}

public:
	ExtractImageNode(wxConnexSpace *parent, const wxPoint& pos = wxDefaultPosition);

	void onPropertyChanged(wxPropertyGridEvent& event){}
	void onPropertyChanging(wxPropertyGridEvent& event){}

	void ExtractImageNode::onSave(wxCommandEvent &event);
	void OnPortDataUpdate(wxConnexPortBase* port);

	inline wxString getName(){ return wxT("Extract Image"); }

protected:
	void apply(wxImage* image);
};

#endif