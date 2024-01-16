#include "Scheduler.h"

#ifndef CLOOKSCHEDULER_H
#define CLOOKSCHEDULER_H

using namespace std;

class CLookScheduler : public Scheduler {
public:
    
    CLookScheduler();
    Instruction* get_next_IO_instruction(int current_head_pos = -1, int last_head_pos = -1, bool instruction_list_empty = false) override;
    void add_IO_instruction(Instruction* instr, int current_head_pos = -1, int last_head_pos = -1) override;
};

#endif 