/*******************************************************************************
File:         tmwxViewSettings.h
Project:      TreeMaker 5.x
Purpose:      Header file for TreeMaker design view settings
Author:       Robert J. Lang
Modified by:  
Created:      2003-11-27
Copyright:    ©2003 Robert J. Lang. All Rights Reserved.
*******************************************************************************/

#ifndef _TMWXVIEWSETTINGS_H_
#define _TMWXVIEWSETTINGS_H_

// Common TreeMaker and wxWidgets headers
#include "tmHeader.h"

/**********
class tmwxViewSettings
Settings for tree design view
**********/
class tmwxViewSettings {
public:
  bool mShowSymmetryLines;

  bool mShowLeafNodes;
  bool mShowBranchNodes;
  bool mShowSubNodes;
  bool mShowNodeDots;
  bool mShowNodeCircles;  
  bool mShowNodeIndices;
  bool mShowNodeCoords;
  bool mShowNodeElevation;
  bool mShowNodeDepth;
  bool mShowNodeLabels;
  bool mShowNodeFlags;
    
  bool mShowAllEdges;  
  bool mShowEdgeDots;
  bool mShowEdgeLines;  
  bool mShowEdgeIndices;
  bool mShowEdgeLengths;
  bool mShowEdgeStrain;
  bool mShowEdgeLabels;
  bool mShowEdgeFlags;
  
  bool mShowLeafPaths;
  bool mShowBranchPaths;
  bool mShowSubPaths;
  bool mShowActivePaths;
  bool mShowBorderPaths;
  bool mShowPolygonPaths;
  bool mShowInfeasiblePaths;
  bool mShowPathDots;
  bool mShowPathLines;
  bool mShowPathIndices;
  bool mShowPathLengths;
  bool mShowPathFlags;
  
  bool mShowMajorPolys;
  bool mShowSubPolys;
  bool mShowPolyDots;
  bool mShowPolyLines;
  bool mShowPolyFills;
  bool mShowPolyIndices;
  bool mShowPolyFlags;
  
  bool mShowMajorVertices;
  bool mShowMinorVertices;
  bool mShowVertexDots;
  bool mShowVertexIndices;
  bool mShowVertexCoords;
  bool mShowVertexElevation;
  bool mShowVertexDepth;
  bool mShowVertexDiscreteDepth;
  bool mShowVertexTreeNode;
  bool mShowVertexFlags;
  
  bool mShowMajorCreases;
  bool mShowMinorCreases;
  bool mShowCreaseDots;
  bool mShowCreaseLines;
  bool mShowCreaseFolds; // 0 = AGRH, 1 = MVF
  bool mShowCreaseIndices;
  bool mShowCreaseFlags;
  
  bool mShowAllFacets;
  bool mShowFacetDots;
  bool mShowFacetArrows;
  bool mShowFacetFills;
  bool mShowFacetIndices;
  bool mShowFacetOrder;
  bool mShowFacetCorridorEdge;
  bool mShowFacetFlags;
  
  bool mShowNodeConditions;
  bool mShowEdgeConditions;
  bool mShowPathConditions;
  bool mShowConditionDots;
  bool mShowConditionLines;
  bool mShowConditionIndices;
  bool mShowConditionTypes;
  
  // Comparison operator
  bool operator==(const tmwxViewSettings& rhs) const;

  // Predefined settings
  static tmwxViewSettings sNoneView;
  static tmwxViewSettings sDesignView;
  static tmwxViewSettings sCreasesView;
  static tmwxViewSettings sTreeView;
  static tmwxViewSettings sPlanView;
  static tmwxViewSettings sAllView;
};

#endif // _TMWXVIEWSETTINGS_H_
