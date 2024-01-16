#include <string>
#include "ProcessState.h"

#ifndef PROCESS_H
#define PROCESS_H

using namespace std;

class Process {

    public:

        int pid;
        int arrivalTime;       
        int totalCPUTime;
        int cpuBurst;
        int ioBurst;
        int staticPriority;  
        int dynamicPriority;
        int finishTime;
        int turnaroundTime;
        int ioTimeDone;
        int cpuTimeDone;
        //cpu wait time is the time it stays in the ready state waiting to get cpu burst
        int cpuWaitTime;
        int state_ts;
        Process_State state;
        int cpuBurstRemAfterQuantum = 0;


        Process(int pid, int arrivalTime,int totalCPUTime, int cpuBurst, int ioBurst, int staticPriority);
};

#endif 