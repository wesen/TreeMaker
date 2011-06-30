/*******************************************************************************
File:         tmOptimizer.cpp
Project:      TreeMaker 5.x
Purpose:      Implementation file for tmOptimizer base class
Author:       Robert J. Lang
Modified by:  
Created:      2005-09-13
Copyright:    ©2005 Robert J. Lang. All Rights Reserved.
*******************************************************************************/

#include "tmOptimizer.h"
#include "tmModel.h"

using namespace std;

/**********
class tmOptimizer
Base class for TreeMaker optimizers. Provides two virtual methods that all
subclasses must support, used for transferring data between optimizer and tree.
**********/

/*****
Constructor
*****/
tmOptimizer::tmOptimizer(tmTree* aTree, tmNLCO* aNLCO)
  : tmTreeCleaner(aTree), mInitialized(false), mNLCO(aNLCO)
{
  aTree->PutSelf(mInitialState);
}


/*****
Revert to the pre-optimization configuration.
*****/
void tmOptimizer::Revert()
{
  GetTree()->GetSelf(mInitialState);
}


/*****
Optimize the tree.
*****/
void tmOptimizer::Optimize()
{
  TMASSERT(mInitialized);
  
  // Minimize the merit function subject to the constraints. Exceptions can be
  // generated either by user cancellation.
  std::vector<double> scratchState = mCurrentStateVec;
  int inform = mNLCO->Minimize(scratchState);
  mCurrentStateVec = scratchState;
  
  // Set status
  if (inform != 0) throw tmNLCO::EX_BAD_CONVERGENCE(inform);
  
  // Copy the data into the tree from the state vector
  DataToTree();
}
