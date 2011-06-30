/*******************************************************************************
File:         tmwxApp.cpp
Project:      TreeMaker 5.x
Purpose:      Source file for TreeMaker application class
Author:       Robert J. Lang
Modified by:  
Created:      2003-11-15
Copyright:    ©2003 Robert J. Lang. All Rights Reserved.
*******************************************************************************/

#include "tmwxApp.h"
#include "tmwxStr.h"
#include "tmwxDocManager.h"
#include "tmwxDoc.h"
#include "tmwxView.h"
#include "tmwxDesignCanvas.h"
#include "tmwxInspectorFrame.h"
#include "tmwxViewSettingsFrame.h"
#include "tmwxFoldedFormFrame.h"
#ifdef TM_USE_LOGFRAME
  #include "tmwxLogFrame.h"
#endif // TM_USE_LOGFRAME
#include "tmwxPrintout.h"
#include "tmwxHtmlHelpController.h"
#include "tmwxHtmlHelpFrame.h"

#include "tmVersion.h"

// Additional wxWidgets headers
#include "wx/docview.h"
#include "wx/cmdproc.h"
#include "wx/stdpaths.h"
#include "wx/splash.h"
#include "wx/image.h"
#include "wx/statline.h"
#include "wx/file.h"
#include "wx/filename.h"
#include "wx/wxhtml.h"
#include "wx/fs_zip.h"
#include "wx/print.h"
#include "wx/cmdline.h"
#include "wx/snglinst.h"
#include "wx/ipc.h"
#include "wx/artprov.h"

// use GNOME print dialog & engine if available
#if wxUSE_LIBGNOMEPRINT
  #include "wx/html/forcelnk.h"
  FORCE_LINK(gnome_print)
#endif // wxUSE_LIBGNOMEPRINT


/**********
Global objects
**********/

tmwxApp* gApp = 0;
tmwxDocManager* gDocManager = 0;
tmwxInspectorFrame* gInspectorFrame = 0;
tmwxViewSettingsFrame* gViewSettingsFrame = 0;
tmwxFoldedFormFrame* gFoldedFormFrame = 0;
#ifdef TM_USE_LOGFRAME
  tmwxLogFrame* gLogFrame = 0;
#endif // TM_USE_LOGFRAME


/*****
Return the common accelerators
*****/
#if wxUSE_ACCEL
wxAcceleratorTable tmwxGetAcceleratorTable()
{
  wxAcceleratorEntry entries[3];
  entries[0].Set(wxACCEL_CTRL, (int) 'I', tmwxID_TOGGLE_INSPECTOR);
  entries[1].Set(wxACCEL_CTRL, (int) 'G', tmwxID_TOGGLE_VIEWSETTINGS);
  entries[2].Set(wxACCEL_CTRL, (int) 'F', tmwxID_TOGGLE_FOLDEDFORM);
  wxAcceleratorTable accel(3, entries);
  return accel;
}
#endif


#ifdef __MWERKS__
  #pragma mark -
#endif


/**********
Standard alerts
**********/

/*****
Alert the user to an error condition
*****/
int tmwxAlertError(const wxString& msg, const wxString& caption, 
  wxWindow* parent)
{
  return wxMessageBox(msg, caption, wxOK | wxICON_ERROR, parent);
}


/*****
Provide some explanation to the user of what just happened.
*****/
int tmwxAlertInfo(const wxString& msg, const wxString& caption, 
  wxWindow* parent)
{
  return wxMessageBox(msg, caption, wxOK | wxICON_INFORMATION, parent);
}


/*****
Query the user to reply yes or no, which will be the return value of the
function (wxYES | wxNO).
*****/
int tmwxAlertQuery(const wxString& msg, const wxString& caption, 
  wxWindow* parent)
{
  return wxMessageBox(msg, caption, wxYES_NO | wxICON_QUESTION, parent);
}


#ifdef __MWERKS__
  #pragma mark -
#endif


/**********
class tmwxApp
The TreeMaker application
**********/

/*****
Preference strings for wxConfig
*****/
const wxString SHOW_ABOUT_AT_STARTUP_KEY = wxT("ShowAboutAtStartup");
const wxString ALGORITHM_KEY = wxT("Algorithm");


/*****
Constructor
*****/
tmwxApp::tmwxApp() :
  wxApp(),
  mIsStarting(true),
  mIsQuitting(false),
  mDataDir(wxEmptyString),
  mHelp(NULL),
  mPrintData(NULL),
  mPageSetupData(NULL)
#ifdef __WXMSW__
  ,
  mChecker(NULL),
  mIPCserv(NULL)
#endif // __WXMSW__
{
  gApp = this;
}


#ifdef __WXMSW__
/**********
class tmwxIPCConnection
Connection for receiving single instance opening messages.
**********/
class tmwxIPCConnection : public wxConnection {
public:
  //! execute handler
  virtual bool OnExecute (const wxString& WXUNUSED(topic), wxChar *data,
    int WXUNUSED(size), wxIPCFormat WXUNUSED(format)) {
    wxString s;
    if (data) s << data;
    if (!s.empty()) gDocManager->CreateDocument(s, wxDOC_SILENT);
    return true;
  }
};


/**********
class tmwxIPCServer
Server for receiving single instance opening messages.
**********/
class tmwxIPCServer : public wxServer {
public:
  //! accept connection handler
  virtual wxConnectionBase *OnAcceptConnection (const wxString& topic) {
    if (topic != wxT("open")) return NULL;
    return new tmwxIPCConnection;
  }
};
#endif // __WXMSW__


#if wxUSE_DRAG_AND_DROP
/*****
Initialization of the application
*****/
bool tmwxDropTarget::OnDropFiles(wxCoord WXUNUSED(x), wxCoord WXUNUSED(y),
                                 const wxArrayString& files)
{
  for (size_t i = 0; i < files.GetCount(); ++i) {
    gDocManager->CreateDocument(files[i], wxDOC_SILENT);
  }
  return true;
}
#endif // wxUSE_DRAG_AND_DROP


/*****
Initialization of the application
*****/
bool tmwxApp::OnInit(void)
{
  if ( !wxApp::OnInit() ) return false;

  // GUI initialization
  SetVendorName(wxT("langorigami.com"));
  SetAppName(wxT(TM_APP_NAME_STR));

#if defined(__WXMSW__)
  wxString instanceName(wxT(TM_APP_NAME_STR));
  instanceName << wxGetUserId();
  mChecker = new wxSingleInstanceChecker(instanceName);
  if ( mChecker->IsAnotherRunning() )
  {
    wxClient client;
    wxConnectionBase *conn = client.MakeConnection(wxEmptyString, 
      instanceName, wxT("open"));
    if (conn) {
      for (size_t i = 0; i < mConfig.mArgs.GetCount(); i++) {
        wxString s(mConfig.mArgs.Item(i));
        conn->Execute(s.c_str(), (s.length() + 1) * sizeof(wxChar));
      }
      delete conn;
    }
    delete mChecker;
    return false;
  }

  // IPC server
  mIPCserv = new tmwxIPCServer();
  if (!mIPCserv->Create (instanceName)) {
    delete mIPCserv;
    mIPCserv = NULL;
  }

  // setup Windows icons
  mConfig.mAppIcon = wxICON(a);
  mConfig.mDocIcon = wxICON(d);
#endif // defined(__WXMSW__)

#if defined(__LINUX__)
  /* Set directory prefix for auxiliary data files. Gives user a
     chance to run TM even if installed differently than built. */
#ifndef INSTALL_PREFIX
#define INSTALL_PREFIX "/usr/local"
#endif
  wxString prefix;
  if (! mConfig.mInstallDir.IsEmpty ())
    prefix = mConfig.mInstallDir;
  else {
    char *p = getenv ("TREEMAKER_PREFIX");
    if (p)
      prefix = wxT (p);
    else
      prefix = wxT (INSTALL_PREFIX);
  }
  dynamic_cast<wxStandardPaths &> (wxStandardPaths::Get()).
    SetInstallPrefix(prefix);
#endif // defined(__LINUX__)

  ::wxInitAllImageHandlers();
  wxFileSystem::AddHandler(new wxZipFSHandler);
  mDataDir = wxStandardPaths::Get().GetDataDir();

#ifdef __LINUX__
  /* Load icons. Silently ignore failures (SetIcon will also silently fail) */
  mConfig.mDocIcon.LoadFile (wxT (mDataDir + "/Icon_doc_48.png"));
  mConfig.mAppIcon.LoadFile (wxT (mDataDir + "/Icon_app_48.png"));
#endif // __LINUX__
  
#ifdef TM_USE_LOGFRAME
  // Create a log window in debugging versions. We create it first so we can
  // capture any log messages generated during creation of the other windows.
  gLogFrame = new tmwxLogFrame();
  gLogFrame->Show(true);
#endif // TM_USE_LOGFRAME

#ifndef __WXMAC__
  // Non-Mac gets the splash screen early in initialization
  if (!ShowSplashScreen())
    return false;
#endif

  // Setup the HTML Help controller and load the help files. If the cache file
  // is not writeable (i.e., app is on a read-only volume or user doesn't have
  // write privileges), then we won't use the cache. But in development builds,
  // we'll let the user know what happened.
  mHelp = new tmwxHtmlHelpController();
  mHelp->UseConfig(wxConfig::Get());
  wxFileName helpFileName(mDataDir, wxT("help.zip"));
  if (IsGoodDataFile(helpFileName, wxT("help"))) {
    wxFileName helpCacheName(mDataDir, 
      wxT("help.zip_zip_treemaker.hhp.cached"));
    if (wxFile::Access(helpCacheName.GetFullPath().c_str(), wxFile::write)) {
      mHelp->SetTempDir(helpFileName.GetPath());
    }
    else {
      TMLOG("help cache file was read-only; no cache will be used.");
      mHelp->SetTempDir(wxEmptyString);
    }
    bool err = mHelp->AddBook(helpFileName);
    if (!err) {
      wxLogError(wxT("Initialization error: couldn't add help book."));
      return false;
    }
  }
  else
    return false;
    
  new wxDocTemplate(gDocManager = new tmwxDocManager(), 
    wxT("TreeMaker design"),      // short description
    wxT("*.tmd5"),                // documents to look for
    wxEmptyString,                // default directory for file selectors
    wxT("tmd5"),                  // extension to automatically append
    wxT("TreeMaker Doc"),         // document type name
    wxT("TreeMaker Design View"), // view type name
    CLASSINFO(tmwxDoc), 
    CLASSINFO(tmwxView)
    );
#if defined(__WXMAC__) || defined(__LINUX__)
  // allow files without extensions or extensions other than .tmd5; on Mac,
  // version 3.x, 4.x files have no extension
  new wxDocTemplate(gDocManager, 
    wxT("TreeMaker design"),      // short description
    wxT("*"),                     // documents to look for
    wxEmptyString,                // default directory for file selectors
    wxT("tmd5"),                  // extension to automatically append
    wxT("TreeMaker Doc"),         // document type name
    wxT("TreeMaker Design View"), // view type name
    CLASSINFO(tmwxDoc), 
    CLASSINFO(tmwxView)
    );
#endif

#ifdef __WXMAC__
  wxFileName::MacRegisterDefaultTypeAndCreator(
    "tmd5", // extension
    'TEXT', // type
    'TrM5'  // creator
    );
#endif // __WXMAC__

#ifdef __WXMAC__
  // Macs show a menu bar even when there are no document windows. 
  wxMenuBar* menuBar = MakeMenuBar();
  wxMenuBar::MacSetCommonMenuBar(menuBar);
  menuBar->MacInstallMenuBar();
#endif // __WXMAC__
  
  // Initialize the model (and the model's dynamic type system, which is needed
  // to properly initialize the inspector frame).
  tmPart::InitTypes();
  
  // Get the algorithm we're using from wxConfig and configure the model to
  // use this type of algorithm. Silently error-check the value in wxConfig.
  int algorithm;
  wxConfig::Get()->Read(ALGORITHM_KEY, &algorithm, 0);
  if (algorithm < 0) algorithm = 0;
  if (algorithm >= tmNLCO::NUM_ALGORITHMS) algorithm = 0;
  tmNLCO::SetAlgorithm(tmNLCO::Algorithm(algorithm));
  
  // Create our three floating window palettes.
  gInspectorFrame = new tmwxInspectorFrame(gDocManager);
  gFoldedFormFrame = new tmwxFoldedFormFrame();
  gViewSettingsFrame = new tmwxViewSettingsFrame();

#ifdef __WXMAC__
  // Mac gets the splash screen late; on Mac, splash screen needs to happen
  // after setting up document templates, type/creator registration, and
  // palette creation because wxYield() causes the 'odoc', 'pdoc' AppleEvents
  // to be processed and if we haven't set up the document system, they have
  // nowhere to go. Worse, if a new doc is opened, it sets the selection of the
  // inspector frame, so we better have the frame in existence. The wxYield is
  // needed for AppleEvent processing even if we don't put up the splash screen.
  if (!ShowSplashScreen())
    return false;
#endif
  
#ifndef __WXMSW__
  // Show the About box before the inspector frame shows so that the inspector
  // frame doesn't block the About box (which is a floating window for !WXMSW)
  ShowOptionalAbout();
#endif

  gInspectorFrame->Show(true);
  
  // Load any files named at the command line. On Mac, the framework already
  // grabbed filenames supplied at the command line and created the new
  // document(s) already via AppleEvents during the wxYield() of the splash
  // screen.
  for (size_t i = 0; i < mConfig.mArgs.GetCount(); i++) {
    wxString s = mConfig.mArgs.Item(i);
    wxFileName fileName(s);
    if (!wxFile::Exists(s) || !wxFile::Access(s, wxFile::read))
      tmwxAlertError(s + 
        wxT(" does not exist, is not a file or is not readable"),
        wxT(" Command Line File Error"));
    else
      gDocManager->CreateDocument(mConfig.mArgs.Item(i), wxDOC_SILENT);
  }
  if (gDocManager->GetDocuments().GetCount() < 1)
    // none found, create a new, anonymous document.
    gDocManager->CreateDocument(wxEmptyString, wxDOC_NEW);

#ifdef __WXMSW__
  // On Windows show the About box after document frame otherwise event loop of
  // modal dialogs does not work and application hungs (visible only in release
  // build, debug has logging frame which initializes event loop earlier)
  ShowOptionalAbout();
#endif

  mIsStarting = false;
  return true;
}


/*****
Termination of the application
*****/
int tmwxApp::OnExit(void)
{
#if defined(__WXMSW__)
  if (mIPCserv) delete mIPCserv;
  delete mChecker;
#endif
  return wxApp::OnExit();
}


/*****
Show the splash screen (but not in debug builds because we quickly tire of it
while debugging).
*****/
bool tmwxApp::ShowSplashScreen()
{
  wxFileName splashFile(mDataDir, wxT("SplashScreen.png"));
  bool ret = IsGoodDataFile(splashFile, wxT("splash image"));
  if (ret) {
#ifndef TMDEBUG
    const int SPLASH_DURATION = 1000;
    wxBitmap* splashScreenBitmap = 
      new wxBitmap(splashFile.GetFullPath(), wxBITMAP_TYPE_PNG);
    new wxSplashScreen(*splashScreenBitmap,
      wxSPLASH_CENTRE_ON_SCREEN | wxSPLASH_TIMEOUT,
      SPLASH_DURATION, NULL, wxID_ANY, wxDefaultPosition, wxDefaultSize,
      wxSIMPLE_BORDER | wxSTAY_ON_TOP);
#endif // TMDEBUG
  }
  else {
    tmwxAlertError(wxString::Format(
      wxT("Splash screen file \"%s\" cannot be found. Please reinstall ")
      wxT("TreeMaker 5."), splashFile.GetFullPath().c_str()),
      wxT("Startup Error"));
    return false;
  }
  // Note: this wxYield is needed on some platforms to process the splash
  // screen and on Mac to process AppleEvents generated at startup.
  wxYield();
  return ret;
}


/*****
Get our first-run setting and if appropriate, show the about dialog. Default
value is true (if there's no wxConfig, this really is the first run).
*****/
void tmwxApp::ShowOptionalAbout()
{
  int showAboutAtStartup;
  wxConfig::Get()->Read(SHOW_ABOUT_AT_STARTUP_KEY, &showAboutAtStartup, 1);
  if (showAboutAtStartup) {
    tmwxAboutDialog dialog;
    dialog.ShowModal();
  }
}


/*****
Initialize command-line switches that we accept
*****/
void tmwxApp::OnInitCmdLine(wxCmdLineParser &argParser) {
  argParser.AddSwitch (_T("v"), _T("version"), 
           _T("show program version"));
  argParser.AddSwitch (_T("h"), _T("help"), 
           _T("show option list"), wxCMD_LINE_OPTION_HELP);
  argParser.AddOption(_T("d"), _T("datadir"),
          _T("TreeMaker data directory path prefix"), 
          wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL);
  argParser.AddParam (_T("document"), wxCMD_LINE_VAL_STRING, 
          wxCMD_LINE_PARAM_OPTIONAL | wxCMD_LINE_PARAM_MULTIPLE);
}


/*****
Respond to command-line arguments after the command line has been parsed.
*****/
bool tmwxApp::OnCmdLineParsed(wxCmdLineParser& parser) {
  // check options
  wxString par;
  if (parser.Found (_T("v"))) {
    std::cout << TM_APP_BUILD_NAME_STR << "\n";
    exit (0);
  }
  if (parser.Found (_T("d"), &par))
    mConfig.mInstallDir = par;
  // parser is local to base method, so must copy non-option parameters here
  size_t nFiles = parser.GetParamCount();
  for (size_t i = 0; i < nFiles; i++)
    mConfig.mArgs.Add (parser.GetParam(i));

  return true;
}


/*****
Check whether the given data file exists. If not, put up an error message and
return false. Variable descr contains the topic area for which the data file
applies.
*****/
bool tmwxApp::IsGoodDataFile(wxFileName& datafile, const wxString& descr) const
{
  if (!datafile.IsOk() || !datafile.FileExists()) {
    tmwxAlertError(wxString::Format(wxT( \
      "Warning: the %s files could not be found. They are necessary for the "\
      "proper functioning of TreeMaker, which will now quit. You should "\
      "re-install TreeMaker from a clean copy, and read the README.txt "\
      "file for installation instructions for your system."), descr.c_str()),
      wxT("Startup Error"));
    return false;
  }
  return true;
}


#ifdef __MWERKS__
  #pragma mark -
#endif


/*
Windows and Linux use decorated menu items. Mac doesn't, per Mac HIG.
*/

#define AppendItem(menu,id,label,help,bitmap) \
  { \
    wxMenuItem *item = new wxMenuItem(menu,id,label,help); \
    item->SetBitmap(bitmap); \
    menu->Append(item); \
  }

#define AppendPlainItem(menu,id,label,help) \
  AppendItem(menu,id,label,help,wxNullBitmap)

#ifndef __WXMAC__
  #define AppendDecoratedItem(menu,id,label,help,bmpid) \
    AppendItem(menu,id,label,help, \
      wxArtProvider::GetBitmap(bmpid,wxART_MENU))
#else
  #define AppendDecoratedItem(menu,id,label,help,bmpid) \
    AppendItem(menu,id,label,help,wxNullBitmap)
#endif // __WXMAC__


/*****
STATIC
Construct the menu bar. If aDoc != NULL, include document-related menu
commands. If aFrame != NULL, attach the menu bar to the frame.
*****/
wxMenuBar* tmwxApp::MakeMenuBar(wxFrame* aFrame, wxDocument* aDoc)
{
#ifdef __WXMSW__
  #define WITH "+"
#else
  #define WITH "-"
#endif

  wxMenuBar* menuBar = new wxMenuBar;

  ////////////////////////////////////////
  // File menu
  ////////////////////////////////////////
  wxMenu *fileMenu = new wxMenu;
  AppendDecoratedItem(fileMenu,wxID_NEW, 
    wxT("&New\tCtrl" WITH "N"),
    wxT("Create a new TreeMaker design"),
    wxART_NEW);

  AppendDecoratedItem(fileMenu,wxID_OPEN,
    wxT("&Open...\tCtrl" WITH "O"),
    wxT("Open an existing TreeMaker design"),
    wxART_FILE_OPEN);

  if (aDoc) {
    AppendPlainItem(fileMenu,wxID_CLOSE, 
      wxT("&Close\tCtrl" WITH "W"),
      wxT("Close the current design"));
    AppendDecoratedItem(fileMenu,wxID_SAVE, 
      wxT("&Save\tCtrl" WITH "S"),
      wxT("Save the current design"),
      wxART_FILE_SAVE);
    AppendDecoratedItem(fileMenu,wxID_SAVEAS, 
      wxT("Save &As...\tCtrl" WITH "Alt" WITH "S"),
      wxT("Save the current design under a new name"),
      wxART_FILE_SAVE_AS);   
    AppendPlainItem(fileMenu,wxID_REVERT, 
      wxT("Revert"),
      wxT("Revert to the saved version of the file"));   
    fileMenu->AppendSeparator();

#ifdef TMDEBUG
    AppendPlainItem(fileMenu,tmwxID_EXPORT_V4, 
      wxT("Export version 4..."),
      wxT("Export the tree in version 4 format"));   
    fileMenu->AppendSeparator();
#endif // TMDEBUG
  }

  AppendDecoratedItem(fileMenu,wxID_PRINT, 
    wxT("&Print...\tCtrl" WITH "P"),
    wxT("Print the current design"),
    wxART_PRINT);
  AppendPlainItem(fileMenu,wxID_PRINT_SETUP, 
    wxT("Print &Setup..."),
    wxT("Set up the page for printing"));
  AppendPlainItem(fileMenu,wxID_PREVIEW, 
    wxT("Print Pre&view...\tCtrl" WITH "Alt" WITH "P"),
    wxT("Preview the print image before printing"));

  // On Mac, Prefs item gets moved to application menu
  AppendPlainItem(fileMenu,wxID_PREFERENCES, 
    wxT("Preferences..."),
    wxT("Edit the application preferences"));
  AppendDecoratedItem(fileMenu,wxID_EXIT, 
    wxT("E&xit\tAlt" WITH "X"), 
    wxT("Quit this program"),
    wxART_QUIT);
  menuBar->Append(fileMenu, wxT("&File"));
  
  // A history of files visited. Use this menu.
  TMASSERT(gDocManager);
#if 0 
  // turn off while DEBUGGING
  gDocManager->FileHistoryUseMenu(fileMenu);
  gDocManager->FileHistoryAddFilesToMenu(fileMenu);
#endif
  
  ////////////////////////////////////////
  // Edit menu
  ////////////////////////////////////////
  if (aDoc) {
    wxMenu *editMenu = new wxMenu;
    AppendDecoratedItem(editMenu,wxID_UNDO, 
      wxT("&Undo\tCtrl" WITH "Z"),
      wxT("Undo the last action"),
      wxART_UNDO);
    AppendDecoratedItem(editMenu,wxID_REDO, 
      wxT("&Redo\tCtrl" WITH "Y"),
      wxT("Redo the last undone action"),
      wxART_REDO);
    editMenu->AppendSeparator();
    
    AppendDecoratedItem(editMenu,wxID_CUT, 
      wxT("Cut\tCtrl" WITH "X"),
      wxT("Cut the selected items and put them on the Clipboard"),
      wxART_CUT);
    AppendDecoratedItem(editMenu,wxID_COPY, 
      wxT("Copy\tCtrl" WITH "C"),
      wxT("Copy the selected items to the Clipboard"),
      wxART_COPY);
    AppendDecoratedItem(editMenu,wxID_PASTE, 
      wxT("Paste\tCtrl" WITH "V"),
      wxT("Paste the contents of the Clipboard into the selection"),
      wxART_PASTE);
    AppendDecoratedItem(editMenu,wxID_CLEAR, 
      wxT("Clear"),
      wxT("Delete the selected items"),
      wxART_DELETE);
    editMenu->AppendSeparator();
    
    wxMenu* selectMenu = new wxMenu;
    selectMenu->Append(tmwxID_SELECT_PART_BY_INDEX, 
      wxT("Part by Index...\tCtrl" WITH "J"),
      wxT("Select a part by specifying its index"));
    selectMenu->Append(wxID_SELECTALL, 
      wxT("All\tCtrl" WITH "A"),
      wxT("Select all nodes and edges"));
    selectMenu->Append(tmwxID_SELECT_NONE, 
      wxT("None\tCtrl" WITH "Shift" WITH "A"),
      wxT("Clear the selection"));
    selectMenu->Append(tmwxID_SELECT_MOVABLE_PARTS, 
      wxT("Movable Parts\tCtrl" WITH "Alt" WITH "A"),
      wxT("Select all nodes and edges that are not pinned"));
    selectMenu->Append(tmwxID_SELECT_PATH_FROM_NODES, 
      wxT("Path from Nodes"),
      wxT("Select the path that connects two nodes"));
    selectMenu->Append(tmwxID_SELECT_CORRIDOR_FACETS, 
      wxT("Corridor Facets"),
      wxT("Select facets in corridors corresponding to the selected edges"));
    editMenu->Append(wxID_ANY, wxT("Select"), selectMenu, 
      wxT("Select parts by various criteria"));
    
    wxMenu* nodesMenu = new wxMenu;
    nodesMenu->Append(tmwxID_MAKE_NODE_ROOT, 
      wxT("Make Root"),
      wxT("Make the selected node the root node (top node of folded form)"));
    nodesMenu->Append(tmwxID_PERTURB_SELECTED_NODES, 
      wxT("Perturb Selected"),
      wxT("Perturb the position of the selected nodes"));
    nodesMenu->Append(tmwxID_PERTURB_ALL_NODES, 
      wxT("Perturb All"),
      wxT("Perturb the position of all nodes in the tree"));
    editMenu->Append(wxID_ANY, wxT("Nodes"), nodesMenu,
      wxT("Change the positions or indices of nodes"));

    wxMenu* edgesMenu = new wxMenu;
    edgesMenu->Append(tmwxID_SET_EDGE_LENGTH, 
      wxT("Set Lengths...\tCtrl" WITH "E"),
      wxT("Set the length of the selected edges to a value"));
    edgesMenu->Append(tmwxID_SCALE_EDGE_LENGTH, 
      wxT("Scale Lengths...\tCtrl" WITH "Alt" WITH "E"),
      wxT("Scale the length of the selected edges by a factor"));
    edgesMenu->Append(tmwxID_RENORMALIZE_TO_EDGE, 
      wxT("Renormalize to Selection"),
      wxT("Renormalize edge lengths and scale so this edge has unit length"));
    edgesMenu->Append(tmwxID_RENORMALIZE_TO_UNIT_SCALE, 
      wxT("Renormalize All to Unit Scale"),
      wxT("Renormalize the edge lengths so that the scale is 1.0"));
    editMenu->Append(wxID_ANY, wxT("Edges"), edgesMenu,
      wxT("Edit the lengths of edges in various ways"));
      
    wxMenu* absorbMenu = new wxMenu;
    absorbMenu->Append(tmwxID_ABSORB_SELECTED_NODES, 
      wxT("Selected Nodes"),
      wxT("Absorb the selected node(s), merging edges on either side into ")
      wxT("a single edge"));
    absorbMenu->Append(tmwxID_ABSORB_REDUNDANT_NODES, 
      wxT("Redundant Nodes"),
      wxT("Absorb all nodes in the tree that have only 2 incident edges"));
    absorbMenu->Append(tmwxID_ABSORB_SELECTED_EDGES, 
      wxT("Selected Edges"),
      wxT("Absorb the selected edges, merging nodes on either side into ")
      wxT("a single node."));
    editMenu->Append(wxID_ANY, wxT("Absorb"), absorbMenu,
      wxT("Absorb nodes or edges into the tree"));
  
    wxMenu* splitMenu = new wxMenu;
    splitMenu->Append(tmwxID_SPLIT_SELECTED_EDGE, 
      wxT("Selected Edge..."),
      wxT("Add a node in the interior of the selected edge"));
    editMenu->Append(wxID_ANY, wxT("Split"), splitMenu,
      wxT("Split an edge into two"));
    
    wxMenu* strainMenu = new wxMenu;
     strainMenu->Append(tmwxID_REMOVE_SELECTION_STRAIN, 
      wxT("Remove Selection"),
      wxT("Set the strain of the selected edges to zero"));
   strainMenu->Append(tmwxID_REMOVE_ALL_STRAIN, 
      wxT("Remove All"),
      wxT("Set all strains to zero without changing any lengths"));
    strainMenu->Append(tmwxID_RELIEVE_SELECTION_STRAIN, 
      wxT("Relieve Selection"),
      wxT("Change the lengths of the selected edges to their current ")
      wxT("strained length but with zero strain"));
    strainMenu->Append(tmwxID_RELIEVE_ALL_STRAIN, 
      wxT("Relieve All"),
      wxT("Change all lengths to equal their current strained length, ")
      wxT("but with zero strain."));
    editMenu->Append(wxID_ANY, wxT("Strain"), strainMenu,
      wxT("Edit the strain of various sets of edges"));
    
    wxMenu* stubMenu = new wxMenu;
    stubMenu->Append(tmwxID_PICK_STUB_NODES, 
      wxT("Pick for Nodes..."),
      wxT("Add a stub to the selected nodes"));
    stubMenu->Append(tmwxID_PICK_STUB_POLY, 
      wxT("Pick for Poly..."),
      wxT("Add a stub to the selected polygon"));
    stubMenu->Append(tmwxID_ADD_LARGEST_STUB_NODES, 
      wxT("Add Largest to Nodes"),
      wxT("Add the largest possible stub to the selected nodes"));
    stubMenu->Append(tmwxID_ADD_LARGEST_STUB_POLY, 
      wxT("Add Largest to Poly"),
      wxT("Add the largest possible stub to the selected polygon"));
    stubMenu->Append(tmwxID_TRIANGULATE_TREE, 
      wxT("Triangulate Tree"),
      wxT("Add stubs until all polygons are triangles"));
    editMenu->Append(wxID_ANY, wxT("Stub"), stubMenu,
      wxT("Add stub(s) to the tree, forming active paths with various nodes."));

    menuBar->Append(editMenu, wxT("&Edit"));
    aDoc->GetCommandProcessor()->SetEditMenu(editMenu);
  } 
  
  ////////////////////////////////////////
  // View menu
  ////////////////////////////////////////
  wxMenu* viewMenu = new wxMenu;
  viewMenu->Append(tmwxID_TOGGLE_INSPECTOR, 
    wxT("Inspector\tCtrl" WITH "I"),
    wxT("Show or hide the Inspector palette"));
  viewMenu->Append(tmwxID_TOGGLE_VIEWSETTINGS, 
    wxT("View Settings\tCtrl" WITH "G"),
    wxT("Show or hide the View Settings palette"));
  viewMenu->Append(tmwxID_TOGGLE_FOLDEDFORM, 
    wxT("Folded Form\tCtrl" WITH "F"),
    wxT("Show or hide the Folded Form window"));
  
  if (aDoc) {
    viewMenu->AppendSeparator();
    viewMenu->AppendCheckItem(tmwxID_TREE_VIEW, 
      wxT("Tree View\tCtrl" WITH "Shift" WITH "T"),
      wxT("Show the tree and associated rivers"));
    viewMenu->AppendCheckItem(tmwxID_DEFAULT_VIEW, 
      wxT("Design View\tCtrl" WITH "Shift" WITH "D"),
      wxT("Show the default view for editing and building a design"));
    viewMenu->AppendCheckItem(tmwxID_CREASES_VIEW, 
      wxT("Creases View\tCtrl" WITH "Shift" WITH "C"),
      wxT("Show only the crease pattern"));
    viewMenu->AppendCheckItem(tmwxID_PLAN_VIEW, 
      wxT("Plan View\tCtrl" WITH "Shift" WITH "P"),
      wxT("Show the crease pattern and coordinates of major vertices"));
    viewMenu->AppendSeparator();
  
    viewMenu->Append(tmwxID_FIT_TO_SCREEN, 
      wxT("Fit to Screen\tCtrl" WITH "Shift" WITH "F"),
      wxT("Expand the window to fit the screen"));
    viewMenu->Append(tmwxID_FIT_TO_WIDTH, 
      wxT("Fit to Width\tCtrl" WITH "Shift" WITH "W"),
      wxT("Expand the diagram to fit the width of window"));
    viewMenu->Append(tmwxID_FIT_TO_HEIGHT, 
      wxT("Fit to Height\tCtrl" WITH "Shift" WITH "H"),
      wxT("Expand the diagram to fit the height of window"));
    viewMenu->Append(tmwxID_SET_PAPER_SIZE, 
      wxT("Set Paper Size...\tCtrl" WITH "Shift" WITH "Y"),
      wxT("Specify the size of the image of the paper"));
  }
  menuBar->Append(viewMenu, wxT("&View"));
  
  ////////////////////////////////////////
  // Action menu
  ////////////////////////////////////////
  if (aDoc) {
    wxMenu* actionMenu = new wxMenu;
    actionMenu->Append(tmwxID_SCALE_EVERYTHING, 
      wxT("Scale Everything\tCtrl" WITH "1"),
      wxT("Find the largest possible base for the given paper"));
    actionMenu->Append(tmwxID_SCALE_SELECTION, 
      wxT("Scale Selection\tCtrl" WITH "2"),
      wxT("Maximize the strain of the selected edges over the selected nodes"));
    actionMenu->Append(tmwxID_MINIMIZE_STRAIN, 
      wxT("Minimize Strain\tCtrl" WITH "3"),
      wxT("Minimize the total strain under the current set of conditions"));
    actionMenu->AppendSeparator();
    
    actionMenu->Append(tmwxID_BUILD_CREASE_PATTERN, 
      wxT("Build Crease Pattern\tCtrl" WITH "4"),
      wxT("Build the crease pattern using universal molecules"));
    actionMenu->Append(tmwxID_KILL_CREASE_PATTERN, 
      wxT("Kill Crease Pattern\tCtrl" WITH "Alt" WITH "4"),
      wxT("Build the crease pattern using universal molecules"));
    menuBar->Append(actionMenu, wxT("&Action"));
  }

  ////////////////////////////////////////
  // Condition menu
  ////////////////////////////////////////
  if (aDoc) {
    wxMenu* conditionMenu = new wxMenu;    
    conditionMenu->Append(tmwxID_NODE_FIXED_TO_SYMMETRY_LINE, 
      wxT("Node(s) Fixed to Symmetry Line"),
      wxT("Add a condition fixing the node to the symmetry line ")
      wxT("(when it's enabled)"));
    conditionMenu->Append(tmwxID_NODE_FIXED_TO_PAPER_EDGE, 
      wxT("Node(s) Fixed to Paper Edge"),
      wxT("Add a condition fixing the node to any edge of the paper"));
    conditionMenu->Append(tmwxID_NODE_FIXED_TO_CORNER, 
      wxT("Node(s) Fixed to Corner"),
      wxT("Add a condition fixing the node to any corner of the paper"));
    conditionMenu->Append(tmwxID_NODE_FIXED_TO_POSITION, 
      wxT("Node(s) Fixed to Position..."),
      wxT("Add a condition fixing the node's x and/or y coordinate"));
  
#ifdef TMDEBUG
    // Debug versions support version 4 - style conditions, but in release
    // versions we want users to use tmConditionNodeCombo for all of these
    // conditions.
    conditionMenu->Append(tmwxID_NODE_FIXED_TO_SYMMETRY_LINE_V4, 
      wxT("Node(s) Fixed to Symmetry Line (v4)"),
      wxT("Add a version 4condition fixing the node to the symmetry line ")
      wxT("(when it's enabled)"));
    conditionMenu->Append(tmwxID_NODE_FIXED_TO_PAPER_EDGE_V4, 
      wxT("Node(s) Fixed to Paper Edge (v4)"),
      wxT("Add a version 4 condition fixing the node to any edge of the paper"));
    conditionMenu->Append(tmwxID_NODE_FIXED_TO_CORNER_V4, 
      wxT("Node(s) Fixed to Corner (v4)"),
      wxT("Add a version 4 condition fixing the node to any corner of the paper"));
    conditionMenu->Append(tmwxID_NODE_FIXED_TO_POSITION_V4, 
      wxT("Node(s) Fixed to Position (v4)..."),
      wxT("Add a version 4 condition fixing the node's x and/or y coordinate"));
#endif // TMDEBUG
  
    conditionMenu->Append(tmwxID_NODES_PAIRED_ABOUT_SYMMETRY_LINE, 
      wxT("2 Nodes Paired about Symmetry Line"),
      wxT("Add a condition causing these two nodes to be mirror images ")
      wxT("of each other"));
    conditionMenu->Append(tmwxID_NODES_COLLINEAR, 
      wxT("3 Nodes Collinear"),
      wxT("Add a condition forcing these three nodes to lie on a single line"));
    conditionMenu->Append(tmwxID_REMOVE_NODE_CONDITIONS, 
      wxT("Remove All Node Conditions"),
      wxT("Delete all conditions that affect node position"));
    conditionMenu->AppendSeparator();
    
    conditionMenu->Append(tmwxID_EDGE_LENGTH_FIXED, 
      wxT("Edge(s) No Strain"),
      wxT("Add a condition that forces this edge to have zero strain"));
    conditionMenu->Append(tmwxID_EDGES_SAME_STRAIN, 
      wxT("2 Edges Same Strain"),
      wxT("Add a condition that forces these two edges to have ")
      wxT("the same strain"));
    conditionMenu->Append(tmwxID_REMOVE_EDGE_CONDITIONS, 
      wxT("Remove All Edge Conditions"),
      wxT("Remove all conditions that affect edge strain"));
    conditionMenu->AppendSeparator();
    
    conditionMenu->Append(tmwxID_PATH_ACTIVE, 
      wxT("Path Active"),
      wxT("Add a condition that forces this path to be active"));
    conditionMenu->Append(tmwxID_PATH_ANGLE_FIXED, 
      wxT("Path Angle Fixed..."),
      wxT("Add a condition that forces this path to be active and ")
      wxT("fixes its angle to a constant value"));
    conditionMenu->Append(tmwxID_PATH_ANGLE_QUANTIZED, 
      wxT("Path Angle Quantized..."),
      wxT("Add a condition that forces this path to be active and ")
      wxT("forces its angle to be quantized."));
      
#ifdef TMDEBUG
    // Debug versions support version 4 - style conditions, but in release
    // versions we want users to use tmConditionPathCombo for all of these
    // conditions.
    conditionMenu->Append(tmwxID_PATH_ACTIVE_V4, 
      wxT("Path Active (v4)"),
      wxT("Add a version 4 condition that forces this path to be active"));
    conditionMenu->Append(tmwxID_PATH_ANGLE_FIXED_V4, 
      wxT("Path Angle Fixed (v4)..."),
      wxT("Add a version 4 condition that forces this path to be active and ")
      wxT("fixes its angle to a constant value"));
    conditionMenu->Append(tmwxID_PATH_ANGLE_QUANTIZED_V4, 
      wxT("Path Angle Quantized (v4)..."),
      wxT("Add a version 4 condition that forces this path to be active and ")
      wxT("forces its angle to be quantized."));
#endif // TMDEBUG

    conditionMenu->Append(tmwxID_REMOVE_PATH_CONDITIONS, 
      wxT("Remove All Path Conditions"),
      wxT("Remove all conditions that affect path activity"));

    conditionMenu->AppendSeparator();
    
    conditionMenu->Append(tmwxID_REMOVE_ALL_CONDITIONS, 
      wxT("Remove All Conditions"),
      wxT("Remove all node, edge, and path conditions"));
    menuBar->Append(conditionMenu, wxT("&Condition"));
  }
  
  ////////////////////////////////////////
  // Debug menu
  ////////////////////////////////////////
#ifdef TMDEBUG
  wxMenu *debugMenu = new wxMenu;
  debugMenu->Append(tmwxID_TOGGLE_LOG, 
    wxT("Log\tCtrl" WITH "L"),
    wxT("Show or hide the log window"));
  debugMenu->AppendSeparator();

  if (aDoc) {
    // This is useful in debugging tmTree::CleanupAfterEdit() & friends
    debugMenu->AppendCheckItem(tmwxID_TOGGLE_QUIT_CLEANUP_EARLY, 
      wxT("Quit Cleanup Early"),
      wxT("Quit cleanup in the model before calculating facet order"));
    debugMenu->AppendSeparator();
  }

  // Debug builds can (sometimes) execute commands used to setup exceptional
  // circumstances.
  debugMenu->Append(tmwxID_APP_DEBUG_ACTION_1, 
    wxT("App Debug Action 1"),
    wxT("Execute an application action specific to debug builds"));
  debugMenu->AppendSeparator();

  // Debug versions have the ability to create test trees
  debugMenu->Append(tmwxID_MAKE_TREE_BLANK, 
    wxT("Make Test Tree Blank"),
    wxT("Replace the current tree with a blank test structure"));
  debugMenu->Append(tmwxID_MAKE_TREE_OPTIMIZED, 
    wxT("Make Test Tree Optimized"),
    wxT("Replace the current tree with an optimized test structure"));
  debugMenu->Append(tmwxID_MAKE_TREE_UNOPTIMIZED, 
    wxT("Make Test Tree Unoptimized"),
    wxT("Replace the current tree with an unoptimized test structure"));
  debugMenu->Append(tmwxID_MAKE_TREE_GUSSET, 
    wxT("Make Test Tree Gusset"),
    wxT("Replace the current tree with a test structure for the ")
    wxT("gusset molecule"));
  debugMenu->Append(tmwxID_MAKE_TREE_CONDITIONED, 
    wxT("Make Test Tree Conditioned"),
    wxT("Replace the current tree with a test structure that contains ")
    wxT("many conditions."));
  menuBar->Append(debugMenu, wxT("&Debug"));
#endif // TMDEBUG  
  
  ////////////////////////////////////////
  // Help menu
  ////////////////////////////////////////
  wxMenu *helpMenu = new wxMenu;
  helpMenu->Append(wxID_HELP, 
#ifdef __WXMSW__
    wxT("TreeMaker &Help\tF1"),
#else
    wxT("TreeMaker &Help\tCtrl" WITH "?"),
#endif
    wxT("Show the Help browser"));
#ifdef __WXMSW__
  helpMenu->AppendSeparator();
#endif
  // On Mac, About item gets moved to application menu
  helpMenu->Append(wxID_ABOUT, 
    wxT("&About TreeMaker..."),
    wxT("Show information about TreeMaker, its version, and who built it"));
  menuBar->Append(helpMenu, wxT("&Help"));

#undef WITH

  if (aFrame)
    aFrame->SetMenuBar(menuBar);
  return menuBar;
}


#ifdef __MWERKS__
  #pragma mark -
#endif


/*****
Set the palettes to display this document, or none if NULL.
*****/
void tmwxApp::SetPalettes(tmwxDoc* doc)
{
  gDocManager->SetCurrentDocumentLocal(doc);
  if (doc) {
    gInspectorFrame->DispatchSetSelection(doc->mTree, doc->mSelection);
    gViewSettingsFrame->
      SetSelection(&doc->GetDesignCanvas()->GetEditableViewSettings());
  }
  else {
    gInspectorFrame->SetSelection();
    gViewSettingsFrame->SetSelection();
  }
  gFoldedFormFrame->SetDoc(doc);
  gInspectorFrame->SetOnTopStyle(doc!=NULL);
  gViewSettingsFrame->SetOnTopStyle(doc!=NULL);
}


/*****
Return the current print data settings
*****/
wxPrintData& tmwxApp::GetPrintData()
{
  if (!mPrintData)
    mPrintData = new wxPrintData();
  return *mPrintData;
}


/*****
Return the current page setup settings
*****/
wxPageSetupData& tmwxApp::GetPageSetupData()
{
  if (!mPageSetupData)
    mPageSetupData = new wxPageSetupData();
  return *mPageSetupData;
}


#ifdef __MWERKS__
  #pragma mark -
#endif


/*
Because of some weirdness in wxWidgets' event handling, it is possible for
events to come directly to the app that are meant to be handled by the doc
manager or the help frame. To deal with this, we let the help frame or the doc
manager have first crack at processing any events that make it to the app. But,
as part of its own event processing, both objects will pass the event back to
the app! So to prevent an endless recursion, we maintain our own stack of
events that we've been called for. If we've already been called once for an
event, we process it normally; if we haven't, we let the doc manager have a
crack at it. 
*/

/*****
OVERRIDE
Process a single event that has (somehow) trickled up to the application.
*****/
bool tmwxApp::ProcessEvent(wxEvent& event)
{
  // First things first: if we're in startup or shutdown, or it's not a command
  // event, or we've already seen this event, then let the superclass handle 
  // it normally.
  static tmArray<wxEvent*> eventStack;
  if (mIsStarting || mIsQuitting || !event.IsCommandEvent() || 
    eventStack.contains(&event))
    return wxApp::ProcessEvent(event);
 
  // If help frame is the top window, give it a shot at processing any command
  // event before passing it on to superclass.
  wxWindow* topWindow = wxTheApp->GetTopWindow();
  if (mHelp && topWindow == (wxWindow*)(mHelp->mHtmlHelpFrame)) {
    eventStack.push_back(&event);
    if (topWindow && topWindow->ProcessEvent(event)) {
      eventStack.pop_back();
      return true;
    }
    eventStack.pop_back();
    return wxApp::ProcessEvent(event);
  }
  
  // Then give the document manager a shot at processing command events.
  eventStack.push_back(&event);
  if (gDocManager && gDocManager->ProcessEvent(event)) {
    eventStack.pop_back();
    return true;
  }
  eventStack.pop_back();
  return wxApp::ProcessEvent(event);
}


/*****
Special actions needed on application activation
*****/
void tmwxApp::SetActive(bool isActive, wxWindow *lastFocus)
{
  wxApp::SetActive(isActive, lastFocus);
  if(gInspectorFrame)gInspectorFrame->SetOnTopStyle(isActive);
  if(gViewSettingsFrame)gViewSettingsFrame->SetOnTopStyle(isActive);
}

#ifdef __MWERKS__
  #pragma mark -
#endif


/*****
Put up a dialog for setting printer parameters.
*****/
void tmwxApp::OnPrintSetup(wxCommandEvent& WXUNUSED(event))
{
  wxPageSetupDialog pageSetupDialog(
#ifdef __WXMAC__
    NULL, 
#else
    GetTopWindow(),
#endif // __WXMAC__
    &GetPageSetupData());
  pageSetupDialog.ShowModal();
  GetPrintData() = pageSetupDialog.GetPageSetupData().GetPrintData();
  GetPageSetupData() = pageSetupDialog.GetPageSetupData();
}


/*****
Handle the Preferences command.
*****/
void tmwxApp::OnPreferences(wxCommandEvent& WXUNUSED(event))
{
  tmwxPrefsDialog dialog;
  dialog.ShowModal();
}


/*****
Quit the application. Kill the Help window, then let the application quit
normally.
*****/
void tmwxApp::OnQuit(wxCommandEvent&)
{
    // mIsQuitting is set only in this command. It lets us know to ignore
    // update events that refer to windows that have already been destroyed.
    mIsQuitting = true;

  // Attempt to close the inspector frame. Since it is the wxDocParentFrame,
  // it will try to close all open documents and if successful, will exit
  // the application. But the user can veto if he/she desires.
  if (gInspectorFrame->Close()) {
    mHelp->Quit();
    gViewSettingsFrame->Destroy();
    gViewSettingsFrame = 0;
    gFoldedFormFrame->Destroy();
    gFoldedFormFrame = 0;
#ifdef TM_USE_LOGFRAME
    gLogFrame->Destroy();
    gLogFrame = 0;
#endif // TM_USE_LOGFRAME
    if (mPrintData) delete mPrintData;
    if (mPageSetupData) delete mPageSetupData;
  }
  else {
    // User aborted the closing of one or more windows, so we're not quitting
    // after all.
    mIsQuitting = false;
  }
}


#ifdef __MWERKS__
  #pragma mark -
#endif


#ifdef __WXMSW__
  #define WITH "+"
#else
  #define WITH "-"
#endif


/*****
Enable visibility of the Inspector window
*****/
void tmwxApp::OnToggleInspectorUpdateUI(wxUpdateUIEvent& event)
{
  event.Enable(true);
  if (gInspectorFrame && gInspectorFrame->IsShown())
    event.SetText(wxT("Hide Inspector\tCtrl" WITH "I"));
  else
    event.SetText(wxT("Show Inspector\tCtrl" WITH "I"));
}


/*****
Toggle visibility of the Inspector window
*****/
void tmwxApp::OnToggleInspector(wxCommandEvent&)
{
  if (!gInspectorFrame) return;
  gInspectorFrame->Show(!gInspectorFrame->IsShown());
  tmwxDoc* theDoc = gDocManager->GetCurrentDocumentLocal();
  if (theDoc) theDoc->GetDesignCanvas()->SetFocus();
}


/*****
Enable visibility of the View Settings window
*****/
void tmwxApp::OnToggleViewSettingsUpdateUI(wxUpdateUIEvent& event)
{
  event.Enable(true);
  if (gViewSettingsFrame && gViewSettingsFrame->IsShown())
    event.SetText(wxT("Hide View Settings\tCtrl" WITH "G"));
  else
    event.SetText(wxT("Show View Settings\tCtrl" WITH "G"));
}


/*****
Toggle visibility of the View Settings window
*****/
void tmwxApp::OnToggleViewSettings(wxCommandEvent&)
{
  if (!gViewSettingsFrame) return;
  gViewSettingsFrame->Show(!gViewSettingsFrame->IsShown());
  tmwxDoc* theDoc = gDocManager->GetCurrentDocumentLocal();
  if (theDoc) theDoc->GetDesignCanvas()->SetFocus();
}


/*****
Enable visibility of the Folded Form window
*****/
void tmwxApp::OnToggleFoldedFormUpdateUI(wxUpdateUIEvent& event)
{
  event.Enable(true);
  if (gFoldedFormFrame && gFoldedFormFrame->IsShown())
    event.SetText(wxT("Hide Folded Form\tCtrl" WITH "F"));
  else
    event.SetText(wxT("Show Folded Form\tCtrl" WITH "F"));
}


/*****
Toggle visibility of the Folded Form window
*****/
void tmwxApp::OnToggleFoldedForm(wxCommandEvent&)
{
  if (!gFoldedFormFrame) return;
  gFoldedFormFrame->Show(!gFoldedFormFrame->IsShown());
  tmwxDoc* theDoc = gDocManager->GetCurrentDocumentLocal();
  if (theDoc) theDoc->GetDesignCanvas()->SetFocus();
#ifdef __WXMSW__
  // on MSW we have to manually bring it to top hierarchy because system puts
  // it behind all other windows
  if(gFoldedFormFrame->IsShown())
    gFoldedFormFrame->Raise();
#endif
}


#undef WITH

#ifdef __MWERKS__
  #pragma mark -
#endif


#ifdef TMDEBUG
/*****
Enable visibility of the log window
*****/
void tmwxApp::OnToggleLogUpdateUI(wxUpdateUIEvent& event)
{
  event.Enable(true);
  if (gLogFrame && gLogFrame->IsShown())
    event.SetText(wxT("Hide Log"));
  else
    event.SetText(wxT("Show Log"));
}


/*****
Toggle visibility of the log window
*****/
void tmwxApp::OnToggleLog(wxCommandEvent&)
{
  if (!gLogFrame) return;
  gLogFrame->Show(!gLogFrame->IsShown());
}


/*****
Enable a a debug-specific action
*****/
void tmwxApp::OnAppDebugAction1UpdateUI(wxUpdateUIEvent& event)
{
  event.Enable(true);
}


/*****
Perform a debug-specific action
*****/
void tmwxApp::OnAppDebugAction1(wxCommandEvent&)
{
//   tmwxDesignCanvas* canvas = gDocManager->GetCurrentViewLocal()->mDesignCanvas;
//   canvas->IsFitPaperSize();
}
#endif // TMDEBUG


#ifdef __MWERKS__
  #pragma mark -
#endif


/*****
Put up an About box, which uses the custom tmwxAboutDialog dialog.
*****/
void tmwxApp::OnAbout(wxCommandEvent& WXUNUSED(event))
{
  tmwxAboutDialog dialog;
  dialog.ShowModal();
  return;
}


/*****
Open up the HTML Help window.
*****/
void tmwxApp::OnHelp(wxCommandEvent& WXUNUSED(event))
{
  mHelp->DisplayContents();
  mHelp->mHtmlHelpFrame->Raise();
}


#ifdef __MWERKS__
  #pragma mark -
#endif


/*****
Prefs dialog constructor
*****/
tmwxPrefsDialog::tmwxPrefsDialog()
  : wxDialog(NULL, wxID_ANY, wxT("Preferences"), wxDefaultPosition, 
    wxDefaultSize, TMWX_MODAL_DIALOG_STYLE)
{
  // Sizer to hold everything
  wxBoxSizer* topSizer = new wxBoxSizer(wxVERTICAL);

  // Build a list of all of the available optimizers by name.
  wxString choices[tmNLCO::NUM_ALGORITHMS];
#ifdef tmUSE_ALM
  choices[tmNLCO::ALM] = wxT("ALM (slowest but most robust)");
#endif
#ifdef tmUSE_WNLIB
  choices[tmNLCO::WNLIB] = wxT("WNLIB (medium speed, medium robustness)");
#endif
#ifdef tmUSE_CFSQP
  choices[tmNLCO::CFSQP] = wxT("CFSQP (fast, occasionally fails to converge)");
#endif
#ifdef tmUSE_RFSQP
  choices[tmNLCO::RFSQP] = wxT("RFSQP (fastest, sometimes crashes)");
#endif

  // Popup menu giving the algorithm choices
  wxStaticBox* algorithmBox = new wxStaticBox(this, wxID_ANY, 
    wxT("Optimization Algorithm"), wxDefaultPosition, wxDefaultSize, 
    TMWX_STATIC_BOX_BORDER);
  wxStaticBoxSizer* algorithmSizer = new wxStaticBoxSizer(algorithmBox,
    wxVERTICAL);
  mAlgorithm = new wxChoice(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 
    int(tmNLCO::NUM_ALGORITHMS), choices);
  mAlgorithm->SetSelection(tmNLCO::GetAlgorithm());
  algorithmSizer->Add(mAlgorithm);
  topSizer->Add(algorithmSizer);
  
  //Checkbox letting user choose to display the first-run dialog
  mShowAboutAtStartup = new wxCheckBox(this, wxID_ANY, 
    wxT("Show \"About...\" window at startup"));
  int showAboutAtStartup;
  wxConfig::Get()->Read(SHOW_ABOUT_AT_STARTUP_KEY, &showAboutAtStartup, 1);
  mShowAboutAtStartup->SetValue(showAboutAtStartup);
  topSizer->Add(mShowAboutAtStartup, 0, wxCENTER | wxTOP, 5);
  
  topSizer->Add(new wxStaticLine(this, wxID_ANY), 0, wxEXPAND | wxALL, 5);
  topSizer->Add(CreateButtonSizer(wxOK | wxCANCEL), 0, wxCENTER | wxALL, 5);
  SetSizerAndFit(topSizer);
  Center(wxBOTH);
}


/*****
Read the preferences from the window.
*****/
bool tmwxPrefsDialog::TransferDataFromWindow()
{
  tmNLCO::Algorithm algorithm = 
    tmNLCO::Algorithm(mAlgorithm->GetCurrentSelection());
  tmNLCO::SetAlgorithm(algorithm);
  wxConfig::Get()->Write(ALGORITHM_KEY, algorithm);
  
  int showAboutAtStartup = mShowAboutAtStartup->GetValue();
  wxConfig::Get()->Write(SHOW_ABOUT_AT_STARTUP_KEY, showAboutAtStartup);

  wxConfig::Get()->Flush();
  return true;
}


#ifdef __MWERKS__
  #pragma mark -
#endif


/**********
class tmwxHtmlWindow
Html window with clickable links used in About window
**********/
class tmwxHtmlWindow : public wxHtmlWindow {
public:
  tmwxHtmlWindow(wxWindow *parent, const wxSize& size);
  virtual void OnLinkClicked(const wxHtmlLinkInfo& link);
};


/*****
Constructor
*****/
tmwxHtmlWindow::tmwxHtmlWindow(wxWindow *parent, const wxSize& size) :
  wxHtmlWindow(parent, wxID_ANY, wxDefaultPosition, size, wxHW_SCROLLBAR_AUTO) 
{
}


/*****
OVERRIDE
Handle clicking on a link; if it's external, launch the browser; if it's
internal, use the default method.
*****/
void tmwxHtmlWindow::OnLinkClicked(const wxHtmlLinkInfo& link)
{
  wxString href = link.GetHref();
  if (href.Left(4).Lower() == wxString(wxT("http")))
    ::wxLaunchDefaultBrowser(link.GetHref());
  else
    wxHtmlWindow::OnLinkClicked(link);
}


#ifdef __MWERKS__
  #pragma mark -
#endif


/*****
About dialog constructor
*****/
tmwxAboutDialog::tmwxAboutDialog() :
  wxDialog(NULL, wxID_ANY, wxT("About TreeMaker"), wxDefaultPosition, 
    wxDefaultSize, TMWX_MODAL_DIALOG_STYLE | wxSTAY_ON_TOP)
{
  wxBoxSizer* topSizer = new wxBoxSizer(wxVERTICAL);
  
  wxStaticText* title = new wxStaticText(this, wxID_ANY, 
    wxT(TM_APP_ABOUT_NAME_STR));
  title->SetFont(wxFont(14, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, 
    wxFONTWEIGHT_BOLD));
  topSizer->Add(title, 0, wxALIGN_CENTER | wxALL, 5);

  mAboutBox = new tmwxHtmlWindow(this, wxSize(640, 450));
  topSizer->Add(mAboutBox);
  int fontSizes[7] = {9, 10, 11, 12, 14, 16, 18};
  mAboutBox->SetFonts(wxEmptyString, wxEmptyString, fontSizes);
  mAboutBox->SetBorders(5);
  wxFileName aboutFile(wxStandardPaths::Get().GetDataDir(), wxT("about.htm"));
  TMASSERT(aboutFile.FileExists());
  mAboutBox->LoadFile(aboutFile);
  
  mShowAboutAtStartup = new wxCheckBox(this, wxID_ANY, 
    wxT("Show this window at startup"));
  int showAboutAtStartup;
  wxConfig::Get()->Read(SHOW_ABOUT_AT_STARTUP_KEY, &showAboutAtStartup, 1);
  mShowAboutAtStartup->SetValue(showAboutAtStartup);
  topSizer->Add(mShowAboutAtStartup, 0, wxCENTER | wxTOP, 5);
  
  topSizer->Add(new wxStaticLine(this, wxID_ANY), 0, wxEXPAND | wxALL, 5);
  topSizer->Add(CreateButtonSizer(wxOK), 0, wxCENTER | wxALL, 5);

#if !defined(TMDEBUG) && !defined(TMPROFILE)
  wxStaticText* buildCode = new wxStaticText(this, wxID_ANY, wxT(BUILD_STR));
  buildCode->SetFont(wxFont(9, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, 
    wxFONTWEIGHT_NORMAL));
  topSizer->Add(buildCode, wxSizerFlags().Right());
#endif // !defined(TMDEBUG) && !defined(TMPROFILE)

  SetSizerAndFit(topSizer);
  Center(wxBOTH);
  return;
}


/*****
Note user choice with regard to showing the dialog at startup
*****/
bool tmwxAboutDialog::TransferDataFromWindow()
{
  int showAboutAtStartup = mShowAboutAtStartup->GetValue();
  wxConfig::Get()->Write(SHOW_ABOUT_AT_STARTUP_KEY, showAboutAtStartup);
  wxConfig::Get()->Flush();
  return true;
}


/*****
Create the application object
*****/
IMPLEMENT_APP(tmwxApp)


/*****
Event table
*****/
BEGIN_EVENT_TABLE(tmwxApp, wxApp)
  // File menu
  EVT_MENU(wxID_PRINT_SETUP, tmwxApp::OnPrintSetup)
  EVT_MENU(wxID_PREFERENCES, tmwxApp::OnPreferences)
  EVT_MENU(wxID_EXIT, tmwxApp::OnQuit)
  
  // Edit menu
  
  // View menu
  EVT_UPDATE_UI(tmwxID_TOGGLE_INSPECTOR, tmwxApp::OnToggleInspectorUpdateUI)
  EVT_MENU(tmwxID_TOGGLE_INSPECTOR, tmwxApp::OnToggleInspector) 
  EVT_UPDATE_UI(tmwxID_TOGGLE_VIEWSETTINGS, tmwxApp::OnToggleViewSettingsUpdateUI)
  EVT_MENU(tmwxID_TOGGLE_VIEWSETTINGS, tmwxApp::OnToggleViewSettings) 
  EVT_UPDATE_UI(tmwxID_TOGGLE_FOLDEDFORM, tmwxApp::OnToggleFoldedFormUpdateUI)
  EVT_MENU(tmwxID_TOGGLE_FOLDEDFORM, tmwxApp::OnToggleFoldedForm) 
  
#ifdef TMDEBUG
  // Debug menu
  EVT_UPDATE_UI(tmwxID_TOGGLE_LOG, tmwxApp::OnToggleLogUpdateUI)
  EVT_MENU(tmwxID_TOGGLE_LOG, tmwxApp::OnToggleLog) 
  EVT_UPDATE_UI(tmwxID_APP_DEBUG_ACTION_1, tmwxApp::OnAppDebugAction1UpdateUI)
  EVT_MENU(tmwxID_APP_DEBUG_ACTION_1, tmwxApp::OnAppDebugAction1) 
#endif // TMDEBUG
  
  // Help menu
  EVT_MENU(wxID_ABOUT, tmwxApp::OnAbout)
  EVT_MENU(wxID_HELP, tmwxApp::OnHelp)
  
END_EVENT_TABLE()
