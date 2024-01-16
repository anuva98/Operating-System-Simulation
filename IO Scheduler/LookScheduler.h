#include "Scheduler.h"

#ifndef LOOKSCHEDULER_H
#define LOOKSCHEDULER_H

using namespace std;

class LookScheduler : public Scheduler {
public:
    
    LookScheduler();
    Instruction* get_next_IO_instruction(int current_head_pos = -1, int last_head_pos = -1, bool instruction_list_empty = false) override;
    void add_IO_instruction(Instruction* instr, int current_head_pos = -1, int last_head_pos = -1) override;

};

#endif 