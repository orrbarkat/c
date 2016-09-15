#include "SPLogger.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

// Global variable holding the logger
SPLogger logger = NULL;

struct sp_logger_t {
	FILE* outputChannel; //The logger file
	bool isStdOut; //Indicates if the logger is stdout
	SP_LOGGER_LEVEL level; //Indicates the level
};

SP_LOGGER_MSG spLoggerCreate(const char* filename, SP_LOGGER_LEVEL level) {
	if (logger != NULL) { //Already defined
		return SP_LOGGER_DEFINED;
	}
	logger = (SPLogger) malloc(sizeof(*logger));
	if (logger == NULL) { //Allocation failure
		return SP_LOGGER_OUT_OF_MEMORY;
	}
	logger->level = level; //Set the level of the logger
	if (filename == NULL || (!strcmp(filename,""))) { //In case the filename is not set use stdout
		logger->outputChannel = stdout;
		logger->isStdOut = true;
	} else { //Otherwise open the file in write mode
		logger->outputChannel = fopen(filename, SP_LOGGER_OPEN_MODE);
		if (logger->outputChannel == NULL) { //Open failed
			free(logger);
			logger = NULL;
			return SP_LOGGER_CANNOT_OPEN_FILE;
		}
		logger->isStdOut = false;
	}
	return SP_LOGGER_SUCCESS;
}

void spLoggerDestroy() {
	if (!logger) {
		return;
	}
	if (!logger->isStdOut) {//Close file only if not stdout
		fclose(logger->outputChannel);
	}
	free(logger);//free allocation
	logger = NULL;
}

SP_LOGGER_MSG spLoggerPrintBase(const char* msg, const char* file, const char* function, const int line){
	if (!logger) { //Undefined logger
		return SP_LOGGER_UNDIFINED;
	}
	if (!msg || !file || !function || line < 0){ // check params
		return SP_LOGGER_INVAlID_ARGUMENT;
	}
	return SP_LOGGER_SUCCESS;
}

SP_LOGGER_MSG spLoggerPrintError(const char* msg, const char* file, const char* function, const int line){
	SP_LOGGER_MSG ret = spLoggerPrintBase(msg, file,function, line);
	if (ret!= SP_LOGGER_SUCCESS){ return ret; }
	FILE* fp;
	fp = logger->outputChannel;
	int counter = fprintf(fp,"%s%s%s\n%s%s\n%s%d\n%s%s\n",ERROR_HEADER, FILE_NAME ,file ,FUNCTION_NAME ,function ,LINE_NUM, line, MSG_BODY, msg);
	if (counter < 0){
		return SP_LOGGER_WRITE_FAIL;
	}
	return SP_LOGGER_SUCCESS;
}

SP_LOGGER_MSG spLoggerPrintWarning(const char* msg, const char* file, const char* function, const int line){
	SP_LOGGER_MSG ret = spLoggerPrintBase(msg, file,function, line);
	if (ret!= SP_LOGGER_SUCCESS){ return ret; }
	if (logger->level != SP_LOGGER_ERROR_LEVEL){
		FILE* fp;
		fp = logger->outputChannel;
		int counter = fprintf(fp,"%s%s%s\n%s%s\n%s%d\n%s%s\n",WARN_HEADER, FILE_NAME ,file ,FUNCTION_NAME ,function ,LINE_NUM, line, MSG_BODY, msg);
		if (counter < 0){
			return SP_LOGGER_WRITE_FAIL;
		}
		return SP_LOGGER_SUCCESS;
	}
	return SP_LOGGER_SUCCESS;
}

SP_LOGGER_MSG spLoggerPrintInfo(const char* msg){
	if (!logger) { //Undefined logger
			return SP_LOGGER_UNDIFINED;
	}
	if (!msg){
		return SP_LOGGER_INVAlID_ARGUMENT;
	}
	if ((logger->level != SP_LOGGER_ERROR_LEVEL) && (logger->level != SP_LOGGER_WARNING_ERROR_LEVEL)){
		FILE* fp;
		fp = logger->outputChannel;
		int counter = fprintf(fp,"%s%s%s\n",INFO_HEADER, MSG_BODY, msg);
		if (counter < 0){
			return SP_LOGGER_WRITE_FAIL;
		}
		return SP_LOGGER_SUCCESS;
	}
	return SP_LOGGER_SUCCESS;
}

SP_LOGGER_MSG spLoggerPrintDebug(const char* msg, const char* file, const char* function, const int line){
	SP_LOGGER_MSG ret = spLoggerPrintBase(msg, file,function, line);
	if (ret!= SP_LOGGER_SUCCESS){ return ret; }
	if (logger->level == SP_LOGGER_DEBUG_INFO_WARNING_ERROR_LEVEL){
		FILE* fp;
		fp = logger->outputChannel;
		int counter = fprintf(fp,"%s%s%s\n%s%s\n%s%d\n%s%s\n",DEBUG_HEADER, FILE_NAME ,file ,FUNCTION_NAME ,function ,LINE_NUM, line, MSG_BODY, msg);
		if (counter < 0){
			return SP_LOGGER_WRITE_FAIL;
		}
		return SP_LOGGER_SUCCESS;
	}
	return SP_LOGGER_SUCCESS;
}

SP_LOGGER_MSG spLoggerPrintMsg(const char* msg){
    if (!logger) { //Undefined logger
        return SP_LOGGER_UNDIFINED;
    }
    if (!msg){
        return SP_LOGGER_INVAlID_ARGUMENT;
    }
    FILE* fp;
    fp = logger->outputChannel;
    int counter = fprintf(fp,"%s\n", msg);
    if (counter < 0){
        return SP_LOGGER_WRITE_FAIL;
    }
    return SP_LOGGER_SUCCESS;
}


