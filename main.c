#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "my_project.h"

void testOnGraph(Graph* graph) {
    int numNodes = graph->numNodes;

    // printGraph(graph);
    // printf("\n");
    // computeAndPrintBiconnectedComponents(graph);

    Timer* timer = (Timer*)malloc(sizeof(Timer));

    startTimer(timer);
    float** densitiesFromAllNodesNaive = computeDensitiesFromAllNodesNaive(graph);
    stopTimer(timer);
    printf("Time taken naive: %f milliseconds\n", elapsedTime(timer));

    startTimer(timer);
    float** densitiesFromAllNodesSmarter = computeDensitiesFromAllNodesSmarter(graph);
    stopTimer(timer);
    printf("Time taken smart: %f milliseconds\n", elapsedTime(timer));

    // printf("\n");
    // printf("densities:\n");
    // printMatrixOfFloats(densitiesFromAllNodesSmarter, numNodes);

    if (!areFloatMatricesAreEqual(densitiesFromAllNodesNaive, densitiesFromAllNodesSmarter, numNodes))
        printf("densities are different: error\n");

    freeSquareMatrixOfFloats(densitiesFromAllNodesNaive, numNodes);
    freeSquareMatrixOfFloats(densitiesFromAllNodesSmarter, numNodes);
    free(timer);
}

void testOnGraphFromFile(const char* filename) {
    Graph* graph = importGraphFromFile(filename);
    testOnGraph(graph);
    freeGraph(graph);
}

void speedTestRandomGraphs(int numberOfTests, int numNodes, int numEdges) {
    printf("speed tests random graphs\n");
    Timer* timer = (Timer*)malloc(sizeof(Timer));
    double totalNaiveTime = 0.0;
    double totalSmartTime = 0.0;
    int fails = 0;
    for (int i=0; i<numberOfTests; i++) {
        printf("\r%d", i+1);
        fflush(stdout);
        Graph* graph = createRandomGraph(numNodes, numEdges);
        while(!isGraphConnected(graph)) {
            freeGraph(graph);
            graph = createRandomGraph(numNodes, numEdges);
        }
        // naive
        startTimer(timer);
        float** densitiesFromAllNodesNaive = computeDensitiesFromAllNodesNaive(graph);
        stopTimer(timer);
        totalNaiveTime += elapsedTime(timer);
        // smart
        startTimer(timer);
        float** densitiesFromAllNodesSmarter = computeDensitiesFromAllNodesSmarter(graph);
        stopTimer(timer);
        totalSmartTime += elapsedTime(timer);
        if (!areFloatMatricesAreEqual(densitiesFromAllNodesNaive, densitiesFromAllNodesSmarter, numNodes))
            fails++;
        freeSquareMatrixOfFloats(densitiesFromAllNodesNaive, numNodes);
        freeSquareMatrixOfFloats(densitiesFromAllNodesSmarter, numNodes);
        freeGraph(graph);
    }
    printf("\n");
    printf("total naive time: %f milliseconds\n", totalNaiveTime);
    printf("total smart time: %f milliseconds\n", totalSmartTime);
    printf("total fails: %d\n", fails);
    free(timer);
}

void randomTestGoodGraphs(int numberOfTests, int numNodes, int numEdges, int numBicComps) {
    printf("speed tests good random graphs\n");
    Timer* timer = (Timer*)malloc(sizeof(Timer));
    double totalNaiveTime = 0.0;
    double totalSmartTime = 0.0;
    int fails = 0;
    for (int i=0; i<numberOfTests; i++) {
        printf("\r%d", i+1);
        fflush(stdout);
        Graph* graph = createRandomCherryPickedGraph(numNodes, numEdges, numBicComps);
        while(!isGraphConnected(graph)) {
            freeGraph(graph);
            graph = createRandomCherryPickedGraph(numNodes, numEdges, numBicComps);
        }
        // naive
        startTimer(timer);
        float** densitiesFromAllNodesNaive = computeDensitiesFromAllNodesNaive(graph);
        stopTimer(timer);
        totalNaiveTime += elapsedTime(timer);
        // smart
        startTimer(timer);
        float** densitiesFromAllNodesSmarter = computeDensitiesFromAllNodesSmarter(graph);
        stopTimer(timer);
        totalSmartTime += elapsedTime(timer);
        if (!areFloatMatricesAreEqual(densitiesFromAllNodesNaive, densitiesFromAllNodesSmarter, numNodes))
            fails++;
        freeSquareMatrixOfFloats(densitiesFromAllNodesNaive, numNodes);
        freeSquareMatrixOfFloats(densitiesFromAllNodesSmarter, numNodes);
        freeGraph(graph);
    }
    printf("\n");
    printf("total naive time: %f milliseconds\n", totalNaiveTime);
    printf("total smart time: %f milliseconds\n", totalSmartTime);
    printf("total fails: %d\n", fails);
    free(timer);
}

int main(int argc, char* argv[]) {
    srand((unsigned int)time(NULL));
    char* fileName = argv[1];
    char* folder = "graphs/";
    char* filePath = malloc(strlen(fileName) + strlen(folder) + 1);
    strcpy(filePath, folder);
    strcat(filePath, fileName);
    Graph* graph = importGraphFromFile(filePath);
    testOnGraph(graph);
    int* coreness = computeCorenessOfGraph(graph);
    int counter = 0;
    for (int i=0; i<graph->numNodes; i++)
        if (coreness[i] == 1)
            counter++;
    printf("percent nodes with core 1: %f\n", (float)counter/graph->numNodes);
    // printf("coreness:\n");
    // printArrayOfInts(coreness, graph->numNodes);
    free(coreness);
    freeGraph(graph);


    // printf("\n");
    // speedTestRandomGraphs(100, 100, 150);
    // printf("\n");
    // randomTestGoodGraphs(15, 40, 160, 15);
    free(filePath);
    return 0;
}