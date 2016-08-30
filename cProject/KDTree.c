//
//  KDTree.c
//  cProject
//
//  Created by Orr Barkat on 04/08/2016.
//  Copyright © 2016 Orr Barkat. All rights reserved.
//

#include "KDTree.h"

struct sp_kdtree_node_t{
    int dim;
    double val;
    SPKDTreeNode left;
    SPKDTreeNode right;
    SPPoint data;
};

// allocates a node
// the point is copied but not freed
// return a node unless the allocation failed then null is returned
SPKDTreeNode spKDTreeCreateLeaf(SPPoint point){
    SPKDTreeNode leaf = (SPKDTreeNode)malloc(sizeof(*leaf));
    if (!leaf){
        spLoggerPrintError(ALLOC_FAIL, __FILE__, __FUNCTION__, __LINE__);
        return NULL;
    }
    leaf->dim = INVALID;
    leaf->val = INVALID;
    leaf->left = NULL;
    leaf->right = NULL;
    leaf->data = spPointCopy(point);
    return leaf;
}

SPKDTreeNode spKDTreeCreateNode(){
    SPKDTreeNode node = (SPKDTreeNode)malloc(sizeof(*node));
    if (!node){
        spLoggerPrintError(ALLOC_FAIL, __FILE__, __FUNCTION__, __LINE__);
        return NULL;
    }
    return node;
}



SPKDTreeNode spKDTreeCreateFromArray(SPKDArray kdArr, int prevDim, SPConfig config, SP_CONFIG_MSG *msg){
    SPKDTreeNode res = NULL;
    SPKDArray *children;
    if (spKDArrayGetNumOfFeatures(kdArr)<1){
        spKDArrayDestroy(kdArr, NO_ROWS);
        return NULL;
    }
    else if (spKDArrayGetNumOfFeatures(kdArr)==1){
        res = spKDTreeCreateLeaf(spKDArrayGetPoint(kdArr, 0));
        spKDArrayDestroy(kdArr, ALL_ROWS);
        return res;
    }
    else{//KD Array is larger than 1 point
        res = spKDTreeCreateNode();
        res->dim = spKDArrayFindSplitDim(kdArr, prevDim, config, msg);
        children = spKDArraySplit(kdArr, res->dim);
        res->val = spKDArrayGetMedianVal(children[LEFT], res->dim);
        spKDArrayDestroy(kdArr, ALL_ROWS);
        res->left = spKDTreeCreateFromArray(children[LEFT], res->dim, config, msg);
        res->right  = spKDTreeCreateFromArray(children[RIGHT], res->dim, config, msg);
    }
    return res;
}


void spKDTreeDestroy(SPKDTreeNode node){
    if(!node){
        return;
    }
    if(node->dim == INVALID){
        spPointDestroy(node->data);
    }
    if(node->left){
        spKDTreeDestroy(node->left);
    }
    if(node->right){
        spKDTreeDestroy(node->right);
    }
    free(node);
}

bool spKNNSearch(SPPoint queryFeature, const SPKDTreeNode root, int knn, int* imageCounter){
}

int* spFindImages(SPPoint* queryFeatures, const int querySize, const SPKDTreeNode root, const SPConfig config, SP_CONFIG_MSG *msg){
    SPBPQueue imagesQ;
    int i, knn;
    int *imageCounter;
    knn = spConfigGet
    for(i=0; i<querySize; i++){
        spKNNSearch(queryFeatures[i], root, <#int knn#>)
    }
    

}





