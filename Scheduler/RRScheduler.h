#include "Scheduler.h"

#ifndef RRSCHEDULER_H
#define RRSCHEDULER_H

using namespace std;

class RRScheduler : public Scheduler {
public:

    RRScheduler();
    void add_process(Process *proc) override;
    Process* get_next_process() override;
};

#endif 