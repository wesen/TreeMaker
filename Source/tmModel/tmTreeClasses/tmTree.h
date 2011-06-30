/*******************************************************************************
File:         tmTree.h
Project:      TreeMaker 5.x
Purpose:      Header file for class tmTree
Author:       Robert J. Lang
Modified by:  
Created:      2003-11-21
Copyright:    ©2003 Robert J. Lang. All Rights Reserved.
*******************************************************************************/

#ifndef _TMTREE_H_
#define _TMTREE_H_

// Standard TreeMaker header
#include "tmHeader.h"

// Standard libraries
#include <iostream>

// TreeMaker classes
#include "tmModel_fwd.h"
#include "tmPart.h"
#include "tmCluster.h"
#include "tmEdgeOwner.h"
#include "tmPolyOwner.h"
#include "tmFacetOwner.h"
#include "tmConditionOwner.h"
#include "tmPoint.h"
#include "tmArray.h"
#include "tmArrayIterator.h"
#include "tmCondition.h"


/**********
class tmTree
The core class of TreeMaker. It contains the full mathematical model.
**********/

class tmTree : public tmPart, private tmCluster, public tmEdgeOwner, 
  public tmPolyOwner, public tmConditionOwner {
public:
  // Exceptions
  // Structural editing errors
  class EX_BAD_KILL_PARTS {
    // attempted KillSomeNodesAndEdges would break the tree
  };
  class EX_BAD_SPLIT_EDGE {
    // requested location was outside of the edge
  };
  class EX_BAD_ABSORB_NODE {
    // requested node was not eligible for absorption
  };
  class EX_BAD_REMOVE_STUB {
    // requested edge was not truly a stub
  };
  class EX_IO_UNRECOGNIZED_CONDITION {
    // file included unfamiliar conditions
    public:
    std::size_t mNumMissed;         // number encountered
    EX_IO_UNRECOGNIZED_CONDITION(std::size_t numMissed) : 
      mNumMissed(numMissed) {};
  };
  
  // stream I/O errors (see also tmPart exceptions)
  class EX_IO_BAD_TREE_TAG : public EX_IO_BAD_TOKEN {
    // the first "tree" tag isn't there.
    public:
    EX_IO_BAD_TREE_TAG(const std::string& token) : 
      EX_IO_BAD_TOKEN(token) {};
  };    
  class EX_IO_BAD_TREE_VERSION : public EX_IO_BAD_TOKEN {
    // bad file version number
    public:
    EX_IO_BAD_TREE_VERSION(const std::string& token) : 
      EX_IO_BAD_TOKEN(token) {};
  };    
  class EX_IO_BAD_REF_INDEX : public EX_IO_BAD_TOKEN {
    // reference index for nonexistent part
    public:
    EX_IO_BAD_REF_INDEX(const std::string& token) : 
      EX_IO_BAD_TOKEN(token) {};
  };    

  // Ctor/dtor
  void InitTree();
  tmTree();
  tmTree(tmTree* aTree);
  ~tmTree();
  tmTree* Clone();
  
  // Getters
  const tmFloat& GetPaperWidth() const {
    // Return the width of the paper (typically 1).    
    return mPaperWidth;
  };
    
  const tmFloat& GetPaperHeight() const {
    // Return the height of the paper (the aspect ratio if width = 1).    
    return mPaperHeight;
  };
    
  const tmPoint GetPaperSize() const {
    // Return the paper size as a point (width, height).
    return tmPoint(mPaperWidth, mPaperHeight);
  };
    
  const tmFloat& GetScale() const {
    // Return the scale, the ratio between paper units and the tree units
    return mScale;
  };
    
  bool HasSymmetry() const {
    // Return true if there is a line of mirror symmetry.
    return mHasSymmetry;
  };
    
  const tmPoint& GetSymLoc() const {
    // Return a point on the line of mirror symmetry.
    return mSymLoc;
  };
    
  const tmFloat& GetSymLocX() const {
    // Return the x-coordinate of the point on the line of mirror symmetry.
    return mSymLoc.x;
  };
    
  const tmFloat& GetSymLocY() const {
    // Return the y-coordinate of the point on the line of mirror symmetry.
    return mSymLoc.y;
  };
    
  const tmFloat& GetSymAngle() const {
    // Return the angle in degrees of the line of mirror symmetry.
    return mSymAngle;
  };
  
  tmPoint GetSymDir() const;
  
  bool IsFeasible() const {
    // Return true if the tree is feasible, i.e., no path conditions are
    // violated.
    return mIsFeasible;
  };
    
  bool IsPolygonValid() const {
    // Return true if the tree is polygon valid, i.e., the convex hull of the
    // leaf nodes is entirely filled with valid polygons.
    return mIsPolygonValid;
  };
  
  bool IsPolygonFilled() const {
    // Return true if all of the polygons in the tree are filled with subpolys
    // and creases.
    return mIsPolygonFilled;
  };
  
  bool IsVertexDepthValid() const {
    // Return true if the tree has valid depth data assigned to all vertices.
    return mIsVertexDepthValid;
  };
  
  bool IsFacetDataValid() const {
    // Return true if the tree has valid facet structure, which allows the
    // construction of facet order and coloring.
    return mIsFacetDataValid;
  };
  
  bool IsLocalRootConnectable() const {
    // Return true if each local root connected component (not including the
    // global root connected component) is connected to at least one hinge
    // crease that is itself not part of the LRCC. This is essential for being
    // able to splice local root networks together during the construction of
    // the facet order graph.
    return mIsLocalRootConnectable;
  };
  
  // Getters for lists of parts
  
  const tmDpptrArray<tmNode>& GetNodes() {
    // Get all nodes in the tree, including both tree nodes and subnodes, which
    // are owned by subpolys).
    return mNodes;
  };
  
  const tmDpptrArray<tmEdge>& GetEdges() {
    // Get all edges in the tree.
    return mEdges;
  };
  
  const tmDpptrArray<tmPath>& GetPaths() {
    // Get all paths in the tree, including both tree paths and subpaths, which
    // are owned by polys.
    return mPaths;
  };
  
  const tmDpptrArray<tmPoly>& GetPolys() {
    // Get all polys in the tree, including both tree polys and subpolys, which
    // are owned by tree polys.
    return mPolys;
  };
  
  const tmDpptrArray<tmVertex>& GetVertices() {
    // Get all vertices in the tree, including both node-owned and path-owned
    // vertices.
    return mVertices;
  };
  
  const tmDpptrArray<tmCrease>& GetCreases() {
    // Get all creases in the tree, including both path-owned and poly-owned
    // creases.
    return mCreases;
  };
  
  const tmDpptrArray<tmFacet>& GetFacets() {
    // Get all facets in the tree; facets are owned by the tree polys.
    return mFacets;
  };
  
  const tmDpptrArray<tmCondition>& GetConditions() {
    // Get all conditions in the tree.
    return mConditions;
  };
  
  template <class P>
    const tmDpptrArray<P>& GetParts() {
      // Get all parts of the given type.
      return tmCluster::GetParts<P>();
    };
    
  template <class P>
    std::size_t GetNumParts() {
      // Get the number of parts of the given type.
      return tmCluster::GetNumParts<P>();
    };
    
  const tmCluster& GetCluster() {
    // Return a read-only reference to the original cluster that this tree is
    // based on.
    return *this;
  };
  
  // Debugging support
#ifdef TMDEBUG
  bool GetQuitCleanupEarly() const {
    // In debugging builds, we can set the mQuitCleanupEarly flag, which tells
    // tmTree::CleanupAfterEdit() to exit before attempting to calculate facet
    // order (the source of many bugs); this lets us examine the CP with and
    // without attempting facet order and not have to rebuild in between.
    return mQuitCleanupEarly;
  }
#endif // TMDEBUG

  // Named queries about size
  std::size_t GetNumNodes() const {return mNodes.size();};
  std::size_t GetNumEdges() const {return mEdges.size();};
  std::size_t GetNumPaths() const {return mPaths.size();};
  std::size_t GetNumPolys() const {return mPolys.size();};
  std::size_t GetNumVertices() const {return mVertices.size();};
  std::size_t GetNumCreases() const {return mCreases.size();};
  std::size_t GetNumConditions() const {return mConditions.size();};
  std::size_t GetNumEditableParts() const;
  
  // Setters
  void SetPaperWidth(const tmFloat& aWidth);
  void SetPaperHeight(const tmFloat& aHeight);
  void SetScale(const tmFloat& aScale);
  void SetHasSymmetry(bool aHasSymmetry);
  void SetSymLoc(const tmPoint& aSymLoc);
  void SetSymLocX(const tmFloat& aSymLocX);
  void SetSymLocY(const tmFloat& aSymLocY);
  void SetSymAngle(const tmFloat& aSymAngle);
  void SetSymmetry(const tmPoint& aSymLoc, const tmFloat& aSymAngle);
  
  // Debugging support
#ifdef TMDEBUG
  void SetQuitCleanupEarly(bool quitCleanupEarly) {
    mQuitCleanupEarly = quitCleanupEarly;
  }
#endif // TMDEBUG

  // Misc queries
  bool HasRedundantNodes() const;
  bool HasStrainedEdges() const;
  bool HasNodeConditions() const;
  bool HasEdgeConditions() const;
  bool HasPathConditions() const;
  bool HasConditions() const;
  bool HasNonTrianglePoly() const;
  tmNode* GetRootNode() const;
  tmEdge* GetEdge(const tmNode* node1, const tmNode* node2) const;
  tmPath* GetPath(const tmNode* node1, const tmNode* node2) const;
  tmPath* GetLeafPath(const tmNode* leafNode1, const tmNode* leafNode2) const;
  std::size_t GetNumLeafNodes() const;
  std::size_t GetNumMovableParts(std::size_t& numNodes, 
   std::size_t& numEdges) const;
  std::size_t GetNumMovableParts() const;
  tmCrease* GetCrease(tmFacet* facet1, tmFacet* facet2) const;
  bool CanGetCorridorFacets() const;
  void GetCorridorFacets(const tmArray<tmEdge*>& edgeList, 
    tmArray<tmFacet*>& facetList) const;

  // Test structures
  static tmTree* MakeTreeBlank();
  static tmTree* MakeTreeUnoptimized();
  static tmTree* MakeTreeOptimized();
  static tmTree* MakeTreeGusset();
  static tmTree* MakeTreeConditioned();

  // Topological modification
  void AddNode(tmNode* fromNode, const tmPoint& where, 
    tmNode*& newNode, tmEdge*& newEdge);
  void SplitEdge(tmEdge* aEdge, const tmFloat& splitLoc, tmNode*& newNode);
  bool CanAbsorbNode(tmNode* aNode) const;
  void AbsorbNode(tmNode* aNode, tmEdge*& newEdge);
  bool CanAbsorbNodes(const tmArray<tmNode*>& nodeList) const;
  void AbsorbNodes(const tmArray<tmNode*>& nodeList);
  bool CanAbsorbRedundantNodes() const;
  void AbsorbRedundantNodes();
  bool CanAbsorbEdge(tmEdge* aEdge) const;
  void AbsorbEdge(tmEdge* aEdge);
  bool CanAbsorbEdges(const tmArray<tmEdge*>& edgeList) const;
  void AbsorbEdges(const tmArray<tmEdge*>& edgeList);
  void AddStub(tmEdge* aEdge, const tmFloat& length, const tmFloat& edgeLoc, 
    const tmPoint& loc);
  void RemoveStub(tmEdge* aEdge, tmEdge*& newEdge);

  void KillSomeNodesAndEdges(tmArray<tmNode*>& markedNodes, 
    tmArray<tmEdge*>& markedEdges);
  void KillSomePolys(tmArray<tmPoly*>& markedPolys);
  void KillSomeConditions(tmArray<tmCondition*>& markedConditions);
  void KillNodeConditions();
  void KillEdgeConditions();
  void KillPathConditions();
  void KillConditions();
  void KillSomeParts(tmArray<tmNode*>& markedNodes, 
    tmArray<tmEdge*>& markedEdges, tmArray<tmCondition*>& markedConditions);
  void KillCreasePattern();
  void KillPolysAndCreasePattern();
  void KillAllParts();

  // Dimensional modification of parts
  bool CanMakeNodeRoot(tmNode* aNode) const;
  void MakeNodeRoot(tmNode* aNode);
  template <class P>
    void SetPartIndex(P* p, std::size_t n);
  void SetConditionIndex(tmCondition* c, std::size_t n);
  void SetNodeLocs(const tmArray<const tmNode*>& movingNodes,
   const tmArray<tmPoint>& newLocs);
  void SetEdgeLengths(tmArray<tmEdge*>& aEdgeList, tmFloat aLength);
  void ScaleEdgeLengths(const tmArray<tmEdge*>& aEdgeList, 
    const tmFloat& factor);
  void ScaleTree(tmFloat scaleFactor);
  void RenormalizeToUnitEdge(tmEdge* aEdge);
  void RenormalizeToUnitPath(tmPath* aPath);
  void RenormalizeToUnitScale();
  void RemoveStrain(tmArray<tmEdge*>& aEdgeList);
  void RemoveAllStrain();
  void RelieveStrain(tmArray<tmEdge*>& aEdgeList);
  void RelieveAllStrain();
  void PerturbNodes(const tmArray<tmNode*>& aNodeList);
  bool CanPerturbAllNodes() const;
  void PerturbAllNodes();
  
  // Queries that build lists of parts
  void GetLeafNodes(tmArray<tmNode*>& aNodeList);
  void GetBorderNodes(tmArray<tmNode*>& aNodeList);
  void GetLeafPaths(tmArray<tmPath*>& aPathList);
  void GetSpanningEdges(const tmArray<tmNode*>& aNodeList, 
    tmArray<tmEdge*>& aEdgeList);
  void GetSpanningPaths(const tmArray<tmNode*>& aNodeList, 
    tmArray<tmPath*>& aPathList);
  template <class C, class P>
    void GetAffectingConditions(P* const p, 
      tmArray<C*>& aConditionList);
  
  // Filter-in-place on lists
  void FilterLeafNodes(tmDpptrArray<tmNode>& aNodeList);
  void FilterMovableParts(tmDpptrArray<tmNode>& aNodeList, 
    tmDpptrArray<tmEdge>& aEdgeList);
  
  // tmCondition queries about parts
  template <class C, class P>
    bool IsConditioned(P* p);
    
  // tmCondition creation. You can only create conditions through these
  // routines.
  template <class C, class P>
    C* MakeOnePartCondition(P* p);
  template <class C, class P>
    C* GetOrMakeOnePartCondition(P* p);
  template <class C, class P>
    void MakeOnePartCondition(const tmArray<P*>& plist, tmArray<C*>& clist);
  template <class C, class P>
    void GetOrMakeOnePartCondition(const tmArray<P*>& plist, tmArray<C*>& clist);
  template <class C, class P>
    C* MakeTwoPartCondition(P* p1, P* p2);
  template <class C, class P>
    C* GetOrMakeTwoPartCondition(P* p1, P* p2);
  template <class C, class P>
    C* MakeThreePartCondition(P* p1, P* p2, P* p3);
  template <class C, class P>
    C* GetOrMakeThreePartCondition(P* p1, P* p2, P* p3);
    
  // Setting conditions on lists of parts
  void SetNodesFixedToSymmetryLine(const tmArray<tmNode*>& nodes);
  void SetNodesFixedToPaperEdge(const tmArray<tmNode*>& nodes);
  void SetNodesFixedToPaperCorner(const tmArray<tmNode*>& nodes);
  void SetNodesFixedToPosition(const tmArray<tmNode*>& nodes, bool xFixed,
    tmFloat xFixValue, bool yFixed, tmFloat yFixValue);
  void SetNodesFixedToSymmetryLinev4(const tmArray<tmNode*>& nodes);
  void SetNodesFixedToPaperEdgev4(const tmArray<tmNode*>& nodes);
  void SetNodesFixedToPaperCornerv4(const tmArray<tmNode*>& nodes);
  void SetNodesFixedToPositionv4(const tmArray<tmNode*>& nodes, bool xFixed,
    tmFloat xFixValue, bool yFixed, tmFloat yFixValue);
  void SetEdgesSameStrain(const tmArray<tmEdge*>& edges);
  void SetPathsActive(const tmArray<tmPath*>& paths);
  void SetPathsAngleFixed(const tmArray<tmPath*>& paths, tmFloat angle);
  void SetPathsAngleQuant(const tmArray<tmPath*>& paths, std::size_t quant,
    tmFloat QuantOffset);
  void SetPathsActivev4(const tmArray<tmPath*>& paths);
  void SetPathsAngleFixedv4(const tmArray<tmPath*>& paths, tmFloat angle);
  void SetPathsAngleQuantv4(const tmArray<tmPath*>& paths, std::size_t quant,
    tmFloat QuantOffset);
  
  // Polygon/crease pattern construction
  void BuildTreePolys();
  void BuildPolysAndCreasePattern();
  bool HasFullCP();
  enum CPStatus {
    HAS_FULL_CP = 0,
    EDGES_TOO_SHORT,
    POLYS_NOT_VALID,
    POLYS_NOT_FILLED,
    POLYS_MULTIPLE_IBPS,
    VERTICES_LACK_DEPTH,
    FACETS_NOT_VALID,
    NOT_LOCAL_ROOT_CONNECTABLE
  };
  CPStatus GetCPStatus(tmArray<tmEdge*>& badEdges, tmArray<tmPoly*>& badPolys, 
    tmArray<tmVertex*>& badVertices, tmArray<tmCrease*>& badCreases, 
    tmArray<tmFacet*>& badFacets);
  
  // Stream I/O (both memory and files)
  void PutSelf(std::ostream& os);
  void GetSelf(std::istream& is);
  void Exportv4(std::ostream& os);

private:
  // User-settable data
  tmFloat mPaperWidth;
  tmFloat mPaperHeight;
  tmFloat mScale;
  bool mHasSymmetry;
  tmPoint mSymLoc;
  tmFloat mSymAngle;
  
  // Set in cleanup
  bool mIsFeasible;
  bool mIsPolygonValid;
  bool mIsPolygonFilled;
  bool mIsVertexDepthValid;
  bool mIsFacetDataValid;
  bool mIsLocalRootConnectable;
  bool mNeedsCleanup;

  // Ownership
  tmTree* NodeOwnerAsTree() {return this;};
  tmPoly* NodeOwnerAsPoly() {return 0;};
  tmTree* PathOwnerAsTree() {return this;};
  tmPoly* PathOwnerAsPoly() {return 0;};
  tmTree* PolyOwnerAsTree() {return this;};
  tmPoly* PolyOwnerAsPoly() {return 0;};

  // Debugging flags (used for runtime switching)
#ifdef TMDEBUG
  bool mQuitCleanupEarly;
#endif // TMDEBUG
  
  // Instantaneous index of a part -- only used in debugging
#ifdef TMDEBUG
  template <class P>
    std::size_t GetInstantIndex(P* const p) const;
#endif // TMDEBUG
  
  // Stream I/O support
  void Putv5Self(std::ostream& os);
  void Getv5Self(std::istream& is);
  void Putv5Condition(std::ostream& os, tmCondition* aCondition);
  void Makev5Condition(std::istream& is);
  
  void Putv4Self(std::ostream& os);
  void Getv4Self(std::istream& is);
  void Putv4Condition(std::ostream& os, tmCondition* aCondition);
  void Makev4Condition(std::istream& is);
  
  void Getv3Self(std::istream& is);
  
  // Stream I/O for ptrs & related references
  template <class P>  
    void PutPtr(std::ostream& os, P* const p);
  template <class P>  
    void GetPtr(std::istream& is, P*& p, bool canFail = false);
  template <class P>  
    void PutPtr(std::ostream& os, const tmDpptr<P>& p);
  template <class P>  
    void GetPtr(std::istream& is, tmDpptr<P>& p, bool canFail = false);
    template <class P>  
    void PutPtrArray(std::ostream& os, const tmArray<P*>& plist);
  template <class P>  
    void GetPtrArray(std::istream& is, tmArray<P*>& plist);
template <class P>  
    void PutPtrArray(std::ostream& os, const tmDpptrArray<P>& plist);
  template <class P>  
    void GetPtrArray(std::istream& is, tmDpptrArray<P>& plist);

  // Stream I/O for (polymorphic) ptr-to-something-Owners
  void PutOwnerPtr(std::ostream& os, tmNodeOwner* const aNodeOwner);
  void GetOwnerPtr(std::istream& is, tmNodeOwner*& aNodeOwner);

  void PutOwnerPtr(std::ostream& os, tmEdgeOwner* const aEdgeOwner);
  void GetOwnerPtr(std::istream& is, tmEdgeOwner*& aEdgeOwner);

  void PutOwnerPtr(std::ostream& os, tmPathOwner* const aPathOwner);
  void GetOwnerPtr(std::istream& is, tmPathOwner*& aPathOwner);

  void PutOwnerPtr(std::ostream& os, tmPolyOwner* const aPolyOwner);
  void GetOwnerPtr(std::istream& is, tmPolyOwner*& aPolyOwner);

  void PutOwnerPtr(std::ostream& os, tmVertexOwner* const aVertexOwner);
  void GetOwnerPtr(std::istream& is, tmVertexOwner*& aVertexOwner);

  void PutOwnerPtr(std::ostream& os, tmCreaseOwner* const aCreaseOwner);
  void GetOwnerPtr(std::istream& is, tmCreaseOwner*& aCreaseOwner);

  void PutOwnerPtr(std::ostream& os, tmFacetOwner* const aFacetOwner);
  void GetOwnerPtr(std::istream& is, tmFacetOwner*& aFacetOwner);

  // Class tag for stream I/O
  TM_DECLARE_TAG()
    
  // Support for CleanupAfterEdit()
  template <class P>
    void RenumberParts();
  void CalcBorderNodesAndPaths(tmArray<tmNode*>& leafNodes);
  void CalcPinnedNodesAndEdges(tmArray<tmNode*>& leafNodes, 
    tmArray<tmPath*>& leafPaths);
  void CalcPolygonNetwork(tmArray<tmNode*>& leafNodes, 
    tmArray<tmPath*>& leafPaths);
  void CalcPolygonValidity(tmArray<tmNode*>& leafNodes);
  void KillOrphanVerticesAndCreases();
  void CalcPartIndices();
  void CalcPolygonFilled();
  void CalcDepthAndBend();
  void CalcVertexDepthValidity();
  void CalcFacetDataValidity();
  void CalcFacetColor();
  void CalcFacetCorridorEdges();
  void CalcRootNetworks(tmArray<tmRootNetwork*>& rootNetworks);
  void CalcWhyNotLocalRootConnectable(tmArray<tmVertex*>& badVertices, 
    tmArray<tmCrease*>& badCreases);
  void CalcFacetOrder();
  void CalcFoldDirections();
  void CleanupAfterEdit();
  
  // Hide ancestor functions
  void ClearAllParts();
  
  // Friend classes
  friend class tmPart;
  friend class tmPart::StringT<tmTree>;
  friend class tmPart::CreatorFnT<tmTree>;
  friend class tmTreeCleaner;
  friend class tmNode;
  friend class tmNodeOwner;
  friend class tmEdge;
  friend class tmEdgeOwner;
  friend class tmPath;
  friend class tmPathOwner;
  friend class tmPoly;
  friend class tmPolyOwner;
  friend class tmVertex;
  friend class tmVertexOwner;
  friend class tmCrease;
  friend class tmCreaseOwner;
  friend class tmFacet;
  friend class tmFacetOwner;
  friend class tmCondition;
  friend class tmConditionNodeCombo;
  friend class tmConditionNodeFixed;
  friend class tmConditionNodeOnCorner;
  friend class tmConditionNodeOnEdge;
  friend class tmConditionNodeSymmetric;
  friend class tmConditionNodesPaired;
  friend class tmConditionNodesCollinear;
  friend class tmConditionEdgeLengthFixed;
  friend class tmConditionEdgesSameStrain;
  friend class tmConditionPathCombo;
  friend class tmConditionPathActive;
  friend class tmConditionPathAngleFixed;
  friend class tmConditionPathAngleQuant;
  friend class tmStubFinder;
};


#ifdef __MWERKS__
  #pragma mark -
#endif


/**********
Template definitions
**********/


#ifdef TMDEBUG
/*****
Return the (1-based) index this part would have if the parts were renumbered
right now. This is only used in debugging.
*****/
template <class P>
std::size_t tmTree::GetInstantIndex(P* const p) const
{
  return tmCluster::GetParts<P>().GetIndex(p);
}
#endif


/*****
Change the index of the given part to a new value. This is a tmTree function
rather than a part member function so this one template function can work for
all types of parts.
*****/
template <class P>
void tmTree::SetPartIndex(P* p, std::size_t n)
{
  tmTreeCleaner tc(this);
  
  // Need to use tmCluster::GetParts<P>() because tmTree::GetParts<P>() only
  // returns a const array and we'll be calling the (non-const) member
  // MoveItem(..).
  tmDpptrArray<P>& plist = tmCluster::GetParts<P>();
  
  // Users should do their own range-checking before calling this routine
  TMASSERT(n > 0);
  TMASSERT(n <= plist.size());

  // Since this can be called for dirty trees, p->mIndex may not be valid. Find
  // the correct current (1-based) index for p in the list.
  std::size_t pindex = plist.GetIndex(p);
  TMASSERT(pindex != tmArray<P>::BAD_INDEX);
  
  // If it's already in the right place, we don't need to do anything.
  if (pindex == n) return;
  
  // Otherwise, perform the move.
  plist.MoveItem(pindex, n);
}


/*****
Return a list of all conditions of type C that affect the given part.
*****/
template <class C, class P>
void tmTree::GetAffectingConditions(P* const p, 
  tmArray<C*>& aConditionList)
{
  aConditionList.clear();
  C* aCondition;
  tmArrayIterator<C*> iConditions(mConditions);
  while (iConditions.Next(&aCondition))
    if (aCondition->Uses(p)) aConditionList.push_back(aCondition);
}


/*****
Return true if this part has a condition of type C.
*****/
template <class C, class P>
bool tmTree::IsConditioned(P* p) 
{
  tmCondition* aCondition;
  tmArrayIterator<tmCondition*> iConditions(mConditions);
  while (iConditions.Next(&aCondition))
    if (dynamic_cast<C*>(aCondition) && aCondition->Uses(p)) return true;
  return false;
}


/*****
Create a tmCondition on a tmPart. 
*****/ 
template <class C, class P>
C* tmTree::MakeOnePartCondition(P* p)
{
  tmTreeCleaner tc(this);
  return new C(this, p);
}


/*****
Either find or create a condition of the given type on the specified part.
*****/ 
template <class C, class P>
C* tmTree::GetOrMakeOnePartCondition(P* p)
{
  for (size_t i = 0; i < mConditions.size(); ++i) {
    tmCondition* c = mConditions[i];
    if (c->Uses(p)) {
      C* cc = dynamic_cast<C*>(c);
      if (cc)
        return cc;
    }
  }
  return MakeOnePartCondition<C, P>(p);
}


/*****
Create a tmCondition on each tmPart in a list. The list may be empty. Return
the list of created parts in clist.
*****/ 
template <class C, class P>
void tmTree::MakeOnePartCondition(const tmArray<P*>& plist, tmArray<C*>& clist)
{
  if (plist.empty()) return;
  clist.clear();
  tmTreeCleaner tc(this);
  for (std::size_t i = 0; i < plist.size(); ++i)
    clist.push_back(MakeOnePartCondition<C, P>(plist[i]));
}


/*****
Either find an existing condition of the given type for each part, or create a
new one. plist can be empty. Upon return, clist will contain one condition for
each part in plist.
*****/ 
template <class C, class P>
void tmTree::GetOrMakeOnePartCondition(const tmArray<P*>& plist, 
  tmArray<C*>& clist)
{
  if (plist.empty()) return;
  clist.clear();
  tmTreeCleaner tc(this);
  for (std::size_t i = 0; i < plist.size(); ++i)
    clist.push_back(GetOrMakeOnePartCondition<C, P>(plist[i]));
}


/*****
Create a tmCondition on 2 distinct tmParts.
*****/ 
template <class C, class P>
C* tmTree::MakeTwoPartCondition(P* p1, P* p2)
{
  TMASSERT(p1 != p2);
  tmTreeCleaner tc(this);
  return new C(this, p1, p2);
}


/*****
Find or create a tmCondition on 2 distinct tmParts.
*****/ 
template <class C, class P>
C* tmTree::GetOrMakeTwoPartCondition(P* p1, P* p2)
{
  TMASSERT(p1 != p2);
  for (size_t i = 0; i < mConditions.size(); ++i) {
    tmCondition* c = mConditions[i];
    if (c->Uses(p1) && c->Uses(p2)) {
      C* cc = dynamic_cast<C*>(c);
      if (cc)
        return cc;
    }
  }
  return MakeTwoPartCondition<C, P>(p1, p2);
}


/*****
Create a tmCondition on 3 distinct tmParts.
*****/ 
template <class C, class P>
C* tmTree::MakeThreePartCondition(P* p1, P* p2, P* p3)
{
  TMASSERT((p1 != p2) && (p2 != p3));
  tmTreeCleaner tc(this);
  return new C(this, p1, p2, p3);
}


/*****
Find or create a tmCondition on 3 distinct tmParts.
*****/ 
template <class C, class P>
C* tmTree::GetOrMakeThreePartCondition(P* p1, P* p2, P* p3)
{
  TMASSERT((p1 != p2) && (p2 != p3));
  for (size_t i = 0; i < mConditions.size(); ++i) {
    tmCondition* c = mConditions[i];
    if (c->Uses(p1) && c->Uses(p2) && c->Uses(p3)) {
      C* cc = dynamic_cast<C*>(c);
      if (cc)
        return cc;
    }
  }
  return MakeThreePartCondition<C, P>(p1, p2, p3);
}


#ifdef __MWERKS__
#pragma mark -
#endif


/*****
Renumber this type of part. Note that the part index is 1-based, not 0-based.
*****/
template <class P>
void tmTree::RenumberParts()
{
  tmDpptrArray<P>& theParts = tmCluster::GetParts<P>();
  for (std::size_t i = 0; i < theParts.size(); ++i) 
    theParts[i]->mIndex = i + 1;
}


#ifdef __MWERKS__
#pragma mark -
#endif


/*****
Write a ptr-to-P to a stream, encoded as its index
*****/
template <class P>  
void tmTree::PutPtr(std::ostream& os, P* const p)
{
  if (p != NULL) 
    PutPOD(os, p->mIndex);
  else 
    PutPOD(os, 0);
}


/*****
Read a ptr-to-P from a stream, encoded as its index
*****/
template <class P>  
void tmTree::GetPtr(std::istream& is, P*& p, bool canFail)
{
  p = 0;
  std::size_t n;
  GetPOD(is, n);
  tmDpptrArray<P>& plist = tmCluster::GetParts<P>();
  if (n < 1 || n > plist.size()) {
    if (canFail) 
      return;
    else {
      std::stringstream ss;
      ss << "[" << P::TagStr() << "] " << int(n);
      throw EX_IO_BAD_REF_INDEX(ss.str());
    }
  }
  p = plist[n - 1];
}
    
    
/*****
Write a tmDpptr<P> to a stream, encoded as its index
*****/
template <class P>  
void tmTree::PutPtr(std::ostream& os, const tmDpptr<P>& pref)
{
  PutPtr(os, (P*)(pref));
}
    
    
/*****
Read a tmDpptr<P> from a stream, encoded as its index
*****/
template <class P>  
void tmTree::GetPtr(std::istream& is, tmDpptr<P>& pref, bool canFail)
{
  P* p;
  GetPtr(is, p, canFail);
  pref = p;
}
    
    
/*****
Write a tmArray<P*> to a stream, encoded as its size followed by a list of
indices.
*****/
template <class P>  
void tmTree::PutPtrArray(std::ostream& os, const tmArray<P*>& plist)
{
  PutPOD(os, plist.size());
  tmArrayIterator<P*> ip(plist);
  P* p;
  while (ip.Next(&p)) PutPtr(os, p);
}


/*****
Read in a tmArray<P*> from a stream, encoded as the size followed by a list
of indices.
*****/
template <class P>  
void tmTree::GetPtrArray(std::istream& is, tmArray<P*>& plist)
{
  std::size_t n;
  GetPOD(is, n);
  for (std::size_t j = 0; j < n; ++j) {
    P* p;
    GetPtr(is, p);
    plist.push_back(p);
  }
}


/*****
Write a tmDpptrArray<P> to a stream, encoded as its size followed by a list of
indices.
*****/
template <class P>  
void tmTree::PutPtrArray(std::ostream& os, const tmDpptrArray<P>& plist)
{
  PutPOD(os, plist.size());
  tmArrayIterator<P*> ip(plist);
  P* p;
  while (ip.Next(&p)) PutPtr(os, p);
}


/*****
Read in a tmDpptrArray<P> from a stream, encoded as the size followed by a list
of indices.
*****/
template <class P>  
void tmTree::GetPtrArray(std::istream& is, tmDpptrArray<P>& plist)
{
  std::size_t n;
  GetPOD(is, n);
  for (std::size_t j = 0; j < n; ++j) {
    P* p;
    GetPtr(is, p);
    plist.push_back(p);
  }
}


#endif // _TMTREE_H_
