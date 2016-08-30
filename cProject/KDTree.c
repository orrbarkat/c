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

void spKNNSearch(SPPoint queryFeature, const SPKDTreeNode node, SPBPQueue q){
    SPListElement element;
    int index, distance;
    bool distanceFlag = false;
    if(!node){//null node
        return;
    }
    if(node->dim==INVALID){//this is a leaf
        index = spPointGetIndex(node->data);
        distance = spPointL2SquaredDistance(queryFeature, node->data);
        element = spListElementCreate(index, distance);
        spBPQueueEnqueue(q, element);
        spListElementDestroy(element);
        return;
    }
    if(spPointGetAxisCoor(queryFeature, node->dim)<= node->val){//go to the left sub tree
        spKNNSearch(queryFeature, node->left, q);
        distance = pow((spPointGetAxisCoor(queryFeature, node->dim) - node->val),2);
        distanceFlag = distance < spBPQueueMaxValue(q);
        if(spBPQueueIsFull(q) || distanceFlag){
            spKNNSearch(queryFeature, node->right, q);
        }
    }else{
        spKNNSearch(queryFeature, node->right, q);
        distance = pow((spPointGetAxisCoor(queryFeature, node->dim) - node->val),2);
        distanceFlag = distance < spBPQueueMaxValue(q);
        if(spBPQueueIsFull(q) || distanceFlag){
            spKNNSearch(queryFeature, node->left, q);
        }
    }
    return;
}

int* getTopImagesFromArray(int* imageCounter, int numOfImages, const int numOfSimilarImg){
    int i, j, maxIndex, maxValue, *res;
    res = (int*)malloc(numOfSimilarImg*sizeof(int));
    if(!res){
        spLoggerPrintError(ALLOC_FAIL, __FILE__, __FUNCTION__, __LINE__);
        return NULL;
    }
    for(i=0; i<numOfSimilarImg; i++){
        maxValue = 0;
        maxIndex = i;
        for(j=0; j<numOfImages; j++){
            if(imageCounter[j]>maxValue){
                maxIndex = j;
                maxValue = imageCounter[j];
            }
        }
        res[i] = maxIndex;
        imageCounter[maxIndex] = NOT_PRESENT;
    }
    return res;
}


int* spFindImages(SPPoint* queryFeatures, const int querySize, const SPKDTreeNode root, const SPConfig config, SP_CONFIG_MSG *msg){
    int i, knn, numOfSimilarImg, numOfImages;
    int *imageCounter, *res;
    SPListElement element;
    knn = spConfigGetKNN(config, msg);

    SPBPQueue q = spBPQueueCreate(knn);
    if(!q){
        spLoggerPrintError(ALLOC_FAIL, __FILE__, __FUNCTION__, __LINE__);
        return NULL;
    }
    numOfImages = spConfigGetNumOfImages(config, msg);
    numOfSimilarImg = spConfigGetNumOfSimilarImages(config, msg);
    imageCounter = (int*)calloc(numOfImages, sizeof(int));
    if(!imageCounter){
        spLoggerPrintError(ALLOC_FAIL, __FILE__, __FUNCTION__, __LINE__);
        spBPQueueDestroy(q);
        return NULL;
    }
    for(i=0; i<querySize; i++){//count the num of neighbours of every image for all features of the query
        spBPQueueClear(q);
        spKNNSearch(queryFeatures[i], root, q);
        while((element = spBPQueuePeek(q))!=NULL){
            imageCounter[spListElementGetIndex(element)]+=1;
            spListElementDestroy(element);
            spBPQueueDequeue(q);
        }
    }
    res = getTopImagesFromArray(imageCounter, numOfImages, numOfSimilarImg);
    free(imageCounter);
    return res;
}





