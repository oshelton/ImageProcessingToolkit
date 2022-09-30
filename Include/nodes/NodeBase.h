/* 
* Owen Shelton
* CSCI 8810 Spring 2011
* Project 2
*/

#ifndef NODEBASE_H
#define NODEBASE_H

#include <wx/button.h>
#include <wx/propgrid/propgrid.h>
#include <wx/propgrid/props.h>
#include <wx/propgrid/advprops.h>
#include "connexnode.h"
#include "DataTypes.h"
#include "ImagePanel.h"
#include "ImagePort.h"
#include "PyramidPort.h"

//Base class for all nodes used in the project.
class NodeBase : public wxConnexNode
{
protected:
	//Abstract function that adds the node specific properties to the property grid.
	virtual void addProperties() = 0;

	//Hold on to pointers for future removal.
	wxButton* saveButton;
	wxPropertyGrid* grid;

	//Keep track of whether or not the Node is connected to the Gui controls.
	bool connected;

public:
	//Constructor, hides some details.
	NodeBase(wxConnexSpace* parent, const wxString& label, const wxPoint& pos, const wxSize& size);
	//Deconstructor, disconnects the node.
	virtual ~NodeBase();

	//Connect the node to the save button and property grid, called when the node in focus changes.
	virtual void connect(wxButton* saveButton, wxPropertyGrid* grid);
	//Disconnect the node to the save button and property grid, called when the node in focus changes.
	virtual void disconnect();

	//Save the node's contents, what this does depends on the sub class.
	virtual void onSave(wxCommandEvent& event) = 0;
	//Called when the Property grid changes one of the nodes values.
	virtual void onPropertyChanged(wxPropertyGridEvent& event) = 0;
	virtual void onPropertyChanging(wxPropertyGridEvent& event) = 0;
	
	//Get the name of the node.
	virtual wxString getName() = 0; 
};

#endif