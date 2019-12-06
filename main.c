#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include "queue.c"

typedef enum {
    FIBONACCI,
    POW,
    BUBBLE_SORT_UINT64,
    STOP
} EType;

typedef struct {
    EType Type;
    uint64_t Size;
    uint64_t *Data;
} TMessage;

TMessage readStruct() {
    TMessage structure;
    int *type = malloc(1 * sizeof(int));
    int *size = malloc(1 * sizeof(int));

    read(0, type, sizeof(int));
    structure.Type = *type;

    read(0, size, sizeof(int));
    structure.Size = *size;
    structure.Data = malloc(*size * sizeof(int));

    int *buf = malloc(*size * sizeof(int));
    for (int i = 0; i < *size; i++) {
        read(0, buf, sizeof(int));
        structure.Data += i;
        *structure.Data = *buf;
        structure.Data -= i;
    }

    return structure;
}

void *reader(void *args) {
    while (1) {
        TMessage structure = readStruct();

        switch (structure.Type) {
            case FIBONACCI:
                printf("FIBONACCI\n");
                break;
            case POW:
                printf("POW\n");
                break;
            case BUBBLE_SORT_UINT64:
                printf("BUBBLE_SORT_UINT64\n");
                break;
            case STOP:
                goto quit;
        }
    }
    quit:
    return 0;
}

int main() {
    tatarTest();
    pthread_t thread;
    pthread_create(&thread, NULL, reader, NULL);
    pthread_join(thread, NULL);
    return 0;
}

//QUEUE
#define QMAX 1000
struct queue {
    int qu[QMAX];
    int last, first;
};

void initQueue(struct queue *q) {
    q->first = 1;
    q->last = 0;
    return;
}

void insertInQueue(struct queue *q, int x) {
    if (q->last < QMAX - 1) {
        q->last++;
        q->qu[q->last] = x;
    }
}

int isemptyQueue(struct queue *q) {
    if (q->last < q->first) return (1);
    else return (0);
}

int removeQueue(struct queue *q) {
    int x, h;
    if (isemptyQueue(q) == 1) {
        return (0);
    }
    x = q->qu[q->first];
    for (h = q->first; h < q->last; h++) {
        q->qu[h] = q->qu[h + 1];
    }
    q->last--;
    return (x);
}

//PRINT STRUCTURE
//    printf("type: %d\n", structure.Type);
//    printf("size: %d\n", structure.Size);
//
//    for (int i = 0; i < structure.Size; i++) {
//        structure.Data += i;
//        printf("element: %d\n", *structure.Data);
//        structure.Data -= i;
//    }

