/*******************************************************************************
File:         tmModelTester.cpp
Project:      TreeMaker 5.x
Purpose:      Implementation file for testing the TreeMaker model (no GUI)
Author:       Robert J. Lang
Modified by:  
Created:      2004-05-29
Copyright:    ©2004 Robert J. Lang. All Rights Reserved.
*******************************************************************************/

/*
This file tests all of the tmTree classes in a console-based format. It's
intended to detect compilation errors and to profile the optimizers when
working with real tree structures.
*/

// standard libraries
#include <iostream>
#include <fstream>
#include <string>
#include <ctime>

using namespace std;

// TreeMaker model classes
#include "tmModel.h"
#include "tmNLCO.h"


/* This is the path to the test files. Users should adjust this for their
own system */

static const string testdir =
  "/users/rjlang/C++Projects/TreeMaker_5/Source/test/tmModelTester/";

/*****
Write the number of function and gradient calls for a single function.
*****/
void ReportCalls(const char* name, tmDifferentiableFn* f)
{
#if TM_PROFILE_OPTIMIZERS
  cout << name << " calls = (" << f->GetNumFuncCalls() << ", " <<
    f->GetNumGradCalls() << ")\n";
#endif
};


/*****
Write the total number of function and gradient calls for a list of functions.
*****/
void ReportCalls(const char* name, 
  const std::vector<tmDifferentiableFn*>& flist)
{
#if TM_PROFILE_OPTIMIZERS
  size_t numFunc = 0;
  size_t numGrad = 0;
  for (size_t i = 0; i < flist.size(); ++i) {
    numFunc += flist[i]->GetNumFuncCalls();
    numGrad += flist[i]->GetNumGradCalls();
  }
  cout << name << " calls = (" << numFunc << ", " << numGrad << ")\n";
#endif
};


/*****
Read the tree in from a file. The previous tree will be overwritten. Note that
the path to the directory of test files is hard-coded; users should change the
value of testdir to reflect their own file organization.
*****/
void DoReadFile(tmTree* theTree, const string& filename)
{
  string fullname = testdir + filename;
  cout << "Reading in file " << fullname << endl;
  ifstream fin(fullname.c_str());
  if (!fin.good()) {
    cout << "Unable to find file " << fullname << endl;
    exit(1);
  }
  try {
    theTree->GetSelf(fin);
  }
  catch(...) {
    cout << "Unexpected exception reading file " << fullname << endl;
    exit(1);
  }
}


/*****
Read in a file and perform a scale optimization on it.
*****/
template <class NLCO>
void DoScaleOptimization(char* name)
{
      if(!name)return;
  const string filename(name);
  tmTree* theTree = new tmTree();
  DoReadFile(theTree, filename);
  cout << "Optimizing scale of " << filename << "..." << endl;
  clock_t startTime, stopTime;
  NLCO* theNLCO = new NLCO();
  tmScaleOptimizer* theOptimizer = new tmScaleOptimizer(theTree, theNLCO);
  theOptimizer->Initialize();
  startTime = clock();
  try {
    theOptimizer->Optimize();
  }
  catch (tmNLCO::EX_BAD_CONVERGENCE ex) {
    cout << "Scale optimization failed with result code " << 
      ex.GetReason() << endl;
  }
  catch(tmScaleOptimizer::EX_BAD_SCALE) {
    cout << "Scale optimization failed with scale too small. " << endl;
  }
  stopTime = clock();
  ReportCalls((filename + " objective").c_str(), 
    theOptimizer->GetNLCO()->GetObjective());
  ReportCalls((filename + " constraint").c_str(), 
    theOptimizer->GetNLCO()->GetConstraints());
  delete theOptimizer;
  delete theNLCO;
  cout << "New scale is " << theTree->GetScale() << endl;
  cout << "Feasibility after optimization is " << 
    (theTree->IsFeasible() ? "true " : "false") << endl;
  cout << "Elapsed time = " << (stopTime - startTime) << " ticks" << endl;
  cout << endl;
  delete theTree;
}


/*****
Read in a file and perform an edge optimization on it.
*****/
template <class NLCO>
void DoEdgeOptimization(char* name)
{
      if(!name)return;
  const string filename = name;
  tmTree* theTree = new tmTree();
  DoReadFile(theTree, filename);
  cout << "Maximizing edge strain of " << filename << "..." << endl;
  clock_t startTime, stopTime = -1;
  NLCO* theNLCO = new NLCO();
  tmEdgeOptimizer* theOptimizer = new tmEdgeOptimizer(theTree, theNLCO);
  tmDpptrArray<tmNode> movingNodes = theTree->GetOwnedNodes();
  tmDpptrArray<tmEdge> stretchyEdges = theTree->GetOwnedEdges();
  theOptimizer->Initialize(movingNodes, stretchyEdges);
  startTime = clock();
  try {
    theOptimizer->Optimize();
  }
  catch (tmNLCO::EX_BAD_CONVERGENCE ex) {
    cout << "Edge optimization failed with result code " << 
      ex.GetReason() << endl;
  stopTime = clock();
  ReportCalls((filename + " objective").c_str(), 
    theOptimizer->GetNLCO()->GetObjective());
  ReportCalls((filename + " constraint").c_str(), 
    theOptimizer->GetNLCO()->GetConstraints());
  delete theOptimizer;
  delete theNLCO;
  cout << "Optimized over " << movingNodes.size() << " nodes and " <<
    stretchyEdges.size() << " edges. " << endl;
  cout << "Edge strain is " << stretchyEdges.front()->GetStrain() << endl;
  }
  if (stopTime < 0)
    stopTime = clock();
  cout << "Feasibility after optimization is " << 
    (theTree->IsFeasible() ? "true " : "false") << endl;
  cout << "Elapsed time = " << (stopTime - startTime) << " ticks" << endl;
  cout << endl;
  delete theTree;
}


/*****
Read in a file and perform a strain optimization on it.
*****/
template <class NLCO>
void DoStrainOptimization(char* name)
{
      if(!name)return;
  const string filename = name;
  tmTree* theTree = new tmTree();
  DoReadFile(theTree, filename);
  cout << "Minimizing strain of " << filename << "..." << endl;
  clock_t startTime, stopTime;
  NLCO* theNLCO = new NLCO();
  tmStrainOptimizer* theOptimizer = new tmStrainOptimizer(theTree, theNLCO);
  tmDpptrArray<tmNode> movingNodes = theTree->GetOwnedNodes();
  tmDpptrArray<tmEdge> stretchyEdges = theTree->GetOwnedEdges();
  theOptimizer->Initialize(movingNodes, stretchyEdges);
  startTime = clock();
  try {
    theOptimizer->Optimize();
  }
  catch (tmNLCO::EX_BAD_CONVERGENCE ex) {
    cout << "Strain optimization failed with result code " << 
      ex.GetReason() << endl;
  }
  stopTime = clock();
  ReportCalls((filename + " objective").c_str(), 
    theOptimizer->GetNLCO()->GetObjective());
  ReportCalls((filename + " constraint").c_str(), 
    theOptimizer->GetNLCO()->GetConstraints());
  delete theOptimizer;
  delete theNLCO;
  cout << "Optimized over " << movingNodes.size() << " nodes and " <<
    stretchyEdges.size() << " edges. " << endl;
  // Compute and report the RMS edge strain, weighting each edge's strain by
  // its stiffness coefficient. (This is the objective function in the
  // minimization.)
  double ss = 0;
  for (size_t i = 0; i < stretchyEdges.size(); ++i) {
    tmEdge* theEdge = stretchyEdges[i];
    ss += theEdge->GetStiffness() * pow(theEdge->GetStrain(), 2);
  }
  ss /= stretchyEdges.size();
  cout << "Stiffness-weighted RMS strain is " << 100 * sqrt(ss) << "%" << 
    endl;
  cout << "Feasibility after optimization is " << 
    (theTree->IsFeasible() ? "true " : "false") << endl;
  cout << "Elapsed time = " << (stopTime - startTime) << " ticks" << endl;
  cout << endl;
  delete theTree;
}


/*****
Peform optimizations on several files using the templated optimization engine.
*****/
template <class NLCO>
void DoSeveralOptimizations()
{
  // SCALE OPTIMIZATION
  
  // Read in a test structure from a file and do an optimization.
  // tmModelTest_1.tmd5 is a simple 3-star, which should optimize quickly.
  DoScaleOptimization<NLCO>("tmModelTester_1.tmd5");
  
  // tmModelText_2.tmd5 is a much more complex structure, a fully
  // unoptimized centipede, but no constraints on it.
  DoScaleOptimization<NLCO>("tmModelTester_2.tmd5");
  
  // tmModelTest_3.tmd5 is the same centipede, but this time with symmetry
  // constraints and an infeasible starting configuration.
  DoScaleOptimization<NLCO>("tmModelTester_3.tmd5");
  
  // EDGE OPTIMIZATION
  
  // tmModelTest_4.tmd5 is an optimized centipede, but now some of the nodes
  // are unpinned, so an edge optimization can be performed.
  DoEdgeOptimization<NLCO>("tmModelTester_4.tmd5");
  
  // STRAIN OPTIMIZATION
  
  // tmModelTest_5.tmd5 is a scorpion design with many path active constraints,
  // edge strain constraints, and an infeasible initial configuration.
  DoStrainOptimization<NLCO>("tmModelTester_5.tmd5");
}


/*****
Main Program
*****/
int main()
{
  cout << "**************************************************\n";
  cout << "TreeMaker Model Test Program\n";
  cout << "**************************************************\n";
  cout << endl;
  
  cout.setf(ios_base::fixed);
  cout.precision(6);

  // Initialize our dynamic type system
  tmPart::InitTypes();
  
  // Do the optimization with all four engines
  
#ifdef tmUSE_ALM
  cout << "Using ALM optimizer" << endl;
  DoSeveralOptimizations<tmNLCO_alm>();
  cout << endl;
#endif // tmUSE_ALM

#ifdef tmUSE_WNLIB
  cout << "Using WNLIB optimizer" << endl;
  DoSeveralOptimizations<tmNLCO_wnlib>();
  cout << endl;
#endif // tmUSE_WNLIB

#ifdef tmUSE_CFSQP
  cout << "Using CFSQP optimizer" << endl;
  DoSeveralOptimizations<tmNLCO_cfsqp>();
  cout << endl;
#endif // tmUSE_CFSQP

#ifdef tmUSE_RFSQP
  cout << "Using RFSQP optimizer" << endl;
  DoSeveralOptimizations<tmNLCO_rfsqp>();
  cout << endl;
#endif // tmUSE_RFSQP
}
