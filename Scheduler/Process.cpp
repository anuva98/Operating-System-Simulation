#include "../include/Process.h"

Process::Process(int pid, int arrivalTime,int totalCPUTime, int cpuBurst, int ioBurst, int staticPriority) {
      this->pid = pid;
      this->arrivalTime = arrivalTime;
      this->totalCPUTime = totalCPUTime;
      this->cpuBurst = cpuBurst;
      this->ioBurst = ioBurst;
      this->staticPriority = staticPriority;
    }

