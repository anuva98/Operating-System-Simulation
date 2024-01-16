#include "../include/Scheduler.h"


std::deque<Instruction*> Scheduler::IO_Q;
Instruction* Scheduler::get_next_IO_instruction(int current_head_pos, int last_head_pos, bool instruction_list_empty) {
    // do nothing
}

void Scheduler::add_IO_instruction(Instruction* instr, int current_head_pos, int last_head_pos){

    // Instruction instr = {instrNumber, arrivalTime, trackAccessed};
    // instructions.push_back(instr);

}

// void Scheduler::rm_instruction(){
//     instructions.pop_front();
// } 

