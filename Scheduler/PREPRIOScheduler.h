#include "Scheduler.h"
#include <vector>

#ifndef PREPRIOSCHEDULER_H
#define PREPRIOSCHEDULER_H

using namespace std;

class PREPRIOScheduler : public Scheduler {
public:
    // at [0] we have activeQ and [1] we have expredQ to begin with
    static int indexOfActiveQ;
    static int indexOfExpiredQ;
    std::vector<deque<Process*> > activeAndExpiredQueues = std::vector<deque<Process*> >(2);
    PREPRIOScheduler();
    void add_process(Process *proc) override;
    void PriorityPushBack(Process *proc, deque<Process*> *q);
    Process* get_next_process() override;
    bool test_preempt(Process *proc, Process* currProc) override;
};

#endif 