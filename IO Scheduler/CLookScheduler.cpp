#include "../include/CLookScheduler.h"


CLookScheduler::CLookScheduler(){
    //empty constructor
}

Instruction* CLookScheduler::get_next_IO_instruction(int current_head_pos, int last_head_pos, bool instruction_list_empty) {

    if (!IO_Q.empty()) {
    int min_seek_time = 9999;
    //don't really need a concept of increasing because in C-Look we only go one direction and since
    //we start from head = 0 that means we'll always go in the ascending order
    std::deque<Instruction *>::iterator rm_pos;
    Instruction* selectedInstruction = nullptr;
    while (true)
    {

        std::deque<Instruction *>::iterator it = IO_Q.begin();
        for (int i = 0; it != IO_Q.end(); ++it, i++)
        {
            
                if (((*it)->track_accessed) >= current_head_pos)
                {
                    if(abs((*it)->track_accessed - current_head_pos) < min_seek_time){
                        rm_pos = it;
                        selectedInstruction = *it;
                        min_seek_time = abs((*it)->track_accessed - current_head_pos);
                    }
                    
                    
                }
            
        }

        if (selectedInstruction == nullptr)
        {
            // we need to switch direction because we didn't find anything in given direction (we're already on the extremes of the directions)
            current_head_pos = 0;
            continue;
        }

        else{
            break;
        }
    }
        if(selectedInstruction != nullptr) IO_Q.erase(rm_pos);
        return selectedInstruction;
    }

    return nullptr;
}

void CLookScheduler::add_IO_instruction(Instruction *instr, int current_head_pos, int last_head_pos) {

    IO_Q.push_back(instr);
}
