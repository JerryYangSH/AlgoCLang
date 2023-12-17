#include "PriorityQueue.h"
#include <assert.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>

#define clean_errno() (errno == 0 ? "None" : strerror(errno))
#define log_error(M, ...) fprintf(stderr, "[ERROR] (%s:%d: errno: %s) " M "\n", __FILE__, __LINE__, clean_errno(), ##__VA_ARGS__)
#define assertfTrue(A, M, ...) if(!(A)) {log_error(M, ##__VA_ARGS__); assert(A); }
#define assertfEqual(A, B, M, ...) if((A) != (B)) {log_error(M, ##__VA_ARGS__); assert((A) == (B)); }

#define INIT_CAPACITY 16

static void testMinheap_basic() {
    PriorityQueue* pq = pq_init(false, INIT_CAPACITY, defaultComparator);
    
    assertfEqual(0, pq_size(pq), "size is expected to be 0");
    V* v1 = allocValue("termbasic", 1, 100);
    
    pq_push(pq, v1);
    assertfEqual(1, pq_size(pq), "size is expected to be 1");
    
    assertfEqual(0, defaultComparator(v1, pq_top(pq)), "top is expected to be v1");
    assertfEqual(0, defaultComparator(v1, pq_pop(pq)), "pop is expected to be v1");
    assertfEqual(0, pq_size(pq), "size is expected to be 0");
    
    deallocValue(v1);
    
    pq_dispose(pq);
}

static void testMinheap_multiple() {
    PriorityQueue* pq = pq_init(true, INIT_CAPACITY, defaultComparator);
    
    assertfEqual(0, pq_size(pq), "size is expected to be 0");
    V* v4 = allocValue("termm4", 4, 400);
    V* v1 = allocValue("termm1", 1, 100);
    V* v3 = allocValue("termm3", 3, 300);
    V* v2 = allocValue("termm2", 2, 200);
    
    pq_push(pq, v4);
    pq_push(pq, v1);
    assertfEqual(2, pq_size(pq), "size is expected to be 2");

    assertfEqual(0, defaultComparator(v1, pq_top(pq)), "top is expected to be v1");
    assertfEqual(0, defaultComparator(v1, pq_pop(pq)), "pop is expected to be v1");
    
    pq_push(pq, v3);
    assertfEqual(0, defaultComparator(v3, pq_top(pq)), "top is expected to be v3");

    
    pq_push(pq, v2);
    assertfEqual(0, defaultComparator(v2, pq_top(pq)), "top is expected to be v2");

    assertfEqual(0, defaultComparator(v2, pq_pop(pq)), "pop is expected to be v2");
    assertfEqual(0, defaultComparator(v3, pq_pop(pq)), "pop is expected to be v3");
    assertfEqual(0, defaultComparator(v4, pq_pop(pq)), "pop is expected to be v4");
    
    
    assertfEqual(0, pq_size(pq), "size is expected to be 0");
    
    deallocValue(v1);
    deallocValue(v2);
    deallocValue(v3);
    deallocValue(v4);
    
    pq_dispose(pq);
}

static void testMinheap_loop() {
    const int count = 10 * 1000;
    PriorityQueue* pq = pq_init(true, INIT_CAPACITY, defaultComparator);
    
    // Insert in order
    V* minItem = allocValue("term", 0, 0);
    
    for (int i = 0; i < count; i++) {
        pq_push(pq, allocValue("term", i, 0));
        V* top = pq_top(pq);
        assertfEqual(0, defaultComparator(minItem, top), "top is expected to be minItem");
    }
    
    // Pop in order
    for (int i = 0; i < count; i++) {
        V* pop = pq_pop(pq);
        assertfEqual(i, pop->payload, "pop payload is expected to be %d, actual %d", i, pop->payload);
        deallocValue(pop);
    }
    
    assertfEqual(0, pq_size(pq), "size is expected to be 0");
    
    deallocValue(minItem);
    
    pq_dispose(pq);
}

static void testMaxheap_delete() {
    PriorityQueue* pq = pq_init(false, INIT_CAPACITY, defaultComparator); // max heap
    pq_push(pq, allocValue("term1", 1, 0));
    pq_push(pq, allocValue("term2", 2, 0));
    pq_push(pq, allocValue("term3", 3, 0));
    pq_push(pq, allocValue("term4", 4, 0));
    
    V* v3copy = allocValue("term3", 3, 0);
    pq_delete(pq, v3copy);
    assertfEqual(3, pq_size(pq), "size is expected to be 3");
    deallocValue(v3copy);
    
    V* v4copy = allocValue("term4", 4, 0);
    pq_delete(pq, v4copy);
    assertfEqual(2, pq_size(pq), "size is expected to be 2");
    deallocValue(v4copy);
    
    V* v2copy = allocValue("term2", 2, 0);
    assertfEqual(0, defaultComparator(v2copy, pq_top(pq)), "top is expected to be v2");
    deallocValue(v2copy);
    
    pq_dispose(pq);
}

static void testMinheap_delete() {
    PriorityQueue* pq = pq_init(true, INIT_CAPACITY, defaultComparator); // min heap
    pq_push(pq, allocValue("term1", 1, 0));
    pq_push(pq, allocValue("term2", 2, 0));
    pq_push(pq, allocValue("term3", 3, 0));
    pq_push(pq, allocValue("term4", 4, 0));
    
    V* v3copy = allocValue("term3", 3, 0);
    pq_delete(pq, v3copy);
    assertfEqual(3, pq_size(pq), "size is expected to be 3");
    deallocValue(v3copy);
    
    V* v2copy = allocValue("term2", 2, 0);
    pq_delete(pq, v2copy);
    assertfEqual(2, pq_size(pq), "size is expected to be 2");
    deallocValue(v2copy);
    
    V* v1copy = allocValue("term1", 1, 0);
    assertfEqual(0, defaultComparator(v1copy, pq_top(pq)), "top is expected to be v1");
    deallocValue(v1copy);
    
    pq_dispose(pq);
}

static bool iterateCountCallback(const V* v, void* ctx) {
    int* count = ctx;
    *count += 1;
    return true;
}

static void _dotest_iterate(bool minHeap) {
    PriorityQueue* pq = pq_init(minHeap, INIT_CAPACITY, defaultComparator); // max heap
    pq_push(pq, allocValue("term1", 1, 0));
    pq_push(pq, allocValue("term2", 2, 0));
    pq_push(pq, allocValue("term3", 3, 0));
    pq_push(pq, allocValue("term4", 4, 0));
    
    int count = 0;
    pq_iterate(pq, iterateCountCallback, &count);
    assertfEqual(count, pq_size(pq), "size(%d) is expected to be %d", count, pq_size(pq));
    
    pq_dispose(pq);
}
static void testheap_iterate() {
    _dotest_iterate(true);
    _dotest_iterate(false);
}

static int qsortComparator(const void* pa, const void* pb) {
    V* a = *(V**)pa;
    V* b = *(V**)pb;
    return defaultComparator(a, b);
}

static void _dotest_random(const bool minHeap, const int count) {
    PriorityQueue* pq = pq_init(minHeap, INIT_CAPACITY, defaultComparator);
    
    V** all = malloc(count * sizeof(V*));
    
    srand(time(NULL));   // Initialization, should only be called once.
    for (int i = 0; i < count; i++) {
        int num = rand();
        char* termbuf = malloc(32);
        int k = snprintf(termbuf, 31, "term%d\n", num);
        termbuf[k] = '\0';
        V* v = allocValue(termbuf, num, i);
        free(termbuf);
        pq_push(pq, v);
        all[i] = v;
    }
    
    int j = 0;
    qsort(all, count, sizeof(V*), qsortComparator);
    while(!pq_empty(pq)) {
        V* pop = pq_pop(pq);
        if (minHeap) {
            assertfEqual(0, defaultComparator(all[j], pop), "pop(%s) is expected to be all[%d](%s) in MinHeap", pop->term, j, all[j]->term);
        } else {
            assertfEqual(0, defaultComparator(all[count-j-1], pop), "pop(%s) is expected to be all[%d](%s) in MaxHeap", pop->term, count-1-j, all[count-1-j]->term);
        }
        deallocValue(pop);
        j++;
    }
    
    
    free(all);
    pq_dispose(pq);
}
static void testHeap_random() {
    _dotest_random(true, 10 * 1000);
    _dotest_random(false, 10 * 1000);
}

static long _dotest_perf(const bool minHeap, const bool reverse, const int count) {
    struct timeval st, et;
    gettimeofday(&st,NULL);
    
    PriorityQueue* pq = pq_init(minHeap, INIT_CAPACITY, defaultComparator);
    for (int i = 0; i < count; i++) {
        const int tag = reverse ? (count-i-1) : i;
        pq_push(pq, allocValue("term", tag, tag));
    }
    pq_dispose(pq);
    
    gettimeofday(&et,NULL);
    const long elapsed = ((et.tv_sec - st.tv_sec) * 1000000) + (et.tv_usec - st.tv_usec);
    
    return elapsed;
}

static void testHeap_perf() {
    const bool heaps[] = {true, false};
    const bool orders[] = {true, false};
    const int counts[] = {100, 1000, 10000, 100000, 1000000, 10000000};
    printf("%-8s %-8s %-8s %-13s %-10s\n", "HEAP", "ORDER", "COUNT", "TOTAL LAT(us)", "AVG LAT(us/1K OPS)");
    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 2; j++) {
            for (int k = 0; k < sizeof(counts) / sizeof(int); k++) {
                long total = _dotest_perf(heaps[i], orders[j], counts[k]);
                printf("%-8s %-8s %8d %12ld  %10ld\n",
                       heaps[i] ? "MinHeap":"MaxHeap",
                       orders[j] ? "REV":"SEQ",
                       counts[k],
                       total,
                       total * 1000/ counts[k]);
            }
        }
    }
}

static void testMinheap_dispose() {
    PriorityQueue* pq = pq_init(false, INIT_CAPACITY, defaultComparator);
    
    assertfEqual(0, pq_size(pq), "size is expected to be 0");
    pq_push(pq, allocValue("term1", 1, 100));
    pq_push(pq, allocValue("term2", 2, 200));
    pq_push(pq, allocValue("term3", 3, 300));

    assertfEqual(3, pq_size(pq), "size is expected to be 3");
    
    // dispose must free the values
    pq_dispose(pq);
}

int main(int argc, char *argv[]) {
    testMinheap_dispose();
    testMinheap_basic();
    testMaxheap_delete();
    testMinheap_delete();
    testheap_iterate();
    testMinheap_multiple();
    testMinheap_loop();
    testHeap_random();
    testHeap_perf();
    
    return 0;
}
