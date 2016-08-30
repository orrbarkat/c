//
//  KDTree.h
//  cProject
//
//  Created by Orr Barkat on 04/08/2016.
//  Copyright © 2016 Orr Barkat. All rights reserved.
//

#ifndef KDTree_h
#define KDTree_h

#define INVALID -1
#define LESS_THAN_K_ELEMENTS

#include <stdio.h>
#include "KDArray.h"
#include "SPConfig.h"
#include "SPLogger.h"
#include "SPPoint.h"
#include "SPBPriorityQueue.h"


typedef struct sp_kdtree_node_t* SPKDTreeNode;


SPKDTreeNode spKDTreeCreateFromArray(SPKDArray kdArr,int prevDim, SPConfig config, SP_CONFIG_MSG *msg);

void spKDTreeDestroy(SPKDTreeNode node);

int* spFindImages(SPPoint* queryFeatures, const int querySize, const SPKDTreeNode node, const SPConfig config, SP_CONFIG_MSG *msg);

#endif /* KDTree_h */
