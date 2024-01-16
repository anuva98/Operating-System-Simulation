#include "../include/FLookScheduler.h"


FLookScheduler::FLookScheduler(){
    indexOfAddQ = 0;
    indexOfActiveQ = 1;
}

Instruction *FLookScheduler::get_next_IO_instruction(int current_head_pos, int last_head_pos, bool instruction_list_empty)
{

    if (activeAndAddQueues[indexOfActiveQ].empty() && activeAndAddQueues[indexOfAddQ].empty())
        return nullptr;

    else
    {
        int min_seek_time = 99999;
        bool increasing = false;
        if (current_head_pos >= last_head_pos) increasing = true;
        std::deque<Instruction *>::iterator rm_pos;
        Instruction *selectedInstruction = nullptr;
        while (true)
        {
            if (activeAndAddQueues[indexOfActiveQ].empty())
            {
                // activeQ is empty, switch indexes and ask again.
                int oldIndexOfActiveQ = indexOfActiveQ;
                indexOfActiveQ = indexOfAddQ;
                indexOfAddQ = oldIndexOfActiveQ;
            }

            std::deque<Instruction *>::iterator it = activeAndAddQueues[indexOfActiveQ].begin();
            for (int i = 0; it != activeAndAddQueues[indexOfActiveQ].end(); ++it, i++)
            {
                if (increasing)
                {

                    if (((*it)->track_accessed) >= current_head_pos)
                    {
                        if (abs((*it)->track_accessed - current_head_pos) < min_seek_time)
                        {
                            rm_pos = it;
                            selectedInstruction = *it;
                            min_seek_time = abs((*it)->track_accessed - current_head_pos);
                        }
                    }
                }
                else
                {
                    if (((*it)->track_accessed) <= current_head_pos)
                    {
                        if (abs((*it)->track_accessed - current_head_pos) < min_seek_time)
                        {
                            rm_pos = it;
                            selectedInstruction = *it;
                            min_seek_time = abs((*it)->track_accessed - current_head_pos);
                        }
                    }
                }
            }

            if (selectedInstruction == nullptr)
            {
                // we need to switch direction because we didn't find anything in given direction (we're already on the extremes of the directions)
                increasing = !(increasing);
                continue;
            }

            else
            {
                break;
            }
        }

        if (selectedInstruction != nullptr)
            activeAndAddQueues[indexOfActiveQ].erase(rm_pos);

        return selectedInstruction;
    }
}

void FLookScheduler::add_IO_instruction(Instruction *instr, int current_head_pos, int last_head_pos)
{
    activeAndAddQueues[indexOfAddQ].push_back(instr);
}