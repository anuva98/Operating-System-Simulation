#include "Scheduler.h"
#include <vector>

#ifndef PRIOSCHEDULER_H
#define PRIOSCHEDULER_H

using namespace std;

class PRIOScheduler : public Scheduler {
public:
    // at [0] we have activeQ and [1] we have expredQ to begin with
    static int indexOfActiveQ;
    static int indexOfExpiredQ;
    vector<deque<Process*> > activeAndExpiredQueues = vector<deque<Process*> >(2);
    PRIOScheduler();
    void add_process(Process *proc) override;
    void PriorityPushBack(Process *proc, deque<Process*> *q);
    Process* get_next_process() override;
};

#endif 