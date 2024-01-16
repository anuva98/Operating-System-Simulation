#include "Scheduler.h"

#ifndef SRTFSCHEDULER_H
#define SRTFSCHEDULER_H

using namespace std;

class SRTFScheduler : public Scheduler {
public:

    SRTFScheduler();
    void add_process(Process *proc) override;
    Process* get_next_process() override;
};

#endif 