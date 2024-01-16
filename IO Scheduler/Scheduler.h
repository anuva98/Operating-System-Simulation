#include "Instruction.h"
#include <deque>
#include <iostream>
#include <vector>

#ifndef SCHEDULER_H
#define SCHEDULER_H

using namespace std;

class Scheduler{

    public:    
    int handIdx; 
    static std::deque<Instruction*> IO_Q;
    virtual Instruction* get_next_IO_instruction(int current_head_pos = -1, int last_head_pos = -1, bool instruction_list_empty = false) = 0; 
    virtual void add_IO_instruction(Instruction* instr, int current_head_pos = -1, int last_head_pos = -1) = 0;
    // void rm_instruction();
};

#endif 