#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <math.h>
#include <string.h>

pthread_mutex_t queueMutex = PTHREAD_MUTEX_INITIALIZER;
int wasStopped = 0;
int mkSeconds = 0;
char *strategy;

typedef enum {
    PER_THREAD
} StrategyType;

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

typedef struct metric {
    int Size;
    int64_t *Data;
} Metric;

struct metric metric_reader_fibonacci;
struct metric metric_reader_pow;
struct metric metric_reader_bubble_sort;
struct metric metric_reader_stop;
struct metric metric_writer_fibonacci;
struct metric metric_writer_pow;
struct metric metric_writer_bubble_sort;
struct metric metric_writer_stop;
struct metric metric_fibonacci;
struct metric metric_pow;
struct metric metric_bubble_sort;
struct metric metric_stop;
//END METRICS

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

void makeDiagramm(Metric metric, FILE *fd) {
    for (int i = 0; i < metric.Size - 1; i++) {
        for (int j = 0; j < metric.Size - i - 1; j++) {
            if (metric.Data[j] > metric.Data[j + 1]) {
                int tmp = metric.Data[j];
                metric.Data[j] = metric.Data[j + 1];
                metric.Data[j + 1] = tmp;
            }
        }
    }

    for (int i = metric.Size - 1; i >= 0; i--) {
        float percentel = (100 * (i + 1)) / metric.Size; //TODO: change
//        fprintf(fd, "[%.0f] = %d мс\n", percentel, metric.Data[i]);
        fprintf(fd, "%d\n", metric.Data[i]);
    }
//    fprintf(fd, "[%.0f] = %d мс\n", 0, 0);
    fprintf(fd, "%d\n", 0); //TODO: change
}

void *bubbleSortThread(TMessage *structure) {
    struct timespec mt1, mt2;
    clock_gettime(CLOCK_MONOTONIC, &mt1);

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

    clock_gettime(CLOCK_MONOTONIC, &mt2);
    int64_t measure = (((mt2.tv_sec * 1000000000L) + mt2.tv_nsec) -
                       ((mt1.tv_sec * 1000000000L) + mt1.tv_nsec)) / 1000L;
    metric_bubble_sort.Data[metric_bubble_sort.Size] = measure;
    metric_bubble_sort.Size++;

    return 0;
}

void *powThread(TMessage *structure) {
    struct timespec mt1, mt2;
    clock_gettime(CLOCK_MONOTONIC, &mt1);

    float result = powf((float) structure->Data[0], (float) structure->Data[1]);
    structure->Data[2] = result;
    insertInQueue(*structure);

    clock_gettime(CLOCK_MONOTONIC, &mt2);
    int64_t measure = (((mt2.tv_sec * 1000000000L) + mt2.tv_nsec) -
                       ((mt1.tv_sec * 1000000000L) + mt1.tv_nsec)) / 1000L;
    metric_pow.Data[metric_pow.Size] = measure;
    metric_pow.Size++;

    return 0;
}

int fibonacciCalculator(int n) {
    if (n == 1 || n == 2)
        return 1;
    return fibonacciCalculator(n - 1) + fibonacciCalculator(n - 2);
}

void *fibonacciThread(TMessage *structure) {
    struct timespec mt1, mt2;
    clock_gettime(CLOCK_MONOTONIC, &mt1);

    if (structure->Data[0] <= 0) {
        structure->Data[1] = 0;
    } else {
        int result = fibonacciCalculator(structure->Data[0]);
        structure->Data[1] = result;
    }
    insertInQueue(*structure);

    clock_gettime(CLOCK_MONOTONIC, &mt2);
    int64_t measure = (((mt2.tv_sec * 1000000000L) + mt2.tv_nsec) -
                       ((mt1.tv_sec * 1000000000L) + mt1.tv_nsec)) / 1000L;

    metric_fibonacci.Data[metric_fibonacci.Size] = measure;
    metric_fibonacci.Size++;

    return 0;
}

void *writer(void *args) {
    FILE *fd;
    int64_t measure;
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
        struct timespec mt1, mt2;
        clock_gettime(CLOCK_MONOTONIC, &mt1);

        switch (message.Type) {
            case FIBONACCI:
                fprintf(fd, "Fibonacci from %.0f is %.0f\r\n", message.Data[0], message.Data[1]);

                clock_gettime(CLOCK_MONOTONIC, &mt2);
                measure = (((mt2.tv_sec * 1000000000L) + mt2.tv_nsec) -
                           ((mt1.tv_sec * 1000000000L) + mt1.tv_nsec)) / 1000L;
                metric_writer_fibonacci.Data[metric_writer_fibonacci.Size] = measure;
                metric_writer_fibonacci.Size++;
                break;
            case POW:
                fprintf(fd, "%.0f^%.0f=%.0f\r\n", message.Data[0], message.Data[1], message.Data[2]);

                clock_gettime(CLOCK_MONOTONIC, &mt2);
                measure = (((mt2.tv_sec * 1000000000L) + mt2.tv_nsec) -
                           ((mt1.tv_sec * 1000000000L) + mt1.tv_nsec)) / 1000L;
                metric_writer_pow.Data[metric_writer_pow.Size] = measure;
                metric_writer_pow.Size++;
                break;
            case BUBBLE_SORT_UINT64:
                fprintf(fd, "bubble sorted array size is %d\r\n", message.Size);
                for (int i = 0; i < message.Size; i++) {
                    fprintf(fd, "e[%d]: %.0f\r\n", i, message.Data[i]);
                }

                clock_gettime(CLOCK_MONOTONIC, &mt2);
                measure = (((mt2.tv_sec * 1000000000L) + mt2.tv_nsec) -
                           ((mt1.tv_sec * 1000000000L) + mt1.tv_nsec)) / 1000L;
                metric_writer_bubble_sort.Data[metric_writer_bubble_sort.Size] = measure;
                metric_writer_bubble_sort.Size++;
                break;
            case STOP:
                fclose(fd);
                return 0;
        }
        fprintf(fd, "\n");
    }
}

void printMetric() {
    FILE *fd;
    fd = fopen("metric.txt", "w");
    if ((int) fd == -1) {
        printf("Cannot open file.\n");
        exit(1);;
    }

    fprintf(fd, "Выполнение фибоначчи:\n");
    makeDiagramm(metric_fibonacci, fd);
    fprintf(fd, "\n");
    fprintf(fd, "Выполнение Вычисления в степернь:\n");
    makeDiagramm(metric_pow, fd);
    fprintf(fd, "\n");
    fprintf(fd, "Выполнение пузырьковой сортиковки:\n");
    makeDiagramm(metric_bubble_sort, fd);
    fprintf(fd, "\n");
    fprintf(fd, "Чтение структуры для фибоначчи:\n");
    makeDiagramm(metric_reader_fibonacci, fd);
    fprintf(fd, "\n");
    fprintf(fd, "Чтение структуры для вычисления в степернь:\n");
    makeDiagramm(metric_reader_pow, fd);
    fprintf(fd, "\n");
    fprintf(fd, "Чтение структуры для пузырьковой сортиковки:\n");
    makeDiagramm(metric_reader_bubble_sort, fd);
    fprintf(fd, "\n");
    fprintf(fd, "Запись структуры для фибоначчи:\n");
    makeDiagramm(metric_writer_fibonacci, fd);
    fprintf(fd, "\n");
    fprintf(fd, "Запись структуры для вычисления в степернь:\n");
    makeDiagramm(metric_writer_pow, fd);
    fprintf(fd, "\n");
    fprintf(fd, "Запись структуры для пузырьковой сортиковки:\n");
    makeDiagramm(metric_writer_bubble_sort, fd);

    fclose(fd);
}

void *metricMethod(void *args) {
    do {
        printMetric();
        usleep(mkSeconds);
    } while (wasStopped == 0);
    printMetric();

    return 0;
}

void *reader(void *args) {
    int threadsMaxCount = 100;
    pthread_t *threads = malloc((threadsMaxCount) * sizeof(pthread_t));
    TMessage *structures = malloc((threadsMaxCount) * sizeof(TMessage));
    int threadsCount = 0;
    int status = 0;
    int flag = 0;
    int64_t measure;

    while (flag == 0) {
        if (threadsCount >= threadsMaxCount) {
            threadsMaxCount += 200;
            if ((threads = (pthread_t *) realloc(threads, sizeof(pthread_t) * threadsMaxCount)) == NULL) {
                printf("realloc error");
                exit(1);
            }
            if ((structures = (TMessage *) realloc(structures, sizeof(TMessage) * threadsMaxCount)) == NULL) {
                perror("realloc error");
                exit(1);
            }
        }

        structures[threadsCount] = readStruct();
        struct timespec mt1, mt2;
        clock_gettime(CLOCK_MONOTONIC, &mt1);

        switch (structures[threadsCount].Type) {
            case FIBONACCI:
                pthread_create(&threads[threadsCount], NULL, fibonacciThread, &structures[threadsCount]);
                threadsCount++;

                clock_gettime(CLOCK_MONOTONIC, &mt2);
                measure = (((mt2.tv_sec * 1000000000L) + mt2.tv_nsec) -
                           ((mt1.tv_sec * 1000000000L) + mt1.tv_nsec)) / 1000L;
                metric_reader_fibonacci.Data[metric_reader_fibonacci.Size] = measure;
                metric_reader_fibonacci.Size++;
                break;
            case POW:
                status = pthread_create(&threads[threadsCount], NULL, powThread, (void *) &structures[threadsCount]);
                if (status != 0) {
                    printf("pow create error: %d\n", status);
                }
                threadsCount++;

                clock_gettime(CLOCK_MONOTONIC, &mt2);
                measure = (((mt2.tv_sec * 1000000000L) + mt2.tv_nsec) -
                           ((mt1.tv_sec * 1000000000L) + mt1.tv_nsec)) / 1000L;
                metric_reader_pow.Data[metric_reader_pow.Size] = measure;
                metric_reader_pow.Size++;
                break;
            case BUBBLE_SORT_UINT64:
                pthread_create(&threads[threadsCount], NULL, bubbleSortThread, (void *) &structures[threadsCount]);
                threadsCount++;

                clock_gettime(CLOCK_MONOTONIC, &mt2);
                measure = (((mt2.tv_sec * 1000000000L) + mt2.tv_nsec) -
                           ((mt1.tv_sec * 1000000000L) + mt1.tv_nsec)) / 1000L;
                metric_reader_bubble_sort.Data[metric_reader_bubble_sort.Size] = measure;
                metric_reader_bubble_sort.Size++;
                break;
            case STOP:
                insertInQueue(structures[threadsCount]);
                flag = 1;
                break;
        }
    }

    for (int i = 0; i < threadsCount; i++) {
        status = pthread_join(threads[i], NULL);
        if (status != 0) {
            printf("join error on [%d] thread: %d\n", i, status);
        }
    }

    return 0;
}

void initMetrics() {
    metric_reader_fibonacci.Size = 0;
    metric_reader_pow.Size = 0;
    metric_reader_bubble_sort.Size = 0;
    metric_reader_stop.Size = 0;
    metric_writer_fibonacci.Size = 0;
    metric_writer_pow.Size = 0;
    metric_writer_bubble_sort.Size = 0;
    metric_writer_stop.Size = 0;
    metric_fibonacci.Size = 0;
    metric_pow.Size = 0;
    metric_bubble_sort.Size = 0;
    metric_stop.Size = 0;

    metric_reader_fibonacci.Data = malloc((10000) * sizeof(int)); //TODO: realloc
    metric_reader_pow.Data = malloc((10000) * sizeof(int)); //TODO: realloc
    metric_reader_bubble_sort.Data = malloc((10000) * sizeof(int)); //TODO: realloc
    metric_reader_stop.Data = malloc((10000) * sizeof(int)); //TODO: realloc
    metric_writer_fibonacci.Data = malloc((10000) * sizeof(int)); //TODO: realloc
    metric_writer_pow.Data = malloc((10000) * sizeof(int)); //TODO: realloc
    metric_writer_bubble_sort.Data = malloc((10000) * sizeof(int)); //TODO: realloc
    metric_writer_stop.Data = malloc((10000) * sizeof(int)); //TODO: realloc
    metric_fibonacci.Data = malloc((10000) * sizeof(int)); //TODO: realloc
    metric_pow.Data = malloc((10000) * sizeof(int)); //TODO: realloc
    metric_bubble_sort.Data = malloc((10000) * sizeof(int)); //TODO: realloc
    metric_stop.Data = malloc((10000) * sizeof(int)); //TODO: realloc
}

int main(int argc, char **argv) {
    initMetrics();

// read params
    if (argc != 5) {
        printf("please, define \"strategy\" and \"t\".\n");
        exit(1);
    }

    for (int i = 1; i < argc - 1; i++) {
        if (strcmp(argv[i], "-t") == 0) {
            sscanf(argv[i + 1], "%d", &mkSeconds);
            if (mkSeconds <= 0) {
                printf("timeout can't be less. than 1");
                exit(1);
            }
        }

        if (strcmp(argv[i], "–strategy") == 0) {
            strategy = argv[i + 1];
        }
    }

    if (strcmp(strategy, "per_thread") != 0) {
        printf("please, use per_thread as value for -strategy.\n");
        exit(1);
    }

    if (mkSeconds == 0) {
        printf("please, define \"strategy\" and \"t\".\n");
        exit(1);
    }
//end read params

    int status = 0;
    pthread_t readerThread;
    pthread_t writerThread;
    pthread_t metricThread;

    initQueue();

    status = pthread_create(&metricThread, NULL, metricMethod, NULL);
    if (status != 0) {
        printf("main error: can't create metric thread, status = %d\n", status);
    }

    status = pthread_create(&readerThread, NULL, reader, NULL);
    if (status != 0) {
        printf("main error: can't create reader thread, status = %d\n", status);
    }

    status = pthread_create(&writerThread, NULL, writer, NULL);
    if (status != 0) {
        printf("main error: can't create writer thread, status = %d\n", status);
    }

    status = pthread_join(readerThread, NULL);
    if (status != 0) {
        printf("main error: can't join reader thread, status = %d\n", status);
    }

    status = pthread_join(writerThread, NULL);
    if (status != 0) {
        printf("main error: can't join writer thread, status = %d\n", status);
    }

    wasStopped = 1;

    status = pthread_join(metricThread, NULL);
    if (status != 0) {
        printf("main error: can't join metric thread, status = %d\n", status);
    }
    return 0;
}
