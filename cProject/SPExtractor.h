//
//  main_aux.h
//  cProject
//
//  Created by Orr Barkat on 16/08/2016.
//  Copyright © 2016 Orr Barkat. All rights reserved.
//

#ifndef SPExtractor_h
#define SPExtractor_h

#include <stdio.h>
#include "SPPoint.h"
#include "SPConfig.h"


/**
 * method summery:
 * spExtractSaveFeatures - creates a file with the img features
 * spExtractReadFeatures - reads a feature file into an array of points
 */

typedef enum sp_extract_msg_t {
    SP_EXTRACT_CANNOT_OPEN_FILE,
    SP_EXTRACT_ALLOC_FAIL,
    SP_EXTRACT_INDEX_OUT_OF_RANGE,
    SP_EXTRACT_SUCCESS
} SP_EXTRACT_MSG;

SP_EXTRACT_MSG spExtractorSaveFeatures(const char *filename, const int numOfPoints, const SPConfig config, SP_CONFIG_MSG *msg, SPPoint *points);

SPPoint* spExtractorLoadImageFeatures(int imageIndex, int *numOfPoints, const SPConfig config, SP_CONFIG_MSG *msg);

SPPoint* spExtractorLoadAllFeatures(int *totalNumOfPoints, int numOfImages, const SPConfig config, SP_CONFIG_MSG *msg);

    
#endif /* main_aux_h */
