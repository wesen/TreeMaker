/*******************************************************************************
File:         tmwxInspectorFrame.h
Project:      TreeMaker 5.x
Purpose:      Header file for TreeMaker object inspector class
Author:       Robert J. Lang
Modified by:  
Created:      2003-11-15
Copyright:    ©2003 Robert J. Lang. All Rights Reserved.
*******************************************************************************/

#ifndef _TMWXINSPECTORFRAME_H_
#define _TMWXINSPECTORFRAME_H_

// Include standard Treemaker and wxWidgets headers
#include "tmHeader.h"

// Additional Treemaker model classes
#include "tmArray.h"
#include "tmPart.h"

// Additional Treemaker GUI classes
#include "tmwxPaletteFrame.h"

// Forward declarations
#include "tmModel_fwd.h"
#include "tmwxGUI_fwd.h"

/**********
class tmwxInspectorFrame
Displays information about Trees and their parts when selected
**********/
class tmwxInspectorFrame: public tmwxPaletteFrame<tmwxDocParentFrame> {
public:
  tmwxInspectorFrame(wxDocManager* manager);
  void DispatchSetSelection(tmTree* aTree, tmCluster& aCluster);
  void SetSelection();
  void SetSelection(tmCluster* aCluster);
  void SetSelection(tmPart* aPart);
  
  // Event handling
  void OnClose(wxCloseEvent& event);
  DECLARE_EVENT_TABLE()

private:
  tmwxTreePanel* mTreePanel;
  tmwxGroupPanel* mGroupPanel;
  tmwxNodePanel* mNodePanel;
  tmwxEdgePanel* mEdgePanel;
  tmwxPathPanel* mPathPanel;
  tmwxPolyPanel* mPolyPanel;
  tmwxVertexPanel* mVertexPanel;
  tmwxCreasePanel* mCreasePanel;
  tmwxFacetPanel* mFacetPanel;
  tmwxConditionNodeComboPanel* mConditionNodeComboPanel;
  tmwxConditionNodeFixedPanel* mConditionNodeFixedPanel;
  tmwxConditionNodeOnCornerPanel* mConditionNodeOnCornerPanel;
  tmwxConditionNodeOnEdgePanel* mConditionNodeOnEdgePanel;
  tmwxConditionNodeSymmetricPanel* mConditionNodeSymmetricPanel;
  tmwxConditionNodesPairedPanel* mConditionNodesPairedPanel;
  tmwxConditionNodesCollinearPanel* mConditionNodesCollinearPanel;
  tmwxConditionEdgeLengthFixedPanel* mConditionEdgeLengthFixedPanel;
  tmwxConditionEdgesSameStrainPanel* mConditionEdgesSameStrainPanel;
  tmwxConditionPathComboPanel* mConditionPathComboPanel;
  tmwxConditionPathActivePanel* mConditionPathActivePanel;
  tmwxConditionPathAngleFixedPanel* mConditionPathAngleFixedPanel;
  tmwxConditionPathAngleQuantPanel* mConditionPathAngleQuantPanel;
    
  void GetPositionSizeInfo(Key key, wxString& keystr, int& val);
  
  typedef void (tmwxInspectorFrame::*SetSelectionFn)(tmPart* p);
  template <class P>
    class PanelTraits;
  template <class P>
    void SetSelectionP(tmPart* p);
  template <class P>
    class SetSelectionFnT {
    public:
      SetSelectionFn operator()() const;
    };
  static tmArray<SetSelectionFn>& GetSetSelectionFns();
  
  // friend classes
  TM_TEMPLATE_FRIENDS(SetSelectionFnT)
  TM_TEMPLATE_FRIENDS(PanelTraits)
};


/**********
Template definitions
**********/

#endif //_TMWXINSPECTORFRAME_H_
