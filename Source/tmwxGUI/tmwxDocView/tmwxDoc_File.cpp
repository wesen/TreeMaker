/*******************************************************************************
File:         tmwxDoc_File.cpp
Project:      TreeMaker 5.x
Purpose:      Source file for TreeMaker document class, File menu commands
Author:       Robert J. Lang
Modified by:  
Created:      2005-12-10
Copyright:    ©2005 Robert J. Lang. All Rights Reserved.
*******************************************************************************/

#include "tmwxDoc.h"
#include "tmModel.h"
#include "tmwxStr.h"
#include "tmwxApp.h"

// Standard libraries
#include <fstream>

// Addtional wxWidgets
#include "wx/filename.h"

using namespace std;

/**********
class tmwxDoc
TreeMaker document class
**********/

/*****
Enable File->Export to version 4
*****/
void tmwxDoc::OnExportv4UpdateUI(wxUpdateUIEvent& event)
{
  event.Enable(true);
}


/*****
Perform File->Export to version 4
Useful for recording tree structures while version 5 format is in flux.
*****/
void tmwxDoc::OnExportv4(wxCommandEvent&)
{
  wxString pname;
  GetPrintableName(pname);
  wxString pname1 = pname.BeforeLast(wxT('.')); // strip extension
  if (pname1.empty()) pname1 = pname;
  pname1 += wxT("_v4.tmd5");
  wxFileDialog fileDialog(NULL, wxT("Export Tree"), wxEmptyString, pname1, 
    wxT("*.tmd5"), wxSAVE | wxOVERWRITE_PROMPT);
  int ret = fileDialog.ShowModal();
  if (ret == wxID_CANCEL) return;
  wxString fname = fileDialog.GetPath();
  ofstream fout(fname.mb_str(), ios_base::binary);
  if (!fout.is_open()) {
    tmwxAlertError(fname + wxT(" is not a valid file name"));
    return;
  }
  mTree->Exportv4(fout);
  fout.close();
#if defined(__WXMAC__)
  wxFileName(fname.fn_str()).MacSetTypeAndCreator('TEXT', 'TrMk');
#endif // __WXMAC__
}
