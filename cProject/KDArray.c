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
//    int size;//this is for keeping track of the proccess
    int** featsMat;
    int numOfFeats;
    int dim;
};

struct sorting_point_t{
    SPPoint point;
    int axis;
    int index;
};

typedef struct sorting_point_t* SORTINGPoint;


void spKDArrayPrintFeaturesMat(SPKDArray kdArr){
    //**************    print the featsMat *****************
    for (int i=0;i<kdArr->dim; i++){
        printf("\n %i: \n", i);
        for(int j=0; j<kdArr->numOfFeats; j++){
            printf("%i ", kdArr->featsMat[i][j]);
        }
    }

}


int compare_points(const void *a, const void *b ){
    SORTINGPoint pA = *(SORTINGPoint*)a;
    SORTINGPoint pB = *(SORTINGPoint*)b;
    double dPointA = spPointGetAxisCoor(pA->point, pA->axis);
    double dPointB = spPointGetAxisCoor(pB->point, pB->axis);
    return (dPointA > dPointB) - (dPointA < dPointB);
}



SPKDArray spKDArrayCreate(SPPoint* arr, int size, int dim){
    int i=0;
    SPKDArray kdArr = (SPKDArray)malloc(sizeof(*kdArr));
    if(!kdArr){
        spLoggerPrintError(ALLOC_FAIL, __FILE__, __FUNCTION__, __LINE__);
        return NULL;
    }
    if (size<1){
        free(kdArr);
        spLoggerPrintError(INVALID_SIZE, __FILE__, __FUNCTION__, __LINE__);
        return NULL;
    }
    
    kdArr->dim = dim;
    kdArr->numOfFeats = size;
    kdArr->points = (SPPoint*)malloc(size*sizeof(*kdArr->points));
    if(!kdArr->points){
        free(kdArr);
        spLoggerPrintError(ALLOC_FAIL, __FILE__, __FUNCTION__, __LINE__);
        return NULL;
    }
    //copy points
    for(;i<size;i++){
        kdArr->points[i] = spPointCopy(arr[i]);
    }
    return kdArr;
}

SPKDArray spKDArrayInit(SPPoint* arr, int size, SPConfig config, SP_CONFIG_MSG *msg){
    //TODO: free sorting point in case of error and free before function exits.
    int i=0,j = spConfigGetPCADim(config, msg);
    SORTINGPoint *sortArray = (SORTINGPoint*)malloc(size*sizeof(*sortArray));
    if(!sortArray){
        spLoggerPrintError(ALLOC_FAIL, __FILE__, __FUNCTION__, __LINE__);
        return NULL;
    }
    SPKDArray kdArr = spKDArrayCreate(arr, size, j);
    if(!kdArr){
        spLoggerPrintError(ALLOC_FAIL, __FILE__, __FUNCTION__, __LINE__);
        free(sortArray);
        return NULL;
    }
        //create the matrix
    kdArr->featsMat = malloc(kdArr->dim*sizeof(*kdArr->featsMat));
    if (!kdArr->featsMat) {
        spLoggerPrintError(ALLOC_FAIL, __FILE__, __FUNCTION__, __LINE__);
        spKDArrayDestroy(kdArr,NO_ROWS);
        return NULL;

    }
    for(i=0; i<size; i++){
        sortArray[i] = (SORTINGPoint)malloc(sizeof(**sortArray));
        if (!sortArray[i]) {
            spLoggerPrintError(ALLOC_FAIL, __FILE__, __FUNCTION__, __LINE__);
            spKDArrayDestroy(kdArr,NO_ROWS);
            for(j=0;j<i; j++){
                spPointDestroy(sortArray[j]->point);
                free(sortArray[j]);
            }
            free(sortArray);
            return NULL;
        }
        sortArray[i]->point = spPointCopy(kdArr->points[i]);
        if (!sortArray[i]->point) {
            spLoggerPrintError(ALLOC_FAIL, __FILE__, __FUNCTION__, __LINE__);
            spKDArrayDestroy(kdArr,NO_ROWS);
            for(j=0;j<i; j++){
                spPointDestroy(sortArray[j]->point);
                free(sortArray[j]);
            }
            free(sortArray[i]);
            free(sortArray);
            return NULL;
        }
        sortArray[i]->index = i;
    }
    
    for(i=0; i<kdArr->dim;i++){
        kdArr->featsMat[i] = malloc(size*sizeof(**kdArr->featsMat));
        if (!kdArr->featsMat[i]) {
            spLoggerPrintError(ALLOC_FAIL, __FILE__, __FUNCTION__, __LINE__);
            spKDArrayDestroy(kdArr, i);
            return NULL;
        }
        for(j=0; j<size; j++){
            sortArray[j]->axis = i;
        }
        qsort(sortArray, size, sizeof(SORTINGPoint), compare_points);
        for(j=0; j<size; j++){
            kdArr->featsMat[i][j] = sortArray[j]->index;
        }
        //TODO: free sorting point
    }
    spLoggerPrintInfo(KDARRAY_CREATED);
    //TODO: check msg to see if there are errors
//    spKDArrayPrintFeaturesMat(kdArr);
    return kdArr;
}

SPKDArray* spKDArraySplit(SPKDArray kdArr, int coor){
    SPKDArray *kd = malloc(2*sizeof(*kd));
    int n, i, temp, cnt = 0, mapLeft=0, mapRight=0, location;
    n = (int)ceilf(kdArr->numOfFeats/2.0);
    SPPoint newPoints[2][n];
    int partitionMap[2][kdArr->numOfFeats];
    if(!kd){
        return NULL;
    }
    // copy left points to new points and create partition map
    for (i=0; i<n; i++){//distribute the points
        location = kdArr->featsMat[coor][i];
        newPoints[LEFT][cnt] = kdArr->points[location];
        partitionMap[LEFT][kdArr->featsMat[coor][i]] = cnt;
        partitionMap[RIGHT][kdArr->featsMat[coor][i]] = NOT_PRESENT;
        cnt++;
    }
    cnt =0;
    //continue with the right part of the points
    for (; i<kdArr->numOfFeats; i++){
        location = kdArr->featsMat[coor][i];
        newPoints[RIGHT][cnt] = kdArr->points[location];
        partitionMap[RIGHT][kdArr->featsMat[coor][i]] = cnt;
        partitionMap[LEFT][kdArr->featsMat[coor][i]] = NOT_PRESENT;
        cnt++;
    }
    kd[LEFT] = spKDArrayCreate(newPoints[LEFT], n, kdArr->dim);
    if (!kd[LEFT]){
        spLoggerPrintError(ALLOC_FAIL, __FILE__, __FUNCTION__, __LINE__);
        free(kd);
        return NULL;
    }
    kd[LEFT]->featsMat = malloc(kdArr->dim*sizeof(*kdArr->featsMat));
    if (!kd[LEFT]->featsMat){
        spLoggerPrintError(ALLOC_FAIL, __FILE__, __FUNCTION__, __LINE__);
        spKDArrayDestroy(kd[LEFT], NO_ROWS);
        free(kd);
        return NULL;
    }
    kd[RIGHT] = spKDArrayCreate(newPoints[RIGHT], (kdArr->numOfFeats-n), kdArr->dim);
    if (!kd[RIGHT]){
        spLoggerPrintError(ALLOC_FAIL, __FILE__, __FUNCTION__, __LINE__);
        spKDArrayDestroy(kd[LEFT], NO_ROWS);
        free(kd);
        return NULL;
    }
    kd[RIGHT]->featsMat = malloc(kdArr->dim*sizeof(*kdArr->featsMat));
    if (!kd[RIGHT]->featsMat){
        spLoggerPrintError(ALLOC_FAIL, __FILE__, __FUNCTION__, __LINE__);
        spKDArrayDestroy(kd[LEFT], NO_ROWS);
        spKDArrayDestroy(kd[RIGHT], NO_ROWS);
        free(kd);
        return NULL;
    }
    // figure out the features matrix
    for(i=0; i<kdArr->dim; i++){//iterate through the different axis
        kd[LEFT]->featsMat[i] = (int*)malloc(n*sizeof(int));
        if(!kd[LEFT]->featsMat[i]){
            spLoggerPrintError(ALLOC_FAIL, __FILE__, __FUNCTION__, __LINE__);
            spKDArrayDestroy(kd[LEFT], i);
            spKDArrayDestroy(kd[RIGHT], i);
            free(kd);
            return NULL;
        }
        kd[RIGHT]->featsMat[i] = (int*)malloc((kdArr->numOfFeats - n)*sizeof(int));
        if(!kd[RIGHT]->featsMat[i]){
            spLoggerPrintError(ALLOC_FAIL, __FILE__, __FUNCTION__, __LINE__);
            spKDArrayDestroy(kd[LEFT], i+1);
            spKDArrayDestroy(kd[RIGHT], i);
            free(kd);
            return NULL;
        }
        mapLeft = 0;
        mapRight= 0;
        for(cnt=0; cnt<kdArr->numOfFeats;cnt++){ //iterate through the point map
            location = kdArr->featsMat[i][cnt];
            if((temp = partitionMap[LEFT][location])>NOT_PRESENT){//exists in left partition
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
//    spLoggerPrintInfo(KDARRAY_CREATED);
//    spKDArrayPrintFeaturesMat(kd[0]);
//    spKDArrayPrintFeaturesMat(kd[1]);
    return kd;
}

void spKDArrayDestroy(SPKDArray kdArr, int size){
    int i = 0;
    if(!kdArr){
        return;
    }
    if(!size){
        size = kdArr->dim;
    }
    for(; i<kdArr->numOfFeats;i++){
        spPointDestroy(kdArr->points[i]);
    }
    free(kdArr->points);
    for(i=0;i<size;i++){
        free(kdArr->featsMat[i]);
    }
    if(kdArr->featsMat){ free(kdArr->featsMat);}
    free(kdArr);
    return;
}

int spKDArrayGetNumOfFeatures(SPKDArray kdArr){
    if(!kdArr){
        spLoggerPrintError(ALLOC_FAIL, __FILE__, __FUNCTION__, __LINE__);
        return 0;
    }
    return kdArr->numOfFeats;
}

SPPoint spKDArrayGetPoint(SPKDArray kdArr, int index){
    if(!kdArr){
        spLoggerPrintError(ALLOC_FAIL, __FILE__, __FUNCTION__, __LINE__);
        return NULL;
    }
    assert(kdArr->numOfFeats>index);
    return kdArr->points[index];
}

int spKDArrayFindSplitDim(const SPKDArray kdArr,int prevDim,const SPConfig config, SP_CONFIG_MSG *msg){
    int res, i, last;
    double maxDiff=0, currentDiff, minCoordinate, maxCoordinate;
    KDTreeSplitMethod method = spConfigGetSplitMethod(config, msg);
    res =0;
    last = kdArr->numOfFeats-1;
    switch (method){
        case MAX_SPREAD:
            for(i=0; i<kdArr->dim;i++){
                maxCoordinate = spPointGetAxisCoor(kdArr->points[kdArr->featsMat[i][last]], i);
                minCoordinate = spPointGetAxisCoor(kdArr->points[kdArr->featsMat[i][FIRST]], i);
                currentDiff = fabs(maxCoordinate - minCoordinate);
                if (currentDiff>maxDiff){
                    maxDiff = currentDiff;
                    res = i;
                }
            }
            return res;
        case RANDOM:
            res = rand()%kdArr->dim;
            break;
        case INCREMENTAL:
            res = (prevDim+1)%kdArr->numOfFeats;
            break;
    }
    return res;
}

double spKDArrayGetMedianVal(SPKDArray kdArr, int splitDim){
    assert(kdArr && splitDim>=0 && splitDim<kdArr->dim);
    int location = kdArr->featsMat[splitDim][spKDArrayGetNumOfFeatures(kdArr)-1];
    return spPointGetAxisCoor(kdArr->points[location], splitDim);
}

