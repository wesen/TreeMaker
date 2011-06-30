/*******************************************************************************
File:         tmwxGUI_fwd.h
Project:      TreeMaker 5.x
Purpose:      Forward declarations for all tmwxGUI classes
Author:       Robert J. Lang
Modified by:  
Created:      2004-05-03
Copyright:    ©2004 Robert J. Lang. All Rights Reserved.
*******************************************************************************/

#ifndef _TMWXGUI_FWD_H_
#define _TMWXGUI_FWD_H_

// tmwxCommon
class tmwxApp;
class tmwxDocManager;
class tmwxGetUserInputDialog;
class tmwxCommand;

// tmwxDocView
class tmwxDoc;
class tmwxView;
class tmwxDesignCanvas;
class tmwxDesignFrame;
class tmwxViewSettings;
class tmwxPrintout;

// twmxFoldedForm
class tmwxFoldedFormFrame;

// tmwxHtmlHelp
class tmwxHtmlHelpController;
class tmwxHtmlHelpFrame;

// tmwxInspector
class tmwxInspectorFrame;
class tmwxInspectorPanel;
class tmwxConditionListBox;
class tmwxGroupPanel;
class tmwxTreePanel;
class tmwxNodePanel;
class tmwxEdgePanel;
class tmwxPathPanel;
class tmwxPolyPanel;
class tmwxVertexPanel;
class tmwxFacetPanel;
class tmwxCreasePanel;
class tmwxConditionNodeComboPanel;
class tmwxConditionNodeFixedPanel;
class tmwxConditionNodeSymmetricPanel;
class tmwxConditionNodeOnEdgePanel;
class tmwxConditionNodeOnCornerPanel;
class tmwxConditionNodesPairedPanel;
class tmwxConditionNodesCollinearPanel;
class tmwxConditionEdgeLengthFixedPanel;
class tmwxConditionEdgesSameStrainPanel;
class tmwxConditionPathComboPanel;
class tmwxConditionPathActivePanel;
class tmwxConditionPathAngleFixedPanel;
class tmwxConditionPathAngleQuantPanel;

// tmwxLogFrame
#ifdef TM_USE_LOGFRAME
  class tmwxLogFrame;
#endif

// tmwxOptimizerDialog
class tmwxOptimizerDialog;

// tmwxPalette
class tmwxPalettePanel;
class tmwxStaticText;
class tmwxTextCtrl;
class tmwxCheckBoxSmall;

// tmwxViewSettings
class tmwxViewSettingsFrame;
class tmwxViewSettingsPanel;

#endif // _TMWXGUI_FWD_H_
