/*******************************************************************************
File:         tmEdge.h
Project:      TreeMaker 5.x
Purpose:      Header file for tmEdge class
Author:       Robert J. Lang
Modified by:  
Created:      2003-11-15
Copyright:    ©2003 Robert J. Lang. All Rights Reserved.
*******************************************************************************/

#ifndef _TMEDGE_H_
#define _TMEDGE_H_

// Common TreeMaker header
#include "tmHeader.h"

// Std libraries
#include <iostream>

// TreeMaker classes
#include "tmModel_fwd.h"
#include "tmPart.h"
#include "tmDpptrTarget.h"
#include "tmDpptrArray.h"


/**********
class tmEdge
Class that represents an edge of the tree graph, and thus corresponds to a flap
or segment of a flap in the folded base.
**********/
class tmEdge : public tmPart, public tmDpptrTarget {
public:
  // Getters
  const char* GetLabel() const {
    // Return the optional label identifying this edge (typically with a feature
    // of the subject).
    return mLabel;};
  
  const tmFloat& GetLength() const {
    // Return the length of this edge in tree units.
    return mLength;};
  
  const tmFloat& GetStrain() const {
    // Return the strain in the edge.
    return mStrain;};
  
  const tmFloat& GetStiffness() const {
    // Return the stiffness enhancement factor for the edge.
    return mStiffness;};
  
  bool IsPinnedEdge() const {
    // Return true if the edge is pinned, i.e., it can't grow any longer without
    // violating a constraint.
    return mIsPinnedEdge;};
  
  bool IsConditionedEdge() const {
    // Return true if this edge has one or more conditions on it.
    return mIsConditionedEdge;};
  
  const tmDpptrArray<tmNode>& GetNodes() const {
    // Return the ordered list of exactly two nodes that make up this edge.
    return mNodes;};
    
  const tmEdgeOwner* GetEdgeOwner() const {
    // Return the owner of this edge (always the tree).
    return mEdgeOwner;};
  
  const tmFloat GetStrainedLength() const;
  const tmFloat GetStrainedScaledLength() const;
  
  // Setters
  void SetLabel(const char* aLabel);
  void SetLength(const tmFloat& aLength);
  void SetStrain(const tmFloat& aStrain);
  void SetStiffness(const tmFloat& aStiffness);
  
  // Queries
  tmNode* GetOtherNode(tmNode* aNode) const;
  static bool ContainsStrainedEdges(const tmArray<tmEdge*>& elist);
  
  // Constants
  static const tmFloat MIN_EDGE_LENGTH;
  
private:
  char mLabel[MAX_LABEL_LEN + 1];
  tmFloat mLength;
  tmFloat mStrain;
  tmFloat mStiffness;
  
  // Dimensional flags  
  bool mIsPinnedEdge;
  bool mIsConditionedEdge;
  
  // Structural attributes
  tmDpptrArray<tmNode> mNodes;
  
  // owner
  tmEdgeOwner* mEdgeOwner;

  // Constructors
  void InitEdge();
  tmEdge(tmTree* aTree);
  tmEdge(tmTree* aTree, tmNode *node1, tmNode *node2, tmFloat aLength = 1, 
    const char* aLabel = "");
        
  // Stream I/O
  void Putv5Self(std::ostream& os);
  void Getv5Self(std::istream& is);
  void Putv4Self(std::ostream& os);
  void Getv4Self(std::istream& is);
  void Getv3Self(std::istream& is);
  
  // Class tag for stream I/O
  TM_DECLARE_TAG()
    
  // Friend classes
  friend class tmPart;
  friend class tmPart::StringT<tmEdge>;
  friend class tmPart::CreatorFnT<tmEdge>;
  friend class tmEdgeOwner;
  friend class tmTree;
  friend class tmPath;
  friend class tmPoly;
  friend class tmStubFinder;
};

#endif // _TMEDGE_H_
