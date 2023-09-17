#pragma once

typedef struct Process {
    int id;
    int priority;
    int arrivalTime;
    int burstTime;
    int remainingTime;
} Process;

typedef struct ProcessNode {
    Process process;
    struct ProcessNode *next;
} ProcessNode;

typedef struct ProcessList {
    ProcessNode *first;
    ProcessNode *last;
} ProcessList;

typedef enum SortType {ARRIVAL, ID} SortType;

ProcessList* createProcessList();
ProcessList* createProcessListFromFile(const char* fileName);
void appendToProcessList(ProcessList *processList, ProcessNode *processNode);
void sortByArrivalTime(ProcessList *processList);
void displayProcessList(ProcessList processList);