#include "PriorityQueue.h"
#include <stdlib.h>
#include <string.h>

int defaultComparator(const V* a, const V* b) {
    if (a == b) return 0;
    const int termRes = strcmp(a->term, b->term);
    if (termRes != 0) {
        return termRes;
    }
    const int payloadRes = a->payload - b->payload;
    if (payloadRes != 0) {
        return payloadRes;
    }

    return a->score - b->score;
}

V* allocValue(const char* term, const int payload, const int score) {
    V* v = malloc(sizeof(V));
    size_t termLen = strlen(term);
    if (termLen) {
        v->term = malloc(termLen + 1);
        strncpy(v->term, term, termLen);
        v->term[termLen] = '\0';
    }
    v->payload = payload;
    v->score = score;
    return v;
}

void deallocValue(V* v) {
    if (v->term) {
        free(v->term);
        v->term = NULL;
    }
    
    free(v);
}

//****************************************************************************************************************************************************************
//************************ Private static functions START ********************************************************************************************************
static inline void swap(PriorityQueue* pq, int idA, int idB) {
    V* tmp = pq->array[idA];
    pq->array[idA] = pq->array[idB];
    pq->array[idB] = tmp;
}
static inline int parent(int id) {
    if (id == 0) {
        return -1;
    }
    return (id - 1) / 2;
}

static inline int lchild(int id) {
    return id * 2 + 1;
}

static inline int rchild(int id) {
    return id * 2 + 2;
}

static inline bool comparesmall(PriorityQueue *pq, const V* a, const V* b) {
    if (pq->compare(a, b) <= 0) {
        return pq->minHeap;
    } else {
        return !pq->minHeap;
    }
}

static void siftup(PriorityQueue *pq, int id) {
    while (parent(id) > -1) {
        int parentId = parent(id);
        if (comparesmall(pq, pq->array[parentId], pq->array[id])) {
            break;
        } else {
            swap(pq, id, parentId);
        }
        id = parentId;
    }
}

static void siftdown(PriorityQueue *pq, int id) {
    while (lchild(id) < pq->size) {
        int leftId = lchild(id);
        int rightId = rchild(id);
        int child;
        if (rightId >= pq->size || (comparesmall(pq, pq->array[leftId], pq->array[rightId]))) {
            child = leftId;
        } else {
            child = rightId;
        }
        if (comparesmall(pq, pq->array[id], pq->array[child])) {
            break;
        } else {
            swap(pq, id, child);
        }
        id = child;
    }
}

static void expand(PriorityQueue *pq) {
    const int newCapacity = pq->capacity << 1;
    pq->array = realloc(pq->array, newCapacity * sizeof(V*));
    pq->capacity = newCapacity;
}

static void shrink(PriorityQueue *pq) {
    const int newCapacity = pq->capacity >> 1;
    if (newCapacity < MIN_CAPACITY) {
        return;
    }
    pq->array = realloc(pq->array, newCapacity * sizeof(V*));
    pq->capacity = newCapacity;
}
//************************ Private static functions END **********************************************************************************************************
//****************************************************************************************************************************************************************

PriorityQueue* pq_init(bool minHeap, int initCapacity, Comparator fn) {
    PriorityQueue* pq = malloc(sizeof(PriorityQueue));
    pq->size = 0;
    pq->minHeap = minHeap;
    pq->capacity = initCapacity; // Initial capacity. could be expanded on demand.
    pq->array = malloc(pq->capacity * sizeof(V*));
    pq->compare = fn;
    
    return pq;
}

int pq_size(PriorityQueue* pq) {
    return pq->size;
}
bool pq_empty(PriorityQueue* pq) {
    return pq->size == 0;
}
V* pq_top(PriorityQueue* pq) {
    return pq->array[0];
}
V* pq_pop(PriorityQueue* pq) {
    V* top = pq->array[0];
    swap(pq, 0, pq->size - 1);
    pq->array[--pq->size] = NULL;
    if (pq->size > 0) {
        siftdown(pq, 0);
    }
    
    if (pq->size * 2 <= pq->capacity) {
        shrink(pq);
    }
    
    return top;
}

void pq_push(PriorityQueue* pq, V* v) {
    if (pq->size >= pq->capacity) {
        expand(pq);
    }
    
    pq->array[pq->size++] = v;
    siftup(pq, pq->size - 1);
}

void pq_delete(PriorityQueue* pq, V* v) {
    int id = -1;
    for (int i = 0; i < pq->size; i++) {
        if (pq->compare(v, pq->array[i]) == 0) {
            id = i;
            break;
        }
    }

    if (id != -1) { // found match
        swap(pq, id, pq->size - 1);
        deallocValue(pq->array[pq->size - 1]);
        pq->array[--pq->size] = NULL;
        if (pq->size > id) {
            siftup(pq, id);
            siftdown(pq, id);
        }
    }
}

// Walk through all values in the pq.
void pq_iterate(PriorityQueue* pq, IterCallback fn, void * ctx) {
    for (int i = 0; i < pq->size; i++) {
        if (!fn(pq->array[i], ctx)) {
            break;
        }
    }
}

void pq_dispose(PriorityQueue* pq) {
    for (int i = 0; i < pq->size; i++) {
        if (pq->array[i]) {
            deallocValue(pq->array[i]);
            pq->array[i] = NULL;
        }
    }
    pq->size = 0;
    if (pq->array) {
        free(pq->array);
        pq->capacity = 0;
    }
    free(pq);
}

