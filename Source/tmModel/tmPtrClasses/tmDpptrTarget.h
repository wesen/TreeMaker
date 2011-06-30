/*******************************************************************************
File:         tmDpptrTarget.h
Project:      TreeMaker 5.x
Purpose:      Header file for class tmDpptrTarget
Author:       Robert J. Lang
Modified by:  
Created:      2003-11-15
Copyright:    ©2003 Robert J. Lang. All Rights Reserved.
*******************************************************************************/

#ifndef _TMDPPTRTARGET_H_
#define _TMDPPTRTARGET_H_

#include <vector>

// Forward declarations
class tmDpptrSrc;

/**********
class tmDpptrTarget
Base class for any object that is pointed to by a tmDpptr<T> or a tmDpptrArray<T>.
**********/
class tmDpptrTarget {
public:
  tmDpptrTarget();
  tmDpptrTarget(const tmDpptrTarget& aTarget);
  tmDpptrTarget& operator=(const tmDpptrTarget& aTarget);
  virtual ~tmDpptrTarget();
  std::size_t GetNumSrcs() {return mDpptrSrcs.size();};
private:
  std::vector<tmDpptrSrc*> mDpptrSrcs;  // list of objects pointing at me
  void AddDpptrSrc(tmDpptrSrc* r);    // add a pointer-to-me
  void RemoveDpptrSrc(tmDpptrSrc* r);    // remove a pointer-to-me
  friend class tmDpptrSrc;        // gives access to AddDpptrSrc() and RemoveDpptrSrc()
};

#endif // _TMDPPTRTARGET_H_
