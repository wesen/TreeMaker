/*******************************************************************************
File:         tmEdge.cpp
Project:      TreeMaker 5.x
Purpose:      Implementation file for tmEdge class
Author:       Robert J. Lang
Modified by:  
Created:      2003-11-25
Copyright:    ©2003 Robert J. Lang. All Rights Reserved.
*******************************************************************************/

#include "tmEdge.h"
#include "tmModel.h"

using namespace std;

/**********
class tmEdge
Class that represents an edge of the tree graph, and thus corresponds to a flap
or segment of a flap in the folded base.
**********/

/*****
Static POD initialization
*****/
const tmFloat tmEdge::MIN_EDGE_LENGTH = 0.01;


/*****
Common initialization
*****/
void tmEdge::InitEdge()
{
  // Register with tmTree
  mTree->mEdges.push_back(this);
  mIndex = mTree->mEdges.size();
  
  // Set settings
  mLength = 0;
  strcpy(mLabel, "");
  mStrain = 0;
  mStiffness = 1;

  // Set flags
  mIsPinnedEdge = false;
  mIsConditionedEdge = false;
  
  // Clear owner
  mEdgeOwner = 0;
}


/*****
Bare constructor.
*****/
tmEdge::tmEdge(tmTree* aTree)  : 
  tmPart(aTree), tmDpptrTarget()
{
  InitEdge();  
}


/*****
Full constructor
*****/
tmEdge::tmEdge(tmTree* aTree, tmNode* node1, tmNode* node2, tmFloat aLength, 
  const char* aLabel)  : 
  tmPart(aTree), tmDpptrTarget()
{
  InitEdge();
  
  // Register with Owner
  mEdgeOwner = aTree;
  mEdgeOwner->mOwnedEdges.push_back(this);
  
  // Set settings
  mLength = aLength;
  strcpy(mLabel, aLabel);

  // Fill with tmNode references
  mNodes.push_back(node1);
  node1->mEdges.push_back(this);
  mNodes.push_back(node2);
  node2->mEdges.push_back(this);
}


/*****
Return the strained length of this edge in tree units.
*****/
const tmFloat tmEdge::GetStrainedLength() const
{
  return mLength * (1 + mStrain);
}


/*****
Return the strained length of this edge scaled to paper units.
*****/
const tmFloat tmEdge::GetStrainedScaledLength() const
{
  return mLength * (1 + mStrain) * mTree->mScale;
}


/*****
Change the label. Users should do their own range-checking on the length of the
string, but if they don't, the actual label will be truncated to fit available
space.
*****/
void tmEdge::SetLabel(const char* aLabel)
{
  TMASSERT(strlen(aLabel) <= MAX_LABEL_LEN);
  strncpy(mLabel, aLabel, MAX_LABEL_LEN);
}


/*****
Change the length. Users should range-check the length to be greater than zero.
*****/
void tmEdge::SetLength(const tmFloat& aLength)
{
  if (mLength == aLength) return;
  tmTreeCleaner tc(mTree);
  TMASSERT(aLength > 0);
  mLength = aLength;
}


/*****
Change the strain.
*****/
void tmEdge::SetStrain(const tmFloat& aStrain)
{
  if (mStrain == aStrain) return;
  tmTreeCleaner tc(mTree);
  mStrain = aStrain;
}


/*****
Change the stiffness.
*****/
void tmEdge::SetStiffness(const tmFloat& aStiffness)
{
  if (mStiffness == aStiffness) return;
  tmTreeCleaner tc(mTree);
  mStiffness = aStiffness;
}


/*****
Return the node at the opposite end of the edge from this one.
*****/
tmNode* tmEdge::GetOtherNode(tmNode* aNode) const
{
  TMASSERT(mNodes.size() == 2);
  TMASSERT(mNodes.front() == aNode || mNodes.back() == aNode);
  if (mNodes.front() == aNode) return mNodes.back();
  else return mNodes.front();
}


/*****
STATIC
Return true if any of the edges in this list have nonzero strain.
*****/
bool tmEdge::ContainsStrainedEdges(const tmArray<tmEdge*>& elist)
{
  for (size_t i = 0; i < elist.size(); ++i)
    if (elist[i]->GetStrain() != 0.0) return true;
  return false;
}


/*****
Put a tmEdge in version 5 format.
*****/
void tmEdge::Putv5Self(ostream& os)
{
  PutPOD(os, GetTagStr());
  PutPOD(os, mIndex);  
  PutPOD(os, mLabel);
  PutPOD(os, mLength);
  PutPOD(os, mStrain);
  PutPOD(os, mStiffness);  
  PutPOD(os, mIsPinnedEdge);
  PutPOD(os, mIsConditionedEdge);
  mTree->PutPtrArray(os, mNodes);  
  mTree->PutOwnerPtr(os, mEdgeOwner);
}
    
    

/*****
Get a tmEdge in version 5 format.
*****/
void tmEdge::Getv5Self(istream& is)
{
  CheckTagStr<tmEdge>(is);  
  GetPOD(is, mIndex);  
  GetPOD(is, mLabel);
  GetPOD(is, mLength);
  GetPOD(is, mStrain);
  GetPOD(is, mStiffness);
  GetPOD(is, mIsPinnedEdge);
  GetPOD(is, mIsConditionedEdge);  
  mTree->GetPtrArray(is, mNodes);  
  mTree->GetOwnerPtr(is, mEdgeOwner);
}


/*****
Put a tmEdge in version 4 format.
*****/
void tmEdge::Putv4Self(ostream& os)
{
  PutPOD(os, GetTagStr());
  PutPOD(os, mIndex);  
  PutPOD(os, mLabel);
  PutPOD(os, mLength);
  PutPOD(os, mStrain);
  PutPOD(os, mStiffness);  
  PutPOD(os, mIsPinnedEdge);
  PutPOD(os, mIsConditionedEdge);
  mTree->PutPtrArray(os, mNodes);  
  mTree->PutOwnerPtr(os, mEdgeOwner);
}
    
    

/*****
Get a tmEdge in version 4 format.
*****/
void tmEdge::Getv4Self(istream& is)
{
  CheckTagStr<tmEdge>(is);  
  GetPOD(is, mIndex);  
  GetPOD(is, mLabel);
  GetPOD(is, mLength);
  GetPOD(is, mStrain);
  GetPOD(is, mStiffness);

  // It's not allowed to have zero stiffness but some older files have that for
  // unknown reason. If we read a zero, replace it with 1.0 (the default).
  if (mStiffness == 0.0) mStiffness = 1.0;
  
  GetPOD(is, mIsPinnedEdge);
  GetPOD(is, mIsConditionedEdge);  
  mTree->GetPtrArray(is, mNodes);  
  mTree->GetOwnerPtr(is, mEdgeOwner);
}


/*****
Get a tmEdge in version 3 format.
*****/
void tmEdge::Getv3Self(istream& is)
{
  CheckTagStr<tmEdge>(is);
  GetPOD(is, mIndex);
  GetPOD(is, mLabel);
  GetPOD(is, mLength);
  GetPOD(is, mIsPinnedEdge);
  mIsConditionedEdge = false;
  mTree->GetPtrArray(is, mNodes);
  
  // Fill in additional fields
  mStrain = 0.0;
  mStiffness = 0.0;
  
  // Set ownership
  mTree->mOwnedEdges.push_back(this);
  mEdgeOwner = mTree;
}


/*****
Dynamic type implementation
*****/
TM_IMPLEMENT_TAG(tmEdge, "edge")
