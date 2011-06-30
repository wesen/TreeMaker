/*******************************************************************************
File:         tmwxApp.h
Project:      TreeMaker 5.x
Purpose:      Header file for TreeMaker application class
Author:       Robert J. Lang
Modified by:  
Created:      2003-11-15
Copyright:    ©2003 Robert J. Lang. All Rights Reserved.
*******************************************************************************/

#ifndef _TMWXAPP_H_
#define _TMWXAPP_H_

// Include standard Treemaker and wxWidgets headers
#include "tmHeader.h"

// Forward declarations
#include "tmwxGUI_fwd.h"

class wxSingleInstanceChecker;
class wxDocument;
class wxBitmap;
class wxHtmlWindow;
class wxHtmlHelpController;
class wxFileName;
class wxServerBase;

// Doc architecture choice
#ifdef TM_USE_MDI
    // Windows can use top level window for parent, child windows for
    // documents. Build system is expected to bring TM_USE_MDI definition such
    // MDI is wanted. This is experimental flag, not fully supported yet.
#else
    // Mac, GTK uses top level window per view.
#endif


// Default border for static boxes
#ifdef __WXMSW__
    #define TMWX_STATIC_BOX_BORDER 0
#else
    #define TMWX_STATIC_BOX_BORDER wxSIMPLE_BORDER
#endif


// Application-wide modal dialog style
#define TMWX_MODAL_DIALOG_STYLE wxCAPTION


/**********
Standard alerts
**********/
int tmwxAlertError(const wxString& msg, const wxString& caption =
  wxT("Error"), wxWindow* parent = NULL);
int tmwxAlertInfo(const wxString& msg, const wxString& caption =
  wxT("Note"), wxWindow* parent = NULL);
int tmwxAlertQuery(const wxString& msg, const wxString& caption =
  wxT("Question"), wxWindow* parent = NULL);


/**********
TreeMaker menu commands
**********/
enum {  
  // Apple menu

  // File menu
  // wxID_NEW,
  // wxID_OPEN,
  // wxID_CLOSE,
  // wxID_SAVE,
  // wxID_SAVEAS,
  // wxID_REVERT,
  tmwxID_EXPORT_V4 = 1000,
  // wxID_PRINT,
  // wxID_PRINT_SETUP,
  // wxID_PREVIEW,
  // wxID_PREFS,
  // wxID_EXIT

  // Edit menu
  // wxID_UNDO,
  // wxID_REDO,
  // wxID_CUT,
  // wxID_COPY,
  // wxID_PASTE,
  // wxID_CLEAR,

  // wxID_SELECTALL,
  tmwxID_SELECT_NONE,
  tmwxID_SELECT_PART_BY_INDEX,
  tmwxID_SELECT_MOVABLE_PARTS,
  tmwxID_SELECT_PATH_FROM_NODES,
  tmwxID_SELECT_CORRIDOR_FACETS,
  
  tmwxID_MAKE_NODE_ROOT,
  tmwxID_PERTURB_SELECTED_NODES,
  tmwxID_PERTURB_ALL_NODES,
  
  tmwxID_SET_EDGE_LENGTH,
  tmwxID_SCALE_EDGE_LENGTH,
  tmwxID_RENORMALIZE_TO_EDGE,
  tmwxID_RENORMALIZE_TO_UNIT_SCALE,

  tmwxID_ABSORB_SELECTED_NODES,
  tmwxID_ABSORB_REDUNDANT_NODES,
  tmwxID_ABSORB_SELECTED_EDGES,

  tmwxID_SPLIT_SELECTED_EDGE,

  tmwxID_REMOVE_SELECTION_STRAIN,
  tmwxID_REMOVE_ALL_STRAIN,
  tmwxID_RELIEVE_SELECTION_STRAIN,
  tmwxID_RELIEVE_ALL_STRAIN,

  tmwxID_PICK_STUB_NODES,
  tmwxID_PICK_STUB_POLY,
  tmwxID_ADD_LARGEST_STUB_NODES,
  tmwxID_ADD_LARGEST_STUB_POLY,
  tmwxID_TRIANGULATE_TREE,
  
  // View menu
  tmwxID_TOGGLE_INSPECTOR,
  tmwxID_TOGGLE_VIEWSETTINGS,
  tmwxID_TOGGLE_FOLDEDFORM,
  tmwxID_DEFAULT_VIEW,
  tmwxID_TREE_VIEW,
  tmwxID_CREASES_VIEW,
  tmwxID_PLAN_VIEW,
  tmwxID_FIT_TO_SCREEN,
  tmwxID_FIT_TO_WIDTH,
  tmwxID_FIT_TO_HEIGHT,
  tmwxID_SET_PAPER_SIZE,
  
  // Action menu
  tmwxID_SCALE_EVERYTHING,
  tmwxID_SCALE_SELECTION,
  tmwxID_MINIMIZE_STRAIN,
  tmwxID_BUILD_CREASE_PATTERN,
  tmwxID_KILL_CREASE_PATTERN,
  
  // Conditions menu
  tmwxID_NODE_FIXED_TO_SYMMETRY_LINE,
  tmwxID_NODE_FIXED_TO_PAPER_EDGE,
  tmwxID_NODE_FIXED_TO_CORNER,
  tmwxID_NODE_FIXED_TO_POSITION,
#ifdef TMDEBUG
  tmwxID_NODE_FIXED_TO_SYMMETRY_LINE_V4,
  tmwxID_NODE_FIXED_TO_PAPER_EDGE_V4,
  tmwxID_NODE_FIXED_TO_CORNER_V4,
  tmwxID_NODE_FIXED_TO_POSITION_V4,
#endif // TMDEBUG
  tmwxID_NODES_PAIRED_ABOUT_SYMMETRY_LINE,
  tmwxID_NODES_COLLINEAR,
  tmwxID_REMOVE_NODE_CONDITIONS,
  tmwxID_EDGE_LENGTH_FIXED,
  tmwxID_EDGES_SAME_STRAIN,
  tmwxID_REMOVE_EDGE_CONDITIONS,
  tmwxID_PATH_ACTIVE,
  tmwxID_PATH_ANGLE_FIXED,
  tmwxID_PATH_ANGLE_QUANTIZED,
#ifdef TMDEBUG
  tmwxID_PATH_ACTIVE_V4,
  tmwxID_PATH_ANGLE_FIXED_V4,
  tmwxID_PATH_ANGLE_QUANTIZED_V4,
#endif // TMDEBUG
  tmwxID_REMOVE_PATH_CONDITIONS,
  tmwxID_REMOVE_ALL_CONDITIONS
  
  // Debug menu
#ifdef TMDEBUG
  ,
  tmwxID_TOGGLE_LOG,
  tmwxID_TOGGLE_QUIT_CLEANUP_EARLY,
  tmwxID_APP_DEBUG_ACTION_1,
  tmwxID_MAKE_TREE_BLANK,
  tmwxID_MAKE_TREE_UNOPTIMIZED,
  tmwxID_MAKE_TREE_OPTIMIZED,
  tmwxID_MAKE_TREE_GUSSET,
  tmwxID_MAKE_TREE_CONDITIONED
  
  // Help menu
  // wxID_ABOUT
  // wxID_HELP
#endif // TMDEBUG
};


/*****
Global variables
*****/
extern tmwxApp* gApp;
extern tmwxDocManager* gDocManager;
extern tmwxInspectorFrame* gInspectorFrame;
extern tmwxViewSettingsFrame* gViewSettingsFrame;
extern tmwxFoldedFormFrame* gFoldedFormFrame;
#ifdef TM_USE_LOGFRAME
  extern tmwxLogFrame* gLogFrame;
#endif // TM_USE_LOGFRAME
#if wxUSE_ACCEL
  wxAcceleratorTable tmwxGetAcceleratorTable();
#endif // wxUSE_ACCEL


/**********
class tmwxApp
The TreeMaker application
**********/
class tmwxApp: public wxApp {
public:
  tmwxApp();
  virtual bool OnInit();
  virtual int OnExit();
  bool ShowSplashScreen();
  void ShowOptionalAbout();
  void OnInitCmdLine(wxCmdLineParser& parser);
  bool OnCmdLineParsed(wxCmdLineParser& parser);
  bool IsGoodDataFile(wxFileName& datafile, const wxString& descr) const;

  static wxMenuBar* MakeMenuBar(wxFrame* aFrame = 0, wxDocument* aDoc = 0);
  static void SetPalettes(tmwxDoc* doc = NULL);
  bool IsQuitting() const {return mIsQuitting;};
  wxPrintData& GetPrintData();
  wxPageSetupData& GetPageSetupData();
  
#if defined(__LINUX__) || defined(__WXMSW__)
  wxIcon GetAppIcon() const { return mConfig.mAppIcon; }
  wxIcon GetDocIcon() const { return mConfig.mDocIcon; }
#endif // defined(__LINUX__) || defined(__WXMSW__)

  virtual void SetActive(bool isActive, wxWindow *lastFocus);

  // Event handling
  bool ProcessEvent(wxEvent& event);
  
  // File menu
  void OnPrintSetup(wxCommandEvent& event); 
  void OnPreferences(wxCommandEvent& event);
  void OnQuit(wxCommandEvent& event);
  
  // View menu
  void OnToggleInspectorUpdateUI(wxUpdateUIEvent& event);
  void OnToggleInspector(wxCommandEvent& event);
  void OnToggleFoldedFormUpdateUI(wxUpdateUIEvent& event);
  void OnToggleFoldedForm(wxCommandEvent& event);
  void OnToggleViewSettingsUpdateUI(wxUpdateUIEvent& event);
  void OnToggleViewSettings(wxCommandEvent& event);
  
  // Debug menu
  void OnToggleLogUpdateUI(wxUpdateUIEvent& event);
  void OnToggleLog(wxCommandEvent& event);
  void OnAppDebugAction1UpdateUI(wxUpdateUIEvent& event);
  void OnAppDebugAction1(wxCommandEvent& event);
  
  // Help menu
  void OnAboutUpdateUI(wxUpdateUIEvent& event);
  void OnAbout(wxCommandEvent& event);
  void OnHelpUpdateUI(wxUpdateUIEvent& event);
  void OnHelp(wxCommandEvent& event);


  void UpdateUI(wxUpdateUIEvent& event);
  void On(wxCommandEvent& event);
  
  void OnUpdateUI(wxUpdateUIEvent& event);
  void OnCommand(wxCommandEvent& event);
  void OnUpdateUIorCommand(int id, bool update, 
    wxCommandEvent* const cmd_event, wxUpdateUIEvent* const ui_event);
  DECLARE_EVENT_TABLE()
private:
  bool mIsStarting;
  bool mIsQuitting;
  wxString mDataDir;
  tmwxHtmlHelpController* mHelp;
  wxPrintData* mPrintData;
  wxPageSetupData* mPageSetupData;
  struct { // runtime configuration/parameters
    wxString mInstallDir; // if ! empty, installation directory
    wxArrayString mArgs; // copy of non-option cmdline arguments
#if defined(__LINUX__) || defined(__WXMSW__)
    wxIcon mAppIcon; // icon for application top-levels (may be ! Ok())
    wxIcon mDocIcon; // icon for document frames (may be ! Ok())
#endif // defined(__LINUX__) || defined(__WXMSW__)
  } mConfig;
#if defined(__WXMSW__)
  wxSingleInstanceChecker *mChecker; // single instance checker
  wxServerBase *mIPCserv; // single server for receiving calls from multiple instances
#endif
};

DECLARE_APP(tmwxApp)


/**********
class tmwxPrefsDialog
Dialog for setting application preferences
**********/
class tmwxPrefsDialog : public wxDialog {
public:
  tmwxPrefsDialog();
  bool TransferDataFromWindow();
private:
  wxChoice* mAlgorithm;
  wxCheckBox* mShowAboutAtStartup;
};


/**********
class tmwxAboutDialog
Dialog displaying information about the program
**********/
class tmwxAboutDialog : public wxDialog {
public:
  tmwxAboutDialog();
  bool TransferDataFromWindow();
private:
  wxHtmlWindow* mAboutBox;
  wxCheckBox* mShowAboutAtStartup;
};


#if wxUSE_DRAG_AND_DROP
#include "wx/dnd.h"
class tmwxDropTarget : public wxFileDropTarget
{
public:
  tmwxDropTarget(){}
  ~tmwxDropTarget(){}
  virtual bool OnDropFiles(wxCoord x, wxCoord y,
                           const wxArrayString& files);
};
#endif // wxUSE_DRAG_AND_DROP

#endif //_TMWXAPP_H_
