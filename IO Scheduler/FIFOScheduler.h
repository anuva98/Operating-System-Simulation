#include "./Scheduler.h"

#ifndef FIFOSCHEDULER_H
#define FIFOSCHEDULER_H

using namespace std;

class FIFOScheduler : public Scheduler {
public:
    
    FIFOScheduler();
    Instruction* get_next_IO_instruction(int current_head_pos = -1, int last_head_pos = -1, bool instruction_list_empty = false) override;
    void add_IO_instruction(Instruction* instr, int current_head_pos = -1, int last_head_pos = -1) override;
};

#endif 