#include "SPConfig.h"

struct sp_config_t {
    char* spImagesDirectory;
    char* spImagesPrefix;
    char* spImagesSuffix;
    char* spPCAFilename;
    char* spLoggerFilename;
    int spNumOfImages;
    int spPCADimension;
    int spNumOfFeatures;
    int spNumOfSimilarImages;
    int spKNN;
    int spLoggerLevel; // 1 error,2 warning, 3 info, 4 debug
    bool spExtractionMode;
    bool spMinimalGUI;
    KDTreeSplitMethod spKDTreeSplitMethod;
};

void spConfigPrintCliError(const char* filename,
						const SP_CONFIG_MSG message){
    switch (message) {
        case SP_CONFIG_CANNOT_OPEN_FILE:
            strcmp(filename,DEFAULT_CONFIG) ? printf(CONF_FILE_DIDNT_OPENED\
            ,filename) : printf(DEF_CONF_FILE_DIDNT_OPENED );
            break;
        default:
            printf(INVALID_CMD);
            break;
    }
}


/**
 * print message to stdout
 */
void spConfigPrintError(const char* filename, int line,
		const SP_CONFIG_MSG* message){
    char* type = PARAM_MISSING_DIR;
    bool print = true;
    switch (*message) {
        case SP_CONFIG_MISSING_DIR:
            break;
        case SP_CONFIG_MISSING_PREFIX:
            type = PARAM_MISSING_PREFIX;
            break;
        case SP_CONFIG_MISSING_SUFFIX:
            type = PARAM_MISSING_SUFFIX;
            break;
        case SP_CONFIG_MISSING_NUM_IMAGES:
            type = PARAM_MISSING_NUM_IMAGES;
            break;
        case SP_CONFIG_CANNOT_OPEN_FILE:
            spConfigPrintCliError(filename, *message);
            print = false;
            break;
        default://all other errors are invalid value
            type = INVALID_VALUE;
            break;
    }
    if(print){
        printf(PRINT_ERROR_FORMAT , filename, line,type);
    }
    return;
}


 
/**
 * sets default values for functions
 */
int setDefaults(SPConfig config, SP_CONFIG_MSG *msg){
    if (!config){ return -1; }
    config->spPCADimension = DEF_CONF_DIM;
    config->spNumOfFeatures = DEF_CONF_FEAT;
    config->spNumOfImages = DEF_CONF_NUM_IMG;
    config->spExtractionMode = true;
    config->spMinimalGUI = false;
    config->spKNN = DEF_CONF_KNN_SIM;
    config->spKDTreeSplitMethod = MAX_SPREAD;
    config->spLoggerLevel = DEF_CONF_LOG_LEV;
    config->spNumOfSimilarImages = DEF_CONF_KNN_SIM;
    config->spImagesDirectory = (char*)calloc(LINE_LENGTH,sizeof(char));
    config->spImagesPrefix = (char*)calloc(LINE_LENGTH,sizeof(char));
    config->spImagesSuffix = (char*)calloc(LINE_LENGTH,sizeof(char));
    config->spPCAFilename = (char*)calloc(LINE_LENGTH,sizeof(char));
    config->spLoggerFilename = (char*)calloc( LINE_LENGTH,sizeof(char));

    // clean in case of allocation failure

    if(!(config->spImagesSuffix)){
    	*msg = SP_CONFIG_ALLOC_FAIL;
    	free(config->spImagesDirectory);
    	free(config);
    	return -1;
    }
    if(!(config->spImagesPrefix)){
    	*msg = SP_CONFIG_ALLOC_FAIL;
    	free(config->spImagesDirectory);
    	free(config->spImagesSuffix);
    	free(config);
    	return -1;
    }
    if(!(config->spPCAFilename)){
    	*msg = SP_CONFIG_ALLOC_FAIL;
    	free(config->spImagesDirectory);
    	free(config->spImagesSuffix);
    	free(config->spImagesPrefix);
    	free(config);
    	return -1;
    }
    if(!(config->spLoggerFilename)){
    	*msg = SP_CONFIG_ALLOC_FAIL;
    	free(config->spImagesDirectory);
    	free(config->spImagesSuffix);
    	free(config->spImagesPrefix);
    	free(config->spPCAFilename);
    	free(config);
    	return -1;
    }
    return 1;
}

/**
 * checks if suffix is in the correct format
 * @return
 * - true if not valid suffix
 * - false if the suffix is valid
 */
bool suffixValidator(const char *val){
    return (0 != (strcmp(val, JPG)*strcmp(val, PNG)
    		*strcmp(val, BMP)*strcmp(val, GIF)));
}

/**
 * function to free allocated data in parseline method
 * @params: data allocated
 * @return:
 * 	- -1 in case of error
 * 	- +1 in case of success
 */
int ParseCleaner(char* temp, char* var1, char* var2, int success){
    free(temp);
    free(var1);
    free(var2);
    return success;
}

/**
 * Line parser for the config file.
 * if a line is invalid the msg is filled according with the create header
 */
int parseLine(SPConfig* config, const char* line, SP_CONFIG_MSG* msg){
    int tempIndex, lineIndex=0, varIndex=0, lineLen=(int)strlen(line);
    bool notFinished, hasSpaces, comment = false;
    char *temp, *variable_1, *variable_2;
    if (!config || !line){
        return -1;
    }
    temp = (char*)calloc(LINE_LENGTH,sizeof(char));
    //allocation problem:
    if (!temp){
      	*msg = SP_CONFIG_ALLOC_FAIL;
       	free(temp);
       	return -1;
       	}
    variable_1 = (char*)calloc(LINE_LENGTH, sizeof(char));
    if (!variable_1){
       	*msg = SP_CONFIG_ALLOC_FAIL;
       	free(temp);
       	free(variable_1);
       	return -1;
    	}
	variable_2 = (char*)calloc(LINE_LENGTH, sizeof(char));
    if (!variable_2){
    	*msg = SP_CONFIG_ALLOC_FAIL;
    	free(temp);
    	free(variable_1);
       	free(variable_2);
       	return -1;
        }


    // parse the two sided equation from spcbir.config
    for(;varIndex<2;varIndex++){
        tempIndex = 0;
        hasSpaces = false;
        notFinished = true;
        if (comment){ break;}
        while(notFinished && lineIndex<lineLen && !comment){
            switch (line[lineIndex]) {
                case '\n':
                case '=':
                    notFinished = false;
                    lineIndex++;
                    break;
                case '#':
                    comment = true;
                    break;
                case '\t':
                case ' ':
                    if(tempIndex){ // variable is not empty by now
                        hasSpaces = true;
                    }
                    lineIndex++;
                    break;
                default:
                    if(hasSpaces){//there is a space between some characters
                        *msg = SP_CONFIG_INVALID_STRING;
                        return ParseCleaner(temp, variable_1, variable_2, -1);
                    }
                    temp[tempIndex] = line[lineIndex];
                    tempIndex++;
                    lineIndex++;
            }
        }

        temp[tempIndex] = '\0';
        if (varIndex == 0){
        	strncpy(variable_1, temp, LINE_LENGTH);
            }
        else{
   			strncpy(variable_2, temp, LINE_LENGTH);
            }
    }
    if(strcmp(variable_1,PARSE_DIR)==0){
        strcpy((*config)->spImagesDirectory, variable_2);
        
    }else if(strcmp(variable_1,PARSE_PRE)==0){
        strcpy((*config)->spImagesPrefix, variable_2);
        
    }else if(strcmp(variable_1,PARSE_SUF)==0){
        if(suffixValidator(variable_2)){
        	// value is not in .jpg , .png , .bmp , .gif
            *msg = SP_CONFIG_INVALID_STRING;
            return ParseCleaner(temp, variable_1, variable_2,-1);
        }
        strcpy((*config)->spImagesSuffix, variable_2);
        
    }else if(strcmp(variable_1,PARSE_FNAME)==0){
        strcpy((*config)->spPCAFilename, variable_2);
        
    }else if(strcmp(variable_1,PARSE_LOGNAME)==0){
        strcpy((*config)->spLoggerFilename, variable_2);
        
    }else if(strcmp(variable_1,PARSE_NUM_IMG)==0){
        if(atoi(variable_2)<=0){//value contains non digit characters
            *msg = SP_CONFIG_INVALID_INTEGER;
            return ParseCleaner(temp, variable_1, variable_2, -1);
        }
        (*config)->spNumOfImages = atoi(variable_2);
        
    }else if(strcmp(variable_1,PARSE_DIM)==0){
        (*config)->spPCADimension = atoi(variable_2);
        
    }else if(strcmp(variable_1,PARSE_FEAT)==0){
        if(atoi(variable_2)<=0){//value contains non digit characters
            *msg = SP_CONFIG_INVALID_INTEGER;
            return ParseCleaner(temp, variable_1, variable_2, -1);
        }
        (*config)->spNumOfFeatures = atoi(variable_2);
        
    }else if(strcmp(variable_1,PARSE_SIM)==0){
        if(atoi(variable_2)<=0){//value contains non digit characters
            *msg = SP_CONFIG_INVALID_INTEGER;
            return ParseCleaner(temp, variable_1, variable_2, -1);
        }
        (*config)->spNumOfSimilarImages = atoi(variable_2);
        
    }else if(strcmp(variable_1,PARSE_KNN)==0){
        if(atoi(variable_2)<=0){//value contains non digit characters
            *msg = SP_CONFIG_INVALID_INTEGER;
            return ParseCleaner(temp, variable_1, variable_2, -1);
        }
        (*config)->spKNN = atoi(variable_2);
        
    }else if(strcmp(variable_1,PARSE_LOG_LEV)==0){
        if(1>atoi(variable_2) || 4<atoi(variable_2)){
        	//value contains non digit characters or digit are not in [1..4]
            *msg = SP_CONFIG_INVALID_INTEGER;
            return ParseCleaner(temp, variable_1, variable_2, -1);
        }
        
        (*config)->spNumOfImages = atoi(variable_2);
        
    }else if(strcmp(variable_1,PARSE_XTRACT)==0){
        if(strcmp(variable_2,PARSE_FALSE)==0){
            (*config)->spExtractionMode = false;
        }else if(strcmp(variable_2,PARSE_TRUE)==0){
                (*config)->spExtractionMode = true;
        }else{
                *msg = SP_CONFIG_INVALID_STRING;
                return ParseCleaner(temp, variable_1, variable_2, -1);
        }

    }else if(strcmp(variable_1,PARSE_MINGUI)==0){
        if(strcmp(variable_2,PARSE_TRUE)==0){ (*config)->spMinimalGUI = true;
        }else if(strcmp(variable_2,PARSE_FALSE)==0){
        	(*config)->spMinimalGUI = false;
        }else{
            *msg = SP_CONFIG_INVALID_STRING;
            return ParseCleaner(temp, variable_1, variable_2, -1);
        }
        
    }else if(strcmp(variable_1,PARSE_SPLIT_METH)==0){
        if(strcmp(PARSE_RAND, variable_2)==0){
        	(*config)->spKDTreeSplitMethod = RANDOM;
        }else if(strcmp(PARSE_INC, variable_2)==0){
        	(*config)->spKDTreeSplitMethod = INCREMENTAL;
        }else if (strcmp(PARSE_SPR, variable_2)==0){
        }else{
            *msg = SP_CONFIG_INVALID_STRING;
            return ParseCleaner(temp, variable_1, variable_2, -1);
        }
        
    }else if(strlen(variable_1)){//invalid variable name
            *msg = SP_CONFIG_INVALID_STRING;
            return ParseCleaner(temp, variable_1, variable_2, -1);
    }
    return ParseCleaner(temp, variable_1, variable_2, 1);
}

/**
 * checks if all configuration requirements were met
 * @return
 * - true if not valid conf
 * - false if the conf is valid
 */
bool invalid(const SPConfig config, SP_CONFIG_MSG* msg){
    bool validity = false;
    if(strcmp(config->spPCAFilename,"")==0){
        strcpy(config->spPCAFilename,PARSE_YML);
    }
    if(strcmp(config->spLoggerFilename,"")==0){
        free(config->spLoggerFilename);
        config->spLoggerFilename = NULL;
    }
    // check that
    if(strcmp(config->spImagesDirectory,"")==0){
        *msg = SP_CONFIG_MISSING_DIR;
        validity=true;
    }
    if(!config->spImagesPrefix){
        *msg = SP_CONFIG_MISSING_PREFIX;
        validity=true;
    }
    if(!config->spImagesSuffix){
        *msg = SP_CONFIG_MISSING_SUFFIX;
        validity=true;
    }
    if(config->spNumOfImages<=0){
        *msg = SP_CONFIG_MISSING_NUM_IMAGES;
        validity = true;
    }
    return validity;
}


SPConfig spConfigCreate(const char* filename, SP_CONFIG_MSG* msg,
									SPLogger logger){
    FILE *fp;
    char line[LINE_LENGTH];
    SPConfig config;
    int lineNumber=1;
    
    assert(msg != NULL);
    if (!filename){
        *msg = SP_CONFIG_INVALID_ARGUMENT;
        spConfigPrintError(filename, 0, msg);
        return NULL;
    }
    config = (SPConfig)malloc(sizeof(*config));
    fp = fopen(filename, "r");
    if (!fp){
        *msg = SP_CONFIG_CANNOT_OPEN_FILE;
        free(config);
        spConfigPrintError(filename, 0, msg);
        return NULL;
    }
    
    if (!config){
        *msg = SP_CONFIG_ALLOC_FAIL;
        fclose(fp);
        return NULL;
    }
    if (setDefaults(config, msg) == -1){
    	fclose(fp);
    	*msg = SP_CONFIG_ALLOC_FAIL;
    	return NULL;
    }
    while (fgets(line, LINE_LENGTH, fp)){
        if ( parseLine(&config, line, msg)<0){
            fclose(fp);
            spConfigPrintError(filename, lineNumber, msg);
            spConfigDestroy(config, logger);
            return NULL;
        }
        lineNumber++;
    }
    fclose(fp);
    if (invalid(config,msg)){
        spConfigPrintError(filename, lineNumber, msg);
        spConfigDestroy(config, logger);
        return NULL;
    }
    spLoggerCreate(config->spLoggerFilename, config->spLoggerLevel);
    *msg = SP_CONFIG_SUCCESS;
    /*
     * checking that number of similar images from configuration is less than
     * number of images. if it doesn't - write an error
     */
    if(config->spNumOfSimilarImages > config->spNumOfImages){
    	*msg = SP_CONFIG_INVALID_ARGUMENT;
    	spConfigPrintError(filename, lineNumber, msg);
    	spConfigDestroy(config, logger);
    	return NULL;
    }
    spLoggerPrintInfo(CREATE_CONFIG);
    return config;
}



bool spConfigIsExtractionMode(const SPConfig config, SP_CONFIG_MSG* msg){
    assert(msg != NULL);
    if (!config){
        *msg = SP_CONFIG_INVALID_ARGUMENT;
        return false;
    }
    *msg = SP_CONFIG_SUCCESS;
    return config->spExtractionMode;
}


bool spConfigMinimalGui(const SPConfig config, SP_CONFIG_MSG* msg){
    assert(msg != NULL);
    if (!config){
        *msg = SP_CONFIG_INVALID_ARGUMENT;
        return false;
    }
    *msg = SP_CONFIG_SUCCESS;
    return config->spMinimalGUI;
}

int spConfigGetNumOfImages(const SPConfig config, SP_CONFIG_MSG* msg){
    assert(msg != NULL);
    if (!config){
        *msg = SP_CONFIG_INVALID_ARGUMENT;
        return -1;
    }
    *msg = SP_CONFIG_SUCCESS;
    return config->spNumOfImages;
}

int spConfigGetNumOfFeatures(const SPConfig config, SP_CONFIG_MSG* msg){
    assert(msg != NULL);
    if (!config){
        *msg = SP_CONFIG_INVALID_ARGUMENT;
        return -1;
    }
    *msg = SP_CONFIG_SUCCESS;
    return config->spNumOfFeatures;
}

int spConfigGetPCADim(const SPConfig config, SP_CONFIG_MSG* msg){
    assert(msg != NULL);
    if (!config){
        *msg = SP_CONFIG_INVALID_ARGUMENT;
        return -1;
    }
    *msg = SP_CONFIG_SUCCESS;
    return config->spPCADimension;
}

SP_CONFIG_MSG spConfigGetImagePath(char* imagePath, const SPConfig config,int index){
    SP_CONFIG_MSG ret;
    if (!config || !imagePath){
        ret = SP_CONFIG_INVALID_ARGUMENT;
    }else if (index>=config->spNumOfImages){
        ret = SP_CONFIG_INDEX_OUT_OF_RANGE;
    }else{
        sprintf(imagePath, "%s%s%i%s",config->spImagesDirectory,
        		config->spImagesPrefix, index, config->spImagesSuffix);
        ret = SP_CONFIG_SUCCESS;
    }
    return ret;
}

SP_CONFIG_MSG spConfigGetSavePath(char* path, const SPConfig config, int index){
    SP_CONFIG_MSG ret;
    if (!config || !path){
        ret = SP_CONFIG_INVALID_ARGUMENT;
        spLoggerPrintError(INVALID_VALUE, __FILE__, __FUNCTION__, __LINE__);
    }else if (index>=config->spNumOfImages){
        ret = SP_CONFIG_INDEX_OUT_OF_RANGE;
        spLoggerPrintError(INDEX_OUT_OF_RANGE, __FILE__, __FUNCTION__,
        		__LINE__);
    }else{
        sprintf(path, "%s%s%i%s",config->spImagesDirectory,
        		config->spImagesPrefix, index, FEATURE_EXTENTION);
        ret = SP_CONFIG_SUCCESS;
    }
    return ret;
}

SP_CONFIG_MSG spConfigGetPCAPath(char* pcaPath, const SPConfig config){
    SP_CONFIG_MSG ret;
    if (!config || !pcaPath){
        ret = SP_CONFIG_INVALID_ARGUMENT;
    }else{
        sprintf(pcaPath, "%s%s",config->spImagesDirectory,
        		config->spPCAFilename);
        ret = SP_CONFIG_SUCCESS;
    }
    return ret;
}


void spConfigDestroy(SPConfig config, SPLogger logger){
    if (config){
        free(config->spImagesDirectory);
        free(config->spImagesPrefix);
        free(config->spImagesSuffix);
        free(config->spPCAFilename);
        free(config->spLoggerFilename);
        free(config);
    }
    if(logger){
    	free(logger);
    }
}

bool spConfigGetConfigFile(int argc, const char * argv[],
		char *filename, SP_CONFIG_MSG *msg){
    bool res = false ;
    if (argc == 1){
        strcpy(filename, DEFAULT_CONFIG);
    }else if(argc == 3 && !strcmp(argv[1],"-c")){
        strcpy(filename, argv[2]);
    }else{
        res = true;
        *msg = SP_CONFIG_INVALID_ARGUMENT;
        spConfigPrintCliError(filename, SP_CONFIG_INVALID_ARGUMENT);
    }
    return res;
}


KDTreeSplitMethod spConfigGetSplitMethod(SPConfig config, SP_CONFIG_MSG *msg){
    assert(msg != NULL);
    assert(config);
    *msg = SP_CONFIG_SUCCESS;
    return config->spKDTreeSplitMethod;
}

int spConfigGetKNN(SPConfig config, SP_CONFIG_MSG *msg){
    assert(msg != NULL);
    assert(config != NULL);
    *msg = SP_CONFIG_SUCCESS;
    return config->spKNN;
}

int spConfigGetNumOfSimilarImages(SPConfig config, SP_CONFIG_MSG *msg){
    assert(msg != NULL);
    assert(config != NULL);
    *msg = SP_CONFIG_SUCCESS;
    return config->spNumOfSimilarImages;
}

