#include "../include/FIFOScheduler.h"


FIFOScheduler::FIFOScheduler(){
    //empty constructor
}

Instruction* FIFOScheduler::get_next_IO_instruction(int current_head_pos, int last_head_pos, bool instruction_list_empty) {

    if (!IO_Q.empty()) {
        Instruction* nextInstruction = IO_Q.front();
        IO_Q.pop_front();
        return nextInstruction;
    }

    return nullptr;
}

void FIFOScheduler::add_IO_instruction(Instruction *instr, int current_head_pos, int last_head_posr) {

    IO_Q.push_back(instr);
}