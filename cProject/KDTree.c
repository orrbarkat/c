//
//  KDTree.c
//  cProject
//

#include "KDTree.h"

struct sp_kdtree_node_t{
    int dim;
    double val;
    SPKDTreeNode left;
    SPKDTreeNode right;
    SPPoint data;
};


/**
 *  Help method that allocates a leaf KDTreeNode.
 *  the point is copied but not freed.
 *  a leaf has default parameters while node doesn't have.
 *  return a node unless allocation failed occurred then null is returned
 **/

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

/**
 *  Help method that allocates a regular KDTreeNode.
 *  the point is copied but not freed.
 *  node doesn't have default parameters.
 *  return a node unless allocation failed occurred then null is returned
 **/
SPKDTreeNode spKDTreeCreateNode(){
    SPKDTreeNode node = (SPKDTreeNode)malloc(sizeof(*node));
    if (!node){
        spLoggerPrintError(ALLOC_FAIL, __FILE__, __FUNCTION__, __LINE__);
        return NULL;
    }
    node->dim=INVALID;
    node->left = NULL;
	node->right = NULL;
	node->data = NULL;
    return node;
}


SPKDTreeNode spKDTreeCreateFromArray(SPKDArray kdArr, int prevDim,
					SPConfig config, SP_CONFIG_MSG *msg, int* problem ){
    SPKDTreeNode res = NULL;
    SPKDArray *children;
    if (spKDArrayGetNumOfFeatures(kdArr)==1){
        res = spKDTreeCreateLeaf(spKDArrayGetPoint(kdArr, 0));
        if(!res){
        	spLoggerPrintError(ALLOC_FAIL, __FILE__, __FUNCTION__, __LINE__);
        	spKDArrayDestroy(kdArr, ALL_ROWS);
        	*problem = true;
        	return NULL;
        }
        spKDArrayDestroy(kdArr, ALL_ROWS);
        return res;
    }
    else{			//** KD Array is larger than 1 point **//
        res = spKDTreeCreateNode();
        if(!res){
        	spLoggerPrintError(ALLOC_FAIL, __FILE__, __FUNCTION__, __LINE__);
        	spKDArrayDestroy(kdArr, ALL_ROWS);
        	*problem = true;
        	return NULL;
        }
        res->dim = spKDArrayFindSplitDim(kdArr, prevDim, config, msg);
        children = spKDArraySplit(kdArr, res->dim);
        if(!children){
        	spKDTreeDestroy(res);
        	spKDArrayDestroy(kdArr, ALL_ROWS);
        	*problem = true;
        	return NULL;
        }
        res->val = spKDArrayGetMedianVal(children[LEFT], res->dim);
        spKDArrayDestroy(kdArr, ALL_ROWS);
        res->left = spKDTreeCreateFromArray(children[LEFT],
        		res->dim, config, msg, problem);
        if(!(res->left)){
        	spKDTreeDestroy(res);
        	spKDArrayDestroy(kdArr, ALL_ROWS);
			spKDMultiArrayDestroy(children, true, ALL_ROWS, true, ALL_ROWS);
			*problem = true;
			return NULL;
        }
        res->right  = spKDTreeCreateFromArray(children[RIGHT],
        		res->dim, config, msg, problem);
        if(!(res->right)){
        	spKDTreeDestroy(res);
        	spKDArrayDestroy(kdArr, ALL_ROWS);
			spKDMultiArrayDestroy(children, true, ALL_ROWS, true, ALL_ROWS);
			*problem = true;
			return NULL;
        }
        spKDMultiArrayDestroy(children, false, false, false, false);
        return res;
    }
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

/**
 * internal help method to find the k nearest neighbors
 */
void spKNNSearch(SPPoint queryFeature, const SPKDTreeNode node, SPBPQueue q){
    SPListElement element;
    int index, distance;
    bool distanceFlag = false;
    if(!node){
        return;
    }
    if(node->dim==INVALID){				//** this is a leaf **//
        index = spPointGetIndex(node->data);
        distance = spPointL2SquaredDistance(queryFeature, node->data);
        element = spListElementCreate(index, distance);
        spBPQueueEnqueue(q, element);
        spListElementDestroy(element);
        return;
    }
    			//** go to the left sub tree **//
    if(spPointGetAxisCoor(queryFeature, node->dim)<= node->val){
        spKNNSearch(queryFeature, node->left, q);
        distance = pow((spPointGetAxisCoor(queryFeature,
        									node->dim) - node->val),2);
        distanceFlag = distance < spBPQueueMaxValue(q);
        if(!spBPQueueIsFull(q) || distanceFlag){
            spKNNSearch(queryFeature, node->right, q);
        }
    }else{
        spKNNSearch(queryFeature, node->right, q);
        distance = pow((spPointGetAxisCoor(queryFeature,
        									node->dim) - node->val),2);
        distanceFlag = distance < spBPQueueMaxValue(q);
        if(!spBPQueueIsFull(q) || distanceFlag){
            spKNNSearch(queryFeature, node->left, q);
        }
    }
    return;
}

/*
 * Internal help getter for the most wanted (similar) images.
 * @param: imageCounter: to bound the max value.
 * @param: numOfImages: the number of images to search from.
 * @param: numOfSimilarImg: according to configuration file - the number of wanted similar
 * images.
 * @return:
 * 	- NULL in case of an error
 * 	- int array that represent the top images
 */
int* getTopImagesFromArray(int* imageCounter, int numOfImages,
									const int numOfSimilarImg){
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


int* spFindImages(SPPoint* queryFeatures, const int querySize,
		const SPKDTreeNode root, const SPConfig config, SP_CONFIG_MSG *msg){
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
    /* count the num of neighbours of every
     * image for all features of the query */
    for(i=0; i<querySize; i++){
        spBPQueueClear(q);
        spKNNSearch(queryFeatures[i], root, q);
        while((element = spBPQueuePeek(q))!=NULL){
            imageCounter[spListElementGetIndex(element)]+=1;
            spListElementDestroy(element);
            spBPQueueDequeue(q);
        }
    }
    spBPQueueClear(q);
    spBPQueueDestroy(q);
    res = getTopImagesFromArray(imageCounter, numOfImages, numOfSimilarImg);
    free(imageCounter);
    return res;
}
