/*******************************************************************************
File:         tmwxInspectorFrame.cpp
Project:      TreeMaker 5.x
Purpose:      Implementation file for TreeMaker object inspector frame class
Author:       Robert J. Lang
Modified by:  
Created:      2003-11-15
Copyright:    ©2003 Robert J. Lang. All Rights Reserved.
*******************************************************************************/

#include "tmwxInspectorFrame.h"
#include "tmModel.h"
#include "tmwxApp.h"
#include "tmwxDocManager.h"
#include "tmwxTreePanel.h"
#include "tmwxGroupPanel.h"
#include "tmwxNodePanel.h"
#include "tmwxEdgePanel.h"
#include "tmwxPathPanel.h"
#include "tmwxPolyPanel.h"
#include "tmwxVertexPanel.h"
#include "tmwxCreasePanel.h"
#include "tmwxFacetPanel.h"
#include "tmwxConditionNodeComboPanel.h"
#include "tmwxConditionNodeFixedPanel.h"
#include "tmwxConditionNodeOnCornerPanel.h"
#include "tmwxConditionNodeOnEdgePanel.h"
#include "tmwxConditionNodeSymmetricPanel.h"
#include "tmwxConditionNodesPairedPanel.h"
#include "tmwxConditionNodesCollinearPanel.h"
#include "tmwxConditionEdgeLengthFixedPanel.h"
#include "tmwxConditionEdgesSameStrainPanel.h"
#include "tmwxConditionPathComboPanel.h"
#include "tmwxConditionPathActivePanel.h"
#include "tmwxConditionPathAngleFixedPanel.h"
#include "tmwxConditionPathAngleQuantPanel.h"

#include "wx/config.h"

/**********
class tmwxInspectorFrame
Displays information about Trees and their parts when selected
**********/

/*****
Constructor creates the frame, which is a tool palette.
*****/
tmwxInspectorFrame::tmwxInspectorFrame(wxDocManager* manager)
  : tmwxPaletteFrame<tmwxDocParentFrame>(manager, wxT("Inspector"), 280)
{
  // Get position and size last stored in wxConfig
  InitPositionAndSize();
  
  // Create all of the inspector panels
  mTreePanel = new tmwxTreePanel(this);
  mGroupPanel = new tmwxGroupPanel(this);
  mNodePanel = new tmwxNodePanel(this);
  mEdgePanel = new tmwxEdgePanel(this);
  mPathPanel = new tmwxPathPanel(this);
  mPolyPanel = new tmwxPolyPanel(this);
  mVertexPanel = new tmwxVertexPanel(this);
  mCreasePanel = new tmwxCreasePanel(this);
  mFacetPanel = new tmwxFacetPanel(this);
  mConditionNodeComboPanel = new tmwxConditionNodeComboPanel(this);
  mConditionNodeFixedPanel = new tmwxConditionNodeFixedPanel(this);
  mConditionNodeOnCornerPanel = new tmwxConditionNodeOnCornerPanel(this);
  mConditionNodeOnEdgePanel = new tmwxConditionNodeOnEdgePanel(this);
  mConditionNodeSymmetricPanel = new tmwxConditionNodeSymmetricPanel(this);
  mConditionNodesPairedPanel = new tmwxConditionNodesPairedPanel(this);
  mConditionNodesCollinearPanel = new tmwxConditionNodesCollinearPanel(this);
  mConditionEdgeLengthFixedPanel = new tmwxConditionEdgeLengthFixedPanel(this);
  mConditionEdgesSameStrainPanel = new tmwxConditionEdgesSameStrainPanel(this);
  mConditionPathComboPanel = new tmwxConditionPathComboPanel(this);
  mConditionPathActivePanel = new tmwxConditionPathActivePanel(this);
  mConditionPathAngleFixedPanel = new tmwxConditionPathAngleFixedPanel(this);
  mConditionPathAngleQuantPanel = new tmwxConditionPathAngleQuantPanel(this);
}


/*****
Change selection to the appropriate panel for the given cluster. Behavior is:
-- if a single part is selected, set the panel to display the selected part
-- if multiple parts are selected, set the panel to display the group panel
-- if no parts are selected, set the panel to display the tree
*****/
void tmwxInspectorFrame::DispatchSetSelection(tmTree* aTree, 
  tmCluster& aCluster)
{
  size_t n = aCluster.GetNumAllParts();
  if (n == 0) 
    // no items selected, use the tree panel
    SetSelection((tmPart*)aTree);  
  else if (n == 1) {
    // exactly one item selected, use the selected item panel
    if (aCluster.mNodes.size() == 1)
      SetSelection(aCluster.mNodes.front());
    else if (aCluster.mEdges.size() == 1)
      SetSelection(aCluster.mEdges.front());
    else if (aCluster.mPaths.size() == 1)
      SetSelection(aCluster.mPaths.front());
    else if (aCluster.mPolys.size() == 1)
      SetSelection(aCluster.mPolys.front());
    else if (aCluster.mVertices.size() == 1)
      SetSelection(aCluster.mVertices.front());
    else if (aCluster.mCreases.size() == 1)
      SetSelection(aCluster.mCreases.front());
    else if (aCluster.mFacets.size() == 1)
      SetSelection(aCluster.mFacets.front());
    else if (aCluster.mConditions.size() == 1)
      SetSelection(aCluster.mConditions.front());
  }
  else {
    // multiple items selected, use the group panel
    SetSelection(&aCluster);
  }
}


/*
Notes on the template magic that makes panel selection work.

Function SetSelection(tmPart*) accepts a pointer to any type of tmPart ,
figures out what its dynamic type, and changes the selection to the appropriate
panel to edit that dynamic type. It does this using the mechanism provided by
the model, in which one builds an array of template functions to be invoked for
the dynamic type.

So...in what follows, P is the dynamic type of the tmPart in question.

template traits class PanelTraits<P> provides the type of the required panel
(panel_t) and a pointer to the required panel itself, via GetProperPanel(..).

This class is used by the template function SetSelectionP<P>(tmPart*), which
takes the generic pointer, dynamically casts it to its proper type P, then uses
PanelTraits<P> to get the right panel and fill it (using its Fill(P*) method).

The class SetSelectionFnT<P> defines the type (required by
tmPart::MakeTypeArray(..)) that can return a pointer to the SetSelectionP<P>
function for each required type P. MakeTypeArray(..) uses this type to create
an array of function pointers to the various incarnations of SetSelectionP<P>.

Thus, in SetSelection(tmPart*), we can use the tag of the tmPart as the index
into our array of function pointers to call the proper flavor of
SetSelectionP<P>.
*/


/**********
class tmwxInspectorFrame::PanelTraits<tmTree>
Traits class that links a tmPart with the panel used to edit it
**********/
template <>
class tmwxInspectorFrame::PanelTraits<tmTree> {
public:
  typedef tmwxTreePanel panel_t;
  static panel_t* GetProperPanel(tmwxInspectorFrame* frame) {
    return frame->mTreePanel;};
};


/**********
class tmwxInspectorFrame::PanelTraits<tmNode>
Traits class that links a tmPart with the panel used to edit it
**********/
template <>
class tmwxInspectorFrame::PanelTraits<tmNode> {
public:
  typedef tmwxNodePanel panel_t;
  static panel_t* GetProperPanel(tmwxInspectorFrame* frame) {
    return frame->mNodePanel;};
};


/**********
class tmwxInspectorFrame::PanelTraits<tmEdge>
Traits class that links a tmPart with the panel used to edit it
**********/
template <>
class tmwxInspectorFrame::PanelTraits<tmEdge> {
public:
  typedef tmwxEdgePanel panel_t;
  static panel_t* GetProperPanel(tmwxInspectorFrame* frame) {
    return frame->mEdgePanel;};
};


/**********
class tmwxInspectorFrame::PanelTraits<tmPath>
Traits class that links a tmPart with the panel used to edit it
**********/
template <>
class tmwxInspectorFrame::PanelTraits<tmPath> {
public:
  typedef tmwxPathPanel panel_t;
  static panel_t* GetProperPanel(tmwxInspectorFrame* frame) {
    return frame->mPathPanel;};
};


/**********
class tmwxInspectorFrame::PanelTraits<tmPoly>
Traits class that links a tmPart with the panel used to edit it
**********/
template <>
class tmwxInspectorFrame::PanelTraits<tmPoly> {
public:
  typedef tmwxPolyPanel panel_t;
  static panel_t* GetProperPanel(tmwxInspectorFrame* frame) {
    return frame->mPolyPanel;};
};


/**********
class tmwxInspectorFrame::PanelTraits<tmVertex>
Traits class that links a tmPart with the panel used to edit it
**********/
template <>
class tmwxInspectorFrame::PanelTraits<tmVertex> {
public:
  typedef tmwxVertexPanel panel_t;
  static panel_t* GetProperPanel(tmwxInspectorFrame* frame) {
    return frame->mVertexPanel;};
};


/**********
class tmwxInspectorFrame::PanelTraits<tmCrease>
Traits class that links a tmPart with the panel used to edit it
**********/
template <>
class tmwxInspectorFrame::PanelTraits<tmCrease> {
public:
  typedef tmwxCreasePanel panel_t;
  static panel_t* GetProperPanel(tmwxInspectorFrame* frame) {
    return frame->mCreasePanel;};
};


/**********
class tmwxInspectorFrame::PanelTraits<tmFacet>
Traits class that links a tmPart with the panel used to edit it
**********/
template <>
class tmwxInspectorFrame::PanelTraits<tmFacet> {
public:
  typedef tmwxFacetPanel panel_t;
  static panel_t* GetProperPanel(tmwxInspectorFrame* frame) {
    return frame->mFacetPanel;};
};


/**********
class tmwxInspectorFrame::PanelTraits<tmConditionNodeCombo>
Traits class that links a tmPart with the panel used to edit it
**********/
template <>
class tmwxInspectorFrame::PanelTraits<tmConditionNodeCombo> {
public:
  typedef tmwxConditionNodeComboPanel panel_t;
  static panel_t* GetProperPanel(tmwxInspectorFrame* frame) {
    return frame->mConditionNodeComboPanel;};
};


/**********
class tmwxInspectorFrame::PanelTraits<tmConditionNodeFixed>
Traits class that links a tmPart with the panel used to edit it
**********/
template <>
class tmwxInspectorFrame::PanelTraits<tmConditionNodeFixed> {
public:
  typedef tmwxConditionNodeFixedPanel panel_t;
  static panel_t* GetProperPanel(tmwxInspectorFrame* frame) {
    return frame->mConditionNodeFixedPanel;};
};


/**********
class tmwxInspectorFrame::PanelTraits<tmConditionNodeOnCorner>
Traits class that links a tmPart with the panel used to edit it
**********/
template <>
class tmwxInspectorFrame::PanelTraits<tmConditionNodeOnCorner> {
public:
  typedef tmwxConditionNodeOnCornerPanel panel_t;
  static panel_t* GetProperPanel(tmwxInspectorFrame* frame) {
    return frame->mConditionNodeOnCornerPanel;};
};


/**********
class tmwxInspectorFrame::PanelTraits<tmConditionNodeOnEdge>
Traits class that links a tmPart with the panel used to edit it
**********/
template <>
class tmwxInspectorFrame::PanelTraits<tmConditionNodeOnEdge> {
public:
  typedef tmwxConditionNodeOnEdgePanel panel_t;
  static panel_t* GetProperPanel(tmwxInspectorFrame* frame) {
    return frame->mConditionNodeOnEdgePanel;};
};


/**********
class tmwxInspectorFrame::PanelTraits<tmConditionNodeSymmetric>
Traits class that links a tmPart with the panel used to edit it
**********/
template <>
class tmwxInspectorFrame::PanelTraits<tmConditionNodeSymmetric> {
public:
  typedef tmwxConditionNodeSymmetricPanel panel_t;
  static panel_t* GetProperPanel(tmwxInspectorFrame* frame) {
    return frame->mConditionNodeSymmetricPanel;};
};


/**********
class tmwxInspectorFrame::PanelTraits<tmConditionNodesPaired>
Traits class that links a tmPart with the panel used to edit it
**********/
template <>
class tmwxInspectorFrame::PanelTraits<tmConditionNodesPaired> {
public:
  typedef tmwxConditionNodesPairedPanel panel_t;
  static panel_t* GetProperPanel(tmwxInspectorFrame* frame) {
    return frame->mConditionNodesPairedPanel;};
};


/**********
class tmwxInspectorFrame::PanelTraits<tmConditionNodesCollinear>
Traits class that links a tmPart with the panel used to edit it
**********/
template <>
class tmwxInspectorFrame::PanelTraits<tmConditionNodesCollinear> {
public:
  typedef tmwxConditionNodesCollinearPanel panel_t;
  static panel_t* GetProperPanel(tmwxInspectorFrame* frame) {
    return frame->mConditionNodesCollinearPanel;};
};


/**********
class tmwxInspectorFrame::PanelTraits<tmConditionEdgeLengthFixed>
Traits class that links a tmPart with the panel used to edit it
**********/
template <>
class tmwxInspectorFrame::PanelTraits<tmConditionEdgeLengthFixed> {
public:
  typedef tmwxConditionEdgeLengthFixedPanel panel_t;
  static panel_t* GetProperPanel(tmwxInspectorFrame* frame) {
    return frame->mConditionEdgeLengthFixedPanel;};
};


/**********
class tmwxInspectorFrame::PanelTraits<tmConditionEdgesSameStrain>
Traits class that links a tmPart with the panel used to edit it
**********/
template <>
class tmwxInspectorFrame::PanelTraits<tmConditionEdgesSameStrain> {
public:
  typedef tmwxConditionEdgesSameStrainPanel panel_t;
  static panel_t* GetProperPanel(tmwxInspectorFrame* frame) {
    return frame->mConditionEdgesSameStrainPanel;};
};


/**********
class tmwxInspectorFrame::PanelTraits<tmConditionPathCombo>
Traits class that links a tmPart with the panel used to edit it
**********/
template <>
class tmwxInspectorFrame::PanelTraits<tmConditionPathCombo> {
public:
  typedef tmwxConditionPathComboPanel panel_t;
  static panel_t* GetProperPanel(tmwxInspectorFrame* frame) {
    return frame->mConditionPathComboPanel;};
};


/**********
class tmwxInspectorFrame::PanelTraits<tmConditionPathActive>
Traits class that links a tmPart with the panel used to edit it
**********/
template <>
class tmwxInspectorFrame::PanelTraits<tmConditionPathActive> {
public:
  typedef tmwxConditionPathActivePanel panel_t;
  static panel_t* GetProperPanel(tmwxInspectorFrame* frame) {
    return frame->mConditionPathActivePanel;};
};


/**********
class tmwxInspectorFrame::PanelTraits<tmConditionPathAngleFixed>
Traits class that links a tmPart with the panel used to edit it
**********/
template <>
class tmwxInspectorFrame::PanelTraits<tmConditionPathAngleFixed> {
public:
  typedef tmwxConditionPathAngleFixedPanel panel_t;
  static panel_t* GetProperPanel(tmwxInspectorFrame* frame) {
    return frame->mConditionPathAngleFixedPanel;};
};


/**********
class tmwxInspectorFrame::PanelTraits<tmConditionPathAngleQuant>
Traits class that links a tmPart with the panel used to edit it
**********/
template <>
class tmwxInspectorFrame::PanelTraits<tmConditionPathAngleQuant> {
public:
  typedef tmwxConditionPathAngleQuantPanel panel_t;
  static panel_t* GetProperPanel(tmwxInspectorFrame* frame) {
    return frame->mConditionPathAngleQuantPanel;};
};


/*****
Set the selection from a generic tmPart pointer. We cast the generic pointer to
the template type, then assign it to the appropriate panel, which comes from
the PanelTraits class.
*****/
template <class P>
void tmwxInspectorFrame::SetSelectionP(tmPart* p)
{
  P* pp = dynamic_cast<P*>(p); 
  TMASSERT(pp);
  typename PanelTraits<P>::panel_t* ppanel = 
    PanelTraits<P>::GetProperPanel(this);
  TMASSERT(ppanel);
  ppanel->Fill(pp);
  if (mObj == pp) return;
  mObj = pp;
  if (mPanel != ppanel) {
    UninstallPanel();
    InstallPanel(ppanel);
  }
}


/*****
Return ptr to the member template function of the templated type
*****/
template <class P>
tmwxInspectorFrame::SetSelectionFn 
  tmwxInspectorFrame::SetSelectionFnT<P>::operator()() const
{
  return &tmwxInspectorFrame::SetSelectionP<P>;
}


/*****
STATIC
Make if necessary and return our list of functions for setting the selection
*****/
tmArray<tmwxInspectorFrame::SetSelectionFn>& 
  tmwxInspectorFrame::GetSetSelectionFns()
{
  static tmArray<SetSelectionFn> sSetSelectionFns;
  if (sSetSelectionFns.empty()) {
    TMASSERT(tmPart::TypesAreInitialized());
    tmPart::MakeTypeArray<SetSelectionFn, SetSelectionFnT>(sSetSelectionFns);
  }
  return sSetSelectionFns;
}


/*****
Explicitly redeclared since overloaded versions would otherwise hide it
*****/
void tmwxInspectorFrame::SetSelection()
{
  tmwxPaletteFrame<tmwxDocParentFrame>::SetSelection();
}


/*****
Change selection panel to the appropriate inspector panel, which depends on the
specific type of part. For this we'll use our array of template functions.
*****/
void tmwxInspectorFrame::SetSelection(tmPart* aPart)
{
  (this->*GetSetSelectionFns()[aPart->GetTag()])(aPart);
}


/*****
Change selection panel to group inspector panel. Specifically overloaded since
SetSelection(tmPart*) won't apply since tmCluster is not descended from tmPart.
*****/
void tmwxInspectorFrame::SetSelection(tmCluster* aCluster)
{
  mGroupPanel->Fill(aCluster);
  if (mObj == aCluster) return;
  mObj = aCluster;
  if (mPanel != mGroupPanel) {
    UninstallPanel();
    InstallPanel(mGroupPanel);
  }
}


/*****
Return the key string and default value for the given position/size component.
*****/
void tmwxInspectorFrame::GetPositionSizeInfo(Key key, wxString& keystr, 
  int& val)
{
  switch(key) {
    case X_KEY:
      keystr = wxT("InspectorFrameX");
      val = 30;
      break;
    case Y_KEY:
      keystr = wxT("InspectorFrameY");
      val = 70;
      break;
    case W_KEY:
      keystr = wxT("InspectorFrameW");
      val = 280;
      break;
    case H_KEY:
      keystr = wxT("InspectorFrameH");
      val = 280;
      break;
    default:
      TMFAIL("unknown key encountered in "\
       "tmwxInspectorFrame::GetPositionSizeInfo(..)");
      break;
  }
}


#ifdef __MWERKS__
  #pragma mark -
#endif


/*****
Attempt to close should only hide the window except at shutdown.
*****/
void tmwxInspectorFrame::OnClose(wxCloseEvent& event)
{
  if (gApp->IsQuitting())
    event.Skip();
  else
    if (event.CanVeto()) {
      event.Veto();
      Hide();
    }
}


/*****
Event table
*****/
BEGIN_EVENT_TABLE(tmwxInspectorFrame, tmwxPaletteFrame<tmwxDocParentFrame>)
  EVT_CLOSE(tmwxInspectorFrame::OnClose)
END_EVENT_TABLE()
