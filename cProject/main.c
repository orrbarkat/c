//
//  main.c
//  cProject
//
//  Created by Orr Barkat on 04/08/2016.
//  Copyright © 2016 Orr Barkat. All rights reserved.
//

#include <stdio.h>
#include "SPPoint.h"
#include "SPConfig.h"
#include "KDTree.h"
#include "SPLogger.h"



int main(int argc, const char * argv[]) {
    SP_CONFIG_MSG *msg = (SP_CONFIG_MSG*)malloc(sizeof(*msg));
    char filename[LINE_LENGTH];
    if( spConfigGetConfigFile(argc, argv, filename)){
        free(msg);
        return 0;
    }
    SPConfig conf = spConfigCreate(filename, msg);
    if (conf){
        printf("it works!!!");
    }
    spConfigDestroy(conf);
    printf("works");
    free(msg);
    return 0;
}
