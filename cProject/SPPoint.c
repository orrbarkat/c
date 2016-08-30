

#include "SPPoint.h"



struct sp_point_t{
	int index;
	int dim;
	double *coords;
};

SPPoint spPointCreate(double* data, int dim, int index){
    assert((dim > 0) && data && (index >= 0));
    //Printdata(&data, dim);
    struct sp_point_t *point = (struct sp_point_t*) malloc(sizeof(*point));
    if (point == NULL) {
        return NULL;
    }
    point->coords = (double *)malloc(dim * sizeof(double));
    if(!point->coords){
        free(point);
        return NULL;
    }
    point->dim = dim;
    point->index = index;
    
    for ( int i = 0; i < dim; i++) {
        (point->coords)[i] = data[i];
    }
    
    return point;
}

SPPoint spPointCopy(SPPoint source){
	SPPoint copy;
	assert (source != NULL);
	copy = spPointCreate(source->coords,source->dim,source->index);
	return copy;
}

void spPointDestroy(SPPoint point){
	if (point != NULL){
		free(point->coords);
		free(point);
	}
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









