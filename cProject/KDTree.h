//
//  KDTree.h
//  cProject
//

#ifndef KDTree_h
#define KDTree_h


#include <stdio.h>
#include "KDArray.h"
#include "SPConfig.h"
#include "SPLogger.h"
#include "SPPoint.h"
#include "SPBPriorityQueue.h"
#include "macros.h"

/**
 * SPKDTree Summary
 * Encapsulates a point out of a KDArray.
 * the KDTree is a struct to store KDTreeNodes that holds points
 * information, and pointers KDTreeNodes children.
 * The struct helps doing manipulations in good time complexity to get
 * the closest in range points.
 *
 * The following functions are supported:
 *
 * spKDTreeCreateFromArray 	- Creates a new kd tree from array
 * spKDTreeDestroy	 		- Free all resources associated with the tree
 * spFindImages				- Gets the closest images according to configuration demands
 *
 *
 */

typedef struct sp_kdtree_node_t* SPKDTreeNode;

/**
 * Creates KDTree struct out of an array.
 * operate recursively.
 * @param: kdarray: the array to make the tree from it
 * @param: prevDim: integer that represent previus dim
 * @param: config: the configuration to act accordingly
 * @return:
 * 	-NULL if an error has occured during method run.
 * 	-Otherwise the root node of the kdtree
 **/
SPKDTreeNode spKDTreeCreateFromArray(SPKDArray kdArr,int prevDim,
		SPConfig config, SP_CONFIG_MSG *msg, int* problem);

/**
 * frees all memory allocated in the KDTree.
 * Acts recursively downwards from node parameter.
 * @param: node: the node from witch we want to free resources.
 **/
void spKDTreeDestroy(SPKDTreeNode node);

/**
 * The main method for this file.
 * the method uses help functions to find the most similar images
 * to the one that queried.
 * @param: queryFeatures:the query features to be compared and to find
 * the most similar images.
 * @param: querySize: query number of features.
 * @param: node: pointer to the root of the kdTree to search within.
 * @param: config: configuration file to act accordingly
 * @param: msg:pointer to massage to be written inside the logger
 * @return:
 * 	- NULL in case of an error
 * 	- in case of success: integer array that represent the desired images indices.
 **/
int* spFindImages(SPPoint* queryFeatures, const int querySize, const SPKDTreeNode node, const SPConfig config, SP_CONFIG_MSG *msg);

#endif /* KDTree_h */
