/*******************************************************************************
File:         tmPart.h
Project:      TreeMaker 5.x
Purpose:      Header file for class tmPart
Author:       Robert J. Lang
Modified by:  
Created:      2003-11-15
Copyright:    ©2003 Robert J. Lang. All Rights Reserved.
*******************************************************************************/

#ifndef _TMPART_H_
#define _TMPART_H_

// Common TreeMaker header
#include "tmHeader.h"

// Standard libraries
#include <iostream>
#include <map>

// TreeMaker objects
#include "tmModel_fwd.h"
#include "tmArray.h"
#include "tmPoint.h"

/**********
Dynamic type system macros
**********/

/*****
Declaration. Applies to any tmPart subclass that can be written to/read from a
stream
*****/
#define TM_DECLARE_TAG() \
public:\
  virtual const std::size_t& GetTag() const {return Tag();};\
  virtual const std::string& GetTagStr() const {return TagStr();};\
protected:\
  static std::size_t& Tag();\
  static const std::string& TagStr();
  

/*****
Implementation. Note that some classes (tmPart, tmCondition) handle this
themselves.
*****/
#define TM_IMPLEMENT_TAG(PART_CLASS, PART_STRING) \
size_t& PART_CLASS::Tag() {\
  static size_t sTag = std::size_t(-1);\
  return sTag;}\
const string& PART_CLASS::TagStr() {\
  static const string sTagStr(PART_STRING);\
  return sTagStr;}


/**********
class tmPart
Base class for all TreeMaker objects. They all point back to the tmTree that
owns them and all have an index.
**********/
class tmPart {
public:
  // Exceptions
  class EX_IO_BAD_TOKEN {
    // Base exception class, stores the offending token
    public:
    std::string mToken;
    EX_IO_BAD_TOKEN(const std::string& token) : 
      mToken(token) {};
  };
  class EX_IO_TOO_LONG_STRING : public EX_IO_BAD_TOKEN {
    // A too-long string was encountered
    public:
    EX_IO_TOO_LONG_STRING(const std::string& token) : 
      EX_IO_BAD_TOKEN(token) {};
  };
  class EX_IO_BAD_ESCAPE : public EX_IO_BAD_TOKEN {
    // A bad escape sequence was encountered
    public:
    EX_IO_BAD_ESCAPE(const std::string& token) : 
      EX_IO_BAD_TOKEN(token) {};
  };
  class EX_IO_UNRECOGNIZED_TAG : public EX_IO_BAD_TOKEN {
    // It's got the right format (4 chars) but I don't know it
    public:
    EX_IO_UNRECOGNIZED_TAG(const std::string& token) : 
      EX_IO_BAD_TOKEN(token) {};
  };
  class EX_IO_BAD_TAG : public EX_IO_BAD_TOKEN {
    // The character string can't be a tag, it isn't 4 chars
    public:
    EX_IO_BAD_TAG(const std::string& token) : 
      EX_IO_BAD_TOKEN(token) {};
  };
  
  // Constant(s)
  enum {MAX_LABEL_LEN = 31
    // maximum length of labels in streams
  };
  enum {DEPTH_NOT_SET = -999
    // a depth value that hasn't been set to anything yet
  };
  
  // Geometric tolerances
  static tmFloat ConvexityTol() {
    // tolerance on angles in convexity
    return 1.0e-4;
  };
  static tmFloat DistTol() {
    // tolerance on distances
    return 1.0e-4;
  };
  static bool IsTiny(const tmFloat& d) {
    // return true if this distance is smaller in magnitude than DistTol().
    return fabs(d) < DistTol();
  };
  
  // Getters
  std::size_t GetIndex() const {return mIndex;};
  tmTree* GetTree() const {return mTree;};
  std::string GetStr() const;
  static std::size_t GetIndex(const tmPart* part);
  static std::string GetStr(const tmPart* part);
  
  // Dynamic type system initialization
  static void InitTypes();
  static bool TypesAreInitialized() {return GetNumTypes() != 0;};
  template <class R, template <class P> class G>
    static void MakeTypeArray(tmArray<R>& gtlist);

protected:
  // Destructor
  virtual ~tmPart() {};

  // Dynamic type system, overloaded/overridden by subclasses
  enum {RAW_TAG = std::size_t(-1)};
  TM_DECLARE_TAG()

private:
  // Member data
  std::size_t mIndex;     // 1-based index of this part in the tree
  tmTree* mTree;          // tree that this part belongs to
  static char sEndl;      // line ending character (\n is default, \r for Mac)
  
  // Constructor
  tmPart(tmTree* aTree);
  
  // Discards [ \t]*(\r | \n | \r\n)?
  static void ConsumeTrailingSpace(std::istream &is);
  
  // Line ending stack class
  struct Endl {
    char mEndl;
    Endl(char eol) {mEndl = sEndl; sEndl = eol;};
    ~Endl() {sEndl = mEndl;};
  };

  // Stream I/O of elemental types
  static void PutPOD(std::ostream& os, const tmArray<tmPoint>& aArray);
  static void GetPOD(std::istream& is, tmArray<tmPoint>& aArray);
    
  static void PutPOD(std::ostream& os, const int& aInt);
  static void GetPOD(std::istream& is, int& aInt);
    
  static void PutPOD(std::ostream& os, const std::size_t& aSize_t);
  static void GetPOD(std::istream& is, std::size_t& aSize_t);
    
  static void PutPOD(std::ostream& os, const bool& aBoolean);
  static void GetPOD(std::istream& is, bool& aBoolean);
    
  static void PutPOD(std::ostream& os, const tmFloat& aFloat);
  static void GetPOD(std::istream& is, tmFloat& aFloat);
    
  static void PutPOD(std::ostream& os, const tmPoint& aPoint);
  static void GetPOD(std::istream& is, tmPoint& aPoint);
    
  static void PutPOD(std::ostream& os, const char* aString);
  static void GetPOD(std::istream& is, char* aString);
  
  static void PutPOD(std::ostream& os, const std::string& aString);
  static void GetPOD(std::istream& is, std::string& aString);

  // Tag I/O
  static void GetPODTag(std::istream& is, std::size_t& tag);
  template <class P>
    static void CheckTagStr(std::istream& is);
  
  // Dynamic type system implementation
  static const std::string& TagToStr(std::size_t tag);
  static std::size_t StrToTag(const std::string& tagstr);

  static tmArray<std::string>& GetTagStrs();
  template <class P>
    class StringT {
    public:
      StringT();
      const std::string& operator()();
    };
  static std::size_t GetNumTypes();
    
  typedef tmPart* (*CreatorFn)(tmTree*);
  static tmArray<CreatorFn>& GetCreatorFns();
  template <class P>
    class CreatorFnT {
    public:
      CreatorFn operator()();
      static tmPart* Create(tmTree* aTree);
    };

  // Hide other constructors
  tmPart();
  tmPart(const tmPart& aPart);
  
  // Friend classes
  friend class Endl;
  friend class tmTree;
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


/**********
Template definitions
**********/

/*****
STATIC
Read the next word from the stream and verify that it's the tag string for the
templated type. Throw an exception if it isn't.
*****/
template <class P>
void tmPart::CheckTagStr(std::istream& is)
{
  TMASSERT(TypesAreInitialized());
  std::string tagstr;
  GetPOD(is, tagstr);
  if (StrToTag(tagstr) != P::Tag()) 
    throw EX_IO_BAD_TAG(tagstr);
}


/*****
Constructor. Set the template type tag, an important side effect of
construction.
*****/
template <class P>
tmPart::StringT<P>::StringT()
{
  P::Tag() = GetNumTypes();
}


/*****
Return the tag string for the template type
*****/
template <class P>
const std::string& tmPart::StringT<P>::operator()()
{
  return P::TagStr();
}


/*****
Return a pointer to the function that creates a new object of the template type
*****/
template <class P>
tmPart::CreatorFn tmPart::CreatorFnT<P>::operator()()
{
  return &tmPart::CreatorFnT<P>::Create;
}


/*****
STATIC
Static member function that creates a new object of the template type
*****/
template <class P>
tmPart* tmPart::CreatorFnT<P>::Create(tmTree* aTree)
{
  return new P(aTree);
}


/*****
STATIC
This template function creates an array of objects G<P> and stores them in
gtlist, where for a type P, the object G<P> has index P::Tag() (accessible via
P::GetTag()). Every list will always be in the same order with respect to the
dynamic types P.  Any new dynamic types added to the model need to get added to
this list.
*****/
template <class R, template <class P> class G>
void tmPart::MakeTypeArray(tmArray<R>& gtlist)
{
  gtlist.clear();
  // This two-step push_back is necessary to prevent a series of warnings
  // "non-const '&' reference initialized to temporary" that arises if we try
  // it in one step, e.g., gtlist.push_back(G<tmTree>()());
  R r;
  r = G<tmTree>()(); gtlist.push_back(r);
  r = G<tmNode>()(); gtlist.push_back(r);
  r = G<tmEdge>()(); gtlist.push_back(r);
  r = G<tmPath>()(); gtlist.push_back(r);
  r = G<tmPoly>()(); gtlist.push_back(r);
  r = G<tmVertex>()(); gtlist.push_back(r);
  r = G<tmCrease>()(); gtlist.push_back(r);
  r = G<tmFacet>()(); gtlist.push_back(r);
  r = G<tmConditionNodeCombo>()(); gtlist.push_back(r);
  r = G<tmConditionNodeFixed>()(); gtlist.push_back(r);
  r = G<tmConditionNodeOnCorner>()(); gtlist.push_back(r);
  r = G<tmConditionNodeSymmetric>()(); gtlist.push_back(r);
  r = G<tmConditionNodeOnEdge>()(); gtlist.push_back(r);
  r = G<tmConditionNodesPaired>()(); gtlist.push_back(r);
  r = G<tmConditionNodesCollinear>()(); gtlist.push_back(r);
  r = G<tmConditionEdgeLengthFixed>()(); gtlist.push_back(r);
  r = G<tmConditionEdgesSameStrain>()(); gtlist.push_back(r);
  r = G<tmConditionPathCombo>()(); gtlist.push_back(r);
  r = G<tmConditionPathActive>()(); gtlist.push_back(r);
  r = G<tmConditionPathAngleFixed>()(); gtlist.push_back(r);
  r = G<tmConditionPathAngleQuant>()(); gtlist.push_back(r);
}


/*****
MACRO
This macro takes a template name G and creates a sequence of declarations of
the form "friend class G<T>;" for each of the persistent classes. It saves a
lot of declarations when the template class G<T> used in the preceding function
accesses a private member of its owner class (e.g., a member template
function). Note that the macro should not be terminated with a semicolon.
*****/
#define TM_TEMPLATE_FRIENDS(G)\
  friend class G<tmTree>;\
  friend class G<tmNode>;\
  friend class G<tmEdge>;\
  friend class G<tmPath>;\
  friend class G<tmPoly>;\
  friend class G<tmVertex>;\
  friend class G<tmCrease>;\
  friend class G<tmFacet>;\
  friend class G<tmConditionNodeCombo>;\
  friend class G<tmConditionNodeFixed>;\
  friend class G<tmConditionNodeOnCorner>;\
  friend class G<tmConditionNodeSymmetric>;\
  friend class G<tmConditionNodeOnEdge>;\
  friend class G<tmConditionNodesPaired>;\
  friend class G<tmConditionNodesCollinear>;\
  friend class G<tmConditionEdgeLengthFixed>;\
  friend class G<tmConditionEdgesSameStrain>;\
  friend class G<tmConditionPathCombo>;\
  friend class G<tmConditionPathActive>;\
  friend class G<tmConditionPathAngleFixed>;\
  friend class G<tmConditionPathAngleQuant>;


#endif // _TMPART_H_
