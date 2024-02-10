#ifndef MY_PROJECT_H
#define MY_PROJECT_H

#include <stdio.h>
#include <stdlib.h>
#include "my_graph.h"

// does a broad first search to compute distances
Tuple* _computeDistancesFromNode(Graph* graph, int startNode) {
    int numNodes = graph->numNodes; //dim
    int visited[numNodes];
    for (int i=0; i<numNodes; i++)
        visited[i] = 0;
    Queue* queue = createQueue();
    int* start = (int*)malloc(sizeof(int));
    *start = startNode;
    enqueue(queue, start);
    int prevOfNode[numNodes];
    for (int i=0; i<numNodes; i++)
        prevOfNode[i] = i;
    int* distanceOfNode = (int*)malloc(numNodes*sizeof(int));
    for (int i=0; i<numNodes; i++)
        distanceOfNode[i] = numNodes;
    distanceOfNode[startNode] = 0;
    int* numEdgesAtDistance  = (int*)calloc(numNodes, sizeof(int));
    while (!isQueueEmpty(queue)) {
        int* elem = (int*)dequeue(queue);
        int nodeValue = *elem;
        free(elem);
        if (!visited[nodeValue]) {
            visited[nodeValue] = 1;
            Node* neighbor = graph->adjacencyList[nodeValue];
            int numNewNodes = 0;
            while (neighbor) {
                int neighborValue = *(int*)neighbor->data;
                if (!visited[neighborValue]) {
                    numNewNodes++;
                    int* neighborElem = (int*)malloc(sizeof(int));
                    *neighborElem = neighborValue;
                    enqueue(queue, neighborElem);
                    if (distanceOfNode[neighborValue] == numNodes) {
                        distanceOfNode[neighborValue] = distanceOfNode[nodeValue]+1;
                        prevOfNode[neighborValue] = nodeValue;
                    }
                }
                neighbor = neighbor->next;
            }
            numEdgesAtDistance[distanceOfNode[nodeValue]+1] += numNewNodes;
        }
    }
    int* numNodesAtDistance = (int*)calloc(numNodes, sizeof(int));
    for (int i=0; i<numNodes; i++) {
        int distance = distanceOfNode[i];
        if (distance != numNodes)
            numNodesAtDistance[distance]++;
    }
    freeEmptyQueue(queue);
    Tuple* distances = createTuple(3);
    setElementInTuple(distances, 0, numNodesAtDistance);
    setElementInTuple(distances, 1, numEdgesAtDistance);
    setElementInTuple(distances, 2, distanceOfNode);
    return distances;
}

Tuple* _computeDistancesFromAllNodes(Graph* graph) {
    int numNodes = graph->numNodes;
    int** nodesAtDistanceFromAllNodes = (int**)malloc(numNodes*sizeof(int*));
    int** edgesAtDistanceFromAllNodes = (int**)malloc(numNodes*sizeof(int*));
    int** distanceBetweenAllNodes = (int**)malloc(numNodes*sizeof(int*));
    for (int i=0; i<numNodes; i++) {
        Tuple* distances = _computeDistancesFromNode(graph, i);
        nodesAtDistanceFromAllNodes[i] = getElementFromTuple(distances, 0);
        edgesAtDistanceFromAllNodes[i] = getElementFromTuple(distances, 1);
        distanceBetweenAllNodes[i] = getElementFromTuple(distances, 2);
        freeTuple(distances);
    }
    Tuple* allDistances = createTuple(3);
    setElementInTuple(allDistances, 0, nodesAtDistanceFromAllNodes);
    setElementInTuple(allDistances, 1, edgesAtDistanceFromAllNodes);
    setElementInTuple(allDistances, 2, distanceBetweenAllNodes);
    return allDistances;
}

float** computeDensitiesFromAllNodesNaive(Graph* graph) {
    int numNodes = graph->numNodes;
    Tuple* allDistances = _computeDistancesFromAllNodes(graph);
    int** nodesAtDistanceFromAllNodes = getElementFromTuple(allDistances, 0);
    int** edgesAtDistanceFromAllNodes = getElementFromTuple(allDistances, 1);
    int** distanceBetweenAllNodes = getElementFromTuple(allDistances, 2);
    freeSquareMatrixOfInts(distanceBetweenAllNodes, numNodes);
    freeTuple(allDistances);
    for (int i=0; i<numNodes; i++) {
        for (int j=1; j<numNodes; j++) {
            nodesAtDistanceFromAllNodes[i][j] += nodesAtDistanceFromAllNodes[i][j-1];
            edgesAtDistanceFromAllNodes[i][j] += edgesAtDistanceFromAllNodes[i][j-1];
        }
    }
    float** densitiesFromAllNodes = (float**)malloc(numNodes*sizeof(float*));
    for (int i=0; i<numNodes; i++) {
        float* densities = (float*)malloc(numNodes*sizeof(float));
        for (int j=0; j<numNodes; j++)
            densities[j] = edgesAtDistanceFromAllNodes[i][j] / (float)nodesAtDistanceFromAllNodes[i][j];
        densitiesFromAllNodes[i] = densities;
    }
    freeSquareMatrixOfInts(nodesAtDistanceFromAllNodes, numNodes);
    freeSquareMatrixOfInts(edgesAtDistanceFromAllNodes, numNodes);
    return densitiesFromAllNodes;
}

Pair* _dfsBiconnectedComponents(Graph* graph, int nodeValue, int* nodeIds, int* nextFreeId, int* prevOfNode, 
                    int* lowPoints, Queue* biconnectedComponents, Queue* cutVertices);

Pair* _computeBiconnectedComponents(Graph* graph) {
    int numNodes = graph->numNodes;
    int nodeIds[numNodes];
    for (int i=0; i<numNodes; i++)
        nodeIds[i] = numNodes;
    int prevOfNode[numNodes];
    for (int i=0; i<numNodes; i++)
        prevOfNode[i] = i;
    int nextFreeId = 0; // used to assign incrementing id to nodes, according to preorder dfs visit
    int lowPoints[numNodes];
    for (int i=0; i<numNodes; i++)
        lowPoints[i] = numNodes;
    Queue* biconnectedComponents = createQueue();
    Queue* cutVertices = createQueue();
    for (int nodeValue=0; nodeValue<numNodes; nodeValue++) {
        if (nodeIds[nodeValue] == numNodes) {
            Pair* p = _dfsBiconnectedComponents(graph, nodeValue, nodeIds, &nextFreeId, prevOfNode, 
                                lowPoints, biconnectedComponents, cutVertices);
            Queue* nodes = getLeftFromPair(p);
            Queue* edges = getRightFromPair(p);
            freePair(p);
            freeQueue(nodes, free);
            freeEmptyQueue(edges);
            if (!isQueueEmpty(cutVertices)) {
                int* node = (int*)removeLastElemFromQueue(cutVertices);
                free(node);
            }
        }
    }
    int numBiconnectedComponents = biconnectedComponents->size;
    for (int id=0; id<numBiconnectedComponents; id++) {
        Pair* bicCompPair = dequeue(biconnectedComponents);
        Queue* bicCompNodes = getLeftFromPair(bicCompPair);
        Queue* bicCompEdges = getRightFromPair(bicCompPair);
        freePair(bicCompPair);
        BiconnectedComponent* biconnectedComponent = createBiconnectedComponent(bicCompNodes, bicCompEdges, numNodes, id);
        enqueue(biconnectedComponents, biconnectedComponent);
        freeEmptyQueue(bicCompNodes);
        freeEmptyQueue(bicCompEdges);
    }
    Pair* result = createPair(biconnectedComponents, cutVertices);
    return result;
}

Pair* _dfsBiconnectedComponents(Graph* graph, int nodeValue, int* nodeIds, int* nextFreeId, int* prevOfNode, 
                    int* lowPoints, Queue* biconnectedComponents, Queue* cutVertices) {
    nodeIds[nodeValue] = *nextFreeId; // id assigned by preorder visit order
    *nextFreeId = *nextFreeId + 1;
    lowPoints[nodeValue] = nodeIds[prevOfNode[nodeValue]];
    Queue* biconnectedComponentNodes = createQueue();
    int* nodeElem = (int*)malloc(sizeof(int));
    *nodeElem = nodeValue;
    enqueue(biconnectedComponentNodes, nodeElem);
    Queue* biconnectedComponentEdges = createQueue();
    Node* neighbor = graph->adjacencyList[nodeValue];
    while (neighbor) {
        int neighborValue = *(int*)neighbor->data;
        neighbor = neighbor->next;
        if (nodeIds[neighborValue] == graph->numNodes) { // unexplored node
            prevOfNode[neighborValue] = nodeValue;
            Pair* neighborBiconnectedComponent = _dfsBiconnectedComponents(graph, neighborValue, nodeIds,
                        nextFreeId, prevOfNode, lowPoints, biconnectedComponents, cutVertices);
            Queue* neighborBiconnectedComponentNodes = getLeftFromPair(neighborBiconnectedComponent);
            Queue* neighborBiconnectedComponentEdges = getRightFromPair(neighborBiconnectedComponent);
            freePair(neighborBiconnectedComponent);
            if (lowPoints[neighborValue] == nodeIds[nodeValue]) {
                int* cutVertex = (int*)malloc(sizeof(int));
                *cutVertex = nodeValue;
                enqueue(cutVertices, cutVertex);
                int* nodeElem = (int*)malloc(sizeof(int));
                *nodeElem = nodeValue;
                enqueue(neighborBiconnectedComponentNodes, nodeElem);
                if (nodeIds[nodeValue] > nodeIds[neighborValue]) {
                    Edge* edge1 = createEdge(nodeValue, neighborValue);
                    enqueue(neighborBiconnectedComponentEdges, edge1);
                }
                neighborBiconnectedComponent = createPair(neighborBiconnectedComponentNodes, neighborBiconnectedComponentEdges);
                enqueue(biconnectedComponents, neighborBiconnectedComponent);
                continue;
            }
            if (lowPoints[neighborValue] < lowPoints[nodeValue])
                lowPoints[nodeValue] = lowPoints[neighborValue];
            appendQueue(biconnectedComponentNodes, neighborBiconnectedComponentNodes);
            freeEmptyQueue(neighborBiconnectedComponentNodes);
            appendQueue(biconnectedComponentEdges, neighborBiconnectedComponentEdges);
            freeEmptyQueue(neighborBiconnectedComponentEdges);
            continue;
        }
        if (prevOfNode[neighborValue] == nodeValue)
            continue;
        if (nodeIds[neighborValue] < lowPoints[nodeValue])
            lowPoints[nodeValue] = nodeIds[neighborValue];
        if (nodeIds[nodeValue] < nodeIds[neighborValue])
            continue;
        Edge* edge = createEdge(nodeValue, neighborValue);
        enqueue(biconnectedComponentEdges, edge);
    }
    Pair* biconnectedComponent = createPair(biconnectedComponentNodes, biconnectedComponentEdges);
    return biconnectedComponent;
}

float** computeDensitiesFromAllNodesSmarter(Graph* graph) {
    int numNodes = graph->numNodes;
    Pair* pair = _computeBiconnectedComponents(graph);
    Queue* biconnectedComponentsQueue = getLeftFromPair(pair);
    Queue* cutVerticesQueue = getRightFromPair(pair);
    freePair(pair);
    int numBiconnectedComponents = biconnectedComponentsQueue->size;
    // converting biconnected components queue to array
    BiconnectedComponent* biconnectedComponents[numBiconnectedComponents];
    while (!isQueueEmpty(biconnectedComponentsQueue)) {
        BiconnectedComponent* biconnectedComponent = (BiconnectedComponent*)dequeue(biconnectedComponentsQueue);
        biconnectedComponents[biconnectedComponent->id] = biconnectedComponent;
    }
    freeEmptyQueue(biconnectedComponentsQueue);
    int numCutVertices = cutVerticesQueue->size;
    int isSeen[numNodes];
    for (int i=0; i<numNodes; i++)
        isSeen[i] = 0;
    // removing duplicates of cutvertices
    for (int i=0; i<numCutVertices; i++) {
        int* cutVertexData = (int*)dequeue(cutVerticesQueue);
        int cutVertex = *cutVertexData;
        if (isSeen[cutVertex]) {
            free(cutVertexData);
            continue;
        }
        isSeen[cutVertex] = 1;
        enqueue(cutVerticesQueue, cutVertexData);        
    }
    // converting cutvertices queue to array
    numCutVertices = cutVerticesQueue->size;
    int cutVertices[numCutVertices];
    int posOfCutVertexInArray[numNodes];
    for (int i=0; i<numNodes; i++)
        posOfCutVertexInArray[i] = -1;
    for (int i=0; i<numCutVertices; i++) {
        int* cutVertexData = (int*)dequeue(cutVerticesQueue);
        cutVertices[i] = *cutVertexData;
        posOfCutVertexInArray[*cutVertexData] = i;
        free(cutVertexData);
    }
    freeEmptyQueue(cutVerticesQueue);
    // building simplified graph (block-cut-tree)
    Queue* cutVerticesNeighbors[numCutVertices];
    for (int i=0; i<numCutVertices; i++) {
        int cutVertex = cutVertices[i];
        Queue* cutVertexNeighbors = createQueue();
        cutVerticesNeighbors[i] = cutVertexNeighbors;
        for (int j=0; j<numBiconnectedComponents; j++) {
            BiconnectedComponent* biconnectedComponent = biconnectedComponents[j];
            if (isCutVertexInBiconnectedComponent(biconnectedComponent, cutVertex)) {
                int* biconnectedComponentId = (int*)malloc(sizeof(int));
                *biconnectedComponentId = biconnectedComponent->id;
                enqueue(cutVertexNeighbors, biconnectedComponentId);
                int* cutVertexData = (int*)malloc(sizeof(int));
                *cutVertexData = cutVertex;
                enqueue(biconnectedComponent->cutVertices, cutVertexData);
            }
        }
    }
    int cutVertexInCommon[numBiconnectedComponents][numBiconnectedComponents];
    for (int i=0; i<numBiconnectedComponents; i++)
        for (int j=0; j<numBiconnectedComponents; j++)
            cutVertexInCommon[i][j] = -1;
    for (int i=0; i<numCutVertices; i++) {
        int cutVertex = cutVertices[i];
        for (int j=0; j<cutVerticesNeighbors[i]->size; j++) {
            Node* neighbor = cutVerticesNeighbors[i]->first;
            int k=j;
            while (k != 0 && neighbor) {
                k--;
                neighbor = neighbor->next;
            }
            if (!neighbor)
                continue;
            int biconnectedComponent1 = *(int*)neighbor->data;
            neighbor = neighbor->next;
            while (neighbor) {
                int biconnectedComponent2 = *(int*)neighbor->data;
                cutVertexInCommon[biconnectedComponent1][biconnectedComponent2] = cutVertex;
                cutVertexInCommon[biconnectedComponent2][biconnectedComponent1] = cutVertex;
                neighbor = neighbor->next;
            }
        }
    }
    int globalNodesAtDistance[numNodes][numNodes];
    int globalEdgesAtDistance[numNodes][numNodes];
    for (int i=0; i<numNodes; i++)
        for (int j=0; j<numNodes; j++) {
            globalNodesAtDistance[i][j] = 0;
            globalEdgesAtDistance[i][j] = 0;
        }
    int** nodesAtDistanceBiconnectedComponents[numBiconnectedComponents];
    int** edgesAtDistanceBiconnectedComponents[numBiconnectedComponents];
    int** distanceOfNodesBiconnectedComponents[numBiconnectedComponents];
    for (int i=0; i<numBiconnectedComponents; i++) {
        Tuple* tuple = _computeDistancesFromAllNodes(biconnectedComponents[i]->graph);
        int** nodesAtDistanceFromAllNodes = getElementFromTuple(tuple, 0);
        int** edgesAtDistanceFromAllNodes = getElementFromTuple(tuple, 1);
        int** distanceBetweenAllNodes = getElementFromTuple(tuple, 2);
        nodesAtDistanceBiconnectedComponents[i] = nodesAtDistanceFromAllNodes;
        edgesAtDistanceBiconnectedComponents[i] = edgesAtDistanceFromAllNodes;
        distanceOfNodesBiconnectedComponents[i] = distanceBetweenAllNodes;
        freeTuple(tuple);
        // initializing global distances with partial values from biconnected comps
        int numNodesBiconnectedComponent = biconnectedComponents[i]->graph->numNodes;
        for (int node=0; node<numNodesBiconnectedComponent; node++) {
            int oldNodeValue = biconnectedComponents[i]->newToOldValue[node];
            for (int d=1; d<numNodesBiconnectedComponent; d++) {
                int nodes = nodesAtDistanceBiconnectedComponents[i][node][d];
                int edges = edgesAtDistanceBiconnectedComponents[i][node][d];
                globalNodesAtDistance[oldNodeValue][d] += nodes;
                globalEdgesAtDistance[oldNodeValue][d] += edges;
            }
        }
    }
    // nodes at distance 0 should be always 1, but it becomes more if the cut vertex partecipates
    // in more than one biconnected component, so initializing all those values back to 1
    for (int i=0; i<numNodes; i++)
        globalNodesAtDistance[i][0] = 1;        
    // needed so biconnected comps dont get visited multiple times from the same node (in particular cut vertices)
    int nodeVisitedBiconnectedComponent[numNodes][numBiconnectedComponents];
    for (int i=0; i<numNodes; i++)
        for (int j=0; j<numBiconnectedComponents; j++)
            nodeVisitedBiconnectedComponent[i][j] = 0;
    Queue* queue = createQueue();
    for (int id=0; id<numBiconnectedComponents; id++) {
        // bfs (on a tree-like structure, actually, the block-cut-tree)
        int visited[numBiconnectedComponents];
        for (int i=0; i<numBiconnectedComponents; i++)
            visited[i] = 0;
        int prevOfNode[numBiconnectedComponents];
        for (int i=0; i<numBiconnectedComponents; i++)
            prevOfNode[i] = -1;
        prevOfNode[id] = id;
        int distance[numBiconnectedComponents];
        for (int i=0; i<numBiconnectedComponents; i++)
            distance[i] = numBiconnectedComponents;
        distance[id] = 0;
        int* elem = (int*)malloc(sizeof(int));
        *elem = id;
        enqueue(queue, elem);
        while (!isQueueEmpty(queue)) {
            int* elem = dequeue(queue);
            int node = *elem;
            free(elem);
            if (visited[node])
                continue;
            visited[node] = 1;
            Node* cutVertexElem = biconnectedComponents[node]->cutVertices->first;
            while (cutVertexElem) {
                int cutVertex = *(int*)(cutVertexElem->data); // old value, not the one in the bic comp
                int pos = posOfCutVertexInArray[cutVertex];
                Node* neighbor = cutVerticesNeighbors[pos]->first;
                while (neighbor) {
                    int neighborId = *(int*)neighbor->data;
                    neighbor = neighbor->next;
                    if (visited[neighborId])
                        continue;
                    int* addingNeighbor = (int*)malloc(sizeof(int));
                    *addingNeighbor = neighborId;
                    enqueue(queue, addingNeighbor);
                    prevOfNode[neighborId] = node;
                    if (node == id)
                        distance[neighborId] = 0;
                    else {
                        int prevCutVertexInCommon = cutVertexInCommon[node][prevOfNode[node]];
                        int cutVertexLabel = biconnectedComponents[node]->oldToNewValue[cutVertex];
                        int prevCutVertexLabel = biconnectedComponents[node]->oldToNewValue[prevCutVertexInCommon];
                        int distanceBetweenCutVertices = distanceOfNodesBiconnectedComponents[node][prevCutVertexLabel][cutVertexLabel];
                        distance[neighborId] = distance[node]+distanceBetweenCutVertices;
                    }
                    int nextOfId = neighborId;
                    while (prevOfNode[nextOfId] != id)
                        nextOfId = prevOfNode[nextOfId];
                    int originalCutVertex = cutVertexInCommon[id][nextOfId];
                    int cutVertexLabel1 = biconnectedComponents[id]->oldToNewValue[originalCutVertex];
                    int cutVertexLabel2 = biconnectedComponents[neighborId]->oldToNewValue[cutVertex];
                    for (int i=0; i<biconnectedComponents[id]->graph->numNodes; i++) {
                        int iLabel = biconnectedComponents[id]->newToOldValue[i];
                        if (cutVertexInCommon[id][neighborId] == iLabel)
                            continue;
                        if (nodeVisitedBiconnectedComponent[iLabel][neighborId])
                            continue;
                        nodeVisitedBiconnectedComponent[iLabel][neighborId] = 1;
                        int d = distanceOfNodesBiconnectedComponents[id][i][cutVertexLabel1] + 1;
                        d += distance[neighborId];
                        for (int j=1; j<biconnectedComponents[neighborId]->graph->numNodes; j++) {
                            globalNodesAtDistance[iLabel][d] += nodesAtDistanceBiconnectedComponents[neighborId][cutVertexLabel2][j];
                            globalEdgesAtDistance[iLabel][d] += edgesAtDistanceBiconnectedComponents[neighborId][cutVertexLabel2][j];
                            d++;
                        }
                    }
                }
                cutVertexElem = cutVertexElem->next;
            }
        }
    }
    for (int i=0; i<numNodes; i++)
        for (int j=1; j<numNodes; j++) {
            globalNodesAtDistance[i][j] += globalNodesAtDistance[i][j-1];
            globalEdgesAtDistance[i][j] += globalEdgesAtDistance[i][j-1];
        }
    float** densitiesFromAllNodes = (float**)malloc(numNodes*sizeof(float*));
    for (int i=0; i<numNodes; i++) {
        float* densities = (float*)malloc(numNodes*sizeof(float));
        for (int j=0; j<numNodes; j++)
            densities[j] = globalEdgesAtDistance[i][j] / (float)globalNodesAtDistance[i][j];
        densitiesFromAllNodes[i] = densities;
    }
    // freeing heap
    freeEmptyQueue(queue);
    for (int i=0; i<numBiconnectedComponents; i++) {
        int** nodesAtDistance = nodesAtDistanceBiconnectedComponents[i];
        freeSquareMatrixOfInts(nodesAtDistance, biconnectedComponents[i]->graph->numNodes);
        int** edgesAtDistance = edgesAtDistanceBiconnectedComponents[i];
        freeSquareMatrixOfInts(edgesAtDistance, biconnectedComponents[i]->graph->numNodes);
        int** distanceOfNodes = distanceOfNodesBiconnectedComponents[i];
        freeSquareMatrixOfInts(distanceOfNodes, biconnectedComponents[i]->graph->numNodes);
        freeBiconnectedComponent(biconnectedComponents[i]);
    }
    for (int i=0; i<numCutVertices; i++)
        freeQueue(cutVerticesNeighbors[i], free);
    return densitiesFromAllNodes;
}

void computeAndPrintBiconnectedComponents(Graph* graph) {
    Pair* result = _computeBiconnectedComponents(graph);
    Queue* biconnectedComponents = getLeftFromPair(result);
    Queue* cutVertices = getRightFromPair(result);
    printf("cut vertices: ");
    printQueue(cutVertices, printInt);
    freePair(result);
    Node* elem = biconnectedComponents->first;
    printf("\n");
    while (elem) {
        BiconnectedComponent* biconnectedComponent = elem->data;
        printf("biconnected component id: %d\n", biconnectedComponent->id);
        printGraphWithMapping(biconnectedComponent->graph, biconnectedComponent->newToOldValue);
        printf("\n");
        elem = elem->next;
    }
    freeQueue(biconnectedComponents, freeBiconnectedComponent);
    freeQueue(cutVertices, free);
}

int* computeCorenessOfGraph(Graph* graph) {
    int numNodes = graph->numNodes;
    int* coreness = (int*)malloc(numNodes*sizeof(int));
    int bucket[numNodes];
    for (int i=0; i<numNodes; i++)
        bucket[i] = 0;
    for (int i=0; i<numNodes; i++) {
        coreness[i] = graph->degree[i];
        bucket[coreness[i]]++;
    }
    int start = 0;
    for (int d=0; d<numNodes; d++) {
        int num = bucket[d];
        bucket[d] = start;
        start += num;
    }
    int sorted[numNodes];
    for (int n=0; n<numNodes; n++) {
        int deg = coreness[n];
        sorted[bucket[deg]] = n;
        bucket[deg]++;
    }
    for (int d=numNodes-1; d>0; d--)
        bucket[d] = bucket[d-1];
    bucket[0] = 0;
    int pos[numNodes];
    for (int i=0; i<numNodes; i++)
        pos[sorted[i]] = i;
    for (int i=0; i<numNodes; i++) {
        int node = sorted[i];
        int nodeDegree = coreness[node];
        Node* neighborElem = graph->adjacencyList[node];
        while (neighborElem) {
            int neighbor = *(int*)neighborElem->data;
            neighborElem = neighborElem->next;
            int neighborDegree = coreness[neighbor];
            if (neighborDegree>nodeDegree) {
                int startPos = bucket[neighborDegree];
                int elemInStartPos = sorted[startPos];
                int neighborPos = pos[neighbor];
                sorted[startPos] = neighbor;
                sorted[neighborPos] = elemInStartPos;
                pos[elemInStartPos] = neighborPos;
                pos[neighbor] = startPos;
                coreness[neighbor]--;
                bucket[neighborDegree]++;
            }
        }
    }
    return coreness;
}

#endif