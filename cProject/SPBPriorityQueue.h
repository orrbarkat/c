#ifndef SPBPRIORITYQUEUE_H_
#define SPBPRIORITYQUEUE_H_
#include "SPListElement.h"
#include <stdbool.h>


/**
 * SP Bounded Priority Queue summary
 *
 * Implements a Bounded Priority Queue (From now and on "BPQ") container type.
 * The element of the BPQ are of type SPList, please refer
 * to SPList.h for usage and further information.
 * The BPQ has a fixed upper bound on the number of elements that can be stored
 * in it Whenever a new element is
 * added to the queue, if the queue is at full capacity, the element with the highest
 * priority value is ejected from the queue.
 *
 * The following functions are available:
 * Create			- creates an empty queue with a given maximum capacity
 * Copy				- creates a copy of a given queue
 * Destroy			- frees all memory allocation associated with the queue
 * Clear			- removes all the elements in the queue
 * GetSize			- returns the number of elements in the queue
 * GetMaxSize		- returns the maximum capacity of the queue
 * Enqueue			- Inserts a new copy element to the queue
 * Dequeue			- removes the element with the lowest value
 * Peek				- returns a NEW COPY of the element with the lowest value
 * PeekLast 		- returns a NEW COPY of the element with the highest value
 * MinValue			- returns the minimum value in the queue
 * MaxValue			- returns the maximum value in the queue
 * IsEmpty 			- returns true if the queue is empty
 * IsFull			- returns true if the queue is full
 *
 */


/** type used to define Bounded priority queue **/
typedef struct sp_bp_queue_t* SPBPQueue;

/** type for error reporting **/
typedef enum sp_bp_queue_msg_t {
	SP_BPQUEUE_OUT_OF_MEMORY,
	SP_BPQUEUE_FULL,
	SP_BPQUEUE_EMPTY,
	SP_BPQUEUE_INVALID_ARGUMENT,
	SP_BPQUEUE_SUCCESS
} SP_BPQUEUE_MSG;

/**
 * Allocates a new BPQ.
 *
 * This function creates a new empty BPQ.
 * @param maxSize the new BPQ max capacity.
 * @assert (maxSize > 0)
 * @return
 * NULL - If allocations failed.
 * A new BPQ in case of success.
 */
SPBPQueue spBPQueueCreate(int maxSize);

/**
 * Creates a copy of target BPQ.
 *
 * The new copy will contain a copy of the source list
 *
 * @param source The target BPQ to copy
 * @assert (source!= NULL)
 * @return
 * NULL if a memory allocation failed.
 * A BPQ containing the same list and maxBound otherwise.
 */
SPBPQueue spBPQueueCopy(SPBPQueue source);

/**
 * BPQDestroy: Deallocates an existing BPQ. Destroys the list by using the
 * stored SPList.spListDestroy function.
 *
 * @param source Target BPQ to be deallocated. If source is NULL nothing will be
 * done
 */
void spBPQueueDestroy(SPBPQueue source);

/**
 * Clears the list element from target BPQ using SPList.spListClear .
 *
 * @param source Target BPQ clear its list of elements
 * @assert (source != NULL)
 */
void spBPQueueClear(SPBPQueue source);

/**
 * Returns the number of elements in the source BPQ list using SPList.spListGetSize .
 *
 * @param source The target BPQ which its size is requested.
 * @assert(source != NULL)
 * @return
 * -1 if the source list is NULL.
 * Otherwise the number of elements in the BPQ list attribute.
 */
int spBPQueueSize(SPBPQueue source);

/**
 * Returns the maximum bound capacity of the source BPQ.
 *
 * @param source The target BPQ which its bound is requested.
 * @assert (source != NULL).
 * @return
 * The bound of the BPQ list elements
 */
int spBPQueueGetMaxSize(SPBPQueue source);

/**
 * Adds a new element to the BPQ->list, the new element will be placed in order by size
 * in case of a full list (where the size of the list equals to the BPQ bound), the new element
 * will be inserted iff its value is smaller than the maximum value of the BPQ list elements.
 * in case of full list and insertion, the last BPQ list element will be ejected and destroyed.
 *
 * @param source The target BPQ for which to add an element in its list
 * @param element The element to insert. A copy of the element will be
 * inserted
 * @return
 * SP_BPQUEUE_INVALID_ARGUMENT if a NULL was sent as source, element or the source->list is NULL
 * SP_BPQUEUE_OUT_OF_MEMORY if an allocation failed
 * SP_BPQUEUE_SUCCESS if the process has successfully completed
 */
SP_BPQUEUE_MSG spBPQueueEnqueue(SPBPQueue source, SPListElement element);

/**
 * Removes the BPQ list element with the lowest value
 *
 * @param source The target BPQ for which to remove the lowest value element in its list
 * @return
 * SP_BPQUEUE_INVALID_ARGUMENT if NULL was sent as source or the source->list is NULL
 * SP_BPQUEUE_EMPTY if the source->list has no elements to remove
 * SP_BPQUEUE_SUCCESS if the process has successfully completed
 */
SP_BPQUEUE_MSG spBPQueueDequeue(SPBPQueue source);

/**
 * Returns a new copy of the element with the lowest value.
 *
 * @param source The target BPQ which its lowest value element is requested.
 * @assert (source!=NULL)
 * @return
 * Null if the source->list is empty.
 * Otherwise SPListElement with minimum value
 */
SPListElement spBPQueuePeek(SPBPQueue source);

/**
 * Returns a new copy of the element with the highest value.
 *
 * @param source The target BPQ which its highest value element is requested.
 * @assert (source!=NULL)
 * @return
 * Null if the source->list is empty.
 * Otherwise SPListElement with maximum value
 */
SPListElement spBPQueuePeekLast(SPBPQueue source);

/**
 * Return the minimum value in queue
 *
 * @param source The target BPQ which its minimum value is requested.
 * @assert (source!= NULL)
 * @return
 * -1 if the source->list is empty
 * Otherwise minimum value in the source BPQ.
 */
double spBPQueueMinValue(SPBPQueue source);

/**
 * Return the maximum value in queue
 *
 * @param source The target BPQ which its maximum value is requested.
 * @assert (source!= NULL)
 * @return
 * -1 if the source->list is empty
 * Otherwise maximum value in the source BPQ.
 */
double spBPQueueMaxValue(SPBPQueue source);

/**
 * Returns True if the queue is empty
 *
 * @param source the target BPQ which we want to know if its list is empty.
 * @assert (source!= NULL)
 * @return
 * True if the list has no elements (less than 1 element)
 * Otherwise False
 */
bool spBPQueueIsEmpty(SPBPQueue source);

/**
 * Returns True if the queue is full
 *
 * @param source the target BPQ which we want to know if its list is full.
 * @assert (source!= NULL)
 * @return
 * True if the list has number of elements equal to its bound/
 * Otherwise False
 */
bool spBPQueueIsFull(SPBPQueue source);

#endif
