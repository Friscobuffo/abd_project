#ifndef MY_GRAPH_H
#define MY_GRAPH_H

#include <stdio.h>
#include <stdlib.h>
#include "my_utils.h"

// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// UNDIRECTED GRAPH
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

struct Graph {
    int numNodes;
    Node** adjacencyList;
    int* degree;
};

typedef struct Graph Graph;

Graph* createGraph(int numNodes) {
    Graph* graph = (Graph*)malloc(sizeof(Graph));
    graph->numNodes = numNodes;
    graph->adjacencyList = (Node**)calloc(numNodes, sizeof(Node*));
    graph->degree = (int*)calloc(numNodes, sizeof(int));
    return graph;
}

// adds edges <from-to> and <to-from>
void addEdge(Graph* graph, int from, int to) {
    int* fromElem = (int*)malloc(sizeof(int));
    *fromElem = from;
    int* toElem = (int*)malloc(sizeof(int));
    *toElem = to;
    
    Node* newNode = createNode(toElem);
    newNode->next = graph->adjacencyList[from];
    graph->adjacencyList[from] = newNode;
    graph->degree[from]++;

    newNode = createNode(fromElem);
    newNode->next = graph->adjacencyList[to];
    graph->adjacencyList[to] = newNode;
    graph->degree[to]++;
}

void printGraph(Graph* graph) {
    for (int i=0; i<graph->numNodes; i++) {
        printf("node: %d neighbors: %d [", i, graph->degree[i]);
        Node* temp = graph->adjacencyList[i];
        while (temp) {
            printf(" %d", *(int*)temp->data);
            temp = temp->next;
        }
        printf(" ]\n");
    }
}

void printGraphWithMapping(Graph* graph, int* mapping) {
    for (int i=0; i<graph->numNodes; i++) {
        int nodeValue = mapping[i];
        printf("node: %d neighbors: %d [", nodeValue, graph->degree[i]);
        Node* temp = graph->adjacencyList[i];
        while (temp) {
            int neighborValue = mapping[*(int*)temp->data];
            printf(" %d", neighborValue);
            temp = temp->next;
        }
        printf(" ]\n");
    }
}

void freeGraph(Graph* graph) {
    for (int i = 0; i < graph->numNodes; i++) {
        Node* current = graph->adjacencyList[i];
        while (current) {
            void* data = current->data;
            free(data);
            Node* next = current->next;
            free(current);
            current = next;
        }
    }
    free(graph->adjacencyList);
    free(graph->degree);
    free(graph);
}

int isEdgeInGraph(Graph* graph, int from, int to) {
    if (graph->degree[from] > graph->degree[to]) {
        int temp = from;
        from = to;
        to = temp;
    }
    Node* node = graph->adjacencyList[from];
    while (node) {
        if (*(int*)node->data == to)
            return 1;
        node = node->next;
    }
    return 0;
}

Graph* createRandomGraph(int numNodes, int numEdges) {
    if (numEdges>(numNodes*(numNodes - 1)/2)) {
        printf("too many edges for an undirected graph\n");
        printf("nodes: %d\n", numNodes);
        printf("edges: %d\n", numEdges);
        printf("max possible edges: %d\n", (numNodes*(numNodes - 1)/2));
        exit(1);
    }
    Graph* graph = createGraph(numNodes);
    int addedEdges = 0;
    while (addedEdges<numEdges) {
        int from = generateRandomInt(0, numNodes-1);
        int to = generateRandomInt(0, numNodes-1);
        while (from == to) {
            to = generateRandomInt(0, numNodes-1);
        }
        if (!isEdgeInGraph(graph, from, to)) {
            addEdge(graph, from, to);
            addedEdges++;
        }
    }
    return graph;
}

Graph* createRandomCherryPickedGraph(int numNodes, int numEdges, int numBicComps) {
    if (numEdges>(numNodes*(numNodes - 1)/2)) {
        printf("too many edges for an undirected graph\n");
        printf("nodes: %d\n", numNodes);
        printf("edges: %d\n", numEdges);
        printf("max possible edges: %d\n", (numNodes*(numNodes - 1)/2));
        exit(1);
    }
    Graph* graph = createGraph(numNodes*numBicComps);
    for (int i=0; i<numBicComps; i++) {
        int addedEdges = 0;
        while (addedEdges<numEdges) {
            int from = generateRandomInt(0, numNodes-1);
            from += i*numNodes;
            int to = generateRandomInt(0, numNodes-1);
            to += i*numNodes;
            while (from == to) {
                to = generateRandomInt(0, numNodes-1);
                to += i*numNodes;
            }
            if (!isEdgeInGraph(graph, from, to)) {
                addEdge(graph, from, to);
                addedEdges++;
            }
        }
        if (i>0)
            addEdge(graph, i*numNodes, (i-1)*numNodes);
    }
    return graph;
}

// just does a broad first search starting from node 0
// and checks if visited nodes are all nodes
int isGraphConnected(Graph* graph) {
    int numNodes = graph->numNodes;
    int* visited = (int*)calloc(numNodes, sizeof(int));
    Queue* queue = createQueue();
    int* start = (int*)malloc(sizeof(int));
    *start = 0;
    int numVisitedNodes = 0;
    enqueue(queue, start);
    while (!isQueueEmpty(queue)) {
        int* node = (int*)dequeue(queue);
        int nodeValue = *node;
        free(node);
        if (!visited[nodeValue]) {
            visited[nodeValue] = 1;
            numVisitedNodes++;
            Node* neighbor = graph->adjacencyList[nodeValue];
            while (neighbor) {
                int neighborValue = *(int*)neighbor->data;
                if (!visited[neighborValue]) {
                    int* neighborElem = (int*)malloc(sizeof(int));
                    *neighborElem = neighborValue;
                    enqueue(queue, neighborElem);
                }
                neighbor = neighbor->next;
            }
        }
    }
    freeEmptyQueue(queue);
    free(visited);
    return (numVisitedNodes == numNodes);
}

Graph* importGraphFromFile(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        printf("Error opening file.\n");
        exit(1);
    }
    int numVertices;
    fscanf(file, "%d", &numVertices);
    Graph* graph = createGraph(numVertices);
    int src, dest;
    while (fscanf(file, "%d %d", &src, &dest) == 2) {
        src--;
        dest--;
        if (src < 0 || src >= numVertices || dest < 0 || dest >= numVertices) {
            printf("Invalid vertex in input file.\n");
            fclose(file);
            return NULL;
        }
        addEdge(graph, src, dest);
    }
    fclose(file);
    return graph;
}

int numberOfEdgesInGraph(Graph* graph) {
    int numberOfEdges = 0;
    for (int i=0; i<graph->numNodes; i++)
        numberOfEdges += graph->degree[i];
    return numberOfEdges;
}

// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// BICONNECTED COMPONENTS
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

struct BiconnectedComponent {
    int id;
    Graph* graph;
    int* oldToNewValue;
    int* newToOldValue;
    int originalNumNodes;
    Queue* cutVertices;
};

typedef struct BiconnectedComponent BiconnectedComponent;

BiconnectedComponent* createBiconnectedComponent(Queue* nodes, Queue* edges, int originalNumNodes, int id) {
    int numNodes = nodes->size;
    Graph* graph = createGraph(numNodes);
    int* oldToNewValue = (int*)malloc(originalNumNodes*sizeof(int));
    int* newToOldValue = (int*)malloc(numNodes*sizeof(int));
    for (int i=0; i<originalNumNodes; i++)
        oldToNewValue[i] = -1;
    int i=0;
    while(!isQueueEmpty(nodes)) {
        int* nodeElem = (int*)dequeue(nodes);
        int nodeValue = *nodeElem;
        free(nodeElem);
        newToOldValue[i] = nodeValue;
        oldToNewValue[nodeValue] = i;
        i++;
    }
    while (!isQueueEmpty(edges)) {
        Edge* edge = (Edge*)dequeue(edges);
        int from = oldToNewValue[edge->from];
        int to = oldToNewValue[edge->to];
        addEdge(graph, from, to);
        free(edge);
    }
    BiconnectedComponent* biconnectedComponent = (BiconnectedComponent*)malloc(sizeof(BiconnectedComponent));
    biconnectedComponent->id = id;
    biconnectedComponent->originalNumNodes = originalNumNodes;
    biconnectedComponent->graph = graph;
    biconnectedComponent->oldToNewValue = oldToNewValue;
    biconnectedComponent->newToOldValue = newToOldValue;
    biconnectedComponent->cutVertices = createQueue();
    return biconnectedComponent;
}

int isCutVertexInBiconnectedComponent(BiconnectedComponent* biconnectedComponent, int cutVertex) {
    return (biconnectedComponent->oldToNewValue[cutVertex] != -1);
}

void freeBiconnectedComponent(void* biconnectedComponent) {
    BiconnectedComponent* bc = (BiconnectedComponent*)biconnectedComponent;
    freeGraph(bc->graph);
    free(bc->oldToNewValue);
    free(bc->newToOldValue);
    freeQueue(bc->cutVertices, free);
    free(bc);
}

#endif