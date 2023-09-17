#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include "../headers/scheduling_algorithm.h"

ResultList* createResultList() {
    ResultList *newResultList = (ResultList*) malloc(sizeof(ResultList));
    newResultList->first = NULL;
    newResultList->last = NULL;
    return newResultList;
}

ResultNode* createResultNode(ProcessResult processResult) {
    ResultNode* resultNode = (ResultNode*) malloc(sizeof(ResultNode));
    resultNode->processResult = processResult;
    resultNode->next = NULL;
    return resultNode;
}

ProcessResult createProcessResult(Process process, int timeElapsed) {
    int completionTime = timeElapsed;
    int turnAroundTime = completionTime - process.arrivalTime;
    int waitingTime = turnAroundTime - process.burstTime;
    int responseTime = waitingTime + process.arrivalTime;
    ProcessResult result = {process.id, completionTime, turnAroundTime, waitingTime, responseTime};
    return result;
}

void addToResultList(ResultList *resultList, ResultNode *addedNode) {
    if(resultList->first == NULL) {
        resultList->first = addedNode;
        resultList->last = addedNode;
    } else {
        ResultNode *prev = NULL, *current = resultList->first;
        while(current != NULL && current->processResult.id < addedNode->processResult.id) {
            prev = current;
            current = current->next;
        }
        addedNode->next = current;
        if(prev != NULL) prev->next = addedNode;
        else resultList->first = addedNode;
    }
}

void deleteResultList(ResultList *resultList) {
    ResultNode* traverser = resultList->first;
    while(traverser != NULL) {
        ResultNode* deletedNode = traverser;
        traverser = traverser->next;
        free(deletedNode);
    }
    free(resultList);
}

void printResult(ProcessResult result) {
    printf("id: %d, ct: %d, tt: %d, wt: %d, rt: %d \n", result.id, result.completionTime, result.turnAroundTime, result.waitingTime, result.responseTime);
}

int compareProcessValues(Process lesser, Process greater, AlgorithmType algorithmType) {
    return (algorithmType == SJF && lesser.remainingTime <= greater.remainingTime) || (algorithmType == PRIO && lesser.priority <= greater.priority) || (algorithmType == FCFS && lesser.arrivalTime <= greater.arrivalTime);
}

void sortedInsert(ProcessList *processList, ProcessNode *insertedNode, AlgorithmType algorithmType) {
    if(processList->first == NULL) {
        processList->first = insertedNode;
        processList->last = insertedNode;
    } else {
        ProcessNode *prev = NULL, *current = processList->first;
        while(current != NULL && compareProcessValues(current->process, insertedNode->process, algorithmType)) {
            prev = current;
            current = current->next;
        }
        insertedNode->next = current;

        // If there is no previous node, then assign it as the first node
        if(prev != NULL) prev->next = insertedNode;
        else processList->first = insertedNode;
        
        // If there is no node after the inserted node, then assign it as the last node
        if(insertedNode->next == NULL) processList->last = insertedNode;
    }
}

ProcessNode* dequeue(ProcessList *processList) {
    ProcessNode* processNode = NULL;
    if(processList->first != NULL) {
        processNode = processList->first;
        processList->first = processList->first->next;
        processNode->next = NULL;

        if(processList->first == NULL) processList->last = NULL;
    } 
    return processNode;
} 

// Parameter is expected to be sorted in increasing order by arrival time
void enqueueArrivedJobsByType(ProcessList* waitList, ProcessList* activeProcess, int timeElapsed, AlgorithmType algorithmType) {
    if(waitList->first == NULL) return;

    if(timeElapsed < waitList->first->process.arrivalTime && activeProcess->first == NULL) {
        timeElapsed = waitList->first->process.arrivalTime;
    }

    while(waitList->first != NULL && waitList->first->process.arrivalTime <= timeElapsed) {
        sortedInsert(activeProcess, dequeue(waitList), algorithmType);
    }
}

ResultList* executeProcessesNonPreemptive(ProcessList *waitList, AlgorithmType algorithmType) {
    int timeElapsed = 0;
    ProcessList *activeProcess = createProcessList();
    ResultList *resultList = createResultList();
    sortByArrivalTime(waitList);
    enqueueArrivedJobsByType(waitList, activeProcess, timeElapsed, algorithmType);
    while(activeProcess->first != NULL) {
        ProcessNode* current = dequeue(activeProcess);
        if(timeElapsed < current->process.arrivalTime) {
            timeElapsed = current->process.arrivalTime;
        }
        timeElapsed += current->process.burstTime;
        addToResultList(resultList, createResultNode(createProcessResult(current->process, timeElapsed)));
        enqueueArrivedJobsByType(waitList, activeProcess, timeElapsed, algorithmType);
    }
    return resultList;
}

ResultList* executeProcessesPreemptive(ProcessList *waitList, AlgorithmType algorithmType, int quantum) {
    int timeElapsed = 0;
    ProcessList *activeProcess = createProcessList();
    ResultList *resultList = createResultList();
    sortByArrivalTime(waitList);
    enqueueArrivedJobsByType(waitList, activeProcess, timeElapsed, algorithmType);
    while(activeProcess->first != NULL) {
        ProcessNode* current = dequeue(activeProcess);
        if(timeElapsed < current->process.arrivalTime) {
            timeElapsed = current->process.arrivalTime;
        }

        // Add the remaining time of the process if it is less than the quantum 
        if(current->process.remainingTime < quantum) {
            timeElapsed += current->process.remainingTime;
        } else {
            timeElapsed += quantum;
        }
        current->process.remainingTime -= quantum;

        
        //If the process finished executing, add the result to the list
        if(current->process.remainingTime <= 0) {
            addToResultList(resultList, createResultNode(createProcessResult(current->process, timeElapsed)));
        }
        
        enqueueArrivedJobsByType(waitList, activeProcess, timeElapsed, algorithmType);

        // Insert it to the active processes if it is not finished
        if(current->process.remainingTime > 0) {
            if(algorithmType == FCFS) appendToProcessList(activeProcess, current);
            else sortedInsert(activeProcess, current, algorithmType);
        }

        displayProcessList(*activeProcess);
    }
    return resultList;
}

AlgorithmType getAlgorithmType(char* algo) {
    AlgorithmType algorithm = FCFS;
    char *loweredInput = strlwr(algo);
    if(strcmp(loweredInput, "sjf") == 0) {
        algorithm = SJF;
    } else if(strcmp(loweredInput, "prio") == 0) {
        algorithm = PRIO;
    }
    return algorithm;
}

ScheduleType getScheduleType(char* sched) {
    ScheduleType scheduleType = NON_PREEMPTIVE;
    char *loweredInput = strlwr(sched);
    if(strcmp(loweredInput, "preemptive") == 0) {
        scheduleType = PREEMPTIVE;
    }
    return scheduleType;
}

void writeResultsToFile(ResultList resultList) {
    if(resultList.first == NULL) return;

    FILE *file = fopen("results.csv", "w");
    ResultNode *traverser = resultList.first;

    fprintf(file, "ID, CompletionTime, TurnAroundTime, WaitingTime, ResponseTime\n");
    while(traverser != NULL) {
        ProcessResult result = traverser->processResult;
        fprintf(file, "%d, %d, %d, %d, %d\n", result.id, result.completionTime, result.turnAroundTime, result.waitingTime, result.responseTime);
        traverser = traverser->next;
    }
    fclose(file);
}

void displayResults(ResultList resultList) {
    ResultNode* traverser = resultList.first;
    while(traverser != NULL) {
        printResult(traverser->processResult);
        traverser = traverser->next;
    }
}
