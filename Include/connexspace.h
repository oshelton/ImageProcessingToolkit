/*
Copyright (c) 2008 Erik Beran

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/

#ifndef _CONNEX_SPACE_H
#define _CONNEX_SPACE_H

#include "wx/window.h"
#include "nodes/NodeBase.h"
#include <list>


DECLARE_EVENT_TYPE(wxEVT_NODE_CHANGED, -1)

class wxConnexSpace : public wxWindow
{
private:
	NodeBase* m_FocusNode;
	//Added by Owen Shelton so a node does not have to delete itself.
	NodeBase*	toDelete;

public:
	typedef std::list<NodeBase*>	NodeList;
	typedef NodeList::iterator			NodeListIterator;

	enum MOUSE_MODE
	{
		MOUSE_DRAG,
		MOUSE_CONNECT,
	};

	bool			m_DrawConnectionShadows;
	MOUSE_MODE		m_MouseMode;
	int				m_MouseFromPlace;
	int				m_MouseToPlace;
	wxPoint			m_LastMousePos;

	NodeList		m_NodeList;

public:
	wxConnexSpace();
	wxConnexSpace(	wxWindow *parent, wxWindowID id = wxID_ANY,
					const wxPoint& pos = wxDefaultPosition,
					const wxSize& size = wxDefaultSize, long style = 0,
					const wxValidator& validator = wxDefaultValidator,
					const wxString& name = wxPanelNameStr );

	void AddNode( NodeBase* node );
	void ConnectPorts( wxConnexPortBase *port0, wxConnexPortBase *port1 );
	void RemoveConnection( wxConnexPortBase *port);
	void setNodeToDelete(wxConnexNode* node){ toDelete = static_cast<NodeBase*>(node); }

	//Added by owen Shelton to fire new event.
	void SetFocusNode(wxConnexNode* node);
	inline NodeBase* GetFocusNode(){ return m_FocusNode; }

	//////////////////////////////////////////////////////////////////////////
	void OnPaint( wxPaintEvent& event );
	void OnMouseEvent( wxMouseEvent& evt );

	//////////////////////////////////////////////////////////////////////////
	DECLARE_CLASS(wxConnexSpace)
	DECLARE_EVENT_TABLE()
};

#endif
