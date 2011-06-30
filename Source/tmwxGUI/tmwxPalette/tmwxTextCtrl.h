/*******************************************************************************
File:         tmwxTextCtrl.h
Project:      TreeMaker 5.x
Purpose:      Header file for text control specialization
Author:       Robert J. Lang
Modified by:  
Created:      2003-12-01
Copyright:    ©2003 Robert J. Lang. All Rights Reserved.
*******************************************************************************/

#ifndef _TMWXTEXTCTRL_H_
#define _TMWXTEXTCTRL_H_

// Common Treemaker and wxWidgets headers
#include "tmHeader.h"

// TreeMaker model headers
#include "tmPoint.h"
#include "tmTree.h"

// TreeMaker GUI headers
#include "tmwxStr.h"

// Forward declarations
#include "tmModel_fwd.h"


/**********
class tmwxTextCtrl
Specialization of wxTextCtrl, for editing data in trees and parts
**********/
class tmwxTextCtrl : public wxTextCtrl {
private:
  wxString mFloatFormat;
public:
  // Constructors
  tmwxTextCtrl(wxWindow* parent, const wxChar* floatFormat = wxEmptyString);

  // Value setting, augments ancestor method SetValue(const wxString&)
  using wxTextCtrl::SetValue;
  void SetValue(const char* c);
  void SetValue(std::size_t i);
  void SetValue(const tmFloat& f);

  // Validation with optional explanation
  bool ValidateSizeT(const wxString& parmName, std::size_t& theValue, 
    const wxString& explanation = wxEmptyString);
  bool ValidateSizeT(const wxString& parmName, const std::size_t& minValue, 
    std::size_t& theValue, const wxString& explanation = wxEmptyString);
  bool ValidateSizeT(const wxString& parmName, const std::size_t& minValue, 
    const std::size_t& maxValue, std::size_t& theValue, 
      const wxString& explanation = wxEmptyString);
  bool ValidateFloat(const wxString& parmName, tmFloat& theValue, 
    const wxString& explanation = wxEmptyString);
  bool ValidateFloat(const wxString& parmName, 
    const tmFloat& minValue, bool includeMin, tmFloat& theValue, 
      const wxString& explanation = wxEmptyString);
  bool ValidateFloat(const wxString& parmName, 
    const tmFloat& minValue, bool includeMin,
    const tmFloat& maxValue, bool includeMax, tmFloat& theValue, 
      const wxString& explanation = wxEmptyString);
  
  // Validation of specific entries
  bool ValidateXCoord(tmTree* aTree, tmFloat& theValue);
  bool ValidateYCoord(tmTree* aTree, tmFloat& theValue);
  bool ValidateAngle(const wxString& parmName, tmFloat& theAngle);
  bool ValidatePartLabel(wxString& theLabel);
  bool ValidateNodeIndex(tmTree* aTree, std::size_t& theIndex);
  bool ValidateNodeIndexForCondition(tmTree* aTree, tmNode*& theNode);
  bool ValidateEdgeIndex(tmTree* aTree, std::size_t& theIndex);
  bool ValidateEdgeIndexForCondition(tmTree* aTree, tmEdge*& theEdge);
  bool ValidateConditionIndex(tmTree* theTree, std::size_t& theIndex);
  
  // Value comparison
  bool SameValue(const char* c);
  bool SameValue(std::size_t i);
  bool SameValue(const tmFloat& f);
};


#endif // _TMWXTEXTCTRL_H_
