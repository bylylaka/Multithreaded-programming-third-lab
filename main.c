#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <math.h>

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
    int Size;
    int *Data;
} TMessage;

TMessage readStruct() {
    TMessage structure;
    int *type = malloc(1 * sizeof(int));
    int *size = malloc(1 * sizeof(int));

    read(0, type, sizeof(int));
    structure.Type = *type;

    read(0, size, sizeof(int));
    structure.Size = *size;
    structure.Data = malloc(structure.Size * sizeof(int));

    int *buf = malloc((structure.Size + 1) * sizeof(int));
    for (int i = 0; i < structure.Size; i++) {
        read(0, buf, sizeof(int));
        structure.Data += i;
        *structure.Data = *buf;
        structure.Data -= i;
    }

    return structure;
}

void *bubbleSortThread(TMessage *structure) {
    printf("size: %d\n", structure->Size);
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
    printf("\n");
    pthread_mutex_unlock(&writerMutex);
    return 0;
}

void *powThread(TMessage *structure) {
    float result = powf((float) structure->Data[0], (float) structure->Data[1]);
    printf("%d   %d   ->   %.0f\n", structure->Data[0], structure->Data[1], result);
    return 0;
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
    return 0;
}

void *reader(void *args) {
    pthread_t threads[100];  //TODO: realloc
    TMessage structures[100]; //TODO: realloc
    int threadsCount = 0;
    int status = 0;
    pthread_t tt;
    int flag = 0;
    while (flag == 0) {
        structures[threadsCount] = readStruct();

        switch (structures[threadsCount].Type) {
            case FIBONACCI:
                pthread_create(&threads[threadsCount], NULL, fibonacciThread, structures[threadsCount].Data[0]);
                threadsCount++;
                break;
            case POW:
                status = pthread_create(&threads[threadsCount], NULL, powThread, (void *) &structures[threadsCount]);
                if (status != 0) {
                    printf("error: %d\n", status);
                }
                threadsCount++;
                break;
            case BUBBLE_SORT_UINT64:
                pthread_create(&threads[threadsCount], NULL, bubbleSortThread, (void *) &structures[threadsCount]);
                threadsCount++;
                break;
            case STOP:
                flag = 1;
                break;
        }
    }

    for (int i = 0; i < threadsCount; i++) {
        int status = pthread_join(threads[i], NULL);
        if (status != 0) {
            printf("error: %d\n", status);
        }
    }

    return 0;
}

int main() {
    int status = 0;
    pthread_t thread;
    status = pthread_create(&thread, NULL, reader, NULL);
    if (status != 0) {
        printf("main error: can't create main thread 0, status = %d\n", status);
    }

    status = pthread_join(thread, NULL);
    if (status != 0) {
        printf("main error: can't create main thread, status = %d\n", status);
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

