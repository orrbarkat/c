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

void spConfigPrintCliError(const char* filename, const SP_CONFIG_MSG message){
    switch (message) {
        case SP_CONFIG_CANNOT_OPEN_FILE:
            strcmp(filename,DEFAULT_CONFIG) ? printf("The configuration file %s couldn’t be open\n"\
            ,filename) : printf("The default configuration file spcbir.config couldn’t be open\n");
            break;
        default:
            printf("Invalid command line : use -c <config_filename>\n");
            break;
    }
}


/**
 * print message to stdout
 */
void spConfigPrintError(const char* filename, int line, const SP_CONFIG_MSG* message){
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
        printf("File: %s\nLine: %i\nMessage: %s\n", filename, line,type);
    }
    return;
}


 
/**
 * sets default values for functions
 */
void setDefaults(SPConfig config, SP_CONFIG_MSG *msg){
    if (!config){ return; }
    //TODO: refactor magic numbers out to header file
    config->spPCADimension = 20;
    config->spNumOfFeatures = 100;
    config->spNumOfImages = -1;
    config->spExtractionMode = true;
    config->spMinimalGUI = false;
    config->spKNN = 1;
    config->spKDTreeSplitMethod = MAX_SPREAD;
    config->spLoggerLevel = 3;
    config->spNumOfSimilarImages = 1;
    config->spImagesDirectory = (char*)malloc(LINE_LENGTH*sizeof(char));
    config->spImagesDirectory = (char*)malloc(LINE_LENGTH*sizeof(char));
    config->spImagesPrefix = (char*)malloc(LINE_LENGTH*sizeof(char));
    config->spImagesSuffix = (char*)malloc(LINE_LENGTH*sizeof(char));
    config->spPCAFilename = (char*)calloc(LINE_LENGTH,sizeof(char));
    config->spLoggerFilename = (char*)calloc( LINE_LENGTH,sizeof(char));
    if (!(config->spImagesDirectory && config->spImagesSuffix && config->spImagesPrefix && config->spPCAFilename && config->spLoggerFilename)){
        *msg = SP_CONFIG_ALLOC_FAIL;
    }
    return;
}

/**
 * checks if suffix is in the correct format
 * @return
 * - true if not valid suffix
 * - false if the suffix is valid
 */
bool suffixValidator(const char *val){
    return (0 != (strcmp(val, ".jpg")*strcmp(val, ".png")*strcmp(val, ".bmp")*strcmp(val, ".gif")));
}

/**
 * Line parser for the config file.
 * if a line is invalid the msg is filled according with the create header
 */
int parseLine(SPConfig* config, const char* line, SP_CONFIG_MSG* msg){
    int lineIndex=0, tempIndex=0, varIndex = 0,res=1, lineLen=(int)strlen(line);
    bool notFinished, hasSpaces, comment = false;
    char temp[lineLen];
    char *variables[2];
    if (!config || !line){
        return -1;
    }
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
                    if(tempIndex){// variable is not empty by now
                        hasSpaces = true;
                    }
                    lineIndex++;
                    break;
                default:
                    if(hasSpaces){//there is a space between some characters
                        *msg = SP_CONFIG_INVALID_STRING;
                        res = -1;
                    }
                    temp[tempIndex] = line[lineIndex];
                    tempIndex++;
                    lineIndex++;
            }
        }

        temp[tempIndex] = '\0';
        variables[varIndex] = (char *)malloc(tempIndex);
        strcpy(variables[varIndex], temp);
    }
 
    // find the right config var and insert the value if it's valid
    if(strcmp(variables[0],"spImagesDirectory")==0){
        strcpy((*config)->spImagesDirectory, variables[1]);
        
    }else if(strcmp(variables[0],"spImagesPrefix")==0){
        strcpy((*config)->spImagesPrefix, variables[1]);
        
    }else if(strcmp(variables[0],"spImagesSuffix")==0){
        if(suffixValidator(variables[1])){ // value is not in .jpg , .png , .bmp , .gif
            *msg = SP_CONFIG_INVALID_STRING;
        }
        strcpy((*config)->spImagesSuffix, variables[1]);
        
    }else if(strcmp(variables[0],"spPCAFilename")==0){
        strcpy((*config)->spPCAFilename, variables[1]);
        
    }else if(strcmp(variables[0],"spLoggerFilename")==0){
        strcpy((*config)->spLoggerFilename, variables[1]);
        
    }else if(strcmp(variables[0],"spNumOfImages")==0){
        if(atoi(variables[1])<=0){//value contains non digit characters
            *msg = SP_CONFIG_INVALID_INTEGER;
        }
        (*config)->spNumOfImages = atoi(variables[1]);
        
    }else if(strcmp(variables[0],"spPCADimension")==0){
//        if((atoi(variables[1])<10) || (atoi(variables[1])>28)){//value contains non digit characters
//            *msg = SP_CONFIG_INVALID_INTEGER;
//        }
        (*config)->spPCADimension = atoi(variables[1]);
        
    }else if(strcmp(variables[0],"spNumOfFeatures")==0){
        if(atoi(variables[1])<=0){//value contains non digit characters
            *msg = SP_CONFIG_INVALID_INTEGER;
        }
        (*config)->spNumOfFeatures = atoi(variables[1]);
        
    }else if(strcmp(variables[0],"spNumOfSimilarImages")==0){
        if(atoi(variables[1])<=0){//value contains non digit characters
            *msg = SP_CONFIG_INVALID_INTEGER;
        }
        (*config)->spNumOfSimilarImages = atoi(variables[1]);
        
    }else if(strcmp(variables[0],"spKNN")==0){
        if(atoi(variables[1])<=0){//value contains non digit characters
            *msg = SP_CONFIG_INVALID_INTEGER;
        }
        (*config)->spKNN = atoi(variables[1]);
        
    }else if(strcmp(variables[0],"spLoggerLevel")==0){
        if(1>atoi(variables[1]) || 4<atoi(variables[1])){//value contains non digit characters or digit are not in [1..4]
            *msg = SP_CONFIG_INVALID_INTEGER;
        }
        
        (*config)->spNumOfImages = atoi(variables[1]);
        
    }else if(strcmp(variables[0],"spExtractionMode")==0){
        if(strcmp(variables[1],"false")==0){
            (*config)->spExtractionMode = false;
        }else if(strcmp(variables[1],"true")==0){
                (*config)->spExtractionMode = true;
        }else{
                *msg = SP_CONFIG_INVALID_STRING;
        }

    }else if(strcmp(variables[0],"spMinimalGUI")==0){
        if(strcmp(variables[1],"true")==0){ (*config)->spMinimalGUI = true;
        }else if(strcmp(variables[1],"false")==0){ (*config)->spMinimalGUI = false;
        }else{
            *msg = SP_CONFIG_INVALID_STRING;
        }
        
    }else if(strcmp(variables[0],"spKDTreeSplitMethod")==0){
        if(strcmp("RANDOM", variables[1])==0){ (*config)->spKDTreeSplitMethod = RANDOM;
        }else if(strcmp("INCREMENTAL", variables[1])==0){(*config)->spKDTreeSplitMethod = INCREMENTAL;
        }else if (strcmp("MAX_SPREAD", variables[1])==0){
        }else{
            *msg = SP_CONFIG_INVALID_STRING;
        }
        
    }else if(strlen(variables[0])){//invalid variable name
            *msg = SP_CONFIG_INVALID_STRING;
    }
    if(*msg ){//got some error on the process
        res =-1;
    }
    free(variables[0]);
    free(variables[1]);
    return res;
}

/**
 * checks if all configuration requirements were met
 * @return
 * - true if not valid conf
 * - false if the conf is valid
 */
bool invalid(const SPConfig config, SP_CONFIG_MSG* msg){
    bool validity = false;
    if(!config->spPCAFilename){
        strcpy(config->spPCAFilename,"pca.yml");
    }
    if(!config->spLoggerFilename){
        free(config->spLoggerFilename);
    }
    // check that
    if(!config->spImagesDirectory){
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



SPConfig spConfigCreate(const char* filename, SP_CONFIG_MSG* msg){
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
    setDefaults(config, msg);
    while (fgets(line, LINE_LENGTH, fp)){
        if ( parseLine(&config, line, msg)<0){
            fclose(fp);
            spConfigPrintError(filename, lineNumber, msg);
            spConfigDestroy(config);
            return NULL;
        }
        lineNumber++;
    }
    fclose(fp);
    if (invalid(config,msg)){
        spConfigPrintError(filename, lineNumber, msg);
        spConfigDestroy(config);
        return NULL;
    }
    spLoggerCreate(config->spLoggerFilename, config->spLoggerLevel);
    *msg = SP_CONFIG_SUCCESS;
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
        sprintf(imagePath, "%s%s%i%s",config->spImagesDirectory, config->spImagesPrefix, index, config->spImagesSuffix);
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
        spLoggerPrintError(INDEX_OUT_OF_RANGE, __FILE__, __FUNCTION__, __LINE__);
    }else{
        sprintf(path, "%s%s%i%s",config->spImagesDirectory, config->spImagesPrefix, index, FEATURE_EXTENTION);
        ret = SP_CONFIG_SUCCESS;
    }
    return ret;
}


SP_CONFIG_MSG spConfigGetPCAPath(char* pcaPath, const SPConfig config){
    SP_CONFIG_MSG ret;
    if (!config || !pcaPath){
        ret = SP_CONFIG_INVALID_ARGUMENT;
    }else{
        sprintf(pcaPath, "%s%s",config->spImagesDirectory, config->spPCAFilename);
        ret = SP_CONFIG_SUCCESS;
    }
    return ret;
}


void spConfigDestroy(SPConfig config){
    if (config){
        free(config->spImagesDirectory);
        free(config->spImagesPrefix);
        free(config->spImagesSuffix);
        free(config->spPCAFilename);
        free(config->spLoggerFilename);
    }
    free(config);
}

bool spConfigGetConfigFile(int argc, const char * argv[], char *filename, SP_CONFIG_MSG *msg){
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

