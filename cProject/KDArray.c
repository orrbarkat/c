//
//  KDArray.c
//  cProject
//
//  Created by Orr Barkat on 14/08/2016.
//  Copyright © 2016 Orr Barkat. All rights reserved.
//
#include "KDArray.h"

struct sp_kdarray_t{
    SPPoint *points;//the size of 
    int size;//this is for keeping track of the proccess
    int** featsMat;
    int numOfFeats;
    int dim;
    int axis;
};


int compare_points(void *kdArr, const void *a, const void *b){
    SPKDArray kd = (SPKDArray)kdArr;
    int pA = *(int*)a, pB = *(int*)b;
    SPPoint pointA = kd->points[pA];
    SPPoint pointB = kd->points[pB];
    double dPointA = spPointGetAxisCoor(pointA, kd->axis);
    double dPointB = spPointGetAxisCoor(pointB, kd->axis);
    return (dPointA > dPointB) - (dPointA < dPointB);
}



SPKDArray spKDArrayCreate(SPPoint* arr, int size, int dim){
    int i=0;
    SPKDArray kdArr = malloc(sizeof(kdArr));
    if(!kdArr){
        return NULL;
    }
    if (size<1){
        free(kdArr);
        return NULL;
    }
    
    kdArr->dim = dim;
    kdArr->numOfFeats = size;
    kdArr->points = (SPPoint*)malloc(size*sizeof(SPPoint));
    if(!kdArr->points){
        free(kdArr);
        return NULL;
    }
    //copy points
    for(;i<size;i++){
        kdArr->points[i] = spPointCopy(arr[i]);
    }
    return kdArr;
}

SPKDArray spKDArrayInit(SPPoint* arr, int size, SPConfig config, SP_CONFIG_MSG *msg){
    int i=0,j = spConfigGetPCADim(config, msg);
    SPKDArray kdArr = spKDArrayCreate(arr, size, j);
        //create the matrix
    kdArr->featsMat = malloc(kdArr->dim*sizeof(*kdArr->featsMat));
    if (!kdArr->featsMat) {
        spLoggerPrintError(ALLOC_FAIL, __FILE__, __FUNCTION__, __LINE__);
        spKDArrayDestroy(kdArr,NOT_PRESENT);
        return NULL;

    }
    for(i=0; i<kdArr->dim;i++){
        kdArr->featsMat[i] = malloc(size*sizeof(**kdArr->featsMat));
        if (!kdArr->featsMat[i]) {
            spKDArrayDestroy(kdArr, i);
            return NULL;
        }
        for(j=0; j<size; j++){
            kdArr->featsMat[i][j] = j;
        }
        qsort_r(kdArr->featsMat[i], size, sizeof(int), kdArr, compare_points);
    }
    spLoggerPrintInfo(KDARRAY_CREATED);
    //TODO: check msg to see if there are errors
    return kdArr;
}

SPKDArray* spKDArraySplit(SPKDArray kdArr, int coor){
    SPKDArray *kd = malloc(2*sizeof(*kd));
    int n = (int)ceilf(kdArr->numOfFeats/2.0),i,temp, cnt = 0, mapLeft=0, mapRight=0;
    SPPoint newPoints[2][n];
    int partitionMap[2][kdArr->numOfFeats];
    
    // copy left points to new points and create partition map
    for (i=0; i<n; i++){//distribute the points
        newPoints[LEFT][cnt] = kdArr->points[kdArr->featsMat[coor][i]];
        partitionMap[LEFT][kdArr->featsMat[coor][i]] = cnt;
        partitionMap[RIGHT][kdArr->featsMat[coor][i]] = NOT_PRESENT;
        cnt++;
    }
    cnt =0;
    //continue with the right part of the points
    for (; i<kdArr->size; i++){
        newPoints[1][cnt] = kdArr->points[kdArr->featsMat[coor][i]];
        partitionMap[RIGHT][kdArr->featsMat[coor][i]] = cnt;
        partitionMap[LEFT][kdArr->featsMat[coor][i]] = NOT_PRESENT;
        cnt++;
    }
    kd[LEFT] = spKDArrayCreate(newPoints[LEFT], n, kdArr->dim);
    kd[LEFT]->featsMat = malloc(kdArr->dim*sizeof(*kdArr->featsMat));
    kd[RIGHT] = spKDArrayCreate(newPoints[RIGHT], (kdArr->numOfFeats-n), kdArr->dim);
    kd[RIGHT]->featsMat = malloc(kdArr->dim*sizeof(*kdArr->featsMat));
    // figure out the features matrix
    for(i=0; i<kdArr->dim; i++){//iterate through the different axis
        kdArr->featsMat[i] = malloc(n*sizeof(**kdArr->featsMat));
        for(cnt=0; cnt<kdArr->numOfFeats;cnt++){ //iterate through the point map
            temp = kdArr->featsMat[i][cnt];
            if((temp = partitionMap[LEFT][temp])>NOT_PRESENT){//exists in left partition
                kd[LEFT]->featsMat[i][mapLeft] = temp;
                mapLeft++;
            }else{
                temp = kdArr->featsMat[i][cnt];
                temp = partitionMap[RIGHT][temp];
                kd[RIGHT]->featsMat[i][mapRight] = temp;
                mapRight++;
            }
        }
    }
    //TODO: check if there are errors
    spLoggerPrintInfo(KDARRAY_CREATED);
    return kd;
}

void spKDArrayDestroy(SPKDArray kdArr, int size){
    int i = 0;
    if(!kdArr){
        return;
    }
    for(; i<kdArr->size;i++){
        spPointDestroy(kdArr->points[i]);
    }
    free(kdArr->points);
    for(i=0;i<size;i++){
        free(kdArr->featsMat[i]);
    }
    free(kdArr->featsMat);
    free(kdArr);
    return;
}
