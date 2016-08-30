//
//  KDArray.h
//  cProject
//
//  Created by Orr Barkat on 14/08/2016.
//  Copyright © 2016 Orr Barkat. All rights reserved.
//

#ifndef KDArray_h
#define KDArray_h

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include "SPPoint.h"
#include "SPLogger.h"
#include "SPConfig.h"

#define LINE_LENGTH 1024
#define KDARRAY_CREATED "created the kd array\n"
#define NO_ROWS -1
#define ALL_ROWS 0
#define NOT_PRESENT -1
#define LEFT 0
#define RIGHT 1
#define FIRST 0

/**
 * SPKDArray Summary
 * Encapsulates a point with variable length dimension. The coordinates
 * values are double types, and each point has a non-negative index which
 * represents the image index to which the point belongs.
 *
 * The following functions are supported:
 *
 * spKDArrayInit        	- Creates a new kd array
 * spKDArraySplit			- Create a new copy of a given point
 * spKDArrayDestroy 		- Free all resources associated with a point
 * spKDArrayGetNumOfFeatures- A getter of the numOfFeats of an array
 * spKDArrayGetpoint		- A getter of the first point in points
 * 
 *
 */

/**
 * A data-structure which is used for points.
 *
 */


typedef struct sp_kdarray_t* SPKDArray;

/**
 * Allocates a new kdarray in the memory.
 * Given data array and the array size.
 * @param arr - the array of all the points
 * @param size - the size of the array
 * @return
 * NULL in case allocation failure ocurred OR data is NULL OR size <=0.
 * Otherwise, the new array is returned
 */
SPKDArray spKDArrayInit(SPPoint* arr, int size, SPConfig config, SP_CONFIG_MSG *msg);


/**
 * Returns two kd-arrays (kdLeft, kdRight) such that the first ⌈𝒏/𝟐⌉ points with respect
 * to the coordinate coor are in kdLeft , and the rest of the points are in kdRight.
 * ** allocates the new arrays inside the method
 * @return
 * - SPKDArray of size 2 if successful
 * - NULL if failed
 */
SPKDArray* spKDArraySplit(SPKDArray kdArr, int coor);


SPKDArray spKDArrayCreate(SPPoint* arr, int size, int dim);


/**
 * frees all the memory the kdarry held
 * size is for freeing only part of the matrix rows
 * if not needed pass NULL or 0.
 */
void spKDArrayDestroy(SPKDArray kdArr, int size);

/**
 * a getter for the array size
 * @return
 * - if the kdArr is NULL then 0 is returned
 * - the numOfFeats is returned otherwise
 */
int spKDArrayGetNumOfFeatures(SPKDArray kdArr);

/**
 * a getter for the point of the array
 * @assert
 * - index has to be in range
 * @ return
 * - the point if success
 * - NULL for fail
 */
SPPoint spKDArrayGetPoint(SPKDArray kdArr,int index);

/**
 * calculates the splitting dim depending on the split method
 * @param 
 *  -kdArr the array that needs to be split
 *  -prevDim the previous dimention that was used to split the array -1 if it's the first split
 * @return
 *  -returns the split dimention
 */
int spKDArrayFindSplitDim(const SPKDArray kdArr,int prevDim,const SPConfig config, SP_CONFIG_MSG *msg);

/**
 * @assert 
 *  - kdArr is not null
 *  - split dim is in range
 * @return
 * - the value of the largest double in the ith axis.
 */
double spKDArrayGetMedianVal(SPKDArray kdArr, int splitDim);


#endif /* KDArray_h */
