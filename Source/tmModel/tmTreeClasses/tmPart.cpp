/*******************************************************************************
File:         tmPart.cpp
Project:      TreeMaker 5.x
Purpose:      Implementation file for class tmPart
Author:       Robert J. Lang
Modified by:  
Created:      2003-11-15
Copyright:    ©2003 Robert J. Lang. All Rights Reserved.
*******************************************************************************/

#include "tmPart.h"
#include "tmModel.h"

using namespace std;

/*  
All parts of the tree -- tmTree objects -- inherit two instances, mTree and
mIndex. tmTree objects are:

  tmTree
  tmNode
  tmEdge
  tmPath
  tmPoly
  tmVertex
  tmCrease
  tmFacet
  tmCondition and its descendants

mTree is a reference to the tree that owns the tmPart. 
mIndex is a unique index that identifies the part in stream I/O and in the user
interface. mIndex is a 1-based index, i.e., if there are N parts of type P, the
indices run from 1 to N. The tree, by arbitrary definition, has an index of 0.

Note that while mIndex can be read using GetIndex(), the only way to change it
is through tmTree::SetPartIndex(..), since changing the index implicitly
involves renumbering the other parts.
  
mTree is set in the tmPart(tmTree*) constructor. All descendants of a tmPart
are immediately assigned to a tmTree upon construction. Default and copy
constructors are left undefined because they shouldn't be used.

Each persistent class has a static member, Tag(), which contains the discrete
unsigned integer tag used to identify objects that are instances of that class.
The values are not necessarily the same from build to build; as we add more
classes, the tag for a given class may change. One should use the tag string
(see next) as an unchangeable indicator of part type. Tags start at 0 and are
consecutive.

Each class also has a static member TagStr(), which contains the unique
4-character code that identifies the object type in file storage and stream
I/O. This 4-character code will never change, as it is essential to file
backward/forward compatibility.
*/

/**********
class tmPart
Base class for all TreeMaker objects. They all point back to the tmTree that
owns them and all have an index.
**********/

#ifdef __MWERKS__
  #pragma mark --PUBLIC--
#endif


/*****
Return a C++ string containing the index of this part. Useful in debugging.
*****/
string tmPart::GetStr() const
{
  ostringstream ss;
  ss << GetIndex();
  return ss.str();
}


/*****
STATIC
Return the index for the passed part. Return 0 if the passed ptr is NULL.
This performs the same function as this->GetIndex() but is safe to use with
a potentially NULL ptr.
*****/
std::size_t tmPart::GetIndex(const tmPart* part)
{
  if (part) return part->GetIndex();
  else return 0;
}


/*****
STATIC
Return a C++ string containing the index of this part, or 0 for a NULL ptr.
This is safe to use with a potentially NULL ptr.
*****/
std::string tmPart::GetStr(const tmPart* part)
{
  if (part) return part->GetStr();
  else return "";
}


#ifdef __MWERKS__
  #pragma mark -
  #pragma mark --PROTECTED--
#endif


/*****
Creator constructor
*****/
tmPart::tmPart(tmTree* aTree)
  : mIndex(0), mTree(aTree)
{
}


#ifdef __MWERKS__
  #pragma mark -
#endif


/*
Notes on line endings.

First, a reminder on conventions (noted here because I can never remember which
is which).

Unix terminates lines with a newline, '\n'. Windows terminates lines with a
carriage return '\r' + newline '\n' Mac Classic terminates lines with a
carriage return, '\r' Mac OS X, being Unix, follows the Unix convention.

In versions 3 and 4, TreeMaker has used the CR ('\r') as the line ending for
file data. Beginning with version 5, we use Unix convention '\n', but for
backward compatibility, we read anything.

All data is stored in files as character strings followed by a newline.
Thus, for example, a tmFloat with a value of 1.0 would be stored as the string

'1.00000\n'    (the number followed by a newline.)

Character strings are handled a bit differently, discussed below. Also noted
below: there are some extra details in how floating-point numbers are handled.

The line ending character is stored in the private static variable
tmPart::sEndl. Normally it is a newline '\n'. When we export to version 4, TM4
wants to see '\r', so we use the private stack class Endl to temporarily change
to the Mac line ending; it automatically is reverted when the Endl object goes
out of scope.
*/

/* 
turn on this flag to echo data to the Log frame as we read it in. Useful for 
debugging I/O, but only for small files.
*/
#define ECHO_INPUT 0


/*****
STATIC
Discards blank (space plus tab) characters; if there's a line break,
discards it too.
*****/
void tmPart::ConsumeTrailingSpace(istream &is) {
  if (is.eof ())
    return;
  char k;
  // skip any whitespace
  while (is.get (k) && (k == ' ' || k == '\t'))
    ; // in GCC, there's isblank(k)
  if (! is.eof())
    switch (k) {
    case '\n': // found Unix or OS X-style eoline
      break;
    case '\r':
      is.get (k);
      if (k >= 0 && k != '\n') // Mac classic-style eoline
  is.putback (k);
      break;  // else eofile or Windows-style eoline
    default: // oops, read too much
      is.putback (k);
    }
}


/*****
STATIC
Put an array of tmPoint to a stream.
*****/
void tmPart::PutPOD(ostream& os, const tmArray<tmPoint>& aArray)
{
  size_t n = aArray.size();
  PutPOD(os, n);
  for (size_t i = 0; i < n; ++i) PutPOD(os, aArray[i]);
}  
    

/*****
STATIC
Get an array of tmPoint from a stream.
*****/
void tmPart::GetPOD(istream& is, tmArray<tmPoint>& aArray)
{
  size_t n;
  GetPOD(is, n);
  aArray.resize(n);
  for (size_t i = 0; i < n; ++i) GetPOD(is, aArray[i]);
#if ECHO_INPUT
  TMLOG(wxString::Format("GetPOD(is, tmArray<tmPoint>) size=%lu", n));
#endif // ECHO_INPUT
}


/*****
STATIC
Put a size_t to a stream.
*****/
void tmPart::PutPOD(ostream& os, const size_t& aSize_t)
{
  os << aSize_t << sEndl;
}  
    

/*****
STATIC
Get a size_t from a stream.
*****/
void tmPart::GetPOD(istream& is, size_t& aSize_t)
{
  is >> aSize_t;
  if (is.bad () || is.fail ())
    throw EX_IO_BAD_TOKEN ("unknown"); // TODO improve diag
  ConsumeTrailingSpace(is);
#if ECHO_INPUT
  TMLOG(wxString::Format("GetPOD(is, size_t) %lu", aSize_t));
#endif // ECHO_INPUT
}
    

/*****
STATIC
Put an int to a stream.
*****/
void tmPart::PutPOD(ostream& os, const int& aInt)
{
  os << aInt << sEndl;
}    
    

/*****
STATIC
Get an int from a stream.
*****/
void tmPart::GetPOD(istream& is, int& aInt)
{
  is >> aInt;
  if (is.bad () || is.fail ())
    throw EX_IO_BAD_TOKEN ("unknown"); // TODO improve diag
  ConsumeTrailingSpace(is);
#if ECHO_INPUT
  TMLOG(wxString::Format("GetPOD(is, int) %d", aInt));
#endif // ECHO_INPUT
}


/*****
STATIC
Put a float to a stream.
*****/
void tmPart::PutPOD(ostream& os, const tmFloat& aFloat)
{
  os << aFloat << sEndl;
}


/*****
STATIC
Get a float from a stream.
*****/
void tmPart::GetPOD(istream& is, tmFloat& aFloat)
{
  // TM4 put some numeric values as NAN(017) and the standard library doesn't
  // handle these (it corrupts our token breaks). This situation only arises
  // when the value is irrelevant; so we'll check for non-numeric input and if
  // it's a NAN, we'll set the appropriate value to zero.
  char ch;
  is.get(ch);  // either normal number or start of "NAN(017)"
  is.putback(ch);
  if (ch == 'N') {
    string dummy;
    is >> dummy;
    aFloat = 0.0;
  }
  else
    is >> aFloat;
  if (is.bad () || is.fail ())
    throw EX_IO_BAD_TOKEN ("unknown"); // TODO improve diag
  ConsumeTrailingSpace(is);
#if ECHO_INPUT
  TMLOG(wxString::Format("GetPOD(is, float) %.6f", aFloat));
#endif // ECHO_INPUT
}
    
    
/*****
STATIC
Put a bool to a stream.
*****/
void tmPart::PutPOD(ostream& os, const bool& aBoolean)
{
  if (aBoolean) PutPOD(os, "true");
  else PutPOD(os, "false");
}
    
    

/*****
STATIC
Get a bool from a stream.
*****/
void tmPart::GetPOD(istream& is, bool& aBoolean)
{
  string s;
  GetPOD(is, s);
  aBoolean = (s == "true");
  ConsumeTrailingSpace(is);
#if ECHO_INPUT
  TMLOG(wxString::Format("  GetPOD(is, bool) %d", int(aBoolean)));
#endif // ECHO_INPUT
}


/*****
STATIC
Put a tmPoint to a stream.
*****/
void tmPart::PutPOD(ostream& os, const tmPoint& aPoint)
{
  PutPOD(os, aPoint.x);
  PutPOD(os, aPoint.y);
}
    
    

/*****
STATIC
Get a tmPoint from a stream.
*****/
void tmPart::GetPOD(istream& is, tmPoint& aPoint)
{
  GetPOD(is, aPoint.x);
  GetPOD(is, aPoint.y);
#if ECHO_INPUT
  TMLOG(wxString::Format("  GetPOD(is, tmPoint) (%.6f, %.6f)", aPoint.x, aPoint.y));
#endif // ECHO_INPUT
}


/*
Notes on character string I/O.

Character strings are also put as a string of characters followed by a line
break, rather than as a null-terminated C string. Since version 3, TreeMaker
has put empty strings as blank lines, i.e., '\r' only.

Now we use the C++ stream mechanism for stream I/O. The default behavior for
streams delimits with whitespace characters, so after reading numbers and
C++ strings we skip trailing whitespace and (if found) a line break; that
way, a C string may have whitespace in any position. An empty string is
denoted by a line break at the beginning or the end, or two consecutive
line breaks.
As a rule, each (C or C++) string item should be at its own line. C++
tokens can share a line, but if a C string follows a C++ token, the former
can't have leading whitespace. C++ tokens can never follow C++ strings.

Note that CodeWarrior compilers (at least on Mac) offer the option of swapping
'\n' and '\r', which offers amazing potential for subtle inconsistencies among
builds. Hence the following check.
*/

#if ('\n' != '\012')
  #error "Error in mapping of newlines and CRs"
#endif
#if ('\r' != '\015')
  #error "Error in mapping of newlines and CRs"
#endif


/*
We use the std library string class for stream I/O rather than C strings
whenever possible to take advantage of its protection against buffer overflows.
*/


/*****
STATIC
Put a std library string to the stream, ending it with a carriage return.
*****/
void tmPart::PutPOD(ostream& os, const string& s)
{
  // for version 3 & 4 back-compatibility, put empty strings as blank lines.
  if (s.empty()) os << sEndl;
  else os << s << sEndl;
}


/*****
STATIC
Read a std library string from the stream.
*****/
void tmPart::GetPOD(istream& is, string& s)
{
  char ch1, ch2;
  is.get(ch1);
  is.get(ch2);

#if 1  // be generous about accepting '\r'
  if ((ch1 == '\n' || ch1 == '\r') && (ch2 == '\n' || ch2 == '\r')) {
#else  // be finicky
  TMASSERT(ch1 != '\r');
  TMASSERT(ch2 != '\r');
  if (ch1 == '\n' && ch2 == '\n') {
#endif
    s = "";
    is.putback(ch2);
  }
  else {
    is.putback(ch2);
    is.putback(ch1);
    is >> s;
    ConsumeTrailingSpace(is);
  }
#if ECHO_INPUT
  TMLOG(wxString::Format("GetPOD(is, string) %s", s.c_str()));
#endif // ECHO_INPUT
}  


/*****
STATIC
Put a C string to the stream, ending it with a carriage return.
*****/
void tmPart::PutPOD(ostream& os, const char* c)
{
  for ( ; *c; ++c)
    // replace special characters by escape sequences
    switch (*c) {
    case '\n': os << "\\n"; break;
    case '\r': os << "\\r"; break;
    case '\\': os << "\\\\"; break;
    default:   os << *c;
    }
  os << sEndl;
}


/*****
STATIC
Read a C string from the stream.
*****/
void tmPart::GetPOD(istream& is, char* c)
{
  // we'd like to use is.getline, but also be forgiving about eoline chars
  int i = 0, ok = 1;
  while (ok) {
    char k;
    if (! is.get (k)) // eof
      break;
    switch (k) {
    case '\r': // Mac Classic or Windows eoline
      if (is.get (k) && k != '\n')
  is.putback (k);
      // fallthrough
    case '\n': // Unix or OS X eoline
      ok = 0;
      break;
    case '\\': // escape sequence
      is.get (k);
      switch (k) {
      case 'n': k = '\n'; break;
      case 'r': k = '\r'; break;
      case '\\':k = '\\'; break;
      default: throw EX_IO_BAD_ESCAPE(string("\\") + k);
  // eof or unknown escape sequence
      }
      // fallthrough
    default: // normal character
      if (i >= MAX_LABEL_LEN)
        throw EX_IO_TOO_LONG_STRING(string(c, size_t(MAX_LABEL_LEN)) + 
          "+[more]");
      c [i++] = k;
    }
  }
  c [i] = 0;
#if ECHO_INPUT
  TMLOG(wxString::Format("  GetPOD(is, char*) %s", c));
#endif // ECHO_INPUT
}  


#ifdef __MWERKS__
  #pragma mark -
#endif


/*****
STATIC
Return a reference to the class tag static variable. Note that this number
gets changed by the dynamic type system when we call InitTypes(), which is why
this returns a reference. Each class overloads this static function, then
overrides the related virtual function GetTag() to return the class tag.
*****/
size_t& tmPart::Tag()
{
  TMFAIL("tmPart::Tag()");  // should never call this
  static size_t sTag = RAW_TAG;
  return sTag;
}


/*****
STATIC
Return the class tag string. Each class overloads this static function, then
overrides the related virtual function GetTagStr() to return the class tag
string.
*****/
const string& tmPart::TagStr()
{
  TMFAIL("tmPart::TagStr()"); // should never call this
  static const string sTagStr("NULL");
  return sTagStr;
}


#ifdef __MWERKS__
  #pragma mark -
  #pragma mark --PRIVATE--
#endif

/*****
STATIC
Default line ending character
*****/
char tmPart::sEndl = '\n';


/*****
STATIC
Read a tag string from a stream and return the resulting tag. If the string
isn't in the right format, throw a EX_IO_BAD_TAG. If it's in the right format but
we don't recognize it, TagToStr() will throw a EX_IO_UNRECOGNIZED_TAG. This is a
good place to remind that tmPart::GetTag() returns the tag for the given part
polymorphically.
*****/
void tmPart::GetPODTag(istream& is, size_t& tag)
{
  TMASSERT(TypesAreInitialized());
  string tagstr;
  GetPOD(is, tagstr);
  if (tagstr.length() != 4) {
    throw EX_IO_BAD_TAG(tagstr);
  }
  tag = StrToTag(tagstr);
#if ECHO_INPUT
  TMLOG(wxString::Format("  GetTag() %s", tagstr.c_str()));
#endif // ECHO_INPUT
}


/*****
STATIC
Return the string associated with this tag, which will be the value of P::Tag()
for some class P. Throw an exception if none is found.
*****/
const string& tmPart::TagToStr(size_t tag)
{
  TMASSERT(TypesAreInitialized());
  TMASSERT(tag != RAW_TAG);
  if (tag >= GetTagStrs().size()) {
    stringstream ss;
    ss << "[tag ID] " << int(tag);
    throw EX_IO_UNRECOGNIZED_TAG(ss.str());
  }
  return GetTagStrs()[tag];
}


/*****
STATIC
Return the tag associated with this string, which should be the value that was
assigned in RegisterAllTags(). This can throw two exceptions: EX_IO_BAD_TAG if
the string has the wrong format, EX_IO_UNRECOGNIZED_TAG if it's the right
format but we don't recognize it.
*****/
size_t tmPart::StrToTag(const string& tagstr)
{
  TMASSERT(TypesAreInitialized());
  if (tagstr.length() > 4) {
    throw EX_IO_BAD_TAG(tagstr);
  }
  for (size_t i = 0; i < GetTagStrs().size(); ++i)
    if (GetTagStrs()[i] == tagstr) return i;
  throw EX_IO_UNRECOGNIZED_TAG(tagstr);
}


#ifdef __MWERKS__
#pragma mark -
#endif


/*****
STATIC
Return a reference to our list of all class tag strings.
*****/
tmArray<string>& tmPart::GetTagStrs()
{
  static tmArray<string> sTagStrs;
  return sTagStrs;
}


/*****
STATIC
Return the number of types we've registered so far.
*****/
size_t tmPart::GetNumTypes()
{
  return GetTagStrs().size();
}


/*****
STATIC
Return a reference to our list of all creator functions.
*****/
tmArray<tmPart::CreatorFn>& tmPart::GetCreatorFns()
{
  static tmArray<tmPart::CreatorFn> sCreatorFns;
  return sCreatorFns;
}


#ifdef __MWERKS__
#pragma mark -
#endif


/*****
STATIC
Register all tags that are used in polymorphic I/O. This should be called
exactly once at program initialization. We use this delayed initialization
rather than some form of static variable initialization so that we can count on
all of the class GetTagStrs() members having been initialized as static variables
(recall that static variable initialization order is undefined). Note that the
size_t tag values associated with each class may change if we've added classes,
but the strings, which are used in stream I/O, will always be the same.
*****/
void tmPart::InitTypes()
{
  TMASSERT(GetTagStrs().empty());
  MakeTypeArray<string, StringT>(GetTagStrs());
  MakeTypeArray<CreatorFn, CreatorFnT>(GetCreatorFns());
}


#ifdef __MWERKS__
  #pragma mark -
#endif


#if 0 // not needed, just for illustration
/*
This snippet illustrates how to use the tmPart::MakeTypeArray<R, G<T> >()
facility to create an array of member function pointers organized by template
type, which is a way of faking virtual function behavior when the base class
object is an argument of some other function, rather than an object invoking a
member function. The need arises when you'd like to add type-dependent
behaviors to a class whose whose declaration hierarchy is closed, so you can't
add more virtual functions to it. We use this capability in TreMaker to allow
clients to add type-dependent behavior to their GUI without having to add GUI
virtual functions to the model objects themselves.

The way this is done is, you create a template class that incorporates the
desired behavior for each type of object. Default behaviors (pseudo-base class)
are defined by the general template definition; specific behaviors
(pseudo-derived class) are defined by template specializations. You first call
MakeTypeArray<R, G<T> >() to construct an array of objects returned by the
template class operator()(), each of which produces the desired behavior for
its type; in the the most common case, these are pointers to member functions,
but they can be as simple as type-dependent strings (see tmPart::GetTagStrs()).
Once the array has been created, you can invoke the desired behavior from any
persistent tmPart (either base class or derived) by using its GetTag() value as
the index into this array.

A nice feature of this approach is that new classes in the hierarchy can be
accommodated throughout the program by altering only MakeTypeArray<R, G<T> >()
(and an adjacent macro) and then adding the appropriate new template
specializations. Another nice feature is that after initialization, all calls
are executed in constant time wrt the number of classes. Previously, I'd
achieved this kind of type-dependent behavior by large switch() statements
which were (a) big and unwieldy, (b) linear time wrt the number of classes, and
(c) every switch() had to be tracked down and edited whenever I added a new
class.

A sample usage idiom is shown below. (Class tmPart uses it for-real in two
places.) The idea in this example is that within the class Foo, we have a
template member function GooP<P>() that does something different for each type
P (which could be entirely different if GooP<P>() is specialized on each type
P). If we call function GooP(tmPart* p), passing it a base class pointer (e.g.,
tmPart* or tmCondition*), the call will get routed to the appropriate
instantiation of function Goo<P>(p), where P is the actual dynamic type of p.

The array of function pointers is implemented as a singleton function so that
initialization of the function pointer array happens at first usage, thus
avoiding potential initialization order issues.
*/

class Foo {
public:
  void GooP(tmPart* p);          // function that receives a base class ptr
  template <class P>
    void Goo(tmPart* p);        // templated member function we want to invoke
  typedef void (Foo::*GooFn)(tmPart*);  // type of ptr to preceding member fn
  template <class P>            // template class, whose operator()() returns
    class GooFnT {            // the desired type-dependent behavior (in
    public:                // this case, a template function ptr).
      GooFn operator()() const;
    };
  static tmArray<GooFn>& GetGooFns(); // This returns the array of function ptrs
};

void Foo::GooP(tmPart* p)
{
  // this call invokes Goo<P>(p) where P is the dynamic type of p
  (this->*GetGooFns()[p->GetTag()])(p);

template <class P>
void Foo::Goo(tmPart* p)
{
  P* pp = dynamic_cast<P*>(p);
  // now we can do something with "true" object ptr pp
}

template <class P>
Foo::GooFn Foo::GooFnT<P>::operator()() const
{
  return &Foo::Goo<P>;
}

tmArray<GooFn>& Foo::GetGooFns()
{
  static tmArray<GooFn> sGooFns;
  if (sGooFns.empty())
    tmPart::MakeTypeArray<GooFn, GooFnT>(sGooFns);
  return sGooFns;
}
#endif
