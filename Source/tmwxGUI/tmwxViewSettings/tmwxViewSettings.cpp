/*******************************************************************************
File:         tmwxViewSettings.cpp
Project:      TreeMaker 5.x
Purpose:      Implementation file for TreeMaker design settings
Author:       Robert J. Lang
Modified by:  
Created:      2003-11-27
Copyright:    ©2003 Robert J. Lang. All Rights Reserved.
*******************************************************************************/

#include "tmwxViewSettings.h"

/**********
class tmwxViewSettings
**********/

/*****
Static initialization
*****/

// predefined settings: the fields of a tmwxViewSettings object are as follows:
// Paper
// Node
// Edge
// Path
// Poly
// Vertex
// Crease
// Facet
// Condition

tmwxViewSettings tmwxViewSettings::sAllView = {
  1,\
  1,1,1,1,1,1,1,1,1,1,1,\
  1,1,1,1,1,1,1,1,\
  1,1,1,1,1,1,1,1,1,1,1,1,\
  1,1,1,1,1,1,1,\
  1,1,1,1,1,1,1,1,1,1,\
  1,1,1,1,0,1,1,\
  1,1,1,1,1,1,1,1,\
  1,1,1,1,1,1,1};
  
tmwxViewSettings tmwxViewSettings::sTreeView = {
  1,\
  1,1,0,1,0,1,0,0,0,1,0,\
  1,1,1,1,1,1,1,0,\
  0,0,0,0,0,0,0,0,0,0,0,0,\
  0,0,0,0,0,0,0,\
  0,0,0,0,0,0,0,0,0,0,\
  0,0,0,0,0,0,0,\
  0,0,0,0,0,0,0,0,\
  1,1,0,1,1,1,1};
  
tmwxViewSettings tmwxViewSettings::sDesignView = {
  1,\
  1,1,0,1,1,1,0,0,0,1,0,\
  1,1,1,1,1,1,1,0,\
  0,0,0,1,1,1,1,1,1,1,0,0,\
  1,1,1,0,1,0,0,\
  0,0,0,0,0,0,0,0,0,0,\
  1,1,0,1,0,0,0,\
  0,0,0,0,0,0,0,0,\
  1,1,1,1,1,1,1};
  
tmwxViewSettings tmwxViewSettings::sCreasesView = {
  0,\
  0,0,0,0,0,0,0,0,0,0,0,\
  0,0,0,0,0,0,0,0,\
  0,0,0,0,0,0,0,0,0,0,0,0,\
  0,0,0,0,0,0,0,\
  0,0,0,0,0,0,0,0,0,0,\
  1,1,0,1,1,0,0,\
  0,0,0,0,0,0,0,0,\
  0,0,0,0,0,0,0};
  
tmwxViewSettings tmwxViewSettings::sPlanView = {
  0,\
  0,0,0,0,0,0,0,0,0,0,0,\
  0,0,0,0,0,0,0,0,\
  0,0,0,0,0,0,0,0,0,0,0,0,\
  0,0,0,0,0,0,0,\
  1,0,0,0,1,0,0,0,0,0,\
  1,1,0,1,1,0,0,\
  0,0,0,0,0,0,0,0,\
  0,0,0,0,0,0,0};
  
tmwxViewSettings tmwxViewSettings::sNoneView = {
  0,\
  0,0,0,0,0,0,0,0,0,0,0,\
  0,0,0,0,0,0,0,0,\
  0,0,0,0,0,0,0,0,0,0,0,0,\
  0,0,0,0,0,0,0,\
  0,0,0,0,0,0,0,0,0,0,\
  0,0,0,0,0,0,0,\
  0,0,0,0,0,0,0,0,\
  0,0,0,0,0,0,0};


/*****
Equality requires all fields be equal
*****/
bool tmwxViewSettings::operator==(const tmwxViewSettings& rhs) const
{
  return
    mShowSymmetryLines == rhs.mShowSymmetryLines &&

    mShowLeafNodes == rhs.mShowLeafNodes &&
    mShowBranchNodes == rhs.mShowBranchNodes &&
    mShowSubNodes == rhs.mShowSubNodes &&
    mShowNodeDots == rhs.mShowNodeDots &&
    mShowNodeCircles == rhs.mShowNodeCircles &&  
    mShowNodeIndices == rhs.mShowNodeIndices &&
    mShowNodeCoords == rhs.mShowNodeCoords &&
    mShowNodeElevation == rhs.mShowNodeElevation &&
    mShowNodeDepth == rhs.mShowNodeDepth &&
    mShowNodeLabels == rhs.mShowNodeLabels &&
    mShowNodeFlags == rhs.mShowNodeFlags &&
      
    mShowAllEdges == rhs.mShowAllEdges &&
    mShowEdgeDots == rhs.mShowEdgeDots &&
    mShowEdgeLines == rhs.mShowEdgeLines &&  
    mShowEdgeIndices == rhs.mShowEdgeIndices &&
    mShowEdgeLengths == rhs.mShowEdgeLengths &&
    mShowEdgeStrain == rhs.mShowEdgeStrain &&
    mShowEdgeLabels == rhs.mShowEdgeLabels &&
    mShowEdgeFlags == rhs.mShowEdgeFlags &&
    
    mShowLeafPaths == rhs.mShowLeafPaths &&
    mShowBranchPaths == rhs.mShowBranchPaths &&
    mShowSubPaths == rhs.mShowSubPaths &&
    mShowActivePaths == rhs.mShowActivePaths &&
    mShowBorderPaths == rhs.mShowBorderPaths &&
    mShowPolygonPaths == rhs.mShowPolygonPaths &&
    mShowInfeasiblePaths == rhs.mShowInfeasiblePaths &&
    mShowPathDots == rhs.mShowPathDots &&
    mShowPathLines == rhs.mShowPathLines &&
    mShowPathIndices == rhs.mShowPathIndices &&
    mShowPathLengths == rhs.mShowPathLengths &&
    mShowPathFlags == rhs.mShowPathFlags &&
    
    mShowMajorPolys == rhs.mShowMajorPolys &&
    mShowSubPolys == rhs.mShowSubPolys &&
    mShowPolyDots == rhs.mShowPolyDots &&
    mShowPolyLines == rhs.mShowPolyLines &&
    mShowPolyFills == rhs.mShowPolyFills &&
    mShowPolyIndices == rhs.mShowPolyIndices &&
    mShowPolyFlags == rhs.mShowPolyFlags &&
    
    mShowMajorVertices == rhs.mShowMajorVertices &&
    mShowMinorVertices == rhs.mShowMinorVertices &&
    mShowVertexDots == rhs.mShowVertexDots &&
    mShowVertexIndices == rhs.mShowVertexIndices &&
    mShowVertexCoords == rhs.mShowVertexCoords &&
    mShowVertexElevation == rhs.mShowVertexElevation &&
    mShowVertexDepth == rhs.mShowVertexDepth &&
    mShowVertexDiscreteDepth == rhs.mShowVertexDiscreteDepth &&
    mShowVertexTreeNode == rhs.mShowVertexTreeNode &&
    mShowVertexFlags == rhs.mShowVertexFlags &&
    
    mShowMajorCreases == rhs.mShowMajorCreases &&
    mShowMinorCreases == rhs.mShowMinorCreases &&
    mShowCreaseDots == rhs.mShowCreaseDots &&
    mShowCreaseLines == rhs.mShowCreaseLines &&
    mShowCreaseFolds == rhs.mShowCreaseFolds &&
    mShowCreaseIndices == rhs.mShowCreaseIndices &&
    mShowCreaseFlags == rhs.mShowCreaseFlags &&
    
    mShowAllFacets == rhs.mShowAllFacets &&
    mShowFacetDots == rhs.mShowFacetDots &&
    mShowFacetArrows == rhs.mShowFacetArrows &&
    mShowFacetFills == rhs.mShowFacetFills &&
    mShowFacetIndices == rhs.mShowFacetIndices &&
    mShowFacetOrder == rhs.mShowFacetOrder &&
    mShowFacetCorridorEdge == rhs.mShowFacetCorridorEdge &&
    mShowFacetFlags == rhs.mShowFacetFlags &&
    
    mShowNodeConditions == rhs.mShowNodeConditions &&
    mShowEdgeConditions == rhs.mShowEdgeConditions &&
    mShowPathConditions == rhs.mShowPathConditions &&
    mShowConditionDots == rhs.mShowConditionDots &&
    mShowConditionLines == rhs.mShowConditionLines &&
    mShowConditionIndices == rhs.mShowConditionIndices &&
    mShowConditionTypes == rhs.mShowConditionTypes;
}
