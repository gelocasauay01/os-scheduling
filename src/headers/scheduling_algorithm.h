#pragma once
#include "process.h"

typedef struct ProcessResult {
    int id;
    int completionTime;
    int turnAroundTime;
    int waitingTime;
    int responseTime;
} ProcessResult;

typedef struct ResultNode {
    ProcessResult processResult;
    struct ResultNode *next;
} ResultNode;

typedef struct ResultList {
    ResultNode *first;
    ResultNode *last;
} ResultList;

typedef enum AlgorithmType {FCFS, SJF, PRIO} AlgorithmType;
typedef enum ScheduleType {PREEMPTIVE, NON_PREEMPTIVE} ScheduleType;

ResultList* executeProcessesNonPreemptive(ProcessList *waitList, AlgorithmType algorithmType);
ResultList* executeProcessesPreemptive(ProcessList *waitList, AlgorithmType algorithmType, int quantum);
AlgorithmType getAlgorithmType(char* algo);
ScheduleType getScheduleType(char* sched);
void writeResultsToFile(ResultList resultList);
void displayResults(ResultList ResultList);