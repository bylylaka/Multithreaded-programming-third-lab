#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <math.h>
#include <string.h>

pthread_mutex_t queueMutex = PTHREAD_MUTEX_INITIALIZER;

typedef enum {
    FIBONACCI,
    POW,
    BUBBLE_SORT_UINT64,
    STOP
} EType;

typedef struct tMessage {
    EType Type;
    int Size;
    float *Data;
} TMessage;

// QUEUE
#define QMAX 1000
struct WriterQueue {
    TMessage qu[QMAX];
    int last, first;
};

struct WriterQueue writerQueue;

void initQueue() {
    writerQueue.first = 1;
    writerQueue.last = 0;
    return;
}

void insertInQueue(TMessage x) {
    pthread_mutex_lock(queueMutex);
    if (writerQueue.last < QMAX - 1) {
        writerQueue.last++;
        writerQueue.qu[writerQueue.last] = x;
    }
    pthread_mutex_unlock(queueMutex);
}

int isemptyQueue() {
    if (writerQueue.last < writerQueue.first) return (1);
    else return (0);
}

TMessage removeQueue() {
    pthread_mutex_lock(queueMutex);
    TMessage x;
    int h;
    if (isemptyQueue() == 1) {
        TMessage empty = {
                .Size = 0
        };
        return empty;
    }
    x = writerQueue.qu[writerQueue.first];
    for (h = writerQueue.first; h < writerQueue.last; h++) {
        writerQueue.qu[h] = writerQueue.qu[h + 1];
    }
    writerQueue.last--;
    pthread_mutex_unlock(queueMutex);
    return (x);
}
// END QUEUE

TMessage readStruct() {
    TMessage structure;
    int *type = malloc(1 * sizeof(int));
    int *size = malloc(1 * sizeof(int));

    read(0, type, sizeof(int));
    structure.Type = *type;

    read(0, size, sizeof(int));
    structure.Size = *size;
    structure.Data = malloc((structure.Size + 1) * sizeof(int));

    int *buf = malloc((structure.Size + 1) * sizeof(int));
    for (int i = 0; i < structure.Size; i++) {
        read(0, buf, sizeof(int));
        structure.Data += i;
        *structure.Data = (float) *buf;
        structure.Data -= i;
    }

    return structure;
}

void *bubbleSortThread(TMessage *structure) {
    for (int i = 0; i < structure->Size - 1; i++) {
        for (int j = 0; j < structure->Size - i - 1; j++) {
            if (structure->Data[j] > structure->Data[j + 1]) {
                int tmp = structure->Data[j];
                structure->Data[j] = structure->Data[j + 1];
                structure->Data[j + 1] = tmp;
            }
        }
    }
    insertInQueue(*structure);
    return 0;
}

void *powThread(TMessage *structure) {
    float result = powf((float) structure->Data[0], (float) structure->Data[1]);
    structure->Data[2] = result;
    insertInQueue(*structure);
    return 0;
}

int fibonacciCalculator(int n) {
    if (n == 1 || n == 2)
        return 1;
    return fibonacciCalculator(n - 1) + fibonacciCalculator(n - 2);
}

void *fibonacciThread(TMessage *structure) {
    if (structure->Data[0] <= 0) {
        return 0;
    }
    int result = fibonacciCalculator(structure->Data[0]);
    structure->Data[1] = result;
    insertInQueue(*structure);
    return 0;
}

void *writer(void *args) {
    FILE *fd;
    fd = fopen("out.txt", "w");
    if ((int) fd == -1) {
        printf("Cannot open file.\n");
        exit(1);;
    }

    while (1) {
        if (isemptyQueue()) {
            continue;
        }
        TMessage message = removeQueue();

        switch (message.Type) {
            case FIBONACCI:
                fprintf(fd, "Fibonacci from %.0f is %.0f\r\n", message.Data[0], message.Data[1]);
                break;
            case POW:
                fprintf(fd, "%.0f^%.0f=%.0f\r\n", message.Data[0], message.Data[1], message.Data[2]);
                break;
            case BUBBLE_SORT_UINT64:
                fprintf(fd, "bubble sorted array size is %d\r\n", message.Size);
                for (int i = 0; i < message.Size; i++) {
                    fprintf(fd, "e[%d]: %.0f\r\n", i, message.Data[i]);
                }
                break;
            case STOP:
                fclose(fd);
                return 0;
        }
        fprintf(fd, "\n");
    }
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
                pthread_create(&threads[threadsCount], NULL, fibonacciThread, &structures[threadsCount]);
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
                insertInQueue(structures[threadsCount]);
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

int main(int argc, char **argv) {
    if (argc != 3 || strcmp(argv[1], "–strategy") != 0) {
        printf("please, use -strategy as all options.\n");
        exit(1);
    }

    if (strcmp(argv[2], "per_thread")) {
        printf("please, use per_thread as value for -strategy.\n");
        exit(1);
    }

    int status = 0;
    pthread_t readerThread;
    pthread_t writerThread;

    initQueue();

    status = pthread_create(&readerThread, NULL, reader, NULL);
    if (status != 0) {
        printf("main error: can't create reader thread 0, status = %d\n", status);
    }

    status = pthread_create(&writerThread, NULL, writer, NULL);
    if (status != 0) {
        printf("main error: can't create writer thread 0, status = %d\n", status);
    }

    status = pthread_join(readerThread, NULL);
    if (status != 0) {
        printf("main error: can't join reader thread, status = %d\n", status);
    }

    status = pthread_join(writerThread, NULL);
    if (status != 0) {
        printf("main error: can't join writer thread, status = %d\n", status);
    }
    return 0;
}
