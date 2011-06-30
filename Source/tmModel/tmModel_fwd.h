/*******************************************************************************
File:         tmModel_fwd.h
Project:      TreeMaker 5.x
Purpose:      Forward declarations of all model classes
Author:       Robert J. Lang
Modified by:  
Created:      2004-05-01
Copyright:    ©2004 Robert J. Lang. All Rights Reserved.
*******************************************************************************/

#ifndef _TMMODEL_FWD_H_
#define _TMMODEL_FWD_H_

// Common header
#include "tmHeader.h"

// All public TreeMaker model classes
class tmPoint;
class tmPart;
class tmCluster;
class tmTreeCleaner;
class tmTree;
class tmNode;
class tmNodeOwner;
class tmEdge;
class tmEdgeOwner;
class tmPath;
class tmPathOwner;
class tmPoly;
class tmPolyOwner;
class tmVertex;
class tmVertexOwner;
class tmCrease;
class tmCreaseOwner;
class tmFacet;
class tmFacetOwner;
class tmCondition;
class tmConditionOwner;
class tmConditionNodeCombo;
class tmConditionNodeFixed;
class tmConditionNodeOnCorner;
class tmConditionNodeOnEdge;
class tmConditionNodeSymmetric;
class tmConditionNodesPaired;
class tmConditionNodesCollinear;
class tmConditionEdgeLengthFixed;
class tmConditionEdgesSameStrain;
class tmConditionPathCombo;
class tmConditionPathActive;
class tmConditionPathAngleFixed;
class tmConditionPathAngleQuant;

class tmRootNetwork;

class tmDifferentiableFn;
class tmNLCO;
class tmNLCOUpdater;
class tmOptimizer;
class tmScaleOptimizer;
class tmEdgeOptimizer;
class tmStrainOptimizer;
class tmStubFinder;

#endif // _TMMODEL_FWD_H_
