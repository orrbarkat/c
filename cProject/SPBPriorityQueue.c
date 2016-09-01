#include "SPBPriorityQueue.h"



struct sp_bp_queue_t{
	SPList list;
	int bound;
};

SPBPQueue spBPQueueCreate(int maxSize){
	assert(maxSize>0);
	SPBPQueue queue = (SPBPQueue)malloc(sizeof(*queue));
	if (queue == NULL) {
		return NULL;
	} else {
		queue->list = spListCreate();
		queue->bound = maxSize;
	}
	return queue;
}

SPBPQueue spBPQueueCopy(SPBPQueue source){
	assert(source);
	SPBPQueue copy = spBPQueueCreate(source->bound);
	if (copy == NULL){
		return NULL;
	}
	spListDestroy(copy->list);
	copy->list = spListCopy(source->list);
	return copy;
}

void spBPQueueDestroy(SPBPQueue source){
	if(source != NULL){
		spListDestroy(source->list);
		free(source);
	}
	return;
}

void spBPQueueClear(SPBPQueue source){
	assert(source);
	spListClear(source->list);
}

int spBPQueueSize(SPBPQueue source){
	assert(source);
	return spListGetSize(source->list);
}

int spBPQueueGetMaxSize(SPBPQueue source){
	assert(source);
	return source->bound;
}



SPListElement spBPQueuePeek(SPBPQueue source){
	assert(source);
	if(spBPQueueIsEmpty(source)){
		return NULL;
	}
	SPListElement ret = spListGetFirst(source->list);
	if (!ret){ return NULL;}
	return spListElementCopy(ret);
}

SPListElement spBPQueuePeekLast(SPBPQueue source){
	assert(source);
		if(spBPQueueIsEmpty(source)){
			return NULL;
		}
		SPListElement ret = spListGetLast(source->list);
		if (!ret){ return NULL;}
		return spListElementCopy(ret);
}

double spBPQueueMinValue(SPBPQueue source){
	assert(source);
	if (spBPQueueIsEmpty(source)){
		return -1;
	}
	return spListElementGetValue(spListGetFirst(source->list));
}

double spBPQueueMaxValue(SPBPQueue source){
	assert(source);
	if (spBPQueueIsEmpty(source)){
		return -1;
	}
	return spListElementGetValue(spListGetLast(source->list));
}

bool spBPQueueIsEmpty(SPBPQueue source){
	assert(source);
	if (spListGetSize(source->list)<1){
		return true;
	}
	return false;
}

bool spBPQueueIsFull(SPBPQueue source){
	assert(source);
	if (spListGetSize(source->list) == spBPQueueGetMaxSize(source)){
		return true;
	}
	return false;
}



SP_BPQUEUE_MSG spBPQueueEnqueue(SPBPQueue source, SPListElement element){

	// variables declaration//
	SPList source_list = source->list;
	SPListElement elem_to_comp;
	int list_size = spListGetSize(source_list);

	// check for bad arguments or allocation problems//
	if (!source || !element || !source_list){
		return SP_BPQUEUE_INVALID_ARGUMENT;
	}


	// insertion process//
	if (spBPQueueIsEmpty(source)){
		spListInsertFirst(source_list, element);
		return SP_BPQUEUE_SUCCESS;
	}
	else{
		elem_to_comp = spListGetFirst(source_list);

		/*starts from the bottom and loop for next source list element up until
		you find the one which is bigger than the inserted one and insert the element
		before that one. you will stop after you ran out of elements to look for */
        
        for(int i=0; i<list_size; i++){
            if (spListElementGetValue(element) <= spListElementGetValue(elem_to_comp)){// element is smaller than next elem, insert before current
                if (spBPQueueIsFull(source)){
                    if (spListInsertBeforeCurrent(source_list, element) == SP_LIST_OUT_OF_MEMORY){
                        return SP_BPQUEUE_OUT_OF_MEMORY;
                    }
                    spListGetLast(source_list);
                    spListRemoveCurrent(source_list);
                    return SP_BPQUEUE_SUCCESS;
                }
                else{
                    if (spListInsertBeforeCurrent(source_list, element) == SP_LIST_OUT_OF_MEMORY){
                        return SP_BPQUEUE_OUT_OF_MEMORY;
                    }
                    return SP_BPQUEUE_SUCCESS;
                }
            }else{
                elem_to_comp = spListGetNext(source_list);
            }
        }
    }
    if (!spBPQueueIsFull(source)){//queue isn't full but this item is the biggest
        if (spListInsertLast(source_list, element)== SP_LIST_OUT_OF_MEMORY){
            return SP_BPQUEUE_OUT_OF_MEMORY;
        }
        return SP_BPQUEUE_SUCCESS;
    }
    return SP_BPQUEUE_SUCCESS; // the element is larger than all others
}

SP_BPQUEUE_MSG spBPQueueDequeue(SPBPQueue source){
    
    // check for bad arguments//
    if(!source || !source->list){
        return SP_BPQUEUE_INVALID_ARGUMENT;
    }
	
    // variables declaration//
	SPList source_list = source->list;

	if (spBPQueueIsEmpty(source)){
		return SP_BPQUEUE_EMPTY;
	}
	else{
		spListGetFirst(source_list);
		spListRemoveCurrent(source_list);
		return SP_BPQUEUE_SUCCESS;
	}
}



