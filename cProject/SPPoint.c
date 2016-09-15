

#include "SPPoint.h"



struct sp_point_t{
	int index;
	int dim;
	double *coords;
};

SPPoint spPointCreate(double* data, int dim, int index){
	int i;
    assert((dim > 0) && data && (index >= 0));
    //Printdata(&data, dim);
    struct sp_point_t *point = (struct sp_point_t*) malloc(sizeof(struct sp_point_t));
    if (point == NULL) {
    	spPointDestroy(point);
    	return NULL;
    }
    point->coords = (double *)calloc(dim, sizeof(double));
    if(!(point->coords)){
    	spPointDestroy(point);
        return NULL;
    }
    for (i = 0; i < dim; i++) {
        (point->coords)[i] = data[i];
    }
    point->dim = dim;
    point->index = index;
    return point;
}

SPPoint spPointCopy(SPPoint source){
	SPPoint copy;
	assert (source != NULL);
	copy = spPointCreate(source->coords,source->dim,source->index);
	if (!copy){
		spPointDestroy(source);
		return NULL;
	}
	return copy;
}

void spPointDestroy(SPPoint point){
	if (point){
		if (point->coords){
			free(point->coords);
		}
		free(point);
	}
	return;
}

void ArrayPointDestroy(SPPoint* ptrarr, int start_ind, int end_ind){
	int i;
	if(ptrarr){
		for(i=start_ind; i<end_ind; i++){
			if(ptrarr[i]){
				spPointDestroy(ptrarr[i]);
			}else{break;}
		}
		free(ptrarr);
	}
	return;
}

int spPointGetDimension(SPPoint point){
	assert (point != NULL);
	return point->dim;
}

int spPointGetIndex(SPPoint point){
	assert (point != NULL);
	return point->index;
}

double spPointGetAxisCoor(SPPoint point, int axis){
	assert (point != NULL && axis < point->dim);
	return (point->coords)[axis];
}

double spPointL2SquaredDistance(SPPoint p, SPPoint q){
	int i, dim;
	double cp, cq, diff, distance = 0;
	assert (p != NULL && q != NULL && (p->dim == q->dim));
	dim = p->dim;

	//Calculating L2 squared distance
	for (i = 0; i < dim; ++i) {
		cp = p->coords[i];
		cq = q->coords[i];
		diff = cp - cq;
		distance += diff*diff;
	}
	return distance;
}









