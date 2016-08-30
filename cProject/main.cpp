//
//  main.c
//  cProject
//
//  Created by Orr Barkat on 04/08/2016.
//  Copyright © 2016 Orr Barkat. All rights reserved.
//

#include <cstdio>
#include <cstdlib>
#include "SPImageProc.h"

extern "C" {
#include "SPPoint.h"
#include "SPConfig.h"
#include "KDTree.h"
#include "KDArray.h"
#include "SPLogger.h"
#include "SPExtractor.h"
}

using namespace sp;


int main(int argc, const char * argv[]) {
    char query[LINE_LENGTH], currentPointPath[LINE_LENGTH];
    ImageProc *processor = nullptr;
    int numOfFeats, numOfImages, i, totalNumOfPoints=0;
    SP_EXTRACT_MSG extractMsg;
    SPPoint *points;
    SPKDArray kdArray;
    SPKDTreeNode root;
    //create a config struct
    SP_CONFIG_MSG *msg = (SP_CONFIG_MSG*)malloc(sizeof(*msg));
    char filename[LINE_LENGTH];
    if( spConfigGetConfigFile(argc, argv, filename, msg)){
        free(msg);
        return 0;
    }
    SPConfig conf = spConfigCreate(filename, msg);
    if (!conf){
        printf("an error has occured with the config process\n");
        return 0;
    }
     //preprocess - extract features if nessacery
    numOfImages = spConfigGetNumOfImages(conf, msg);
    if(*msg != SP_CONFIG_SUCCESS){
        spConfigDestroy(conf);
        free(msg);
        spLoggerPrintError(PARAM_MISSING_NUM_IMAGES, __FILE__, __FUNCTION__, __LINE__);
        return 0;
    }
    if( spConfigIsExtractionMode(conf, msg)){
        processor = new ImageProc(conf);
        for(i=0; i<numOfImages; i++){
            spConfigGetImagePath(currentPointPath, conf, i);
            points = processor->getImageFeatures(currentPointPath, i, &numOfFeats);
            spConfigGetSavePath(currentPointPath, conf, i);
            if((extractMsg = spExtractorSaveFeatures(currentPointPath, numOfFeats, conf, msg, points)) != SP_EXTRACT_SUCCESS){
                if(extractMsg != SP_EXTRACT_BAD_INPUT){
                    for(i=0; i<numOfFeats; i++){
                        spPointDestroy(points[i]);
                    }
                    free(points);
                    spConfigDestroy(conf);
                    delete processor;
                    free(msg);
                    return 0;
                }
            }
            totalNumOfPoints += numOfFeats;
            free(points);
        }
        
    }//get the points whether they where extracted or not
    points = spExtractorLoadAllFeatures(&totalNumOfPoints, numOfImages, conf, msg);
    if(!points){
        spLoggerPrintError(EXTRACT_FAIL, __FILE__, __FUNCTION__, __LINE__);
        spConfigDestroy(conf);
        delete processor;
        free(msg);
        return 0;
    }
    //initialize data structures
    kdArray = spKDArrayInit(points, totalNumOfPoints, conf, msg);
    if(!kdArray){
        spLoggerPrintError(KDARRAY_FAIL, __FILE__, __FUNCTION__, __LINE__);
        spConfigDestroy(conf);
        delete processor;
        free(msg);
        return 0;
    }
    root = spKDTreeCreateFromArray(kdArray, -1, conf, msg);
    //Query
    printf("Please enter an image path:\n");
    scanf("%s", query);
    
    
    
    
    spKDTreeDestroy(root);
    spConfigDestroy(conf);
    printf("works");
    delete processor;
    free(msg);
    return 0;
}
