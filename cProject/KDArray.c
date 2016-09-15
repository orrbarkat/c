//
//  KDArray.c
//  cProject
//
#include "KDArray.h"

struct sp_kdarray_t{
    SPPoint *points;	//the size of
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

/**
 * internal help function to free SORTINGPoint struct
 */
void sortingPointDestroyArray (SORTINGPoint *pointArray, int size){
	int i;
	for(i=0; i<size; i++){
		if (pointArray[i]->point){
			spPointDestroy(pointArray[i]->point);
		}
		free(pointArray[i]);
	}
	free (pointArray);
}

// global points comparator
int compare_points(const void *a, const void *b ){
    SORTINGPoint pA = *(SORTINGPoint*)a;
    SORTINGPoint pB = *(SORTINGPoint*)b;
    double dPointA = spPointGetAxisCoor(pA->point, pA->axis);
    double dPointB = spPointGetAxisCoor(pB->point, pB->axis);
    return (dPointA > dPointB) - (dPointA < dPointB);
}

/**
 * internal help method to create kdarray.
 * used in "spKDArrayInit" and "spKDArraySplit"
 * @param: arr: array of SPPoint to be copied
 * @param: size: number of features
 * @param: dim: number of dimensions
 * @return:
 * 	- NULL in case of ERROR
 * 	- SPKDArray in case of success
 **/
SPKDArray spKDArrayCreate(SPPoint* arr, int size, int dim){
    int i=0;
    SPKDArray kdArr;
    if (size<1 || !arr){
        spLoggerPrintError(INVALID_SIZE, __FILE__, __FUNCTION__, __LINE__);
        return NULL;
    }

    kdArr = (SPKDArray)malloc(sizeof(*kdArr));
    if(!kdArr){
        spLoggerPrintError(ALLOC_FAIL, __FILE__, __FUNCTION__, __LINE__);
        return NULL;
    }
    
    kdArr->dim = dim;
    kdArr->numOfFeats = size;
    kdArr->points = (SPPoint*)malloc(size*sizeof(*kdArr->points));
    if(!kdArr->points){
    	spLoggerPrintError(ALLOC_FAIL, __FILE__, __FUNCTION__, __LINE__);
    	spKDArrayDestroy(kdArr, NO_ROWS);
        return NULL;
    }
    //copy points
    for(;i<size;i++){
        kdArr->points[i] = spPointCopy(arr[i]);
        if(!kdArr->points[i]){
        	spLoggerPrintError(ALLOC_FAIL, __FILE__, __FUNCTION__, __LINE__);
        	spKDArrayDestroy(kdArr, i);
        	return NULL;
        }
    }
    return kdArr;
}


SPKDArray spKDArrayInit(SPPoint* arr, int size, SPConfig config, SP_CONFIG_MSG *msg){
    int i = 0;
    int j = spConfigGetPCADim(config, msg);
    SPKDArray kdArr;

    SORTINGPoint *sortArray = (SORTINGPoint*)malloc(size*sizeof(*sortArray));
    if(!sortArray){
        spLoggerPrintError(ALLOC_FAIL, __FILE__, __FUNCTION__, __LINE__);
        return NULL;
    }

    kdArr = spKDArrayCreate(arr, size, j);
    if(!kdArr){
        spLoggerPrintError(ALLOC_FAIL, __FILE__, __FUNCTION__, __LINE__);
        free(sortArray);
        return NULL;
    }

    //create the matrix
    kdArr->featsMat = malloc(kdArr->dim*sizeof(*kdArr->featsMat));
    if (!kdArr->featsMat) {
        spLoggerPrintError(ALLOC_FAIL, __FILE__, __FUNCTION__, __LINE__);
        spKDArrayDestroy(kdArr, ALL_ROWS);
        free(sortArray);
        return NULL;
    }

    //creates according to size
    for(i=0; i<size; i++){

    	sortArray[i] = (SORTINGPoint)malloc(sizeof(**sortArray));
        if (!sortArray[i]) {
            spLoggerPrintError(ALLOC_FAIL, __FILE__, __FUNCTION__, __LINE__);
            spKDArrayDestroy(kdArr, ALL_ROWS);
            sortingPointDestroyArray (sortArray, i);
            return NULL;
        }

        sortArray[i]->point = spPointCopy(kdArr->points[i]);
        if (!sortArray[i]->point) {
            spLoggerPrintError(ALLOC_FAIL, __FILE__, __FUNCTION__, __LINE__);
            spKDArrayDestroy(kdArr, ALL_ROWS);
            sortingPointDestroyArray (sortArray, i);
            return NULL;
        }

        sortArray[i]->index = i;
    }
    
    //creates according to dim
    for(i=0; i<kdArr->dim;i++){

    	kdArr->featsMat[i] = malloc(size*sizeof(**kdArr->featsMat));
        if (!kdArr->featsMat[i]) {
            spLoggerPrintError(ALLOC_FAIL, __FILE__, __FUNCTION__, __LINE__);
            spKDArrayDestroy(kdArr, ALL_ROWS);
            sortingPointDestroyArray(sortArray, size);
            return NULL;
        }

        for(j=0; j<size; j++){
            sortArray[j]->axis = i;
        }
        qsort(sortArray, size, sizeof(SORTINGPoint), compare_points);
        for(j=0; j<size; j++){
            kdArr->featsMat[i][j] = sortArray[j]->index;
        }
    }
    spLoggerPrintInfo(KDARRAY_CREATED);
    sortingPointDestroyArray (sortArray, size);
    return kdArr;
}

/*
 * internal help function to free allocated memory used for
 * spKDArraySplit method/
 * @params: all allocated variables except return value.
 */
void EscapeArraySplit(int *partitionMap_1,int *partitionMap_2,
		SPPoint* newPoints_1, SPPoint* newPoints_2, int error){
	if (error){
		spLoggerPrintError(ALLOC_FAIL, __FILE__, __FUNCTION__, __LINE__);
	}
	if(newPoints_1){
		free(newPoints_1);
	}
	if(newPoints_2){
		free(newPoints_2);
	}
	if(partitionMap_1){
		free(partitionMap_1);
	}
	if(partitionMap_2){
		free(partitionMap_2);
	}
	return;
}


SPKDArray* spKDArraySplit(SPKDArray kdArr, int coor){
	int n, i, temp, location;
	int cnt = 0, mapLeft=0, mapRight=0;
	int *partitionMap_1=NULL, *partitionMap_2=NULL;
	SPKDArray* kd=NULL;
	SPPoint* newPoints_1 = NULL, *newPoints_2=NULL;
	n = (int)ceilf(kdArr->numOfFeats/2.0);

	newPoints_1 = (SPPoint*)malloc(n*sizeof(SPPoint));
	if(!newPoints_1){
		return NULL;
	}

	newPoints_2 = (SPPoint*)malloc(n*sizeof(SPPoint));
	if(!newPoints_2){
		EscapeArraySplit(NULL,NULL,
				newPoints_1, NULL, true);
		return NULL;
	}

    kd = (SPKDArray*)malloc(2*sizeof(SPKDArray));
    if(!kd){
		EscapeArraySplit(NULL,NULL,
				newPoints_1, newPoints_2, true);
        return NULL;
    }

    partitionMap_1 = (int*)malloc((kdArr->numOfFeats)*sizeof(int));
    if(!partitionMap_1){
		EscapeArraySplit(NULL,NULL, newPoints_1, newPoints_2, true);
		spKDMultiArrayDestroy(kd, false, false, false, false);
    	return NULL;
    }

    partitionMap_2 = (int*)malloc((kdArr->numOfFeats)*sizeof(int));
    if(!partitionMap_2){
		EscapeArraySplit(partitionMap_1,NULL, newPoints_1, newPoints_2, true);
		spKDMultiArrayDestroy(kd, false, false, false, false);
    	return NULL;
    }

    // copy left points to new points and create partition map
    for (i=0; i<n; i++){ //distribute the points
        location = kdArr->featsMat[coor][i];
        newPoints_1[cnt] = kdArr->points[location];
        partitionMap_1[kdArr->featsMat[coor][i]] = cnt;
        partitionMap_2[kdArr->featsMat[coor][i]] = NOT_PRESENT;
        cnt++;
    }
    cnt =0;
    //continue with the right part of the points
    for (; i<kdArr->numOfFeats; i++){
        location = kdArr->featsMat[coor][i];
        newPoints_2[cnt] = kdArr->points[location];
        partitionMap_2[kdArr->featsMat[coor][i]] = cnt;
        partitionMap_1[kdArr->featsMat[coor][i]] = NOT_PRESENT;
        cnt++;
    }

    kd[LEFT] = spKDArrayCreate(newPoints_1, n, kdArr->dim);
    if (!kd[LEFT]){
		EscapeArraySplit(partitionMap_1,partitionMap_2,
				newPoints_1, newPoints_2, true);
		spKDMultiArrayDestroy(kd, false, false, false, false);
        return NULL;
    }

    kd[LEFT]->featsMat = malloc(kdArr->dim*sizeof(*kdArr->featsMat));
    if (!kd[LEFT]->featsMat){
		EscapeArraySplit(partitionMap_1,partitionMap_2,
				newPoints_1, newPoints_2, true);
		spKDMultiArrayDestroy(kd, false, false, false, false);
        return NULL;
    }

    kd[RIGHT] = spKDArrayCreate(newPoints_2, (kdArr->numOfFeats-n), kdArr->dim);
    if (!kd[RIGHT]){
		EscapeArraySplit(partitionMap_1,partitionMap_2,
				newPoints_1, newPoints_2, true);
		spKDMultiArrayDestroy(kd, true, false, false, false);
        return NULL;
    }

    kd[RIGHT]->featsMat = malloc(kdArr->dim*sizeof(*kdArr->featsMat));
    if (!kd[RIGHT]->featsMat){
        EscapeArraySplit(partitionMap_1,partitionMap_2,
        				newPoints_1, newPoints_2, true);
		spKDMultiArrayDestroy(kd, true, false, true, false);
        return NULL;
    }

    // figure out the features matrix
    for(i=0; i<kdArr->dim; i++){//iterate through the different axis
        mapLeft = 0;
        mapRight= 0;

        kd[LEFT]->featsMat[i] = (int*)malloc(n*sizeof(int));
        if(!kd[LEFT]->featsMat[i]){
            EscapeArraySplit(partitionMap_1,partitionMap_2,
            				newPoints_1, newPoints_2, true);
            spKDMultiArrayDestroy(kd, true, i, true, i);
            return NULL;
        }

        kd[RIGHT]->featsMat[i] = (int*)malloc(
        					(kdArr->numOfFeats - n)*sizeof(int));
        if(!kd[RIGHT]->featsMat[i]){
            EscapeArraySplit(partitionMap_1,partitionMap_2,
            				newPoints_1, newPoints_2, true);
            spKDMultiArrayDestroy(kd, true, i, true, i-1);
            return NULL;
        }

        //iterate through the point map//
        for(cnt=0; cnt<kdArr->numOfFeats;cnt++){
            location = kdArr->featsMat[i][cnt];
            //exists in left partition//
            if((temp = partitionMap_1[location])>NOT_PRESENT){
                kd[LEFT]->featsMat[i][mapLeft] = temp;
                mapLeft++;
            }
            else{
                temp = kdArr->featsMat[i][cnt];
                temp = partitionMap_2[temp];
                kd[RIGHT]->featsMat[i][mapRight] = temp;
                mapRight++;
            }
        }
    }
    EscapeArraySplit(partitionMap_1,partitionMap_2,
    				newPoints_1, newPoints_2, false);
    return kd;
}


void spKDMultiArrayDestroy(SPKDArray* kdArr,bool left,
		int l_size, bool right, int r_size){
	if(kdArr){
		if(left){
			spKDArrayDestroy(kdArr[LEFT], l_size);
		}
		if(right){
			spKDArrayDestroy(kdArr[RIGHT], r_size);
		}
		free(kdArr);
	}
	return;
}


void spKDArrayDestroy(SPKDArray kdArr,int size){
    int i;
    if(size == ALL_ROWS){
    	size = kdArr->numOfFeats;
    }
    if(kdArr){
    	 //** destroy all points + points itself **//
    	if (kdArr->points){
    		for(i=0; i<size;i++){
    			if (kdArr->points[i]){
    			spPointDestroy(kdArr->points[i]);
    			}else{
    				break;
    			}
    	    }
		free(kdArr->points);
    	}
    	//** destroy featsMat**//
		if(kdArr->featsMat){
			for(i=0;i<kdArr->dim;i++){
				if(kdArr->featsMat[i]){
					free(kdArr->featsMat[i]);
				}else{break;}
			}
			free(kdArr->featsMat);
		}
		free(kdArr);
	}
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


int spKDArrayFindSplitDim(const SPKDArray kdArr,int prevDim,
				const SPConfig config, SP_CONFIG_MSG *msg){
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
            break;
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

