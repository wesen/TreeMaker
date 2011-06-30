/*******************************************************************************
File:         tmTree_TestTrees.cpp
Project:      TreeMaker 5.x
Purpose:      Implementation file for tmTree test tree routines
Author:       Robert J. Lang
Modified by:  
Created:      2003-11-21
Copyright:    ©2003 Robert J. Lang. All Rights Reserved.
*******************************************************************************/

#include "tmTree.h"
#include "tmModel.h"

#include <cstdlib>

using namespace std;

/*
This file contains several routines for creating various
dummy trees that can be used for debugging.
*/

/*****
tmTree* tmTree::MakeTreeBlank()
Create a new blank tree (no nodes or edges)
*****/
tmTree* tmTree::MakeTreeBlank()
{
  tmTree* theTree = new tmTree();
  TMASSERT(theTree);
  theTree->mScale = 0.1;
  theTree->CleanupAfterEdit();
  return theTree;
}


/*****
tmTree* tmTree::MakeTreeUnoptimized()
Create an 8-tmNode, 6-appendage unoptimized tree
*****/
tmTree* tmTree::MakeTreeUnoptimized()
{
  tmTree* theTree = new tmTree();
  TMASSERT(theTree);

  tmNode* aNode1;
  tmEdge* aEdge1;
  theTree->AddNode(NULL, tmPoint(0.40, 0.80), aNode1, aEdge1);
  strcpy(aNode1->mLabel, "node1");
  
  tmNode* aNode2;
  theTree->AddNode(aNode1, tmPoint(0.45, 0.60), aNode2, aEdge1);
  strcpy(aNode2->mLabel, "node2");
  strcpy(aEdge1->mLabel, "edge1");
  
  tmNode* aNode3;
  tmEdge* aEdge2;
  theTree->AddNode(aNode2, tmPoint(0.55, 0.35), aNode3, aEdge2);
  strcpy(aNode3->mLabel, "node3");
  strcpy(aEdge2->mLabel, "edge2");
  
  tmNode* aNode4;
  tmEdge* aEdge3;
  theTree->AddNode(aNode3, tmPoint(0.55, 0.01), aNode4, aEdge3);
  strcpy(aNode4->mLabel, "node4");
  strcpy(aEdge3->mLabel, "edge3");
  
  tmNode* aNode5;
  tmEdge* aEdge4;
  theTree->AddNode(aNode2, tmPoint(0.05, 0.95), aNode5, aEdge4);
  strcpy(aNode5->mLabel, "node5");
  strcpy(aEdge4->mLabel, "edge4");
  
  tmNode* aNode6;
  tmEdge* aEdge5;
  theTree->AddNode(aNode2, tmPoint(0.95, 0.95), aNode6, aEdge5);
  strcpy(aNode6->mLabel, "node6");
  strcpy(aEdge5->mLabel, "edge5");
  
  tmNode* aNode7;
  tmEdge* aEdge6;
  theTree->AddNode(aNode3, tmPoint(0.05, 0.05), aNode7, aEdge6);
  strcpy(aNode7->mLabel, "node7");
  strcpy(aEdge6->mLabel, "edge6");
  
  tmNode* aNode8;
  tmEdge* aEdge7;
  theTree->AddNode(aNode3, tmPoint(0.95, 0.05), aNode8, aEdge7);
  strcpy(aNode8->mLabel, "node8");
  strcpy(aEdge7->mLabel, "edge7");
  
  // Add some symmetry to the tree: a mirror line in the center at 90 degrees
  theTree->mHasSymmetry = true;
  theTree->mSymLoc = tmPoint(0.5, 0.5);
  theTree->mSymAngle = 90.0;
  
  // Add some symmetry conditions on the upper nodes
  new tmConditionNodesPaired(theTree, aNode5, aNode6);
  new tmConditionEdgesSameStrain(theTree, aEdge4, aEdge5);
  new tmConditionNodeSymmetric(theTree, aNode1);
  
  // Set the scale to a small enough value that insures all paths are valid.
  theTree->mScale = 0.1;
  theTree->CleanupAfterEdit();
  
  return theTree;
}


/*****
tmTree* tmTree::MakeTreeOptimized()
Create an 8-tmNode, 6-appendage already-optimized tree
*****/  
tmTree* tmTree::MakeTreeOptimized()
{
  tmTree* theTree = new tmTree();
  TMASSERT(theTree);
  
  tmNode* aNode1;
  tmEdge* aEdge1;
  theTree->AddNode(NULL, tmPoint(0.5, 0.812792), aNode1, aEdge1);
  strcpy(aNode1->mLabel, "node1");
  
  tmNode* aNode2;
  theTree->AddNode(aNode1, tmPoint(0.45, 0.60), aNode2, aEdge1);
  strcpy(aNode2->mLabel, "node2");
  strcpy(aEdge1->mLabel, "edge1");
  
  tmNode* aNode2a;
  tmEdge* aEdge1a;
  theTree->AddNode(aNode2, tmPoint(0.52, 0.48), aNode2a, aEdge1a);
  strcpy(aNode2a->mLabel, "node2a");
  strcpy(aEdge1a->mLabel, "edge1a");
  aEdge1a->mLength = 0.5;
  
  tmNode* aNode3;
  tmEdge* aEdge2;
  theTree->AddNode(aNode2a, tmPoint(0.55, 0.35), aNode3, aEdge2);
  strcpy(aNode3->mLabel, "node3");
  strcpy(aEdge2->mLabel, "edge2");
  aEdge2->mLength = 0.5;
  
  tmNode* aNode4;
  tmEdge* aEdge3;
  theTree->AddNode(aNode3, tmPoint(0.5, 0.0), aNode4, aEdge3);
  strcpy(aNode4->mLabel, "node4");
  strcpy(aEdge3->mLabel, "edge3");
  
  tmNode* aNode5;
  tmEdge* aEdge4;
  theTree->AddNode(aNode2, tmPoint(0.0, 1.0), aNode5, aEdge4);
  strcpy(aNode5->mLabel, "node5");
  strcpy(aEdge4->mLabel, "edge4");
  
  tmNode* aNode6;
  tmEdge* aEdge5;
  theTree->AddNode(aNode2, tmPoint(1.0, 1.0), aNode6, aEdge5);
  strcpy(aNode6->mLabel, "node6");
  strcpy(aEdge5->mLabel, "edge5");
  
  tmNode* aNode7;
  tmEdge* aEdge6;
  theTree->AddNode(aNode3, tmPoint(0.0, 0.187208), aNode7, aEdge6);
  strcpy(aNode7->mLabel, "node7");
  strcpy(aEdge6->mLabel, "edge6");
  
  tmNode* aNode8;
  tmEdge* aEdge7;
  theTree->AddNode(aNode3, tmPoint(1.0, 0.187208), aNode8, aEdge7);
  strcpy(aNode8->mLabel, "node8");
  strcpy(aEdge7->mLabel, "edge7");
  
  theTree->mScale = 0.266949;
  theTree->CleanupAfterEdit();
  return theTree;
}


/*****
tmTree* tmTree::MakeTreeGusset()
Create a 5-node, 4-appendage already-optimized tree to test the construction 
of the gusset quad.
*****/  
tmTree* tmTree::MakeTreeGusset()
{
  tmTree* theTree = new tmTree();
  TMASSERT(theTree);
  
  tmNode* aNode1;
  tmEdge* aEdge1;
  theTree->AddNode(NULL, tmPoint(0.0, 0.0), aNode1, aEdge1);
  strcpy(aNode1->mLabel, "node1");
  
  tmNode* aNode2;
  theTree->AddNode(aNode1, tmPoint(0.45, 0.50), aNode2, aEdge1);
  strcpy(aNode2->mLabel, "node2");
  strcpy(aEdge1->mLabel, "edge1");
  aEdge1->mLength = 0.6;
  
  tmNode* aNode3;
  tmEdge* aEdge2;
  theTree->AddNode(aNode2, tmPoint(1.0, 0.0), aNode3, aEdge2);
  strcpy(aNode3->mLabel, "node3");
  strcpy(aEdge2->mLabel, "edge2");
  aEdge2->mLength = 0.4;
  
  tmNode* aNode4;
  tmEdge* aEdge3;
  theTree->AddNode(aNode2, tmPoint(0.0, 1.0), aNode4, aEdge3);
  strcpy(aNode4->mLabel, "node4");
  strcpy(aEdge3->mLabel, "edge3");
  aEdge3->mLength = 0.4;
  
  tmNode* aNode5;
  tmEdge* aEdge4;
  theTree->AddNode(aNode2, tmPoint(1.0, 1.0), aNode5, aEdge4);
  strcpy(aNode5->mLabel, "node5");
  strcpy(aEdge4->mLabel, "edge4");
  aEdge4->mLength = 0.6;

  theTree->mScale = 1.0;
  theTree->CleanupAfterEdit();
  return theTree;
}


/*****
tmTree* tmTree::MakeTreeConditioned()
Create a tree with examples of each type of condition.
*****/
tmTree* tmTree::MakeTreeConditioned()
{
  tmTree* theTree = new tmTree();
  TMASSERT(theTree);

  tmNode* aNode1;
  tmEdge* aEdge1;
  theTree->AddNode(NULL, tmPoint(0.50, 0.10), aNode1, aEdge1);
  
  tmNode* aNode2;
  theTree->AddNode(aNode1, tmPoint(0.60, 0.50), aNode2, aEdge1);
  
  tmNode* aNode3;
  tmEdge* aEdge2;
  theTree->AddNode(aNode2, tmPoint(0.65, 0.75), aNode3, aEdge2);
  
  tmNode* aNode4;
  tmEdge* aEdge3;
  theTree->AddNode(aNode2, tmPoint(0.05, 0.05), aNode4, aEdge3);
  
  tmNode* aNode5;
  tmEdge* aEdge4;
  theTree->AddNode(aNode2, tmPoint(1.00, 0.5), aNode5, aEdge4);
  
  tmNode* aNode6;
  tmEdge* aEdge5;
  theTree->AddNode(aNode3, tmPoint(0.05, 0.95), aNode6, aEdge5);
  
  tmNode* aNode7;
  tmEdge* aEdge6;
  theTree->AddNode(aNode3, tmPoint(1.00, 1.00), aNode7, aEdge6);
  
  tmNode* aNode8;
  tmEdge* aEdge7;
  theTree->AddNode(aNode3, tmPoint(0.50, 0.85), aNode8, aEdge7);
  
  // Add some symmetry to the tree: a mirror line in the center at 90 degrees
  theTree->mHasSymmetry = true;
  theTree->mSymLoc = tmPoint(0.5, 0.5);
  theTree->mSymAngle = 90.0;
  
  // Add one of each type of symmetry condition.
  new tmConditionNodeFixed(theTree, aNode1, true, .5, false, 0.);
  new tmConditionNodeOnCorner(theTree, aNode7);
  new tmConditionNodeOnEdge(theTree, aNode5);
  new tmConditionNodesCollinear(theTree, aNode4, aNode1, aNode5);
  new tmConditionNodesPaired(theTree, aNode6, aNode7);
  new tmConditionNodeSymmetric(theTree, aNode8);
  new tmConditionEdgeLengthFixed(theTree, aEdge2);
  new tmConditionEdgesSameStrain(theTree, aEdge5, aEdge6);
  new tmConditionPathActive(theTree, 
    theTree->FindLeafPath(aNode5, aNode7));
  new tmConditionPathAngleFixed(theTree,
    theTree->FindLeafPath(aNode4, aNode1), 22.5);
  new tmConditionPathAngleQuant(theTree,
    theTree->FindLeafPath(aNode4, aNode6), 8, 0.);
  
  // Set the scale to something that makes tmNode circles visible
  theTree->mScale = 0.1;
  theTree->CleanupAfterEdit();
  return theTree;
}
