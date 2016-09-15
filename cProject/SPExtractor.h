//
//  cProject
//

#ifndef SPExtractor_h
#define SPExtractor_h

#include <stdio.h>
#include "SPPoint.h"
#include "SPConfig.h"
#include "macros.h"


/**
 * method summary:
 * spExtractorSaveFeatures - creates a file with the img features
 * spExtractorReadFeatures - reads a feature file into an array of points
 */

/**
 * enumerator for massages to be displayed during extraction mode.
 */
typedef enum sp_extract_msg_t {
    SP_EXTRACT_CANNOT_OPEN_FILE,
    SP_EXTRACT_ALLOC_FAIL,
    SP_EXTRACT_INDEX_OUT_OF_RANGE,
    SP_EXTRACT_BAD_INPUT,
    SP_EXTRACT_SUCCESS
} SP_EXTRACT_MSG;

/**
 * Saves all image features to a file based on spConfigGetSavePath(...)
 * the function frees all the points in the given array if it was successful.
 * format is: imageIndex , numOfPoints, dim p_1[0] p_1[1] ... p_numOfPoints[dim -1] EOF
 * @return
 *	- SP_EXTRACT_CANNOT_OPEN_FILE if file didn't open
 *  - SP_EXTRACT_SUCCESS if written successfully
 *  - SP_EXTRACT_ALLOC_FAIL if there is an allocation failure
 *  - SP_EXTRACT_BAD_INPUT if there are no points given 
 */
SP_EXTRACT_MSG spExtractorSaveFeatures(const char *filename, const int numOfPoints, const SPConfig config, SP_CONFIG_MSG *msg, SPPoint *points);

/**
 * Load features for a single image.
 * used in future method "spExtractorLoadAllFeatures" as helper.
 * @param: imageIndex: the index of the image to be join to path name
 * @param: *numOfPoints: the number of points to create in the method
 * @param: config:configuration file to act accordingly
 * @return
 * 	- NULL in case of allocation error
 * 	- SPPoint in case of successful load
 */
SPPoint* spExtractorLoadImageFeatures(int imageIndex, int *numOfPoints, const SPConfig config);

/**
 * Load features for pool of images.
 * @param: totalNumOfPoints: index to be loaded
 * @param: numOfImages: to load
 * @param: config:configuration file to act accordingly
 * @param: msg: to be used in the logger file
 * @return
 * 	- NULL in case of allocation error
 * 	- SPPoint in case of successful load
 **/
SPPoint* spExtractorLoadAllFeatures(int *totalNumOfPoints, int numOfImages, const SPConfig config);

    
#endif /* main_aux_h */
