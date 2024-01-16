#include "Scheduler.h"

#ifndef LCFSSCHEDULER_H
#define LCFSSCHEDULER_H

using namespace std;

class LCFSScheduler : public Scheduler {
public:
    
    LCFSScheduler();
    void add_process(Process *proc) override;
    Process* get_next_process() override;
};

#endif 