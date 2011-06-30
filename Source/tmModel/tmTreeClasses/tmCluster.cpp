/*******************************************************************************
File:         tmCluster.cpp
Project:      TreeMaker 5.x
Purpose:      Implementation file for a group of parts
Author:       Robert J. Lang
Modified by:  
Created:      2003-11-27
Copyright:    ©2003 Robert J. Lang. All Rights Reserved.
*******************************************************************************/

#include "tmCluster.h"
#include "tmModel.h"

using namespace std;

/*
Class tmCluster implements a collection of all types of tmParts. It's a base class
of a tmTree, but can also used for other purposes, e.g., to hold the selection in a GUI.
*/

/**********
class tmCluster
A group of tmParts; a base class of a tmTree
**********/

/*****
size_t tmCluster::GetNumAllParts() const
Return the number of all parts in the cluster.
*****/
size_t tmCluster::GetNumAllParts() const
{
  return
    GetNumNodes() +
    GetNumEdges() +
    GetNumPaths() +
    GetNumPolys() +
    GetNumVertices() +
    GetNumCreases() +
    GetNumFacets() +
    GetNumConditions();
}


/*****
size_t tmCluster::GetNumEditableParts()
Return the number of editable parts in the cluster (everything but paths)
*****/
size_t tmCluster::GetNumEditableParts() const
{
  return
    GetNumNodes() +
    GetNumEdges() +
    GetNumPolys() +
    GetNumVertices() +
    GetNumCreases() +
    GetNumFacets() +
    GetNumConditions();
}


/*****
tmPart* tmCluster::GetFirstPart()
Return the first part in the cluster, NULL if it's empty.
*****/
tmPart* tmCluster::GetFirstPart() const
{
  if (mNodes.size()) return mNodes.front();
  if (mEdges.size()) return mEdges.front();
  if (mPaths.size()) return mPaths.front();
  if (mPolys.size()) return mPolys.front();
  if (mVertices.size()) return mVertices.front();
  if (mCreases.size()) return mCreases.front();
  if (mFacets.size()) return mFacets.front();
  if (mConditions.size()) return mConditions.front();
  return NULL;
}


/*****
void tmCluster::ClearAllParts()
Clear out all items in a tmCluster. We're erasing pointers, not deleting the
underlying objects.
*****/
void tmCluster::ClearAllParts()
{
  GetNodes().clear();
  GetEdges().clear();
  GetPaths().clear();
  GetPolys().clear();
  GetVertices().clear();
  GetCreases().clear();
  GetFacets().clear();
  GetConditions().clear();
}


/*****
void tmCluster::AddParts(const tmCluster& aCluster)
Add all the parts from the given cluster to this one.
*****/
void tmCluster::AddParts(const tmCluster& aCluster)
{
  mNodes.union_with(aCluster.GetNodes());
  mEdges.union_with(aCluster.GetEdges());
  mPaths.union_with(aCluster.GetPaths());
  mPolys.union_with(aCluster.GetPolys());
  mVertices.union_with(aCluster.GetVertices());
  mCreases.union_with(aCluster.GetCreases());
  mFacets.union_with(aCluster.GetFacets());
  mConditions.union_with(aCluster.GetConditions());
}
