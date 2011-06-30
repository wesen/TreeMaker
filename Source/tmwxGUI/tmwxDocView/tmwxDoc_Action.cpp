/*******************************************************************************
File:         tmwxDoc_Action.cpp
Project:      TreeMaker 5.x
Purpose:      Source file for TreeMaker document class, Action menu commands
Author:       Robert J. Lang
Modified by:  
Created:      2005-12-10
Copyright:    ©2005 Robert J. Lang. All Rights Reserved.
*******************************************************************************/

#include "tmwxDoc.h"
#include "tmModel.h"
#include "tmwxStr.h"
#include "tmwxOptimizerDialog.h"
#include "tmwxApp.h"

using namespace std;

/**********
class tmwxDoc
TreeMaker document class
**********/

/*****
Enable Action->Scale Everything
*****/
void tmwxDoc::OnScaleEverythingUpdateUI(wxUpdateUIEvent& event)
{
  event.Enable(mTree->GetNumLeafNodes() >= 3);
}


/*****
Perform Action->Scale Everything
*****/
void tmwxDoc::OnScaleEverything(wxCommandEvent&)
{
  if (mTree->GetNumLeafNodes() < 3) {
    tmwxAlertError(wxT("Sorry, you need at least 3 leaf nodes to "\
      "optimize the scale."));
    return;
  }
  tmNLCO* theNLCO = tmNLCO::MakeNLCO();
#ifdef tmUSE_CFSQP
  // set best algorithm for this type of optimization
  tmNLCO_cfsqp* theNLCO_cfsqp = dynamic_cast<tmNLCO_cfsqp*>(theNLCO);
  if (theNLCO_cfsqp) theNLCO_cfsqp->SetCFSQPMode(110);
#endif
  tmScaleOptimizer* theOptimizer = new tmScaleOptimizer(mTree, theNLCO);
  theOptimizer->Initialize();
  size_t ne = theNLCO->GetNumEqualities();
  size_t nv = theNLCO->GetNumVariables();
  if (ne >= nv && QueryEqnsVblsContinue(ne, nv) == wxNO) {
    delete theOptimizer;
    delete theNLCO;
    return;
  }
  // The actual calculation takes place during the modal dialog event loop
  tmwxOptimizerDialog theDialog(this, theOptimizer, 
    wxT("Maximizing scale...")); 
  theNLCO->SetUpdater(&theDialog);
  wxString cmd;
  bool reverted = false;
  int ret = theDialog.ShowModal();
  switch(ret) {
    case tmwxOptimizerDialog::NORMAL_TERMINATION: {
      ::wxBell();
      cmd = wxT("scale optimization");
      break;
    }
    case tmwxOptimizerDialog::USER_CANCELLED: {
      if (QueryUserCancelledRevert() == wxYES) {
        reverted = true;
        theOptimizer->Revert();
        break;
      }
      else {
        cmd = wxT("cancelled scale optimization");
        break;
      }
    }
    case tmwxOptimizerDialog::OTHER_TERMINATION: {
      if (QueryBadConvergenceRevert(tmNLCO::GetAlgorithm(), 
        theDialog.GetReason()) == wxYES) {
        reverted = true;
        theOptimizer->Revert();
        break;
      }
      else {
        cmd = wxT("cancelled scale optimization");
        break;
      }
    }
    default:
      TMFAIL("unknown optimizer return value in "\
        "tmwxDoc::DoScaleOptimization()");
  }
  // theOptimizer is descended from tmTreeCleaner, so we want it to go out of
  // scope (and invoke cleanup, if needed) before we submit a command (which
  // causes an immediate redraw). So we delete it before submitting the command.
  delete theOptimizer;
  delete theNLCO;
  if (reverted) UpdateAllViews();
  else SubmitCommand(cmd);
}


/*****
Enable Action->Scale Selection
*****/
void tmwxDoc::OnScaleSelectionUpdateUI(wxUpdateUIEvent& event)
{
  // TBD, do further check
  event.Enable(mTree->GetNumLeafNodes() >= 3);
}


/*****
Perform Action->Scale Selection
*****/
void tmwxDoc::OnScaleSelection(wxCommandEvent&)
{
  if (mTree->GetNumLeafNodes() < 3) {
    tmwxAlertError(wxT("Sorry, you need at least 3 leaf nodes to "\
      "optimize selected edge scale."));
    return;
  }
  tmNLCO* theNLCO = tmNLCO::MakeNLCO();
#ifdef tmUSE_CFSQP
  // set best algorithm for this type of optimization
  tmNLCO_cfsqp* theNLCO_cfsqp = dynamic_cast<tmNLCO_cfsqp*>(theNLCO);
  if (theNLCO_cfsqp) theNLCO_cfsqp->SetCFSQPMode(110);
#endif
  tmEdgeOptimizer* theOptimizer = new tmEdgeOptimizer(mTree, theNLCO);
  try {
    theOptimizer->Initialize(mSelection.GetNodes(), mSelection.GetEdges());
  }
  catch (tmEdgeOptimizer::EX_NO_MOVING_NODES) {
    tmwxAlertError(wxT("Sorry, there are no movable nodes."));
    delete theOptimizer;
    delete theNLCO;
    return;
  }
  catch (tmEdgeOptimizer::EX_NO_MOVING_EDGES) {
    tmwxAlertError(wxT("Sorry, there are no strainable edges."));
    delete theOptimizer;
    delete theNLCO;
    return;
  }
  size_t ne = theNLCO->GetNumEqualities();
  size_t nv = theNLCO->GetNumVariables();
  if (ne >= nv && QueryEqnsVblsContinue(ne, nv) == wxNO) {
    delete theOptimizer;
    delete theNLCO;
    return;
  }
  tmwxOptimizerDialog theDialog(this, theOptimizer, 
    wxT("Maximizing strain...")); 
  theNLCO->SetUpdater(&theDialog);
  wxString cmd;
  bool reverted = false;
  int ret = theDialog.ShowModal();
  switch(ret) {
    case tmwxOptimizerDialog::NORMAL_TERMINATION: {
      ::wxBell();
      cmd = wxT("edge optimization");
      break;
    }
    case tmwxOptimizerDialog::USER_CANCELLED: {
      if (QueryUserCancelledRevert() == wxYES) {
        theOptimizer->Revert();
        reverted = true;
        break;
      }
      else {
        cmd = wxT("cancelled edge optimization");
        break;
      }
    }
    case tmwxOptimizerDialog::OTHER_TERMINATION: {
      if (QueryBadConvergenceRevert(tmNLCO::GetAlgorithm(), 
        theDialog.GetReason()) == wxYES) {
        theOptimizer->Revert();
        reverted = true;
        break;
      }
      else {
        mTree->FilterMovableParts(mSelection.mNodes, mSelection.mEdges);
        cmd = wxT("cancelled edge optimization");
        break;
      }
    }
    default:
      TMFAIL("unknown optimizer return value in "\
        "tmwxDoc::DoEdgeOptimization()");
  }
  delete theOptimizer;
  delete theNLCO;
  mTree->FilterMovableParts(mSelection.mNodes, mSelection.mEdges);
  if (reverted) UpdateAllViews();
  else SubmitCommand(cmd);
}


/*****
Enable Action->Minimize Strain
*****/
void tmwxDoc::OnMinimizeStrainUpdateUI(wxUpdateUIEvent& event)
{
  event.Enable((mTree->GetNumLeafNodes() >= 3) && 
    (!mTree->IsFeasible() || mTree->HasStrainedEdges()));
}


/*****
Perform Action->Minimize Strain
*****/
void tmwxDoc::OnMinimizeStrain(wxCommandEvent&)
{
  if (mTree->GetNumLeafNodes() < 3) {
    tmwxAlertError(wxT("Sorry, you need at least 3 leaf nodes to "\
      "optimize the strain."));
    return;
  }
  if (mTree->IsFeasible() && !mTree->HasStrainedEdges()) {
    tmwxAlertInfo(wxT("This tree is already feasible and has no strain; "\
      "no optimization is needed."));
    return;
  }
  tmNLCO* theNLCO = tmNLCO::MakeNLCO();
#ifdef tmUSE_CFSQP
  // set best algorithm for this type of optimization
  tmNLCO_cfsqp* theNLCO_cfsqp = dynamic_cast<tmNLCO_cfsqp*>(theNLCO);
  if (theNLCO_cfsqp) theNLCO_cfsqp->SetCFSQPMode(100);
#endif
  tmStrainOptimizer* theOptimizer = new tmStrainOptimizer(mTree, theNLCO);
  try {
    theOptimizer->Initialize(mSelection.GetNodes(), mSelection.GetEdges());
  }
  catch (tmStrainOptimizer::EX_NO_MOVING_NODES_OR_EDGES) {
    tmwxAlertError(
      wxT("Sorry, there are no movable nodes or no strainable edges."));
    delete theOptimizer;
    delete theNLCO;
    return;
  }
  size_t ne = theNLCO->GetNumEqualities();
  size_t nv = theNLCO->GetNumVariables();
  if (ne >= nv && QueryEqnsVblsContinue(ne, nv) == wxNO) {
    delete theOptimizer;
    delete theNLCO;
    return;
  }
  tmwxOptimizerDialog theDialog(this, theOptimizer, 
    wxT("Minimizing RMS strain...")); 
  theNLCO->SetUpdater(&theDialog);
  wxString cmd;
  bool reverted = false;
  int ret = theDialog.ShowModal();
  switch(ret) {
    case tmwxOptimizerDialog::NORMAL_TERMINATION: {
      ::wxBell();
      cmd = wxT("strain optimization");
      break;
    }
    case tmwxOptimizerDialog::USER_CANCELLED: {
      if (QueryUserCancelledRevert() == wxYES) {
        theOptimizer->Revert();
        reverted = true;
        break;
      }
      else {
        cmd = wxT("cancelled strain optimization");
        break;
      }
    }
    case tmwxOptimizerDialog::OTHER_TERMINATION: {
      if (QueryBadConvergenceRevert(tmNLCO::GetAlgorithm(), 
        theDialog.GetReason()) == wxYES) {
        theOptimizer->Revert();
        reverted = true;
        break;
      }
      else {
        cmd = wxT("cancelled strain optimization");
        break;
      }
    }
    default:
      TMFAIL("unknown optimizer return value in "\
        "tmwxDoc::DoStrainOptimization()");
  }
  delete theOptimizer;
  delete theNLCO;
  if (reverted) UpdateAllViews();
  else SubmitCommand(cmd);
}


/*****
Enable Action->Build Crease Pattern
*****/
void tmwxDoc::OnBuildCreasePatternUpdateUI(wxUpdateUIEvent& event)
{
  event.Enable(mTree->GetNumLeafNodes() >= 3);
}


/*****
Perform Action->Build Crease Pattern
*****/
void tmwxDoc::OnBuildCreasePattern(wxCommandEvent&)
{
  mSelection.ClearAllParts();
  mTree->BuildPolysAndCreasePattern();
  if (!mTree->HasFullCP()) {
    tmArray<tmEdge*> badEdges;
    tmArray<tmPoly*> badPolys;
    tmArray<tmVertex*> badVertices;
    tmArray<tmCrease*> badCreases;
    tmArray<tmFacet*> badFacets;
    tmTree::CPStatus status = mTree->GetCPStatus(badEdges, badPolys, 
      badVertices, badCreases, badFacets);
    switch(status) {
      case tmTree::EDGES_TOO_SHORT:
        MsgEdgesTooShort();
        mSelection.AddParts(badEdges);
        break;
      case tmTree::POLYS_NOT_VALID:
        MsgNotPolygonValid();
        break;
      case tmTree::POLYS_NOT_FILLED:
        MsgNotPolygonFilled();
        mSelection.AddParts(badPolys);
        break;
      case tmTree::POLYS_MULTIPLE_IBPS:
        MsgMultipleInactiveBorderPaths();
        mSelection.AddParts(badPolys);
        break;
      case tmTree::VERTICES_LACK_DEPTH:
        MsgNotVertexDepthValid();
        mSelection.AddParts(badVertices);
        break;
      case tmTree::FACETS_NOT_VALID:
        MsgNotFacetDataValid();
        mSelection.AddParts(badVertices);
        mSelection.AddParts(badFacets);
        break;
      case tmTree::NOT_LOCAL_ROOT_CONNECTABLE:
        MsgNotLocalRootConnectable();
        mSelection.AddParts(badVertices);
        mSelection.AddParts(badCreases);
        break;
      default:
        TMFAIL("unknown CPStatus encountered in "\
          "tmwxDoc::DoBuildCreasePattern()");
    }
  }
  SubmitCommand(wxT("Build Crease Pattern"));
}


/*****
Enable Action->Kill Crease Pattern
*****/
void tmwxDoc::OnKillCreasePatternUpdateUI(wxUpdateUIEvent& event)
{
  event.Enable(mTree->GetCreases().not_empty());
}


/*****
Perform Action->Kill Crease Pattern
*****/
void tmwxDoc::OnKillCreasePattern(wxCommandEvent&)
{
  mTree->KillPolysAndCreasePattern();
  SubmitCommand(wxT("Kill Crease Pattern"));
}


#ifdef __MWERKS__
  #pragma mark -
#endif


/*****
Put up an alert explaining why we couldn't build the CP.
*****/
void tmwxDoc::MsgEdgesTooShort()
{
  tmwxAlertInfo(
    wxT("TreeMaker wasn't able to construct the crease pattern because ")
    wxT("one or more edges are too short. This makes it impossible to ")
    wxT("distinguish hinge creases. Try absorbing the tiny edge(s). "));
}


/*****
Put up an alert explaining why we couldn't build all polygons for this tree.
*****/
void tmwxDoc::MsgNotPolygonValid()
{
  tmwxAlertInfo(
    wxT("TreeMaker wasn't able to construct all polygons, possibly because ")
    wxT("a polygon was nonconvex or contained one or more nodes in its ")
    wxT("interior. ")
    wxT("This is common with many-branched trees. Try selecting all ")
    wxT("unpinned edges, performing an edge optimization, then rebuilding ")
    wxT("the polygons and/or crease pattern."));
}


/*****
Put up an alert explaining why we couldn't build the rest of the crease pattern.
*****/
void tmwxDoc::MsgNotPolygonFilled()
{
  tmwxAlertInfo(
    wxT("TreeMaker wasn't able to construct the crease pattern because some ")
    wxT("of the polygons did not have their interiors filled. Try rebuilding ")
    wxT("the crease pattern by selecting ")
    wxT("Action->Build Crease Pattern. "));
}


/*****
Put up an alert explaining why we couldn't build the rest of the crease pattern.
*****/
void tmwxDoc::MsgMultipleInactiveBorderPaths()
{
  tmwxAlertInfo(
    wxT("TreeMaker wasn't able to assign creases ")
    wxT("because at least one of the polygons contains two or more ")
    wxT("inactive paths on its boundary. ")
    wxT("Try selecting unpinned edges, performing an edge optimization, ")
    wxT("and rebuilding the crease pattern. "));
}


/*****
Put up a box explaining why vertex depth is not fully set in this tree.
*****/
void tmwxDoc::MsgNotVertexDepthValid()
{
  tmwxAlertInfo(
    wxT("TreeMaker wasn't able to compute the depth for all vertices ")
    wxT("and thus can't construct the folded form of the base. ")
    wxT("This is probably because the tree isn't fully optimized. That is, ")
    wxT("you can make some of the edges of the tree longer at the current ")
    wxT("scale. Try selecting all ")
    wxT("unpinned edges, performing an edge optimization, then rebuilding ")
    wxT("the crease pattern."));
}


/*****
Put up a box explaining why facet data is not valid in this tree.
*****/
void tmwxDoc::MsgNotFacetDataValid()
{
  tmwxAlertInfo(
    wxT("TreeMaker wasn't able to fully construct the facets ")
    wxT("and thus can't construct the facet ordering or crease assignment. ")
    wxT("This is usually due to numerical roundoff issues resulting in the ")
    wxT("formation of \"sliver\" facets. The crease pattern will fold into ")
    wxT("the base, but you will have to find the crease assignment by hand. ")
    wxT("The offending vertices and creases have been selected. "));
}


/*****
Put up a box explaining why the corridors are not connected.
*****/
void tmwxDoc::MsgNotLocalRootConnectable()
{
  tmwxAlertInfo(
    wxT("TreeMaker wasn't able to fully compute the facet ordering ")
    wxT("because there were multiple zero-depth local root networks or ")
    wxT("a local root network was not connectable to ")
    wxT("lower-depth root networks. This means that the direct algorithm ")
    wxT("for facet ordering and crease assignment won't work. While you may ")
    wxT("still be able to find a flat-foldable crease assignment, this ")
    wxT("usually means that the initial configuration was far from optimal. ")
    wxT("Try moving some leaf nodes so that the disconnected parts of the ")
    wxT("corridor wall formed by the selected vertices and creases ")
    wxT("will be forced closer together and re-optimize. "));
}


#ifdef __MWERKS__
  #pragma mark -
#endif


/*****
Put up an alert with the number of equations and variables and ask the user
if he/she wants to continue.
*****/
int tmwxDoc::QueryEqnsVblsContinue(size_t ne, size_t nv)
{
  wxString text;
  text.Printf(wxT("There are %s equations and %s variables. Continue?"),
    tmwxStr(ne).c_str(), tmwxStr(nv).c_str());
  return tmwxAlertQuery(text);
}


/*****
Put up a dialog indicating cancellation by user and ask the user if he/she wants
to revert.
*****/
int tmwxDoc::QueryUserCancelledRevert()
{
  return tmwxAlertQuery(wxT("Optimization was cancelled. Revert?"));
}


/*****
Put up an alert explaining the reason for bad convergence (which depends on the
reason parameter taken from the optimizer) and ask the user if he/she wants to
revert.
*****/
int tmwxDoc::QueryBadConvergenceRevert(int nlco, int reason)
{
  wxString preamble;
  wxString msg;
  switch(nlco) {
#ifdef tmUSE_CFSQP
    case tmNLCO::CFSQP:
    preamble = wxT("Bad convergence in CFSQP optimizer. ");
    switch(reason) {
      case tmNLCO_cfsqp::ERROR_INFEASIBLE_LINEAR:
        msg = wxT("The current configuration ")
          wxT("is infeasible for linear constraints and CFSQP is unable to ")
          wxT("find a feasible point. ");
        break;
      case tmNLCO_cfsqp::ERROR_INFEASIBLE_NONLINEAR:
        msg = wxT("The current configuration ")
          wxT("is infeasible for nonlinear inequality and linear constraints ")
          wxT("and CFSQP is unable to find a feasible point. ");
        break;
      case tmNLCO_cfsqp::ERROR_MAX_ITERATIONS:
        msg = wxT("The maximum number of iterations was reached before a ")
        wxT("solution was obtained. ");
        break;
      case tmNLCO_cfsqp::ERROR_LINESEARCH_FAILED:
        msg = wxT("The line search failed ")
          wxT("to find a new iterate because the step size became too small.");
        break;
      case tmNLCO_cfsqp::ERROR_QPSOLVER_FAILED_D0:
        msg = wxT("The QP solver failed in attempting to construct d0. ");
        break;
      case tmNLCO_cfsqp::ERROR_QPSOLVER_FAILED_D1:
        msg = wxT("The QP solver failed in attempting to construct d1. ");
        break;
      case tmNLCO_cfsqp::ERROR_INPUT_NOT_CONSISTENT:
        msg = wxT("The input data are not consistent. ");
        break;
      case tmNLCO_cfsqp::ERROR_STOPPING_NOT_REACHED:
        msg = wxT("The new iterate is numerically equivalent to the previous ")
          wxT("iterate, although the stopping criterion is not yet satisfied. ");
        break;
      case tmNLCO_cfsqp::ERROR_PENALTY_TOO_LARGE:
        msg = wxT("One of the penalty parameters became too large. ")
          wxT("The algorithm ")
          wxT("is having trouble satisfying a nonlinear equality constraint. ");
        break;
      default:
        TMFAIL("unknown return value encountered from CFSQP optimization");
    }
    break;
#endif
#ifdef tmUSE_RFSQP
    case tmNLCO::RFSQP:
    preamble = wxT("Bad convergence in RFSQP optimizer. ");
    switch(reason) {
      case tmNLCO_rfsqp::ERROR_QPSOLVER_FAILED:
        msg = wxT("The QP solver failed to solve a quadratic subproblem. ");
        break;
      case tmNLCO_rfsqp::ERROR_LINE_SEARCH_FAILED:
        msg = wxT("The line search failed to find a new iterate because the ")
          wxT("step size became too small. ");
        break;
      case tmNLCO_rfsqp::ERROR_MAX_ITERATIONS:
        msg = wxT("The maximum number of iterations was reached before ")
          wxT("a solution was obtained. ");
        break;
      case tmNLCO_rfsqp::ERROR_INFEASIBLE_EQUALITY:
        msg = wxT("The solver was unable to generate a feasible initial ")
          wxT("point for nonlinear equality constraints. ");
        break;
      case tmNLCO_rfsqp::ERROR_INFEASIBLE_AFFINE:
        msg = wxT("The solver was unable to generate a feasible initial ")
          wxT("point for affine constraints. ");
        break;
      default:
        TMFAIL("unknown return value encountered from RFSQP optimization");
    }
    break;
#endif
#ifdef tmUSE_ALM
    case tmNLCO::ALM:
    preamble = wxT("Bad convergence in ALM optimizer. ");
    switch(reason) {
      case tmNLCO_alm::ERROR_TOO_MANY_ITERATIONS:
        msg = wxT("The solver reached the iteration limit before convergence. ");
        break;
      default:
        TMFAIL("unknown return value encountered from ALM optimization");
    }
    break;
#endif
#ifdef tmUSE_WNLIB
    case tmNLCO::WNLIB:
    preamble = wxT("Bad convergence in WNLIB optmizer. ");
    msg = wxT("Reason is unspecified. ");
    break;
#endif
    default:
      TMFAIL("unknown optimizer code encountered in "\
        "tmwxDoc::QueryBadConvergenceRevert(int, int)");
  }
  return tmwxAlertQuery(preamble + msg + wxString(wxT("Revert?")));
}
