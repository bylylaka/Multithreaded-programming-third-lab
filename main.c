#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

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

int main() {
    TMessage structure = readStruct();

    printf("type: %d\n", structure.Type);
    printf("size: %d\n", structure.Size);

    for (int i = 0; i < structure.Size; i++) {
        structure.Data += i;
        printf("element: %d\n", *structure.Data);
        structure.Data -= i;
    }

//    int *size = malloc(1 * sizeof(int));
//
//    read(0, size, sizeof(int));
//    printf("%d\n", *size);
//
//    int *buf  = malloc(10 * sizeof(int));
//    for (int i = 0; i < *size; i++) {
//        read(0, buf, sizeof(int));
//        printf("%d\n", *buf);
//    }

    return 0;
}

