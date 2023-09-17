#include <stdio.h>
#include <malloc.h>
#include "../headers/process.h"

ProcessNode* createProcessNode(Process process) {
    ProcessNode* processNode = (ProcessNode*) malloc(sizeof(ProcessNode));
    processNode->process = process;
    processNode->next = NULL;
    return processNode;
}

void copyProcessList(ProcessList *source, ProcessList *dest) {
    ProcessNode* traverser = source->first;
    while(traverser != NULL) {
        appendToProcessList(dest, createProcessNode(traverser->process));
        traverser = traverser->next;
    }
}

void deleteProcessList(ProcessList* processList) {
    ProcessNode* traverser = processList->first;
    while(traverser != NULL) {
        ProcessNode* deletedNode = traverser;
        traverser = traverser->next;
        free(deletedNode);
    }
    free(processList);
}

void printProcess(Process process) {
    printf("id: %d, p: %d, at: %d, bt: %d, rt: %d\n", process.id, process.priority, process.arrivalTime, process.burstTime, process.remainingTime);
}

void switchNodePosition(ProcessList* processList, ProcessNode* left, ProcessNode* mid, ProcessNode* right) {
    ProcessNode* temp;
    
    // Switch Places
    mid->next = right->next;
    right->next = mid;
    if(left != NULL) {
        left->next = right;
    } else {
        processList->first = right;
    }

    // Reset the Positions
    temp = mid;
    mid = right;
    right = temp;

    // If there is no more next for right, it means it is the last node
    if(right->next == NULL) {
        processList->last = right;
    }
}  

ProcessList* createProcessList() {
    ProcessList *newProcessList = (ProcessList*) malloc(sizeof(ProcessList));
    newProcessList->first = NULL;
    newProcessList->last = NULL;
    return newProcessList;
}

ProcessList* createProcessListFromFile(const char* fileName) {
    int id = 1, priority, arrivalTime, burstTime;
    FILE* file = fopen(fileName, "r");
    ProcessList *processList = createProcessList();

    // Make the file start reading at the second line
    char buffer[100];
    fgets(buffer, 100, file);

    while(fscanf(file, "%d, %d, %d", &priority, &arrivalTime, &burstTime) != EOF) {
        Process process = {id, priority, arrivalTime, burstTime, burstTime};
        ProcessNode *processNode = createProcessNode(process);
        appendToProcessList(processList, processNode);
        id++;
    }
    
    fclose(file);
    return processList;
}

void appendToProcessList(ProcessList *processList, ProcessNode *processNode) {
    if(processList->first == NULL) {
        processList->first = processNode;
        processList->last = processNode;
    } else {
        processList->last->next = processNode;
        processList->last = processNode;
    }
}

void sortByArrivalTime(ProcessList *processList) {
    if(processList->first == NULL || processList->first == processList->last) return;

    int isChanged = 1;
    while(isChanged) {
        ProcessNode *right = processList->first->next;
        ProcessNode *mid = processList->first;
        ProcessNode *left = NULL;

        isChanged = 0;
        while(right != NULL) {
            if(right->process.arrivalTime < mid->process.arrivalTime) {
                switchNodePosition(processList, left, mid, right);
                isChanged = 1;
            }
            left = mid;
            mid = right;
            right = right->next;
        }
    }
} 

void displayProcessList(ProcessList processList) {
    ProcessNode* traverser = processList.first;
    while(traverser != NULL) {
        printProcess(traverser->process);
        traverser = traverser->next;
    }
    printf("\n");
}