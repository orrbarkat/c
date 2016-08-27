//
//  KDTree.h
//  cProject
//
//  Created by Orr Barkat on 04/08/2016.
//  Copyright © 2016 Orr Barkat. All rights reserved.
//

#ifndef KDTree_h
#define KDTree_h

#include <stdio.h>
#include "KDArray.h"
#include "SPConfig.h"
#include "SPLogger.h"
#include "SPPoint.h"


typedef struct sp_kdtree_node_t* SPKDTreeNode;

typedef enum sp_kdtree_split_method_t {
    RANDOM,
    MAX_SPREAD,
    INCREMENTAL
} KDTreeSplitMethod;



#endif /* KDTree_h */
