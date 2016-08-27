

#include "SPPoint.h"

#include <stdlib.h>

#include <assert.h>

struct sp_point_t{
	int index;
	int dim;
	double *coords;
};

SPPoint spPointCreate(double* data, int dim, int index){
	SPPoint p = (SPPoint)malloc(sizeof(SPPoint)); //SPPoint memory allocation
	int i;

	/* Checking if arguments are valid */
	if (data == NULL || dim <= 0 || index < 0){
		free(p);
		return NULL;
	}

	/* Updating fields of p */
	p->index = index;
	p->dim = dim;
	p->coords = (double *)malloc(dim*sizeof(double));
	for (i = 0; i < dim; ++i) {
		(p->coords)[i] = data[i]; //p_i = data[i]
	}
	return p;
}

SPPoint spPointCopy(SPPoint source){
	SPPoint copy;
	assert (source != NULL);
	copy = spPointCreate(source->coords,source->dim,source->index);
	return copy;
}

void spPointDestroy(SPPoint point){
	if (point != NULL){
		free((*point).coords);
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









