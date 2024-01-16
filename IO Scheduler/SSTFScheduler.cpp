#include "../include/SSTFScheduler.h"


SSTFScheduler::SSTFScheduler(){
    //empty constructor
}

Instruction* SSTFScheduler::get_next_IO_instruction(int current_head_pos, int last_head_pos, bool instruction_list_empty) {

    if (!IO_Q.empty()) {

    int min_seek_time = 99999;
    std::deque<Instruction *>::iterator rm_pos;
    Instruction* selectedInstruction;
    std::deque<Instruction *>::iterator it = IO_Q.begin();
    for (int i = 0; it != IO_Q.end(); ++it, i++)
    {
        int Q_seek_time = abs((*it)->track_accessed - current_head_pos);
        if(Q_seek_time < min_seek_time){
            min_seek_time = Q_seek_time;
            rm_pos = it;
            selectedInstruction = *it;
        }
    }

        IO_Q.erase(rm_pos);
        return selectedInstruction;
    }

    return nullptr;
}

void SSTFScheduler::add_IO_instruction(Instruction *instr, int current_head_pos, int last_head_pos) {

    IO_Q.push_back(instr);
}