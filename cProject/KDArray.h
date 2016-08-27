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
#define NOT_PRESENT -1
#define LEFT 0
#define RIGHT 1

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
 * spPointGetDimension		- A getter of the dimension of a point
 * spPointGetIndex			- A getter of the index of a point
 * spPointGetAxisCoor		- A getter of a given coordinate of the point
 * spPointL2SquaredDistance	- Calculates the L2 squared distance between two points
 *
 */

/**
 * A data-structure which is used for pointes.
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
 * @param 
 */
SPKDArray* spKDArraySplit(SPKDArray kdArr, int coor);


SPKDArray spKDArrayCreate(SPPoint* arr, int size, int dim);

void spKDArrayDestroy(SPKDArray kdArr, int size);

#endif /* KDArray_h */
