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



SPConfig spConfigCreate(const char* filename, SP_CONFIG_MSG* msg){
    FILE *fp;
    char line[LINE_LENGTH];
    SPConfig config;
    
    assert(msg != NULL);
    if (!filename){
        *msg = SP_CONFIG_INVALID_ARGUMENT;
        return NULL;
    }
    
    fp = fopen(filename, "r");
    if (!fp){
        *msg = SP_CONFIG_CANNOT_OPEN_FILE;
        return NULL;
    }
    config = (SPConfig)malloc(sizeof(config));
    if (!config){
        *msg = SP_CONFIG_ALLOC_FAIL;
        return NULL;
    }
    setDefaults(&config);
    while (fgets(line, LINE_LENGTH, fp)){
        if ( parseLine(&config, line, msg)<0){
            spConfigDestroy(config);
            return NULL;
        }
    }
    if (invalid(config,msg)){
        spConfigDestroy(config);
        return NULL;
    }
    return config;
}

static void setDefaults(SPConfig* config){
    if (!config){
        return;
    }
    (*config)->spPCADimension = 20;
    (*config)->spPCAFilename = "pca.yml";
    (*config)->spNumOfFeatures = 100;
    (*config)->spExtractionMode = true;
    (*config)->spMinimalGUI = false;
    (*config)->spKNN = 1;
    (*config)->spKDTreeSplitMethod = MAX_SPREAD;
    (*config)->spLoggerLevel = 3;
    (*config)->spLoggerFilename = "stdout";
    return;
}

static int parseLine(SPConfig* config, const char* line, SP_CONFIG_MSG* msg){
    int i=0, var_index=0, value_index=0, lineLen=strlen(line);
    bool not_finished = true, hasSpaces=false;
    char variable[lineLen],value[lineLen];
    if (!config || !line){
        return -1;
    }
    while(not_finished && (i< lineLen)){//fill the variable part
        switch (line[i]) {
            case '#':
                return 1;
                break;
            case '=':
                not_finished= false;
                i++;
                break;
            case ' ':
                if(var_index){// variable is not empty by now
                    hasSpaces = true;
                    i++;
                    break;
            default:
                if(hasSpaces){//there is a space between some characters
                    *msg = SP_CONFIG_INVALID_STRING;
                    return -1;
                }
                variable[var_index] = line[i];
                var_index++;
                i++;
        }
    }
    
    //TODO: might need to trim the length of variable?
    not_finished = true;
    
    while(not_finished && (i< lineLen)){// fill the value part of the config line
        switch (line[i]) {
            case '#':
                not_finished = false;
                break;
            case ' ':
                if (value_index){//value is not empty
                    hasSpaces = true;
                }
                i++;
                break;
            default:
                if(hasSpaces){//there is a space between some characters
                    *msg = SP_CONFIG_INVALID_STRING;
                    return -1;
                }
                value[value_index] = line[i];
                value_index++;
                i++;
        }
        
    }
    
    // find the right config var and insert the value if it's valid
    if (!(var_index && value_index)){// var or val are empty
        *msg = SP_CONFIG_INVALID_STRING;
        return -1;
    }else if(strcmp(variable,"spImagesDirectory")==0){
        (*config)->spImagesDirectory = (char*)malloc(value_index*sizeof(char));
        if (!(*config)->spImagesDirectory){
            *msg = SP_CONFIG_ALLOC_FAIL;
            return -1;
        }
        strcpy((*config)->spImagesDirectory, value);
        
        
    }else if(strcmp(variable,"spImagesPrefix")==0){
        (*config)->spImagesPrefix = (char*)malloc(value_index*sizeof(char));
        if (!(*config)->spImagesPrefix){
            *msg = SP_CONFIG_ALLOC_FAIL;
            return -1;
        }
        strcpy((*config)->spImagesPrefix, value);
        
        
    }else if(strcmp(variable,"spImagesSuffix")==0){
        (*config)->spImagesSuffix = (char*)malloc(value_index*sizeof(char));
        if (!(*config)->spImagesSuffix){
            *msg = SP_CONFIG_ALLOC_FAIL;
            return -1;
        }
        strcpy((*config)->spImagesSuffix, value);
        
        
    }else if(strcmp(variable,"spPCAFilename")==0){
        (*config)->spPCAFilename = (char*)realloc((*config)->spPCAFilename,value_index*sizeof(char));
        if (!(*config)->spPCAFilename){
            *msg = SP_CONFIG_ALLOC_FAIL;
            return -1;
        }
        strcpy((*config)->spPCAFilename, value);
        
        
    }else if(strcmp(variable,"spLoggerFilename")==0){
        (*config)->spLoggerFilename = (char*)realloc((*config)->spLoggerFilename, value_index*sizeof(char));
        if (!(*config)->spLoggerFilename){
            *msg = SP_CONFIG_ALLOC_FAIL;
            return -1;
        }
        strcpy((*config)->spLoggerFilename, value);
        
    }else if(strcmp(variable,"spNumOfImages")==0){
        if(!atoi(value)){//value contains non digit characters
            *msg = SP_CONFIG_INVALID_INTEGER;
            return -1;
        }
        (*config)->spNumOfImages = atoi(value);
        
    }else if(strcmp(variable,"spPCADimension")==0){
        if(!atoi(value)){//value contains non digit characters
            *msg = SP_CONFIG_INVALID_INTEGER;
            return -1;
        }
        (*config)->spPCADimension = atoi(value);
        
    }else if(strcmp(variable,"spNumOfFeatures")==0){
        if(!atoi(value)){//value contains non digit characters
            *msg = SP_CONFIG_INVALID_INTEGER;
            return -1;
        }
        (*config)->spNumOfFeatures = atoi(value);
        
    }else if(strcmp(variable,"spNumOfSimilarImages")==0){
        if(!atoi(value)){//value contains non digit characters
            *msg = SP_CONFIG_INVALID_INTEGER;
            return -1;
        }
        (*config)->spNumOfSimilarImages = atoi(value);

    }else if(strcmp(variable,"spKNN")==0){
        if(!atoi(value)){//value contains non digit characters
            *msg = SP_CONFIG_INVALID_INTEGER;
            return -1;
        }
        (*config)->spNumOfImages = atoi(value);
        
    }else if(strcmp(variable,"spLoggerLevel")==0){
        if(!atoi(value)){//value contains non digit characters
            *msg = SP_CONFIG_INVALID_INTEGER;
            return -1;
        }
        (*config)->spNumOfImages = atoi(value);
        
    }else if(strcmp(variable,"spExtractionMode")==0){
        if(strcmp(value,"true")==0){ (*config)->spExtractionMode = true;
        }else if(strcpy(value,"false")==0){ (*config)->spExtractionMode = false;
        }else{ *msg = SP_CONFIG_INVALID_STRING;}
            
    }else if(strcmp(variable,"spMinimalGUI")==0){
        if(strcmp(value,"true")==0){ (*config)->spMinimalGUI = true;
        }else if(strcmp(value,"false")==0){ (*config)->spMinimalGUI = false;
        }else{
            *msg = SP_CONFIG_INVALID_STRING;
            return -1;
        }
            
    }else if(strcmp(variable,"spKDTreeSplitMethod")==0){
        if(strcmp("RANDOM", value)==0){ (*config)->spKDTreeSplitMethod = RANDOM;
        }else if(strcmp("INCREMENTAL", value)==0){(*config)->spKDTreeSplitMethod = INCREMENTAL;
        }else if (strcmp("MAX_SPREAD", value)==0){
        }else{
            *msg = SP_CONFIG_INVALID_STRING;
            return -1;
        }
        
    }else{//invalid variable name
        *msg = SP_CONFIG_INVALID_STRING;
        return -1;
    }
    return 1;
}

static bool invalid(const SPConfig config, SP_CONFIG_MSG* msg){
    bool validity = true;
    // check that
    if(!(config->spImagesDirectory)){
        *msg = SP_CONFIG_MISSING_DIR;
        validity=false;
    }
    if(!(config->spImagesPrefix)){
        *msg = SP_CONFIG_MISSING_PREFIX;
        validity=false;
    }
    if(!(config->spImagesSuffix)){
        *msg = SP_CONFIG_MISSING_SUFFIX;
        validity=false;
    }
    if(!(config->spNumOfImages)){
        *msg = SP_CONFIG_MISSING_NUM_IMAGES;
        validity = false;
    }
    return validity;
}
