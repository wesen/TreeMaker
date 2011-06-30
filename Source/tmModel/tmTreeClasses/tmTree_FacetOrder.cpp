/*******************************************************************************
File:         tmTree_FacetOrder.cpp
Project:      TreeMaker 5.x
Purpose:      Implementation file for class tmTree
Author:       Robert J. Lang
Modified by:  
Created:      2005-09-03
Copyright:    ©2005 Robert J. Lang. All Rights Reserved.
*******************************************************************************/

#include "tmTree.h"
#include "tmModel.h"

#ifdef TMDEBUG
  #include <fstream>
#endif

using namespace std;

/*
This file contains the tmTree routines that are specific to the construction
of the global facet ordering.
*/

#ifdef __MWERKS__
  #pragma mark --PRIVATE--
#endif


/**********
class tmRootNetwork
Represents a connected component of the network of local root hinges and local
root vertices within the tree. It also contains its own spanning tree, and some
additional fields used in splicing together pieces of the facet ordering graph.
**********/

class tmRootNetwork {
public:
  enum {
    INELIGIBLE = 0,
    NOT_YET = 1,
    ALREADY_ADDED = 2
  };

  size_t mDiscreteDepth;          // depth of this root network
  bool mIsConnectable;            // true = can be connected to other LRNs
  tmArray<tmVertex*> mCCVertices; // vertices in the connected component
  tmArray<tmCrease*> mCCCreases;  // creases in the connected component
  tmArray<tmPoly*> mCCPolys;      // polys that have a crease in the cc
  tmArray<tmVertex*> mSTVertices; // spanning tree vertices
  tmArray<tmCrease*> mSTCreases;  // spanning tree creases
  tmArray<tmVertex*> mCC0;        // vertices of deg=0 in cc
  tmArray<tmVertex*> mCC1;        // vertices of deg=1 in cc
  tmArray<tmVertex*> mCC2ST1;     // vertices of deg=2 in cc, deg=1 in st
  tmArray<tmVertex*> mCC2ST2;     // vertices of deg=2 in cc, deg=2 in st

  tmRootNetwork(size_t discreteDepth);
  
  void TryAddVertexToConnectedComponent(tmVertex* aVertex);
  void TryAddCreaseToConnectedComponent(tmCrease* aCrease);
  void TryAddVertexToSpanningTree(tmVertex* aVertex);
  void TryAddCreaseToSpanningTree(tmCrease* aCrease);
  void BuildSpanningTree();
  void ClassifyVerticesByDegree();
  void ConnectFacetGraph();
  bool CanAbsorb(tmRootNetwork* aNetwork, tmVertex*& atVertex);
  void Absorb(tmRootNetwork* aNetwork, tmVertex* atVertex);
  void BreakOneLink();
  
#ifdef TMDEBUG
  void PutSelf(ostream& os);  // only used in debugging
#endif
};


/*****
Constructor
*****/
tmRootNetwork::tmRootNetwork(size_t discreteDepth)
   : mDiscreteDepth(discreteDepth), mIsConnectable(false)
 {
 }
 

/*****
Add a vertex to the connected component; then add all of its incident
hinge creases. If the vertex straddles a pseudohinge, its other vertex also
gets added.
*****/
void tmRootNetwork::TryAddVertexToConnectedComponent(tmVertex* aVertex) {
  TMASSERT(aVertex->mCCFlag != INELIGIBLE);
  if (aVertex->mCCFlag == ALREADY_ADDED) return;
  aVertex->mCCFlag = ALREADY_ADDED;
  mCCVertices.push_back(aVertex);
  for (size_t i = 0; i < aVertex->mCreases.size(); ++i)
    TryAddCreaseToConnectedComponent(aVertex->mCreases[i]);
  tmVertex* mateVertex = aVertex->mLeftPseudohingeMate;
  if (mateVertex) TryAddVertexToConnectedComponent(mateVertex);
  mateVertex = aVertex->mRightPseudohingeMate;
  if (mateVertex) TryAddVertexToConnectedComponent(mateVertex);

  // While normally polys get added to mCCPolys when a crease goes into the
  // connected component, if the vertex is a leaf node vertex, it's the root
  // node, and we'll need to add all of its incident polys to our list of polys
  // because there are no creases extending into the poly from the vertex. We
  // get the incident polys by looking at the owners of the incident ridge
  // creases.
  if (aVertex->mTreeNode->IsLeafNode()) {
    for (size_t i = 0; i < aVertex->mCreases.size(); ++i) {
      tmCrease* aCrease = aVertex->mCreases[i];
      if (!aCrease->IsRidgeCrease()) continue;
      tmPoly* thePoly = aCrease->GetOwnerAsPoly();
      TMASSERT(thePoly);
      mCCPolys.union_with(thePoly);
    }
  }
}


/*****
Try adding a crease to the connected component. After checking that it's a
hinge crease, we'll add it; we'll also add its polygon to our polygon list.
Then we'll add its vertices.
*****/
void tmRootNetwork::TryAddCreaseToConnectedComponent(tmCrease* aCrease)
{
  if (!aCrease->IsHingeCrease()) return;
  if (aCrease->mCCFlag == INELIGIBLE) return;
  if (aCrease->mCCFlag == ALREADY_ADDED) return;
  aCrease->mCCFlag = ALREADY_ADDED;
  mCCCreases.push_back(aCrease);
  tmPoly* thePoly = aCrease->GetOwnerAsPoly();
  TMASSERT(thePoly);
  mCCPolys.union_with(thePoly);
  TryAddVertexToConnectedComponent(aCrease->mVertices.front());
  TryAddVertexToConnectedComponent(aCrease->mVertices.back());
}


/*****
Add a vertex to the spanning tree. Then add all of its hinge creases to
the spanning tree.
*****/
void tmRootNetwork::TryAddVertexToSpanningTree(tmVertex* aVertex)
{
  TMASSERT(aVertex->mSTFlag != INELIGIBLE);
  if (aVertex->mSTFlag == ALREADY_ADDED) return;
  aVertex->mSTFlag = ALREADY_ADDED;
  mSTVertices.push_back(aVertex);
  for (size_t i = 0; i < aVertex->mCreases.size(); ++i)
    TryAddCreaseToSpanningTree(aVertex->mCreases[i]);
  tmVertex* mateVertex = aVertex->mLeftPseudohingeMate;
  if (mateVertex) TryAddVertexToSpanningTree(mateVertex);
  mateVertex = aVertex->mRightPseudohingeMate;
  if (mateVertex) TryAddVertexToSpanningTree(mateVertex);
}


/*****
Try adding a crease to the spanning tree. Unlike adding to the connected
component, we'll only add a crease if at least one of its vertices isn't
already part of the spanning tree.
*****/
void tmRootNetwork::TryAddCreaseToSpanningTree(tmCrease* aCrease)
{
  if (!aCrease->IsHingeCrease()) return;
  if (aCrease->mCCFlag == INELIGIBLE) return;
  if (aCrease->mSTFlag == ALREADY_ADDED) return;
  tmVertex* v1 = aCrease->mVertices.front();
  tmVertex* v2 = aCrease->mVertices.back();
  const bool c1 = (v1->mSTFlag == ALREADY_ADDED);
  const bool c2 = (v2->mSTFlag == ALREADY_ADDED);
  if (c1 && c2) return;
  aCrease->mSTFlag = ALREADY_ADDED;
  mSTCreases.push_back(aCrease);
  if (!c1) TryAddVertexToSpanningTree(v1);
  if (!c2) TryAddVertexToSpanningTree(v2);
}


/*****
Build the spanning tree for this connected component, by starting with the
first vertex in the connected component.
*****/
void tmRootNetwork::BuildSpanningTree()
{
  TryAddVertexToSpanningTree(mCCVertices.front());
}


/*****
Classify the axial vertices by degree within this connected component
and/or its spanning tree. This populates the arrays mCC0, mCC1, mCC2ST1, mCC2ST2.
In addition, we check whether any vertex can serve as a connection to another
polygon cluster and note the possibility in the flag mIsConnectable.
*****/
void tmRootNetwork::ClassifyVerticesByDegree()
{
  for (size_t i = 0; i < mCCVertices.size(); ++i) {
    tmVertex* theVertex = mCCVertices[i];
    if (!theVertex->IsAxialVertex()) continue;
    size_t ccDegree = theVertex->GetDegree(mCCCreases);
    size_t stDegree = theVertex->GetDegree(mSTCreases);
    if (ccDegree == 0) mCC0.push_back(theVertex);
    else if (ccDegree == 1) mCC1.push_back(theVertex);
    else if (ccDegree == 2)
      if (stDegree == 1) mCC2ST1.push_back(theVertex);
      else if (stDegree == 2) mCC2ST2.push_back(theVertex);
      else TMFAIL("ClassifyVerticesByDegree(): bad stDegree for ccDegree == 2");
    else TMFAIL("ClassifyVerticesByDegree(): bad ccDegree");
    size_t axDegree = theVertex->GetNumHingeCreases();
    mIsConnectable |= (axDegree == 2 && ccDegree == 1);
  }
}


/*****
Add connections to the facet ordering graph (FOG) that splice the pieces
surrounding this connected component into a single large loop (for depth>0
pieces) or a sortable graph (for the depth=0 piece). The difference is, in
the depth=0 piece, we leave out a single splice, which breaks the loop at a
place that leaves the graph with a single source and sink.
*****/
void tmRootNetwork::ConnectFacetGraph()
{
  // Vertices of degree 0 in the connected component. There will be either zero
  // or one of these, and if there is one, that means this constitutes the
  // entire depth-0 root network.
  TMASSERT(mCC0.size() <= 1);
  if (mCC0.size() == 1) {
    tmVertex* theVertex = mCC0.front();
    // Unlink across all ridge creases incident on the vertex.
    for (size_t i = 0; i < theVertex->mCreases.size(); ++i) {
      tmCrease* theCrease = theVertex->mCreases[i];
      if (theCrease->IsRidgeCrease()) {
        tmFacet::Unlink(theCrease->mFwdFacet, theCrease->mBkdFacet);
      }
    }
    // Re-link across all axial creases incident on the vertex (for a border
    // vertex) or all but one (for an interior vertex).
    bool needsSkip = !theVertex->IsBorderVertex();
    for (size_t i = 0; i < theVertex->mCreases.size(); ++i) {
      tmCrease* theCrease = theVertex->mCreases[i];
      if (!theCrease->IsBorderCrease() && theCrease->IsAxialCrease()) {
        if (needsSkip) needsSkip = false;
        else tmFacet::Link(theCrease->mFwdFacet, theCrease->mBkdFacet);
      }
    }
    // if this connected component had a degree-0 vertex, then it won't have
    // any more vertices, so we're done.
    return;
  }
  
  // Vertices of degree 2 in the connected component and of degree 2 in the
  // spanning tree. Swap the links around the vertex.
  for (size_t i = 0; i < mCC2ST2.size(); ++i) {
    tmVertex* theVertex = mCC2ST2[i];
    theVertex->SwapLinks();
  }
}


/*****
Return true if this network (which is the global root network) can absorb the
given network (which has a lower depth). This basically boils down to whether
the given network has a root vertex of degree 1 in the connected component that
is incident to an axial vertex of the global root network.
*****/
bool tmRootNetwork::CanAbsorb(tmRootNetwork* aNetwork, tmVertex*& atVertex)
{
  for (size_t i = 0; i < mCCPolys.size(); ++i) {
    // thePoly is a poly in the global root network
    tmPoly* thePoly = mCCPolys[i];
    // theVertex should be one of the non-root axial vertices of thePoly,
    // i.e., a potential point of attachment of the lower-depth network.
    // Since we don't have a list specifically of non-root axial vertices,
    // we'll have to take the long way. We'll go through all paths of the
    // poly (which own any non-corner axial vertices, which are all we're
    // interested in); then we'll go through the owned vertices of each path.
    // We'll still go through a bunch of vertices that have no chance of
    // being a point of attachment, but that can't be helped.
    for (size_t j = 0; j < thePoly->mRingPaths.size(); ++j) {
      tmPath* thePath = thePoly->mRingPaths[j];
      for (size_t k = 0; k < thePath->mOwnedVertices.size(); ++k) {
        tmVertex* theVertex = thePath->mOwnedVertices[k];
        if (theVertex->mDiscreteDepth != aNetwork->mDiscreteDepth) continue;
        if (!aNetwork->mCC1.contains(theVertex)) continue;
        atVertex = theVertex;
        return true;
      }
    }
  }
  return false;
}


/*****
Absorb the given network, rearranging connections at the given vertex. When we
absorb a network, we take over its polys (which are used for incidence testing:
see CanAbsorb()). We don't bother doing anything with the lists of vertices and
creases because we'll want the global root network to still know its own local
root vertices and creases.
*****/
void tmRootNetwork::Absorb(tmRootNetwork* aNetwork, tmVertex* atVertex)
{
  // Swap the links around the vertex being joined.
  atVertex->SwapLinks();
  
  // Take over all of the polys of the lower network.
  mCCPolys.union_with(aNetwork->mCCPolys);
}


/*****
After the global root network has absorbed all other local root networks, we
break a single link in the large cycle of the facet ordering graph to turn it
into a proper sortable graph.
*****/
void tmRootNetwork::BreakOneLink()
{
  // If the root node is a leaf node of the tree, then the original root
  // network is only a single vertex; in this case, the facet ordering graph is
  // already sortable and we don't have to do anything.
  if (mCC0.not_empty()) return;
  
  // But if the root node was a branch node, then we need to find one link to
  // break. We'll look through the lists of local root vertices in the global
  // root network, which are all of depth 0 and thus are the only vertices
  // eligible for the break.
  
  // Look first at local root vertices of degree 1 in the connected component.
  // These will be vertices on the border of the paper. They could be adjacent
  // to pseudohinges, so we'll have to go to the first non-pseudohinge facets
  // to either side of the crease to unlink.
  if (mCC1.not_empty()) {
    tmVertex* theVertex = mCC1.front();
    tmCrease* theCrease = theVertex->GetHingeCrease();
    tmFacet::Unlink(theCrease->GetLeftNonPseudohingeFacet(), 
      theCrease->GetRightNonPseudohingeFacet());
    return;
  }
  
  // If we didn't find one, then we'll have to look at the vertices of degree
  // 2. There must be at least one vertex of degree 2 in the connected
  // component and degree 1 in the spanning tree; if they were all of degree 2,
  // the spanning tree would be a loop. We'll choose one of those to break.
  TMASSERT(mCC2ST1.not_empty());
  tmVertex* theVertex = mCC2ST1.front();
  tmCrease *crease1, *crease2;
  theVertex->GetHingeCreases(crease1, crease2);
  tmFacet::Unlink(crease1->mFwdFacet, crease1->mBkdFacet);
}


#ifdef __MWERKS__
  #pragma mark -
#endif


/*****
Put this local root network to a stream in text format. Useful in debugging.
*****/
#ifdef TMDEBUG
void tmRootNetwork::PutSelf(ostream& os)
{
  os << "  discrete depth = " << mDiscreteDepth << endl;
  os << "  is connectable = " << mIsConnectable << endl;
  os << "     CC vertices = " << mCCVertices.GetStr() << endl;
  os << "      CC creases = " << mCCCreases.GetStr() << endl;
  os << "        CC polys = " << mCCPolys.GetStr() << endl;
  os << "     ST vertices = " << mSTVertices.GetStr() << endl;
  os << "      ST creases = " << mSTCreases.GetStr() << endl;
  os << "    CC0 vertices = " << mCC0.GetStr() << endl;
  os << "    CC1 vertices = " << mCC1.GetStr() << endl;
  os << "CC2 ST1 vertices = " << mCC2ST1.GetStr() << endl;
  os << "CC2 ST2 vertices = " << mCC2ST2.GetStr() << endl;
}
#endif // TMDEBUG


#ifdef __MWERKS__
  #pragma mark -
#endif


/**********
class tmTree
The core class of TreeMaker. It contains the full mathematical model.
**********/

/*****
First part of the facet order graph calculation. Construct the array of local
root networks, which entails calculating the local facet order within each
poly, identifying the local root vertices and creases, and then building up the
local root networks, which are the connected components across polys of
vertices and creases that are local root within their molecule. We then compute
the spanning tree of each root network and wrap up by classifying all the
vertices in the network according to their degree in various graphs.
*****/
void tmTree::CalcRootNetworks(tmArray<tmRootNetwork*>& rootNetworks)
{
  TMASSERT(rootNetworks.empty());
  
  // Start by (re)constructing the ordering graph for every poly viewed in
  // isolation; this will give a collection of disconnected pieces of the graph.
  // It also constructs the local root vertices and creases for each poly.
  for (size_t i = 0; i < mOwnedPolys.size(); ++i)
    mOwnedPolys[i]->CalcLocalFacetOrder();
  
  // Collect all the local root hinges across all polys and their vertices.
  tmArray<tmVertex*> localRootVertices;
  tmArray<tmCrease*> localRootCreases;
  for (size_t i = 0; i < mOwnedPolys.size(); ++i) {
    tmPoly* thePoly = mOwnedPolys[i];
    localRootVertices.union_with(thePoly->mLocalRootVertices);
    localRootCreases.union_with(thePoly->mLocalRootCreases);
  }

  // Reset the flags used in construction of local root network connected
  // components and spanning trees. 
  for (size_t i = 0; i < mVertices.size(); ++i) {
    tmVertex* theVertex = mVertices[i];
    theVertex->mCCFlag = tmRootNetwork::INELIGIBLE;
    theVertex->mSTFlag = tmRootNetwork::INELIGIBLE;
  }
  for (size_t i = 0; i < mCreases.size(); ++i) {
    tmCrease* theCrease = mCreases[i];
    theCrease->mCCFlag = tmRootNetwork::INELIGIBLE;
    theCrease->mSTFlag = tmRootNetwork::INELIGIBLE;
  }
  
  // Now mark just the local root vertices and creases as NOT_YET; any vertex
  // or crease that's not a local root vertex or crease will be left as
  // INELIGIBLE.
  for (size_t i = 0; i < localRootVertices.size(); ++i) {
    tmVertex* theVertex = localRootVertices[i];
    theVertex->mCCFlag = tmRootNetwork::NOT_YET;
    theVertex->mSTFlag = tmRootNetwork::NOT_YET;
  }
  for (size_t i = 0; i < localRootCreases.size(); ++i) {
    tmCrease* theCrease = localRootCreases[i];
    theCrease->mCCFlag = tmRootNetwork::NOT_YET;
    theCrease->mSTFlag = tmRootNetwork::NOT_YET;
  }
  
  // Create the connected components of local root hinges and vertices. Go
  // through each vertex and make sure it's either in an existing tmRootNetwork
  // or start a new one.
  for (size_t i = 0; i < localRootVertices.size(); ++i) {
    tmVertex* theVertex = localRootVertices[i];
    bool alreadyRecorded = false;
    for (size_t i = 0; i < rootNetworks.size(); ++i)
      if (rootNetworks[i]->mCCVertices.contains(theVertex)) {
        alreadyRecorded = true;
        break;
      }
    if (alreadyRecorded) continue;
    tmRootNetwork* newRootNetwork = 
      new tmRootNetwork(theVertex->GetDiscreteDepth());
    newRootNetwork->TryAddVertexToConnectedComponent(theVertex);
    rootNetworks.push_back(newRootNetwork);
  }

  // For each, construct its spanning tree.
  for (size_t i = 0; i < rootNetworks.size(); ++i)
    rootNetworks[i]->BuildSpanningTree();

  // For each connected component of local root hinges, constructs lists of
  // the local axial root vertices according to their degree within the CC and
  // within the ST.
  for (size_t i = 0; i < rootNetworks.size(); ++i)
    rootNetworks[i]->ClassifyVerticesByDegree();
}


/*****
Return the vertices and creases of the non-connectable local root network in
the passed arrays. Called by GetCPStatus().
*****/
void tmTree::CalcWhyNotLocalRootConnectable(tmArray<tmVertex*>& badVertices, 
  tmArray<tmCrease*>& badCreases)
{
  badVertices.clear();
  badCreases.clear();
  tmArray<tmRootNetwork*> rootNetworks;
  CalcRootNetworks(rootNetworks);
  tmRootNetwork* zeroDepthNetwork = 0;
  for (size_t i = 0; i < rootNetworks.size(); ++i) {
    tmRootNetwork* theRootNetwork = rootNetworks[i];
    if (theRootNetwork->mDiscreteDepth == 0)
      if (!zeroDepthNetwork) 
        zeroDepthNetwork = theRootNetwork;
      else {
        // oh-oh, there are two (or more) zero-depth root networks.
        badVertices.union_with(theRootNetwork->mCCVertices);
        badCreases.union_with(theRootNetwork->mCCCreases);
        badVertices.union_with(zeroDepthNetwork->mCCVertices);
        badCreases.union_with(zeroDepthNetwork->mCCCreases);
      }
    else if (!theRootNetwork->mIsConnectable) {
      badVertices.union_with(theRootNetwork->mCCVertices);
      badCreases.union_with(theRootNetwork->mCCCreases);
    }
  }
  for (size_t i = 0; i < rootNetworks.size(); ++i)
    delete rootNetworks[i];
}


/*****
Build the complete facet order, by adding additional ordering links between
facets in different pieces of the facet ordering graph, both within molecules
and across different molecules.
*****/
void tmTree::CalcFacetOrder()
{
  // Construct the local root networks, which are the connected components of
  // the graphs composed of local root vertices and creases.
  tmArray<tmRootNetwork*> rootNetworks;
  CalcRootNetworks(rootNetworks);
  
  // At this point, local root networks and associated spanning trees are
  // calculated, but before we can go any farther we have to verify that all
  // depth>0 LRNs are connectable and the depth-0 LRN is unique.
  size_t numDepthZero = 0;
  mIsLocalRootConnectable = true;
  for (size_t i = 0; i < rootNetworks.size(); ++i) {
    tmRootNetwork* theRootNetwork = rootNetworks[i];
    if (theRootNetwork->mDiscreteDepth == 0) 
      ++numDepthZero;
    else if (!theRootNetwork->mIsConnectable) {
      mIsLocalRootConnectable = false;
      continue;
    }
  }
  mIsLocalRootConnectable &= (numDepthZero == 1);
  if (!mIsLocalRootConnectable) {
    for (size_t j = 0; j < rootNetworks.size(); ++j)
      delete rootNetworks[j];
    return;
  }

  // Now, for each connected component, connect the bits of the facet ordering
  // graph into one connected sortable graph (for depth-0) and loop graph (for
  // all other depths).
  for (size_t i = 0; i < rootNetworks.size(); ++i)
    rootNetworks[i]->ConnectFacetGraph();
  
  // Find the depth-0 component and take it out of our list. This is the global
  // root network, and it will absorb all the other pieces to create the full
  // network.
  tmRootNetwork* globalRootNetwork = 0;
  for (size_t i = 0; i < rootNetworks.size(); ++i) {
    tmRootNetwork* theRootNetwork = rootNetworks[i];
    if (theRootNetwork->mDiscreteDepth == 0) {
      globalRootNetwork = theRootNetwork;
      rootNetworks.erase_remove(theRootNetwork);
      break;
    }
  }
  TMASSERT(globalRootNetwork);
  
// DEBUGGING -- dump each local root network to a file
//   ofstream fout("FACET_ORDER_DUMP.TXT");
//   fout << "globalRootNetwork" << endl;
//   globalRootNetwork->PutSelf(fout);
//   for (size_t i = 0; i < rootNetworks.size(); ++i) {
//     fout << "rootNetworks[" << i << "]" << endl;
//     rootNetworks[i]->PutSelf(fout);
//   }
// END DEBUGGING

  // Now, one by one, let the global root network absorb other pieces that are
  // incident upon it.
  while (rootNetworks.not_empty()) {
    bool absorbedOne = false;
    for (size_t i = 0; i < rootNetworks.size(); ++i) {    
      tmRootNetwork* theRootNetwork = rootNetworks[i];
      tmVertex* atVertex;
      if (globalRootNetwork->CanAbsorb(theRootNetwork, atVertex)) {
        globalRootNetwork->Absorb(theRootNetwork, atVertex);
        rootNetworks.erase_remove(theRootNetwork);
        delete theRootNetwork;
        absorbedOne = true;
        break;
      }
    }
// DEBUGGING
//     if (!absorbedOne) {
//       ofstream fout("ABSORPTION_FAILURE_DUMP.TXT");
//       fout << "globalRootNetwork" << endl;
//       globalRootNetwork->PutSelf(fout);
//       for (size_t i = 0; i < rootNetworks.size(); ++i) {
//         fout << "rootNetworks[" << i << "]" << endl;
//         rootNetworks[i]->PutSelf(fout);
//       }
//     }
// END DEBUGGING
    TMASSERT(absorbedOne);
  }
  
  // Last, we break a single link in the giant facet ordering graph, which makes
  // it sortable (if it wasn't already).
  globalRootNetwork->BreakOneLink();
    
  // At this point, we should have one big network in globalRootNetwork with
  // a single source and sink; in other words, it's the complete facet ordering
  // graph. We no longer need the globalRootNetwork data structure, so we'll
  // get rid of it.
  delete globalRootNetwork;
  
  // Now the facet ordering graph is complete. There is a single source and a
  // single sink. We'll use this graph to assign an order value to every facet
  // in the crease pattern. We can then compare the order values for any two
  // facets to determine their relative order within the graph.
  
  // Find the source facet. And while we're here, initialize the mOrder for
  // each facet.
  tmFacet* sourceFacet = 0;
  for (size_t i = 0; i < mFacets.size(); ++i) {
    tmFacet* theFacet = mFacets[i];
    theFacet->mOrder = size_t(-1);
    if (theFacet->IsSourceFacet()) {
      TMASSERT(!sourceFacet);
      sourceFacet = theFacet;
    }
  }
    
  // Now start at the source facet and assign a facet order number to every
  // facet, using each number once, and insuring that for any two facets that
  // are reachable, the direction of the path connecting them in the facet
  // ordering graph goes from smaller number to larger number.
  size_t nextOrder = 0;
  sourceFacet->CalcOrder(nextOrder);
  
  // We're done. The facets are fully ordered. The ordering function for any
  // pair of facets can be evaluated in constant time by comparing the order
  // values for the two facets, no matter how far separated they are in the 
  // facet ordering graph.
}
