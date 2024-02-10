#ifndef MY_UTILS_H
#define MY_UTILS_H

#include <stdlib.h>
#include <stdio.h>
#include <time.h>

struct Node {
    void* data;
    struct Node* next;
};

typedef struct Node Node;

Node* createNode(void* data) {
    Node* node = (Node*)malloc(sizeof(Node));
    node->data = data;
    node->next = NULL;
    return node;
}

struct Queue {
    Node* first;
    Node* last;
    int size;
};

typedef struct Queue Queue;

Queue* createQueue() {
    Queue* queue = (Queue*)malloc(sizeof(Queue));
    queue->first = NULL;
    queue->last = NULL;
    queue->size = 0;
    return queue;
}

int isQueueEmpty(Queue* queue) {
    return (queue->size == 0);
}

void enqueue(Queue* queue, void* data) {
    Node* newNode = createNode(data);
    if (isQueueEmpty(queue)) {
        queue->first = queue->last = newNode;
        queue->size++;
        return;
    }
    queue->size++;
    queue->last->next = newNode;
    queue->last = newNode;
}

void* dequeue(Queue* queue) {
    if (isQueueEmpty(queue)) {
        printf("cannot dequeue from an empty queue\n");
        exit(1);
    }
    void* data = queue->first->data;
    Node* temp = queue->first;
    queue->first = queue->first->next;
    if (queue->first == NULL)
        queue->last = NULL;
    free(temp);
    queue->size--;
    return data;
}

void appendQueue(Queue* queue1, Queue* queue2) {
    if (isQueueEmpty(queue2))
        return;
    if (isQueueEmpty(queue1))
        queue1->first = queue2->first;
    else
        queue1->last->next = queue2->first;
    queue1->last = queue2->last;
    queue1->size += queue2->size;
    queue2->first = NULL;
    queue2->last = NULL;
    queue2->size = 0;
}

void printQueue(Queue* queue, void (*printFunction)(void*)) {
    if (isQueueEmpty(queue)) {
        printf("Queue is empty.\n");
        return;
    }
    Node* current = queue->first;
    while (current) {
        printFunction(current->data);
        printf(" ");
        current = current->next;
    }
    printf("\n");
}

void printInt(void* data) {
    printf("%d", *(int*)data);
}

void* removeLastElemFromQueue(Queue* queue) {
    // If the queue is empty, return NULL (you might want to handle this differently)
    if (isQueueEmpty(queue)) {
        printf("Queue is empty. Cannot dequeue last element.\n");
        return NULL;
    }
    if (queue->first == queue->last)
        return dequeue(queue);
    struct Node* current = queue->first;
    while (current->next->next != NULL)
        current = current->next;
    void* data = current->next->data;
    free(current->next);
    current->next = NULL;
    queue->last = current;
    queue->size = queue->size-1;
    return data;
}

void freeQueue(Queue* queue, void (*freeFunction)(void*)) {
    while (!isQueueEmpty(queue)) {
        void* data = dequeue(queue);
        freeFunction(data);
    }
    free(queue);
}

void freeEmptyQueue(Queue* queue) {
    if (!isQueueEmpty(queue)) {
        printf("queue should be empty, but is not\n");
        exit(1);
    }
    free(queue);
}

int generateRandomInt(int min, int max) {
    return min + rand() % (max - min + 1);
}

void printArrayOfInts(int arr[], int size) {
    for (int i = 0; i < size; i++)
        printf("%d ", arr[i]);
    printf("\n");
}

void printArrayOfFloats(float arr[], int size) {
    for (int i = 0; i < size; i++)
        printf("%.3f ", arr[i]);
    printf("\n");
}

struct Edge {
    int from;
    int to;
};

typedef struct Edge Edge;

Edge* createEdge(int from, int to) {
    Edge* edge = (Edge*)malloc(sizeof(Edge));
    edge->from = from;
    edge->to = to;
    return edge;
}

void printEdge(void* edge) {
    Edge* e = (Edge*)edge;
    printf("(%d-%d)", e->from, e->to);
}

// pair

struct Pair {
    void* _left;
    void* _right;
};

typedef struct Pair Pair;

Pair* createPair(void* left, void* right) {
    Pair* pair = (Pair*)malloc(sizeof(Pair));
    pair->_left = left;
    pair->_right = right;
    return pair;
}

void* getLeftFromPair(Pair* pair) {
    void* left = pair->_left;
    pair->_left = NULL;
    return left;
}

void* getRightFromPair(Pair* pair) {
    void* right = pair->_right;
    pair->_right = NULL;
    return right;
}

void freePair(Pair* pair) {
    if (pair->_left != NULL) {
        printf("error: trying to free a not empty pair\n");
        exit(1);
    }
    if (pair->_right != NULL) {
        printf("error: trying to free a not empty pair\n");
        exit(1);
    }
    free(pair);
}

// tuple

struct Tuple {
    void** _elements;
    int size;
};

typedef struct Tuple Tuple;

Tuple* createTuple(int size) {
    Tuple* tuple = (Tuple*)malloc(sizeof(Tuple));
    tuple->size = size;
    tuple->_elements = (void**)calloc(size, sizeof(void*));
    return tuple;
}

void setElementInTuple(Tuple* tuple, int index, void* element) {
    if (tuple->_elements[index] != NULL) {
        printf("tuple already has an element in that position\n");
        exit(1);
    }
    tuple->_elements[index] = element;
}

void* getElementFromTuple(Tuple* tuple, int index) {
    void* element = tuple->_elements[index];
    tuple->_elements[index] = NULL;
    return element;
}

void freeTuple(Tuple* tuple) {
    for (int i=0; i<tuple->size; i++) {
        if (tuple->_elements[i] != NULL) {
            printf("tuple is not empty, must be empty to be freed\n");
            exit(1);
        }
    }
    free(tuple->_elements);
    free(tuple);
}

// square matrix

void printMatrixOfInts(int** matrix, int size) {
    for (int i=0; i<size; i++)
        printArrayOfInts(matrix[i], size);
}

void printMatrixOfFloats(float** matrix, int size) {
    for (int i=0; i<size; i++)
        printArrayOfFloats(matrix[i], size);
}

void freeSquareMatrixOfInts(int** matrix, int size) {
    for (int i=0; i<size; i++)
        free(matrix[i]);
    free(matrix);
}

void freeSquareMatrixOfFloats(float** matrix, int size) {
    for (int i=0; i<size; i++)
        free(matrix[i]);
    free(matrix);
}

int areFloatMatricesAreEqual(float** matrix1, float** matrix2, int size) {
    for (int i=0; i<size; i++)
        for (int j=0; j<size; j++)
            if (matrix1[i][j] != matrix2[i][j])
                return 0;
    return 1;
}

// timer

struct Timer {
    clock_t startTime;
    clock_t endTime;
};

typedef struct Timer Timer;

void startTimer(Timer* timer) {
    timer->startTime = clock();
}

void stopTimer(Timer* timer) {
    timer->endTime = clock();
}

double elapsedTime(Timer* timer) {
    // time in milliseconds
    double time = ((double)(timer->endTime - timer->startTime) / CLOCKS_PER_SEC) * 1000.0;
    return time;
}

#endif