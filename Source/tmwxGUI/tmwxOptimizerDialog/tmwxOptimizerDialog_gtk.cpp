/*******************************************************************************
File:         tmwxOptimizerDialog_gtk.cpp
Project:      TreeMaker 5.x
Purpose:      Source file for class tmwxOptimizerDialog, specialization for GTK
Author:       Carlos A. Furuti (adapting wxGTK dialog to TM5's model
                for event handling during long computation)
Modified by:  
Created:      2004-04-22
Copyright:    ©1998 Robert Roebling (original wxGTK code, released
                under the wxWidgets license)
Copyright:    ©2004 Carlos A. Furuti. All Rights Reserved.
*******************************************************************************/

#ifdef __WXGTK__

#include <gtk/gtk.h>

#include "tmwxStr.h"
#include "tmwxOptimizerDialog.h"

extern int g_openDialogs;

void tmwxOptimizerDialog::DoStartModal() {
  /* CAF - essentially lifted from wxGTK 2.5.1's wxDialog::ShowModal, up to
     grabbing the focus. */
    if (IsModal()) {
       wxFAIL_MSG( wxT("wxDialog:ShowModal called twice") );
       mStatus = GetReturnCode();
       return;
    }

    // use the apps top level window as parent if none given unless explicitly
    // forbidden
    if (! GetParent() && !(GetWindowStyleFlag() & wxDIALOG_NO_PARENT)) {
        wxWindow *parent = wxTheApp->GetTopWindow();
        if (parent && parent != this &&
            parent -> IsBeingDeleted() &&
            ! (parent->GetExtraStyle() & wxWS_EX_TRANSIENT)) {
            m_parent = parent;
            gtk_window_set_transient_for (GTK_WINDOW(m_widget),
            GTK_WINDOW(parent->m_widget) );
        }
    }

    wxBeginBusyCursor ();
    Show (true);
    SetFocus();
    m_modalShowing = true;
    g_openDialogs++;
    gtk_grab_add (m_widget);
}

void tmwxOptimizerDialog::DoEventLoopOnce() {
  while (gtk_events_pending ())
    gtk_main_iteration ();
}

void tmwxOptimizerDialog::DoFinishModal() {
    gtk_grab_remove (m_widget);
    g_openDialogs--;
    wxEndBusyCursor ();
}

#endif  // __WXGTK__

