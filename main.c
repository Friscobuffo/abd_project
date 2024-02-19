#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "my_project.h"

void testOnGraph(Graph* graph, int tests) {
    int numNodes = graph->numNodes;

    // printGraph(graph);
    // printf("\n");
    // computeAndPrintBiconnectedComponents(graph);

    Timer* timer = (Timer*)malloc(sizeof(Timer));
    double totalTimeNaive = 0.0;
    double totalTimeSmarter = 0.0;

    for (int i=0; i<tests; i++) {
        printf("test number [%d]\n", i);
        startTimer(timer);
        float** densitiesFromAllNodesNaive = computeDensitiesFromAllNodesNaive(graph);
        stopTimer(timer);
        double time = elapsedTime(timer);
        totalTimeNaive += time;
        printf("Time taken naive: %f milliseconds\n", time);

        startTimer(timer);
        float** densitiesFromAllNodesSmarter = computeDensitiesFromAllNodesSmarter(graph);
        stopTimer(timer);
        time = elapsedTime(timer);
        totalTimeSmarter += time;
        printf("Time taken smart: %f milliseconds\n", time);
        if (!areFloatMatricesAreEqual(densitiesFromAllNodesNaive, densitiesFromAllNodesSmarter, numNodes))
            printf("densities are different: error\n");

        freeSquareMatrixOfFloats(densitiesFromAllNodesNaive, numNodes);
        freeSquareMatrixOfFloats(densitiesFromAllNodesSmarter, numNodes);
    }

    // printf("\n");
    // printf("densities:\n");
    // printMatrixOfFloats(densitiesFromAllNodesSmarter, numNodes);

    printf("Total time taken naive: %f milliseconds\n", totalTimeNaive);
    printf("Total time taken smart: %f milliseconds\n", totalTimeSmarter);

    free(timer);
}

void testOnGraphFromFile(const char* filename) {
    Graph* graph = importGraphFromFile(filename);
    testOnGraph(graph, 1);
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

    FILE *file_pointer;
    char string[100];
    char temp[50];
    sprintf(temp, "%f", totalNaiveTime);
    strcpy(string, temp);
    strcat(string, ",");
    sprintf(temp, "%f", totalSmartTime);
    strcat(string, temp);
    strcat(string, "\n");
    file_pointer = fopen("output.txt", "a");
    if (file_pointer == NULL) {
        printf("Unable to open file.\n");
        exit(1);
    }
    fputs(string, file_pointer);
    fclose(file_pointer);

    free(timer);
}

void corenessDensitiesRandomGraphs(int numberOfTests, int numNodes) {
    FILE *file_pointer;
    file_pointer = fopen("output3.txt", "a");
    char string[100];
    char temp[50];
    for (int i=2; i<numberOfTests+2; i++) {
        printf("\r%d", i+1);
        fflush(stdout);
        Graph* graph = createRandomGraph(numNodes, numNodes*i);
        while(!isGraphConnected(graph)) {
            freeGraph(graph);
            graph = createRandomGraph(numNodes, numNodes*i);
        }
        int* coreness = computeCorenessOfGraph(graph);
        float** densitiesFromAllNodes = computeDensitiesFromAllNodesNaive(graph);
        for (int n=0; n<numNodes; n++) {
            double average = 0.0;
            for (int j=1; j<4; j++)
                average += densitiesFromAllNodes[n][j];
            // average /= 3.0;
            sprintf(temp, "%f", average);
            strcpy(string, temp);
            strcat(string, ",");
            sprintf(temp, "%d", coreness[n]);
            strcat(string, temp);
            strcat(string, "\n");
            fputs(string, file_pointer);
        }
        // smart
        freeSquareMatrixOfFloats(densitiesFromAllNodes, numNodes);
        freeGraph(graph);
        free(coreness);
    }
    printf("\n");
    fclose(file_pointer);
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

    FILE *file_pointer;
    char string[100];
    char temp[50];
    sprintf(temp, "%f", totalNaiveTime);
    strcpy(string, temp);
    strcat(string, ",");
    sprintf(temp, "%f", totalSmartTime);
    strcat(string, temp);
    strcat(string, "\n");
    file_pointer = fopen("output2.txt", "a");
    if (file_pointer == NULL) {
        printf("Unable to open file.\n");
        exit(1);
    }
    fputs(string, file_pointer);
    fclose(file_pointer);

    free(timer);
}

int main(int argc, char* argv[]) {
    srand((unsigned int)time(NULL));
    char* fileName = argv[1];
    if (!strcmp(fileName, "random")) {
        remove("output.txt");
        for (int i=2; i<50; i++) {
            printf("%d\n", i);
            speedTestRandomGraphs(20, 250, i*250);
        }
        printf("\n");
        return 0;
    }
    if (!strcmp(fileName, "good-random")) {
        remove("output2.txt");
        
        randomTestGoodGraphs(20, 25, 75, 10);
        printf("\n");
        
        randomTestGoodGraphs(20, 50, 150, 5);
        printf("\n");

        randomTestGoodGraphs(20, 125, 375, 2);
        printf("\n");

        randomTestGoodGraphs(20, 250, 750, 1);
        printf("\n");

        return 0;
    }
    if (!strcmp(fileName, "coreness")) {
        remove("output3.txt");
        
        corenessDensitiesRandomGraphs(30, 100);

        return 0;
    }
    char* folder = "graphs/";
    char* filePath = malloc(strlen(fileName) + strlen(folder) + 1);
    strcpy(filePath, folder);
    strcat(filePath, fileName);
    Graph* graph = importGraphFromFile(filePath);
    testOnGraph(graph, 5);
    int* coreness = computeCorenessOfGraph(graph);
    int counter = 0;
    for (int i=0; i<graph->numNodes; i++)
        if (coreness[i] == 1)
            counter++;
    printf("percent nodes with core 1: %f\n", (float)counter/graph->numNodes);
    printf("number of nodes: %d\n", graph->numNodes);
    printf("number of edges: %d\n", numberOfEdgesInGraph(graph));
    
    // printf("coreness:\n");
    // printArrayOfInts(coreness, graph->numNodes);
    free(coreness);
    freeGraph(graph);
    free(filePath);
    
    return 0;
}