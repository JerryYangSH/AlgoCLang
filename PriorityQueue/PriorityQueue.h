#ifndef PriorityQueue_h
#define PriorityQueue_h

#include <stdbool.h>
#include <string.h>

#define MIN_CAPACITY 4
typedef struct _V {
    char* term;
    int payload;
    int score;
} V;

V* allocValue(const char* term, const int payload, const int score);
void deallocValue(V* v);

typedef int (*Comparator)(const V* a, const V* b);
int defaultComparator(const V* a, const V* b);

// return true if should continue
typedef bool (*IterCallback)(const V* v, void* ctx);

typedef struct _PriorityQueue {
    V** array;
    int capacity;// array capacity
    
    int size; // actual size
    
    bool minHeap;
    Comparator compare;
} PriorityQueue;

PriorityQueue* pq_init(bool minHeap, int initCapacity, Comparator fn);
int pq_size(PriorityQueue* pq);
bool pq_empty(PriorityQueue* pq);
V* pq_top(PriorityQueue* pq);
V* pq_pop(PriorityQueue* pq);
void pq_push(PriorityQueue* pq, V* v);
void pq_delete(PriorityQueue* pq, V* v);
void pq_iterate(PriorityQueue* pq, IterCallback fn, void * ctx);
void pq_dispose(PriorityQueue* pq);

#endif /* PriorityQueue_h */
