#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <math.h>
#include <errno.h>

pthread_mutex_t queueMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t writerMutex = PTHREAD_MUTEX_INITIALIZER;

typedef enum {
    FIBONACCI,
    POW,
    BUBBLE_SORT_UINT64,
    STOP
} EType;

typedef struct tMessage {
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

void *bubbleSortThread(TMessage *structure) {
    pthread_mutex_unlock(&writerMutex);

    for (int i = 0; i < structure->Size - 1; i++) {
        for (int j = 0; j < structure->Size - i - 1; j++) {
            if (structure->Data[j] > structure->Data[j + 1]) {
                int tmp = structure->Data[j];
                structure->Data[j] = structure->Data[j + 1];
                structure->Data[j + 1] = tmp;
            }
        }
    }

    pthread_mutex_lock(&writerMutex);   // move to writer
    for (int i = 0; i < structure->Size; i++) {
        printf("e[%d]: %d\n", i, structure->Data[i]);
    }
    pthread_mutex_unlock(&writerMutex);
    printf("\n\n\n\n\n\n");

    return 0;
}

void *powThread(void *args) {
    TMessage *structure = args;
    float result = powf((float) structure->Data[0], (float) structure->Data[1]);
    printf("%d   %d   ->   %.0f\n", structure->Data[0], structure->Data[1], result);
}

int fibonacciCalculator(int n) {
    if (n == 1 || n == 2)
        return 1;
    return fibonacciCalculator(n - 1) + fibonacciCalculator(n - 2);
}

void *fibonacciThread(int n) {
    if (n <= 0) {
        return 0;
    }
    int result = fibonacciCalculator(n);
    printf("fib: %d\n", result);
}

void *reader(void *args) {
    pthread_t reader_threads[100];  //TODO: realloc
    int threadsCount = 0;


    while (1) {
        TMessage structure = readStruct();
        int status;

        switch (structure.Type) {
            case FIBONACCI:
//                pthread_create(&thread[threadsCount], NULL, fibonacciThread, structure.Data[0]);
//                threadsCount++;
                break;
                break;
            case POW:
//                pthread_create(&thread[threadsCount], NULL, powThread, &structure);
//                threadsCount++;
                break;
            case BUBBLE_SORT_UINT64: {
//                pthread_create(&reader_threads[threadsCount], NULL, bubbleSortThread,
//                               (void *) &structure);   //threadsCount
//                pthread_join(reader_threads[threadsCount], NULL);
//
//                threadsCount++;
                break;
            }

            case STOP:
                goto quit;
        }
    }
    quit:

    for (int i = 0; i < threadsCount; i++) {
        pthread_join(reader_threads[i], NULL);
    }

    return 0;
}

int main() {
    pthread_t thread;
    pthread_create(&thread, NULL, reader, NULL);

    int status = pthread_join(thread, NULL);
    if (status != 0) {
        printf("main error: can't create thread, status = %d\n", status);
    }
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
    pthread_mutex_lock(queueMutex);
    if (q->last < QMAX - 1) {
        q->last++;
        q->qu[q->last] = x;
    }
    pthread_mutex_unlock(queueMutex);
}

int isemptyQueue(struct queue *q) {
    if (q->last < q->first) return (1);
    else return (0);
}

int removeQueue(struct queue *q) {
    pthread_mutex_lock(queueMutex);
    int x, h;
    if (isemptyQueue(q) == 1) {
        return (0);
    }
    x = q->qu[q->first];
    for (h = q->first; h < q->last; h++) {
        q->qu[h] = q->qu[h + 1];
    }
    q->last--;
    pthread_mutex_unlock(queueMutex);
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

