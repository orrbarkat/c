#include "SPConfig.h"

struct sp_config_t {
    char* spImagesDirectory;
    char* spImagesPrefix;
    char* spImagesSuffix;
    int spNumOfImages;
    int spPCADimension;
    char* spPCAFilename;
    int spNumOfFeatures;
    bool spExtractionMode;
    int spNumOfSimilarImages;
    KDTreeSplitMethod spKDTreeSplitMethod;
    int spKNN;
    bool spMinimalGUI;
    int spLoggerLevel; // 1 error,2 warning, 3 info, 4 debug
    char* spLoggerFilename;
};



SPConfig spConfigCreate(const char* filename, SP_CONFIG_MSG* msg){
    FILE *fp;
    char line[LINE_LENGTH];
    SPConfig config;
    
    assert(msg != NULL);
    if (!filename){
        *msg = SP_CONFIG_INVALID_ARGUMENT;
    }
    
    fp = fopen(filename, "r");
    if (!fp){
        *msg = SP_CONFIG_INVALID_ARGUMENT;
        return NULL;
    }
    config = (SPConfig)malloc(sizeof(config));
    if (!config){
        *msg = SP_CONFIG_ALLOC_FAIL;
        return NULL;
    }
    setDefaults(&config);
    while (fgets(line, LINE_LENGTH, fp)){
        
    }
    
    
    return config;
}

void setDefaults(SPConfig* config){
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

int parseLine(SPConfig* config, const char* line, char* msg){
    int i = 0, var_index = 0, value_index=0, lineLen=strlen(line);
    bool not_finished = true;
    char variable[lineLen],value[lineLen], temp[lineLen];
    if (!config || !line){
        return -1;
    }
    while(not_finished && (i< lineLen)){
        switch (line[i]) {
            case '#':
                return 0;
                break;
            case '=':
                not_finished= false;
                break;
            default:
                if(line[i]!=' '){
                    variable[var_index] = line[i];
                    var_index++;
                }
                i++;
                break;
        }
    }
    if (!variable){
        *msg = SP_CONFIG_INVALID_STRING;
        return -1;
    }
    switch (variable) {
        case "spImagesDirectory":
            <#statements#>
            break;
        case "spImagesPrefix":
            <#statements#>
            break;
        case "spImagesSuffix":
            <#statements#>
            break;
        case spImagesDirectory:
            <#statements#>
            break;
        case spImagesDirectory:
            <#statements#>
            break;
        case spImagesDirectory:
            <#statements#>
            break;
        case spImagesDirectory:
            <#statements#>
            break;
        case spImagesDirectory:
            <#statements#>
            break;
        case spImagesDirectory:
            <#statements#>
            break;
        default:
            *msg = SP_CONFIG_INVALID_STRING;
            break;
    }
}