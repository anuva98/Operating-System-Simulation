#include "Scheduler.h"
#include <vector>
#include <deque>

#ifndef FLOOKSCHEDULER_H
#define FLOOKSCHEDULER_H

using namespace std;

class FLookScheduler : public Scheduler {
public:
    // at [0] we have activeQ and [1] we have expredQ to begin with
    int indexOfActiveQ;
    int indexOfAddQ;
    vector<deque<Instruction*> > activeAndAddQueues = vector<deque<Instruction*> >(2);
    FLookScheduler();
    Instruction* get_next_IO_instruction(int current_head_pos = -1, int last_head_pos = -1, bool instruction_list_empty = false) override;
    void add_IO_instruction(Instruction* instr, int current_head_pos = -1, int last_head_pos = -1) override;

};

#endif 