/*******************************************************************************
File:         tmwxTextCtrl.cpp
Project:      TreeMaker 5.x
Purpose:      Implementation file for text control specialization
Author:       Robert J. Lang
Modified by:  
Created:      2003-12-01
Copyright:    ©2003 Robert J. Lang. All Rights Reserved.
*******************************************************************************/

#include "tmwxTextCtrl.h"
#include "tmModel.h"
#include "tmwxStr.h"
#include "tmwxApp.h"

/*****
Constructor
*****/
tmwxTextCtrl::tmwxTextCtrl(wxWindow* parent, const wxChar* floatFormat) : 
  wxTextCtrl(parent, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 
    wxTE_PROCESS_ENTER),
  mFloatFormat(floatFormat)
{
}


#ifdef __MWERKS__
#pragma mark -
#endif


/*****
Set the value from a C string
*****/
void tmwxTextCtrl::SetValue(const char* c)
{
  wxString str = wxString::FromAscii(c);
  SetValue(str);
}


/*****
Set the value from a size_t
*****/
void tmwxTextCtrl::SetValue(size_t i)
{
  SetValue(tmwxStr(i));
}


/*****
Set the value from a tmFloat, using the format string we were initialized with
*****/
void tmwxTextCtrl::SetValue(const tmFloat& f)
{
  TMASSERT(mFloatFormat.length() != 0); // insure we were initialized
  wxString text;
  text.Printf(mFloatFormat.c_str(), f);
  SetValue(text);
}


#ifdef __MWERKS__
#pragma mark -
#endif


/*****
Validate the text entry as a nonnegative integer, returning the valid value in
theValue.
*****/
bool tmwxTextCtrl::ValidateSizeT(const wxString& parmName, size_t& theValue, 
  const wxString& explanation)
{
  long newValue;
  if (!GetValue().ToLong(&newValue) || newValue < 0) {
    wxString text = wxString::Format(
      wxT("Sorry, \"%s\" is not a valid %s because it is not a nonnegative ")
      wxT("integer. "),
      GetValue().c_str(),
      parmName.c_str());
    tmwxAlertError(text + explanation);
    return false;
  }
  theValue = size_t(newValue);
  return true;
}


/*****
Validate the text entry as a nonnegative integer with a minimum value. Return
the valid value in theValue.
*****/
bool tmwxTextCtrl::ValidateSizeT(const wxString& parmName, 
  const size_t& minValue, size_t& theValue, const wxString& explanation)
{
  size_t newValue;
  if (!ValidateSizeT(parmName, newValue)) return false;
  if (newValue < minValue) {
    wxString text = wxString::Format(
      wxT("Sorry, \"%s\" is not a valid %s because it is not %d or larger. "), 
      GetValue().c_str(),
      parmName.c_str(), 
      int(minValue));
    tmwxAlertError(text + explanation);
    return false;
  }
  theValue = newValue;
  return true;
}
  
  
/*****
Validate the text entry as a integral value with minimum and maximum values.
Return the valid value in theValue.
*****/
bool tmwxTextCtrl::ValidateSizeT(const wxString& parmName, 
  const size_t& minValue, const size_t& maxValue, size_t& theValue, 
  const wxString& explanation)
{
  size_t newValue;
  if (!ValidateSizeT(parmName, newValue)) return false;
  if ((newValue < minValue) || (newValue > maxValue)) {
    wxString text = wxString::Format(
      wxT("Sorry, \"%s\" is not a valid %s because it is not ")
      wxT("between %d and %d. "),
      GetValue().c_str(),
      parmName.c_str(), int(minValue), int(maxValue));
    tmwxAlertError(text + explanation);
    return false;
  }
  theValue = newValue;
  return true;
}


/*****
Validate the text entry as a floating-point value, returning the valid value in
theValue.
*****/
bool tmwxTextCtrl::ValidateFloat(const wxString& parmName, tmFloat& theValue, 
  const wxString& explanation)
{
  double newValue;
  if (!GetValue().ToDouble(&newValue)) {
    wxString text;
    text.Printf(
      wxT("Sorry, \"%s\" is not a valid %s because it is not a valid number. "),
      GetValue().c_str(),
      parmName.c_str());
    tmwxAlertError(text + explanation);
    return false;
  }
  theValue = tmFloat(newValue);
  return true;
}


/*****
Validate the text entry as a floating-point value with a minimum value.
includeMin specifies whether the the minimum value is included in the valid
range. Return the valid value in theValue.
*****/
bool tmwxTextCtrl::ValidateFloat(const wxString& parmName, 
  const tmFloat& minValue, bool includeMin, tmFloat& theValue, 
  const wxString& explanation)
{
  tmFloat newValue;
  if (!ValidateFloat(parmName, newValue)) return false;
  if (includeMin ? (newValue < minValue) : (newValue <= minValue)) {
    wxString format, text;
    if (includeMin)
      format.Printf(
        wxT("Sorry, \"%%s\" is not a valid %%s because it is not ")\
        wxT("greater than %s. "), 
        mFloatFormat.c_str());
    else
      format.Printf(
        wxT("Sorry, \"%%s\" is not a valid %%s because it is not ")\
        wxT("greater than or equal to %s. "), 
        mFloatFormat.c_str());
    text.Printf(format.c_str(), GetValue().c_str(), parmName.c_str(), minValue);
    tmwxAlertError(text + explanation);
    return false;
  }
  theValue = newValue;
  return true;
}
  
  
/*****
Validate the text entry as a floating-point value with minimum and maximum
values. includeMin and includeMax specify whether the min and max values are
included in the valid range. Return the valid value in theValue.
*****/
bool tmwxTextCtrl::ValidateFloat(const wxString& parmName, 
  const tmFloat& minValue, bool includeMin, 
  const tmFloat& maxValue, bool includeMax, tmFloat& theValue, 
  const wxString& explanation)
{
  tmFloat newValue;
  if (!ValidateFloat(parmName, newValue)) return false;
  if ((includeMin ? (newValue < minValue) : (newValue <= minValue)) ||
    (includeMax ? (newValue > maxValue) : (newValue >= maxValue))) {
    wxString format, text;
    if (includeMin)
      if (includeMax)
        format.Printf(
          wxT("Sorry, \"%%s\" is not a valid %%s because it is not ")
          wxT("greater than or equal to %s and less than or equal to %s. "), 
          mFloatFormat.c_str(), mFloatFormat.c_str());
      else // !includeMax
        format.Printf(
          wxT("Sorry, \"%%s\" is not a valid %%s because it is not ")
          wxT("greater than or equal to %s and less than %s. "), 
          mFloatFormat.c_str(), mFloatFormat.c_str());
    else // !includeMin
      if (includeMax)
        format.Printf(
          wxT("Sorry, \"%%s\" is not a valid %%s because it is not ")
          wxT("greater than %s and less than or equal to %s. "), 
          mFloatFormat.c_str(), mFloatFormat.c_str());
      else // !includeMax
        format.Printf(
          wxT("Sorry, \"%%s\" is not a valid %%s because it is not ")
          wxT("greater than %s and less than %s. "), 
          mFloatFormat.c_str(), mFloatFormat.c_str());
    text.Printf(format.c_str(), GetValue().c_str(), parmName.c_str(), 
      minValue, maxValue);
    tmwxAlertError(text + explanation);
    return false;
  }
  theValue = newValue;
  return true;
}


#ifdef __MWERKS__
#pragma mark -
#endif


/*
These validation routines return true if valid data was entered and set the
last variable to the indexed part. If invalid data was entered, they put up an
error dialog and return false, leaving the return value unchanged.

Note that there are templated validation routines for integral and
floating-point types.
*/


/*****
Validate the text entry as an X coordinate within the paper. Return the valid
value in theValue.
*****/
bool tmwxTextCtrl::ValidateXCoord(tmTree* aTree, tmFloat& theValue)
{
  return ValidateFloat(wxT("X coordinate"), 0.0, true, aTree->GetPaperWidth(), 
    true, theValue, wxT("Coordinate values must lie within the paper."));
}


/*****
Validate the text entry as a Y coordinate within the paper. Return the valid
value in theValue.
*****/
bool tmwxTextCtrl::ValidateYCoord(tmTree* aTree, tmFloat& theValue)
{
  return ValidateFloat(wxT("Y coordinate"), 0.0, true, aTree->GetPaperHeight(), 
    true, theValue, wxT("Coordinate values must lie within the paper."));
}


/*****
Validate the text entry as an angle (between -360 and 360 degrees). Return the
valid angle in theAngle.
*****/
bool tmwxTextCtrl::ValidateAngle(const wxString& parmName, tmFloat& theAngle)
{
  return ValidateFloat(parmName, -360., false, 360., false, theAngle,
  wxT("An angle must lie in the range (-360, 360) degrees."));
}


/*****
Validate the text entry as a part label, returning the valid value in theLabel.
*****/
bool tmwxTextCtrl::ValidatePartLabel(wxString& theLabel)
{
  wxString newLabel = GetValue();
  if (!(newLabel.Len() <= tmPart::MAX_LABEL_LEN)) {
    wxString text = wxString::Format(
      wxT("Sorry, \"%s\" is not a valid label. ")
      wxT("The maximum label length is %d characters."), 
      GetValue().c_str(),
      tmPart::MAX_LABEL_LEN);
    tmwxAlertError(text);
    return false;
  }
  theLabel = newLabel;
  return true;
}


/*****
Validate the text entry as a node index, returning the valid value in theIndex.
*****/
bool tmwxTextCtrl::ValidateNodeIndex(tmTree* theTree, size_t& theIndex)
{
  unsigned long newIndex;
  if (!GetValue().ToULong(&newIndex) || 
    (newIndex < 1) || (newIndex > theTree->GetNodes().size())) {
    wxString text = wxString::Format(
      wxT("Sorry, \"%s\" is not a valid node index. ")
      wxT("A node index must be an integer between 1 and %d."),
      GetValue().c_str(),
      int(theTree->GetNodes().size()));
    tmwxAlertError(text);
    return false;
  }
  theIndex = newIndex;
  return true;
}


/*****
Validate the text entry as a condition index, returning the valid node in
theNode. It must be a valid index and the node must be a leaf node.
*****/
bool tmwxTextCtrl::ValidateNodeIndexForCondition(tmTree* theTree, 
  tmNode*& theNode)
{
  size_t newIndex;
  if (!ValidateNodeIndex(theTree, newIndex)) return false;
  theNode = theTree->GetNodes()[newIndex - 1];
  if (!theNode->IsLeafNode()) {
    wxString text = wxString::Format(
      wxT("Sorry, \"%s\" is not a valid conditioned node index. ")
      wxT("Conditioned nodes must be leaf nodes."),
      GetValue().c_str());
    tmwxAlertError(text);
    return false;
  }
  return true;
}


/*****
Validate the text entry as an edge index, returning the valid value in theIndex.
*****/
bool tmwxTextCtrl::ValidateEdgeIndex(tmTree* theTree, size_t& theIndex)
{
  unsigned long newIndex;
  bool ok = GetValue().ToULong(&newIndex);
  if (!ok || (newIndex < 1) || (newIndex > theTree->GetEdges().size())) {
    wxString text = wxString::Format(
      wxT("Sorry, \"%s\" is not a valid edge index. ")
      wxT("An edge index must be an integer between 1 and %d."),
      GetValue().c_str(),
      int(theTree->GetEdges().size()));
    tmwxAlertError(text);
    return false;
  }
  theIndex = newIndex;
  return true;
}


/*****
Validate the text entry as an edge index, returning the valid edge in theEdge.
*****/
bool tmwxTextCtrl::ValidateEdgeIndexForCondition(tmTree* theTree, 
  tmEdge*& theEdge)
{
  size_t newIndex;
  if (!ValidateEdgeIndex(theTree, newIndex)) return false;
  theEdge = theTree->GetEdges()[newIndex - 1];
  return true;
}


/*****
Validate the text entry as a condition index, returning the valid value in
theIndex.
*****/
bool tmwxTextCtrl::ValidateConditionIndex(tmTree* theTree, size_t& theIndex)
{
  unsigned long newIndex;
  bool ok = GetValue().ToULong(&newIndex);
  if (!ok || (newIndex < 1) || (newIndex > theTree->GetConditions().size())) {
    wxString text = wxString::Format(
      wxT("Sorry, \"%s\" is not a valid condition index. ")
      wxT("A condition index must be an integer between 1 and %d."),
      GetValue().c_str(),
      int(theTree->GetConditions().size()));
    tmwxAlertError(text);
    return false;
  }
  theIndex = newIndex;
  return true;
}


#ifdef __MWERKS__
  #pragma mark -
#endif


/*
The SameValue(..) family of routines is useful for checking whether the
contents of a wxTextCtrl have been changed by the user. This is particularly
important for floating-point values, where the test for equivalence is taken to
be whether the two values get formatted as the same string, using the
previously-defined number of digits.
*/

/*****
Return true if the contents are exactly the same as the string c.
*****/
bool tmwxTextCtrl::SameValue(const char* c)
{
  wxString str = wxString::FromAscii(c);
  return GetValue().IsSameAs(str);
}


/*****
Return true if the integer in the box is exactly the same as the value i.
*****/
bool tmwxTextCtrl::SameValue(size_t i)
{
  wxString text = wxString::Format(wxT("%s"), tmwxStr(i).c_str());
  return (GetValue() == text);
}


/*****
Return true if the variable in the box formats to the same value as the stored
value.
*****/
bool tmwxTextCtrl::SameValue(const tmFloat& f)
{
  TMASSERT(mFloatFormat.length() != 0); // insure initialized with a float
  wxString text;
  text.Printf(mFloatFormat.c_str(), f);
  return (GetValue() == text);
}

