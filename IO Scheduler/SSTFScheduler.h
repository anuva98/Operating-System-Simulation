#include "Scheduler.h"

#ifndef SSTFSCHEDULER_H
#define SSTFSCHEDULER_H

using namespace std;

class SSTFScheduler : public Scheduler {
public:
    SSTFScheduler();
    Instruction* get_next_IO_instruction(int current_head_pos = -1, int last_head_pos = -1, bool instruction_list_empty = false) override;
    void add_IO_instruction(Instruction* instr, int current_head_pos = -1, int last_head_pos = -1) override;
};

#endif 