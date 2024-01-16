#include "Scheduler.h"

#ifndef FCFSSCHEDULER_H
#define FCFSSCHEDULER_H

using namespace std;

class FCFSScheduler : public Scheduler {
public:
    
    FCFSScheduler();
    void add_process(Process *proc) override;
    Process* get_next_process() override;
};

#endif 