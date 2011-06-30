/*******************************************************************************
File:         tmCluster.h
Project:      TreeMaker 5.x
Purpose:      Header file for a group of parts
Author:       Robert J. Lang
Modified by:  
Created:      2003-11-27
Copyright:    ©2003 Robert J. Lang. All Rights Reserved.
*******************************************************************************/

#ifndef _TMCLUSTER_H_
#define _TMCLUSTER_H_

// Common TreeMaker header
#include "tmHeader.h"

// TreeMaker classes
#include "tmModel_fwd.h"
#include "tmDpptrArray.h"


/**********
class tmCluster
A group of tmParts; also a base class of a tmTree
**********/
class tmCluster {
public:
  // Member data is directly accessible
  tmDpptrArray<tmNode> mNodes;
  tmDpptrArray<tmEdge> mEdges;
  tmDpptrArray<tmPath> mPaths;
  tmDpptrArray<tmPoly> mPolys;
  tmDpptrArray<tmVertex> mVertices;
  tmDpptrArray<tmCrease> mCreases;
  tmDpptrArray<tmFacet> mFacets;
  tmDpptrArray<tmCondition> mConditions;

  // Named getters that return lists
  tmDpptrArray<tmNode>& GetNodes() {return mNodes;};
  tmDpptrArray<tmEdge>& GetEdges() {return mEdges;};
  tmDpptrArray<tmPath>& GetPaths() {return mPaths;};
  tmDpptrArray<tmPoly>& GetPolys() {return mPolys;};
  tmDpptrArray<tmVertex>& GetVertices() {return mVertices;};
  tmDpptrArray<tmCrease>& GetCreases() {return mCreases;};
  tmDpptrArray<tmFacet>& GetFacets() {return mFacets;};
  tmDpptrArray<tmCondition>& GetConditions() {return mConditions;};

  const tmDpptrArray<tmNode>& GetNodes() const {return mNodes;};
  const tmDpptrArray<tmEdge>& GetEdges() const {return mEdges;};
  const tmDpptrArray<tmPath>& GetPaths() const {return mPaths;};
  const tmDpptrArray<tmPoly>& GetPolys() const {return mPolys;};
  const tmDpptrArray<tmVertex>& GetVertices() const {return mVertices;};
  const tmDpptrArray<tmCrease>& GetCreases() const {return mCreases;};
  const tmDpptrArray<tmFacet>& GetFacets() const {return mFacets;};
  const tmDpptrArray<tmCondition>& GetConditions() const {return mConditions;};

  // Templated getters returning lists, all specialized
  template <class P>
    tmDpptrArray<P>& GetParts();
  template <class P>
    const tmDpptrArray<P>& GetParts() const;

  // Named queries about size
  std::size_t GetNumNodes() const {return mNodes.size();};
  std::size_t GetNumEdges() const {return mEdges.size();};
  std::size_t GetNumPaths() const {return mPaths.size();};
  std::size_t GetNumPolys() const {return mPolys.size();};
  std::size_t GetNumVertices() const {return mVertices.size();};
  std::size_t GetNumCreases() const {return mCreases.size();};
  std::size_t GetNumFacets() const {return mFacets.size();};
  std::size_t GetNumConditions() const {return mConditions.size();};
  
  // Templated queries about size and contents
  template <class P>
    std::size_t GetNumParts() const;
  template <class P>
    bool Contains(P* p) const;
  template <class P>
    bool ContainsAtLeast(std::size_t n) const;
  template <class P>
    bool ContainsExactly(std::size_t n) const;
  template <class P>
    bool ContainsExclusively(std::size_t n) const;
  
  std::size_t GetNumAllParts() const;
  std::size_t GetNumEditableParts() const;
  
  // Query returning a part
  tmPart* GetFirstPart() const;
  
  // Modifying routines
  template <class P>
    void AddPart(P* const p);
  template <class P>
    void AddParts(const tmArray<P*>& parts);
  template <class P>
    void RemovePart(P* const p);
  void AddParts(const tmCluster& aCluster);
  void ClearAllParts();
  template <class P>
    void ChangeToPart(P* const p);
};


/**********
Template definitions
**********/

/*****
Specialization of GetParts<P>
*****/
template <>
inline tmDpptrArray<tmNode>& tmCluster::GetParts<tmNode>()
{
  return mNodes;
}


/*****
Specialization of GetParts<P>
*****/
template <>
inline const tmDpptrArray<tmNode>& tmCluster::GetParts<tmNode>() const
{
  return mNodes;
}


/*****
Specialization of GetParts<P>
*****/
template <>
inline tmDpptrArray<tmEdge>& tmCluster::GetParts<tmEdge>()
{
  return mEdges;
}


/*****
Specialization of GetParts<P>
*****/
template <>
inline const tmDpptrArray<tmEdge>& tmCluster::GetParts<tmEdge>() const
{
  return mEdges;
}


/*****
Specialization of GetParts<P>
*****/
template <>
inline tmDpptrArray<tmPath>& tmCluster::GetParts<tmPath>()
{
  return mPaths;
}


/*****
Specialization of GetParts<P>
*****/
template <>
inline const tmDpptrArray<tmPath>& tmCluster::GetParts<tmPath>() const
{
  return mPaths;
}


/*****
Specialization of GetParts<P>
*****/
template <>
inline tmDpptrArray<tmPoly>& tmCluster::GetParts<tmPoly>()
{
  return mPolys;
}


/*****
Specialization of GetParts<P>
*****/
template <>
inline const tmDpptrArray<tmPoly>& tmCluster::GetParts<tmPoly>() const
{
  return mPolys;
}


/*****
Specialization of GetParts<P>
*****/
template <>
inline tmDpptrArray<tmVertex>& tmCluster::GetParts<tmVertex>()
{
  return mVertices;
}


/*****
Specialization of GetParts<P>
*****/
template <>
inline const tmDpptrArray<tmVertex>& tmCluster::GetParts<tmVertex>() const
{
  return mVertices;
}


/*****
Specialization of GetParts<P>
*****/
template <>
inline tmDpptrArray<tmCrease>& tmCluster::GetParts<tmCrease>()
{
  return mCreases;
}


/*****
Specialization of GetParts<P>
*****/
template <>
inline const tmDpptrArray<tmCrease>& tmCluster::GetParts<tmCrease>() const
{
  return mCreases;
}


/*****
Specialization of GetParts<P>
*****/
template <>
inline tmDpptrArray<tmFacet>& tmCluster::GetParts<tmFacet>()
{
  return mFacets;
}


/*****
Specialization of GetParts<P>
*****/
template <>
inline const tmDpptrArray<tmFacet>& tmCluster::GetParts<tmFacet>() const
{
  return mFacets;
}


/*****
Specialization of GetParts<P>
*****/
template <>
inline tmDpptrArray<tmCondition>& tmCluster::GetParts<tmCondition>()
{
  return mConditions;
}


/*****
Specialization of GetParts<P>
*****/
template <>
inline const tmDpptrArray<tmCondition>& tmCluster::GetParts<tmCondition>() const
{
  return mConditions;
}


/*****
Return the number of parts of the given type
*****/
template <class P>
std::size_t tmCluster::GetNumParts() const
{
  return GetParts<P>().size();
}


/*****
Return true if the cluster contains the given part.
*****/
template <class P>
bool tmCluster::Contains(P* p) const
{
  return GetParts<P>().contains(p);
}


/*****
Return true if the cluster at least N parts of type P, maybe more, maybe others.
*****/
template <class P>
bool tmCluster::ContainsAtLeast(std::size_t n) const
{
  return GetParts<P>().size() >= n;
}


/*****
Return true if the cluster contains exactly N parts of type P, maybe other types.
*****/
template <class P>
bool tmCluster::ContainsExactly(std::size_t n) const
{
  return GetParts<P>().size() == n;
}


/*****
Return true if the cluster contains exclusively N parts of type P and no other types.
*****/
template <class P>
bool tmCluster::ContainsExclusively(std::size_t n) const
{
  return GetParts<P>().size() == n && GetNumAllParts() == n;
}


/*****
Add a single part to a cluster.
*****/
template <class P>
void tmCluster::AddPart(P* const p)
{
  GetParts<P>().push_back(p);
}


/*****
Add a list of parts to a cluster.
*****/
template <class P>
void tmCluster::AddParts(const tmArray<P*>& plist)
{
  GetParts<P>().union_with(plist);
}


/*****
Remove a single part from a cluster.
*****/
template <class P>
void tmCluster::RemovePart(P* const p)
{
  GetParts<P>().erase_remove(p);
}


/*****
Clear the current contents and replace with a single part.
*****/
template <class P>
void tmCluster::ChangeToPart(P* const p)
{
  ClearAllParts();
  GetParts<P>().push_back(p);
}


#endif // _TMCLUSTER_H_
