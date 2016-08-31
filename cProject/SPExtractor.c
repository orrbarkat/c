//
//  main_aux.c
//  cProject
//
//  Created by Orr Barkat on 16/08/2016.
//  Copyright © 2016 Orr Barkat. All rights reserved.
//

#include "SPExtractor.h"

SP_EXTRACT_MSG spExtractorSaveFeatures(const char *filename, const int numOfPoints, const SPConfig config, SP_CONFIG_MSG *msg, SPPoint *points){
    FILE *fp;
    int imageIndex, dim, i,j;
    if(!points){
        spLoggerPrintError(EXTRACT_FAIL, filename, __FUNCTION__, __LINE__);
        return SP_EXTRACT_CANNOT_OPEN_FILE;
    }
    dim = spConfigGetPCADim(config, msg);
    imageIndex = spPointGetIndex(*points);
    
    fp = fopen(filename , "w");
    if(!fp){
        spLoggerPrintError(FILE_IO_FAIL, filename, __FUNCTION__, __LINE__);
        return SP_EXTRACT_CANNOT_OPEN_FILE;
    }
    fprintf(fp, "%i %i %i ", imageIndex , numOfPoints, dim);
    for(i=0; i<numOfPoints; i++){
        for(j=0; j<dim; j++){
            fprintf(fp, "%lf ", spPointGetAxisCoor(points[i], j));
        }
        spPointDestroy(points[i]);
    }
    fprintf(fp, "%i", EOF);
    fclose(fp);
    return SP_EXTRACT_SUCCESS;
}

SPPoint* spExtractorLoadImageFeatures(int imageIndex, int *numOfPoints, const SPConfig config){
    FILE *fp;
    int i,j, dim;
    char filename[LINE_LENGTH];
    SPPoint *points;
    double *data;
    spConfigGetSavePath(filename, config, imageIndex);
    fp = fopen(filename, "r");
    if(!fp){
        spLoggerPrintError(FILE_IO_FAIL, filename, __FUNCTION__, __LINE__);
        return NULL;
    }
    fscanf(fp, "%i %i %i ", &imageIndex , numOfPoints, &dim);
    data = malloc(dim*sizeof(double));
    if(!data){
        spLoggerPrintError(ALLOC_FAIL, filename, __FUNCTION__, __LINE__);
        fclose(fp);
        return NULL;
    }
    points = malloc((*numOfPoints)*sizeof(*points));
    if(!points){
        spLoggerPrintError(ALLOC_FAIL, filename, __FUNCTION__, __LINE__);
        fclose(fp);
        free(data);
        return NULL;
    }
    for(i=0; i<*numOfPoints; i++){
        for(j=0; j<dim; j++){
            fscanf(fp, "%lf ", &data[j]);
        }
        points[i] = spPointCreate(data, dim, imageIndex);
    }
    free(data);
    fclose(fp);
    return points;
}

SPPoint* spExtractorLoadAllFeatures(int *totalNumOfPoints, int numOfImages, const SPConfig config, SP_CONFIG_MSG *msg){
    int i,j, currentNumOfPoints=0, allocated, index=0;
    *totalNumOfPoints=0;
    SPPoint *current = NULL, *result, *temp;
    if((allocated=spConfigGetNumOfFeatures(config, msg))<=0){
        spLoggerPrintError(EXTRACT_FAIL, __FILE__, __FUNCTION__, __LINE__);
        return NULL;
    }
    allocated*= numOfImages;
    result = malloc(allocated*sizeof(*result));//set the num of features initially to numOfFeats*numOfImages
    if(!result){
        spLoggerPrintError(ALLOC_FAIL, __FILE__, __FUNCTION__, __LINE__);
        return NULL;
    }
    for(i=0; i<numOfImages; i++){
        current = spExtractorLoadImageFeatures(i, &currentNumOfPoints, config);
        if (!current){//there was an error in feats
            spLoggerPrintError(EXTRACT_FAIL, __FILE__, __FUNCTION__, __LINE__);
            while(index>0){
                index--;
                spPointDestroy(result[index]);
            }
            free(result);
            return NULL;
        }
        *totalNumOfPoints+=currentNumOfPoints;
        if((index+currentNumOfPoints)>allocated){
            temp = realloc(result, sizeof(*result)*(index+currentNumOfPoints));
            if(!temp){
                spLoggerPrintError(ALLOC_FAIL, __FILE__, __FUNCTION__, __LINE__);
                while(index>0){
                    index--;
                    spPointDestroy(result[index]);
                }
                free(result);
                while(index<currentNumOfPoints){
                    spPointDestroy(current[index]);
                    index++;
                }
                free(current);
                return NULL;
            }else{
                result = temp;
            }
        }
        for(j=0; j<currentNumOfPoints; j++){
            result[index] = spPointCopy(current[j]);
            index++;
            spPointDestroy(current[j]);
        }
        free(current);
    }
    *totalNumOfPoints = index;
    return result;
}

