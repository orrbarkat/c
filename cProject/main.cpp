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
#include "SPLogger.h"
#include "SPExtractor.h"
}

using namespace sp;


int main(int argc, const char * argv[]) {
    char query[LINE_LENGTH], currentPointPath[LINE_LENGTH];
    ImageProc *processor;
    int numOfFeats, numOfImages, i, totalNumOfPoints=0;
    SPPoint *points;
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
        processor = new ImageProc::ImageProc(conf);
        for(i=0; i<numOfImages; i++){
            spConfigGetImagePath(currentPointPath, conf, i);
            points = processor->getImageFeatures(currentPointPath, i, &numOfFeats);
            spConfigGetSavePath(currentPointPath, conf, i);
            spExtractorSaveFeatures(currentPointPath, numOfFeats, conf, msg, points);
            free(points);
            totalNumOfPoints += numOfFeats;
        }
    }//get the points whether they where extracted or not
    points = spExtractorLoadAllFeatures(&totalNumOfPoints, numOfImages, conf, msg);
    
    //initialize data structures
    
    //Query
    printf("Please enter an image path:\n");
//    scanf("%s", query);
    
    
    
    
    
    spConfigDestroy(conf);
    printf("works");
    free(processor);
    free(msg);
    return 0;
}
