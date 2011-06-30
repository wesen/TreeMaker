/*******************************************************************************
File:         tmTree_IO.cpp
Project:      TreeMaker 5.x
Purpose:      Implementation file for tmTree I/O routines
Author:       Robert J. Lang
Modified by:  
Created:      2003-12-07
Copyright:    ©2003 Robert J. Lang. All Rights Reserved.
*******************************************************************************/

#include "tmTree.h"
#include "tmModel.h"

using namespace std;

/*
This file contains the routines that relate to stream I/O of trees. Most of
these routines started out as a separate class TreeFile in TreeMaker 4 and
earlier versions. Beginning in version 5, we use standard C++ streams for data
I/O, which lets us use the same routines to store structures in files and in
memory buffers (which is required to implement Undo/Redo). As a consequence,
stream I/O is moved into the mathematical model, and most I/O routines are
implemented as tmTree member functions.

TreeMaker streams the tree as '\n'-delimited ASCII data. See a discussion of
line endings ('\r' vs '\n') in tmPart::PutPOD(ostream&, char*).

I/O routines are grouped hierarchically using a Put/Get construction.
At the top level are Put/Get routines for parts:
  
tmNode, tmEdge, tmPath, tmPoly, tmVertex, tmCrease, tmFacet, tmCondition
  
The part routines all simply Put the member data fields and references to the
file.
  
Note, however, that Conditions are handled a bit special. Conditions are
polymorphic, so we Put a tag string with each condition, which is the
conditionID, so that when we reconstruct during Get() we create the right type
of condition.

(In fact, we Put a tag string for every object, even though it isn't needed for
the non-polymorphic classes.)
  
At the next level are routines for references to parts: pointers, tmDpptr<T>,
and tmDpptrArray<T>.
  
The lowest level routines are for literals, i.e., elemental types and vectors
of same.
  
TreeMaker has partial support for cross-version compatibility, which is realized
by different versions of Put and Get for different document versions.

CHANGES IN VERSION 5

Routines specific to storing or reading version 5 format data are called
Putv5(..) and Getv5(..).
  
CHANGES IN VERSION 4

Routines specific to storing or reading version 4 format data are called
Putv4Self(..) and Getv4Self(..).
  
For backward compatibility, we can read or write version 3 format data. However,
translation isn't perfect because of the following:
  
  extensive changes to the tmPoly data structure (including hierarchical data)
  elimination of the Fold object
  introduction of the tmVertex and tmCrease objects
    
So for reading in version 3 format we only read in nodes, Edges, and Paths and
replace conditions formerly directly associated with nodes with Conditions. We
discard all tmPoly and Fold information.
  
For writing out in version 3 format, we write nodes, Edges, and Paths, but
discard all tmPoly, tmVertex, tmCrease, and tmCondition information.
  
Routines specific to storing or reading version 3 format data are called
Putv3() and Getv3Self().
*/

#ifdef __MWERKS__
  #pragma mark --PUBLIC--
#endif


/*****
Put the tree to a stream.
*****/
void tmTree::PutSelf(ostream& os)
{
  Putv5Self(os);
}


/*****
Read the tree in from a stream, overwriting anything that already exists. If
the file version is bad, we throw a EX_IO_BAD_TREE_VERSION() and leave the tree blank.
If there were unrecognized conditions, Getv5Self(is) and Getv4Self(is) ignore
them and throws a EX_IO_UNRECOGNIZED_CONDITION, but we'll leave the tree in a
valid state.
*****/
void tmTree::GetSelf(istream& is)
{
  // Start by looking for the "tree" tag that begins all TreeMaker files. If we
  // don't see it, then something's wrong with the file, so throw an exception.
  try {
    CheckTagStr<tmTree>(is);
  }
  catch (EX_IO_BAD_TAG exc) {
    throw EX_IO_BAD_TREE_TAG(exc.mToken);
  }
  catch (EX_IO_UNRECOGNIZED_TAG exc) {
    throw EX_IO_BAD_TREE_TAG(exc.mToken);
  }
  
  // Next, read in the version string to determine which version we have.
  string version;
  GetPOD(is, version);
  
  // Read in data from the stream depending on which version we're using
  if (version == string("3.0")) {
    tmTreeCleaner tc(this);
    KillAllParts();
    Getv3Self(is);
  }
  else if (version == string("4.0")) {
    tmTreeCleaner tc(this);
    KillAllParts();
    Getv4Self(is);
    // Need to get rid of polys, vertices, and creases because we've added new
    // fields since version 4. User will have to rebuild them.
    KillPolysAndCreasePattern();
  }
  else if (version == string("5.0")) {
    // Only need to cleanup if we kill parts, and we'll have to do that only
    // if we're overwriting an existing tree.
    if (GetNumAllParts() > 0) {
      tmTreeCleaner tc(this);
      KillAllParts();
    }
    Getv5Self(is);
  }
  else throw EX_IO_BAD_TREE_VERSION(version);
}


/*****
Export the tree in version 4 format to a stream
*****/
void tmTree::Exportv4(ostream& os)
{
  // To export to version 4, we can't have any polys, vertices, edges, or
  // creases; we also can't have internal nodes. So we'll make a copy of this
  // tree, kill those parts, then put the stripped tree to the stream in
  // version 4 format.
  tmTree* theTree = Clone();
  theTree->KillPolysAndCreasePattern();
  theTree->Putv4Self(os);
  delete theTree;
}


#ifdef __MWERKS__
  #pragma mark -
  #pragma mark --PRIVATE--
#endif


/**********
class StreamPeek
Useful class for peeking at the current upcoming contents of a stream.
**********/
template <int N>
class StreamPeek {
public:
  char data[N + 1];
  StreamPeek(istream& is) {
    for (size_t i = 0; i < N; ++i) is.get(data[i]);
    for (size_t i = N; i > 0; i--) is.putback(data[i - 1]);
    data[N] = '\0';
  };
};


#ifdef __MWERKS__
  #pragma mark -
#endif


/*****
Write the tree to a stream in version 5 format
*****/
void tmTree::Putv5Self(ostream& os)
{
  os.setf(os.fixed, os.floatfield);
  os.precision(10);
  
  PutPOD(os, GetTagStr());    // put the tag string
  PutPOD(os, "5.0");          // put the version
  
  // Put all the parts of the tree  
  PutPOD(os, mPaperWidth);
  PutPOD(os, mPaperHeight);
  PutPOD(os, mScale);
  
  PutPOD(os, mHasSymmetry);
  PutPOD(os, mSymLoc);
  PutPOD(os, mSymAngle);
  
  PutPOD(os, mIsFeasible);
  PutPOD(os, mIsPolygonValid);
  PutPOD(os, mIsPolygonFilled);
  PutPOD(os, mIsVertexDepthValid);
  PutPOD(os, mIsFacetDataValid);
  PutPOD(os, mIsLocalRootConnectable);
  PutPOD(os, mNeedsCleanup);

  // Put the total number of nodes, edges, paths, polys, conditions, vertices,
  // creases  
  size_t numNodes = mNodes.size();
  size_t numEdges = mEdges.size();
  size_t numPaths = mPaths.size();
  size_t numPolys = mPolys.size();
  size_t numVertices = mVertices.size();
  size_t numCreases = mCreases.size();
  size_t numFacets = mFacets.size();
  size_t numConditions = mConditions.size();
  
  PutPOD(os, numNodes);
  PutPOD(os, numEdges);
  PutPOD(os, numPaths);
  PutPOD(os, numPolys);
  PutPOD(os, numVertices);
  PutPOD(os, numCreases);
  PutPOD(os, numFacets);
  PutPOD(os, numConditions);
  
  // Put all of the parts of the tree to the stream
  for (size_t i = 0; i < numNodes; ++i) mNodes[i]->Putv5Self(os);
  for (size_t i = 0; i < numEdges; ++i) mEdges[i]->Putv5Self(os);
  for (size_t i = 0; i < numPaths; ++i) mPaths[i]->Putv5Self(os);
  for (size_t i = 0; i < numPolys; ++i) mPolys[i]->Putv5Self(os);
  for (size_t i = 0; i < numVertices; ++i) mVertices[i]->Putv5Self(os);
  for (size_t i = 0; i < numCreases; ++i) mCreases[i]->Putv5Self(os);
  for (size_t i = 0; i < numFacets; ++i) mFacets[i]->Putv5Self(os);
  
  // Conditions are put using special routine
  for (size_t i = 0; i < numConditions; ++i) Putv5Condition(os, mConditions[i]);
  
  // Put the lists of owned parts. (Note that we don't have to put
  // mOwnedConditions, because we don't need it to read in the conditions).
  PutPtrArray(os, mOwnedNodes);
  PutPtrArray(os, mOwnedEdges);
  PutPtrArray(os, mOwnedPaths);
  PutPtrArray(os, mOwnedPolys);
}


/*****
Read the tree from a stream in version 5 format
*****/
void tmTree::Getv5Self(istream& is)
{
  // Get all the parts of the tree  
  GetPOD(is, mPaperWidth);
  GetPOD(is, mPaperHeight);
  GetPOD(is, mScale);
  
  GetPOD(is, mHasSymmetry);
  GetPOD(is, mSymLoc);
  GetPOD(is, mSymAngle);
  
  GetPOD(is, mIsFeasible);
  GetPOD(is, mIsPolygonValid);
  GetPOD(is, mIsPolygonFilled);
  GetPOD(is, mIsVertexDepthValid);
  GetPOD(is, mIsFacetDataValid);
  GetPOD(is, mIsLocalRootConnectable);
  GetPOD(is, mNeedsCleanup);

  // Get the number of parts of each type 
  size_t numNodes, numEdges, numPaths, numPolys, numVertices, numCreases, 
    numFacets, numConditions;
  GetPOD(is, numNodes);
  GetPOD(is, numEdges);
  GetPOD(is, numPaths);
  GetPOD(is, numPolys);
  GetPOD(is, numVertices);
  GetPOD(is, numCreases);
  GetPOD(is, numFacets);
  GetPOD(is, numConditions);
  
  // Create the proper number of blank tmParts, except Conditions, which are
  // created on the fly
  for (size_t i = 0; i < numNodes; ++i) new tmNode(this);
  for (size_t i = 0; i < numEdges; ++i) new tmEdge(this);
  for (size_t i = 0; i < numPaths; ++i) new tmPath(this);
  for (size_t i = 0; i < numPolys; ++i) new tmPoly(this);
  for (size_t i = 0; i < numVertices; ++i) new tmVertex(this);
  for (size_t i = 0; i < numCreases; ++i) new tmCrease(this);
  for (size_t i = 0; i < numFacets; ++i) new tmFacet(this);
  
  // Read in each part in order. Each part contains various ptrs and lists of
  // ptrs to other parts which were serialized as indices. Since we've already
  // created blank parts (other than conditions), as we read in the part all
  // reference indices can get replaced by the actual pointers as we read in
  // the part. Fortunately, no part references a condition, because they don't
  // exist yet.
  for (size_t i = 0; i < numNodes; ++i) mNodes[i]->Getv5Self(is);
  for (size_t i = 0; i < numEdges; ++i) mEdges[i]->Getv5Self(is);
  for (size_t i = 0; i < numPaths; ++i) mPaths[i]->Getv5Self(is);
  for (size_t i = 0; i < numPolys; ++i) mPolys[i]->Getv5Self(is);
  for (size_t i = 0; i < numVertices; ++i) mVertices[i]->Getv5Self(is);
  for (size_t i = 0; i < numCreases; ++i) mCreases[i]->Getv5Self(is);
  for (size_t i = 0; i < numFacets; ++i) mFacets[i]->Getv5Self(is);
  
  // Conditions are special. First, they are created as they are read in.
  // Second, when a condition is read it, its owner automatically receives
  // a pointer to it (unlike all other parts).
  for (size_t i = 0; i < numConditions; ++i) Makev5Condition(is);

  // Get the lists of owned parts -- except conditions, because mOwnedConditions
  // already contains all the conditions.
  GetPtrArray(is, mOwnedNodes);
  GetPtrArray(is, mOwnedEdges);
  GetPtrArray(is, mOwnedPaths);
  GetPtrArray(is, mOwnedPolys);
  
  // Eat remaining newlines/whitespace. We don't set eof because we'll not care
  // about extra characters, and we want to be able to re-use is if it's a
  // stringstream.
  ConsumeTrailingSpace(is);
  
  // If we didn't create as many conditions as there were in the file, throw
  // an exception.
  size_t numMissed = numConditions - mConditions.size();
  if (numMissed) 
    throw EX_IO_UNRECOGNIZED_CONDITION(numMissed);
}


/*
Note: we change the serialization format for all conditions from version 4 to
version 5 (even version-4-specific conditions). In version 5, we put the
condition index and feasibility.
*/


/*****
Put a condition to a stream in version 5 format
*****/
void tmTree::Putv5Condition(ostream& os, tmCondition* aCondition)
{
  // Put the tag string to the file
  PutPOD(os, aCondition->GetTagStr());
  
  // Member data common to all conditions
  PutPOD(os, aCondition->mIndex);
  PutPOD(os, aCondition->mIsFeasibleCondition);

  // Put the number of lines unique to each subclass of the tmCondition
  size_t numLines = aCondition->GetNumLinesRest();
  PutPOD(os, numLines);

  // Put the contents of the tmCondition to the stream
  aCondition->PutRestv4(os);
}


/*****
Create a condition polymorphically in version 5 format
*****/
void tmTree::Makev5Condition(istream& is)
{
  // Get the tag from the file, by reading the tag string and looking up the
  // tag. In this one place, we'll allow for the possibility of unrecognized
  // tags.
  size_t ctag;
  try {
    tmPart::GetPODTag(is, ctag); // can throw EX_IO_UNRECOGNIZED_TAG or EX_IO_BAD_TAG

    // Create a new condition of the appropriate type, using Create<T>(),
    // and read the rest of its guts from the stream.
    tmCondition* c = dynamic_cast<tmCondition*>(
      tmPart::GetCreatorFns()[ctag](this));
    
    // If ctag was a known tag but not a tmCondition tag, then c is a tmPart*
    // but not a tmCondition*, so there's something wrong; bail completely.
    if (!c) {
      stringstream ss;
      ss << "[tag ID] " << int(ctag);
      throw EX_IO_BAD_TAG(ss.str());
    }
    
    // member data common to all conditions
    c->GetPOD(is, c->mIndex);
    c->GetPOD(is, c->mIsFeasibleCondition);
    
    // Eat the number of lines in the condition
    size_t numLines;
    GetPOD(is, numLines);

    // Read in the rest of the condition polymorphically.
    c->GetRestv4(is);
  }
  catch(EX_IO_UNRECOGNIZED_TAG) {
    // if it's a valid condition tag but not one we recognize, we'll ignore it.
    // Eat the remaining data.
    string dummy;
    GetPOD(is, dummy);  // mIndex
    GetPOD(is, dummy);  // mIsFeasible
    size_t numLines;
    GetPOD(is, numLines);
    for (size_t i = 0; i < numLines; ++i)
      GetPOD(is, dummy);
    return;
  }
}


#ifdef __MWERKS__
  #pragma mark -
#endif


/*****
Write an existing tree to a stream in version 4 format.
*****/
void tmTree::Putv4Self(ostream& os)
{
  os.setf(os.fixed, os.floatfield);
  os.precision(6);
  Endl eol('\r');             // use Mac line endings for TM4 compatibility
  
  PutPOD(os, GetTagStr());    // put the tag string
  PutPOD(os, "4.0");          // put the version
  
  // Put all the parts of the tree  
  PutPOD(os, mPaperWidth);
  PutPOD(os, mPaperHeight);
  PutPOD(os, mScale);
  
  PutPOD(os, mHasSymmetry);
  PutPOD(os, mSymLoc);
  PutPOD(os, mSymAngle);
  
  // Put the total number of nodes, edges, paths, polys, conditions, vertices,
  // creases  
  size_t numNodes = mNodes.size();
  size_t numEdges = mEdges.size();
  size_t numPaths = mPaths.size();
  // size_t numPolys = mPolys.size();
  // size_t numVertices = mVertices.size();
  // size_t numCreases = mCreases.size();
  size_t numConditions = mConditions.size();
  
  // Since we've so radically changed the crease pattern format (extending
  // down to polygons, we only put nodes, edges, and paths. But we have to
  // put placeholders for polys, vertices, and creases.
  PutPOD(os, numNodes);
  PutPOD(os, numEdges);
  PutPOD(os, numPaths);
  PutPOD(os, 0);
  PutPOD(os, 0);
  PutPOD(os, 0);
  PutPOD(os, numConditions);
  
  // Put all of the parts of the tree to the stream
  for (size_t i = 0; i < numNodes; ++i) mNodes[i]->Putv4Self(os);
  for (size_t i = 0; i < numEdges; ++i) mEdges[i]->Putv4Self(os);
  for (size_t i = 0; i < numPaths; ++i) mPaths[i]->Putv4Self(os);
  // no polys
  // no vertices
  // no creases
  for (size_t i = 0; i < numConditions; ++i) Putv4Condition(os, mConditions[i]);
  
  // Put the lists of owned parts 
  PutPtrArray(os, mOwnedNodes);
  PutPtrArray(os, mOwnedEdges);
  PutPtrArray(os, mOwnedPaths);
  PutPOD(os, 0);  // no owned polys
}


/*****
Read the tmTree from the file assuming version 4.0 format. If we don't
recognize certain conditions, we'll ignore them, and then at the end of things
throw a EX_IO_UNRECOGNIZED_CONDITION.
*****/  
void tmTree::Getv4Self(istream& is)
{
  // Get all the parts of the tree  
  GetPOD(is, mPaperWidth);
  GetPOD(is, mPaperHeight);
  GetPOD(is, mScale);
  
  GetPOD(is, mHasSymmetry);
  GetPOD(is, mSymLoc);
  GetPOD(is, mSymAngle);
  
  // Get the number of parts of each type 
  size_t numNodes, numEdges, numPaths, numPolys, numVertices, numCreases, 
    numConditions;
  GetPOD(is, numNodes);
  GetPOD(is, numEdges);
  GetPOD(is, numPaths);
  GetPOD(is, numPolys);
  GetPOD(is, numVertices);
  GetPOD(is, numCreases);
  GetPOD(is, numConditions);
  
  // Create the proper number of blank tmParts, except Conditions, which are
  // created on the fly
  for (size_t i = 0; i < numNodes; ++i) new tmNode(this);
  for (size_t i = 0; i < numEdges; ++i) new tmEdge(this);
  for (size_t i = 0; i < numPaths; ++i) new tmPath(this);
  for (size_t i = 0; i < numPolys; ++i) new tmPoly(this);
  for (size_t i = 0; i < numVertices; ++i) new tmVertex(this);
  for (size_t i = 0; i < numCreases; ++i) new tmCrease(this);

  // Read in each part in order
  for (size_t i = 0; i < numNodes; ++i) mNodes[i]->Getv4Self(is);
  for (size_t i = 0; i < numEdges; ++i) mEdges[i]->Getv4Self(is);
  for (size_t i = 0; i < numPaths; ++i) mPaths[i]->Getv4Self(is);
  for (size_t i = 0; i < numPolys; ++i) mPolys[i]->Getv4Self(is);
  for (size_t i = 0; i < numVertices; ++i) mVertices[i]->Getv4Self(is);
  for (size_t i = 0; i < numCreases; ++i) mCreases[i]->Getv4Self(is);
  
  // Conditions are special; some might have been ignored
  for (size_t i = 0; i < numConditions; ++i) Makev4Condition(is);

  // Get the lists of owned parts
  GetPtrArray(is, mOwnedNodes);
  GetPtrArray(is, mOwnedEdges);
  GetPtrArray(is, mOwnedPaths);
  GetPtrArray(is, mOwnedPolys);
  
  // Eat remaining newlines/whitespace
  ConsumeTrailingSpace(is);
  
  // Recalculate the feasibility flag, which wasn't stored in version 4 files.
  for (size_t i = 0; i < mConditions.size(); ++i)
    mConditions[i]->CalcFeasibility();
  
  // If we didn't create as many conditions as there were in the file, throw
  // an exception.
  size_t numMissed = numConditions - mConditions.size();
  if (numMissed) throw EX_IO_UNRECOGNIZED_CONDITION(numMissed);
}


/*****
Put a tmCondition in version 4 format
*****/
void tmTree::Putv4Condition(ostream& os, tmCondition* aCondition)
{
  // Put the tag string to the file
  PutPOD(os, aCondition->GetTagStr());

  // Put the number of lines in this tmCondition
  size_t numLines = aCondition->GetNumLinesRest();
  PutPOD(os, numLines);

  // Put the contents of the tmCondition to the stream
  aCondition->PutRestv4(os);
}


/*****
Read the tag from the stream, create the appropriate condition, and then let
the condition make itself from the remaining information on the stream.
*****/
void tmTree::Makev4Condition(istream& is)
{
  // Get the tag from the file, by reading the tag string and looking up the
  // tag. In this one place, we'll allow for the possibility of unrecognized
  // tags.
  size_t ctag;
  try {
    tmPart::GetPODTag(is, ctag); // can throw EX_IO_UNRECOGNIZED_TAG or EX_IO_BAD_TAG
    // Create a new condition of the appropriate type, using Create<T>(),
    // and read the rest of its guts from the stream.
    tmCondition* c = dynamic_cast<tmCondition*>(
      tmPart::GetCreatorFns()[ctag](this));
    
    // If ctag was a known tag but not a tmCondition tag, then c is a tmPart*
    // but not a tmCondition*, so there's something wrong.
    if (!c) {
      stringstream ss;
      ss << "[tag ID] " << ctag;
      throw EX_IO_BAD_TAG(ss.str());
    }
    
    // Other parts store their own indices, but version 4 conditions do not,
    // so we need to set it as we read it in.
    c->mIndex = mConditions.size();
    
    // Eat the number of lines (not used unless we didn't recognize tag)
    size_t numLines;
    GetPOD(is, numLines);

    // Read in the rest of the condition polymorphically.
    c->GetRestv4(is);
    
    // Note that feasibility hasn't been set yet; we'll have to go back and
    // set all conditions after we've fully constructed the tree.
  }
  catch(EX_IO_UNRECOGNIZED_TAG) {
    // if it's a valid tag but not one we recognize, we'll ignore it.
    // Eat the number of lines in the condition
    size_t numLines;
    GetPOD(is, numLines);
    string dummy;
    for (size_t i = 0; i < numLines; ++i)
      GetPOD(is, dummy);
    return;
  }
  // if GetTag() threw EX_IO_BAD_TAG, we let that propagate upward.
}


#ifdef __MWERKS__
#pragma mark -
#endif


/*****
Read the tmTree from the file assuming version 3.0 format
*****/  
void tmTree::Getv3Self(istream& is)
{
  // Get all the parts of the tree  
  GetPOD(is, mPaperWidth);
  GetPOD(is, mPaperHeight);
  GetPOD(is, mScale);
  
  GetPOD(is, mHasSymmetry);
  GetPOD(is, mSymLoc);
  GetPOD(is, mSymAngle);
  
  // Get the number of parts of each type and create blank parts. 
  size_t n, numNodes;
  GetPOD(is, numNodes);
  for (n = 1; n <= numNodes; n++) new tmNode(this);

  size_t numEdges;
  GetPOD(is, numEdges);
  for (n = 1; n <= numEdges; n++) new tmEdge(this);

  size_t numPaths;
  GetPOD(is, numPaths);
  for (n = 1; n <= numPaths; n++) new tmPath(this);

  // Since Polys have changed drastically going to version 4, we keep our life
  // simple and don't read in any Polys. We still read in the number of polys
  // since that's part of the file format.
  size_t numPolys;
  GetPOD(is, numPolys);
  
  
  // Read in each part in order
  for (size_t i = 0; i < numNodes; ++i) mNodes[i]->Getv3Self(is);
  for (size_t i = 0; i < numEdges; ++i) mEdges[i]->Getv3Self(is);
  for (size_t i = 0; i < numPaths; ++i) mPaths[i]->Getv3Self(is);

  // Eat the final newline plus one more character to set eof.
  char ch;
  is.get(ch);
  is.get(ch);
}


#ifdef __MWERKS__
#pragma mark -
#endif


/*****
Write a tmNodeOwner*, which can be a tmPoly* or a tmTree*, to a stream
*****/
void tmTree::PutOwnerPtr(ostream& os, tmNodeOwner* const aNodeOwner)
{
  if (tmPoly* aPoly = aNodeOwner->NodeOwnerAsPoly()) {
    size_t isPoly = 1;
    PutPOD(os, isPoly);
    PutPtr(os, aPoly);
  }
  else {
    size_t isPoly = 0;
    PutPOD(os, isPoly);
  }
}


/*****
Read in a tmNodeOwner*, which can be a tmPoly* or a tmTree*, from a stream
*****/
void tmTree::GetOwnerPtr(istream& is, tmNodeOwner*& aNodeOwner)
{
  size_t isPoly;
  GetPOD(is, isPoly);
  if (isPoly) {
    tmPoly* aPoly;
    GetPtr(is, aPoly);
    aNodeOwner = aPoly;
  }
  else {
    aNodeOwner = this;
  }
}


/*****
Write out a tmEdgeOwner*, which must be a tmTree, from a stream
*****/
void tmTree::PutOwnerPtr(ostream&, tmEdgeOwner* const)
{
  // Don't need to put anything
}


/*****
Read in a tmEdgeOwner*, which must be a tmTree*, from a stream
*****/
void tmTree::GetOwnerPtr(istream&, tmEdgeOwner*& aEdgeOwner)
{
  aEdgeOwner = this;
}


/*****
Write out a tmPathOwner*, which can be a tmPoly* or a tmTree*, to a stream
*****/
void tmTree::PutOwnerPtr(ostream& os, tmPathOwner* const aPathOwner)
{
  if (tmPoly* aPoly = aPathOwner->PathOwnerAsPoly()) {
    size_t isPoly = 1;
    PutPOD(os, isPoly);
    PutPtr(os, aPoly);
  }
  else {
    size_t isPoly = 0;
    PutPOD(os, isPoly);
  }
}


/*****
Read in a tmPathOwner*, which can be a tmPoly* or a tmTree*, from a stream
*****/
void tmTree::GetOwnerPtr(istream& is, tmPathOwner*& aPathOwner)
{
  size_t isPoly;
  GetPOD(is, isPoly);
  if (isPoly) {
    tmPoly* aPoly;
    GetPtr(is, aPoly);
    aPathOwner = aPoly;
  }
  else {
    aPathOwner = this;
  }
}


/*****
Write out a tmPolyOwner*, which can be a tmPoly* or a tmTree*,to a stream
*****/
void tmTree::PutOwnerPtr(ostream& os, tmPolyOwner* const aPolyOwner)
{
  if (tmPoly* aPoly = aPolyOwner->PolyOwnerAsPoly()) {
    size_t isPoly = 1;
    PutPOD(os, isPoly);
    PutPtr(os, aPoly);
  }
  else {
    size_t isPoly = 0;
    PutPOD(os, isPoly);
  }
}


/*****
Read in a tmPolyOwner*, which can be a tmPoly* or a tmTree*, from a stream
*****/
void tmTree::GetOwnerPtr(istream& is, tmPolyOwner*& aPolyOwner)
{
  size_t isPoly;
  GetPOD(is, isPoly);
  if (isPoly) {
    tmPoly* aPoly;
    GetPtr(is, aPoly);
    aPolyOwner = aPoly;
  }
  else {
    aPolyOwner = this;
  }
}


/*****
Write out a tmVertexOwner*, which can be a tmNode* or a tmPath*, to a stream
*****/
void tmTree::PutOwnerPtr(ostream& os, tmVertexOwner* const aVertexOwner)
{
  if (tmNode* aNode = aVertexOwner->VertexOwnerAsNode()) {
    PutPOD(os, size_t(1));
    PutPtr(os, aNode);
  }
  else {
    tmPath* aPath = aVertexOwner->VertexOwnerAsPath();
    PutPOD(os, size_t(0));
    PutPtr(os, aPath);
  }
}


/*****
Read in a tmVertexOwner*, which can be a tmNode* or a tmPath*, from a stream
*****/
void tmTree::GetOwnerPtr(istream& is, tmVertexOwner*& aVertexOwner)
{
  size_t isNode;
  GetPOD(is, isNode);
  if (isNode) {
    tmNode* aNode;
    GetPtr(is, aNode);
    aVertexOwner = aNode;
  }
  else {
    tmPath* aPath;
    GetPtr(is, aPath);
    aVertexOwner = aPath;
  }
}


/*****
Write out a tmCreaseOwner*, which can be a tmPath* or a tmPoly*, to a stream
*****/
void tmTree::PutOwnerPtr(ostream& os, tmCreaseOwner* const aCreaseOwner)
{
  if (tmPoly* aPoly = aCreaseOwner->CreaseOwnerAsPoly()) {
    PutPOD(os, size_t(1));
    PutPtr(os, aPoly);
  }
  else {
    tmPath* aPath = aCreaseOwner->CreaseOwnerAsPath();
    PutPOD(os, size_t(0));
    PutPtr(os, aPath);
  }
}


/*****
Read in a tmCreaseOwner*, which can be a tmPath* or a tmPoly*, from a stream
*****/
void tmTree::GetOwnerPtr(istream& is, tmCreaseOwner*& aCreaseOwner)
{
  size_t isPoly;
  GetPOD(is, isPoly);
  if (isPoly) {
    tmPoly* aPoly;
    GetPtr(is, aPoly);
    aCreaseOwner = aPoly;
  }
  else {
    tmPath* aPath;
    GetPtr(is, aPath);
    aCreaseOwner = aPath;
  }
}


/*****
Write out a tmFacetOwner*, which can only be a tmPoly*, to a stream
*****/
void tmTree::PutOwnerPtr(ostream& os, tmFacetOwner* const aFacetOwner)
{
  tmPoly* aPoly = aFacetOwner->FacetOwnerAsPoly();
  TMASSERT(aPoly);
  PutPtr(os, aPoly);
}


/*****
Read in a tmFacetOwner*, which can only be a tmPoly*, from a stream
*****/
void tmTree::GetOwnerPtr(istream& is, tmFacetOwner*& aFacetOwner)
{
  tmPoly* aPoly;
  GetPtr(is, aPoly);
  aFacetOwner = aPoly;
}


#ifdef __MWERKS__
  #pragma mark -
#endif


/*****
Dynamic type implementation
*****/
TM_IMPLEMENT_TAG(tmTree, "tree")
