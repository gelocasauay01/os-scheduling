#include <stdio.h>
#include <stdlib.h>
#include "headers/process.h"
#include "headers/scheduling_algorithm.h"

int main(int argv, char** argc) {
    if(argv > 1) {
        const int FILE_NAME_INDEX = 1;
        const int ALGORITHM_INDEX = 2;
        const int SCHEDULE_TYPE_INDEX = 3;
        const int QUANTUM_INDEX = 4;

        AlgorithmType algorithm = getAlgorithmType(argc[ALGORITHM_INDEX]);
        ScheduleType scheduleType = getScheduleType(argc[SCHEDULE_TYPE_INDEX]);
        ProcessList *processList = createProcessListFromFile(argc[FILE_NAME_INDEX]);
        ResultList *results;

        switch(scheduleType) {
            case NON_PREEMPTIVE: {
                results = executeProcessesNonPreemptive(processList, algorithm);
                break;
            }
            case PREEMPTIVE: {
                int quantum = atoi(argc[QUANTUM_INDEX]);
                results = executeProcessesPreemptive(processList, algorithm, quantum);
                break;
            }
        }
        writeResultsToFile(*results);
    }
    return 0;
}