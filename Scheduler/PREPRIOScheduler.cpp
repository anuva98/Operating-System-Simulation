#include "../include/Process.h"
#include "../include/PREPRIOScheduler.h"
#include <deque>
#include <vector>


int PREPRIOScheduler::indexOfActiveQ;
int PREPRIOScheduler::indexOfExpiredQ;

PREPRIOScheduler::PREPRIOScheduler(){
    indexOfActiveQ = 0;
    indexOfExpiredQ = 1;
}

void PREPRIOScheduler::add_process(Process *proc) {

    if(proc->dynamicPriority == -1){
        proc-> dynamicPriority = proc->staticPriority -1;
        PriorityPushBack(proc, &activeAndExpiredQueues[indexOfExpiredQ]);
    }
    else{
        PriorityPushBack(proc, &activeAndExpiredQueues[indexOfActiveQ]);
    }
    
}

void PREPRIOScheduler::PriorityPushBack(Process *proc, deque<Process*> *q){

    int dynamicPrioOfIncomingProc = proc->dynamicPriority;
    std::deque<Process *>::iterator it = (*q).begin();
    for (int i = 0; it != (*q).end(); ++it, i++)
    {
        
        if (dynamicPrioOfIncomingProc > (*it)->dynamicPriority)
        {
            std::deque<Process *>::iterator insertPosition = std::next((*q).begin(), i);
            (*q).insert(insertPosition, proc);
            break;
        }

        if (dynamicPrioOfIncomingProc == (*it)->dynamicPriority)
        {

            std::deque<Process *>::iterator peekAtNextElement = it;
            std::advance(peekAtNextElement, 1);
            // in case we have multiple of same dynamicPrio then we assign it the order it came into the q
            if (peekAtNextElement != (*q).end() && dynamicPrioOfIncomingProc == ((*peekAtNextElement)->dynamicPriority))
            {
                continue;
            }

            std::deque<Process *>::iterator insertPosition = std::next((*q).begin(), i + 1);
            (*q).insert(insertPosition, proc);
            break;
        }
    }

    if (it == (*q).end())
    {

        (*q).push_back(proc);
    }

    for (deque<Process *>::iterator it = (*q).begin(); it != (*q).end(); ++it)
    {

        Process *process = *it;
    }
}

Process* PREPRIOScheduler::get_next_process() {

    if (!activeAndExpiredQueues[indexOfActiveQ].empty()) {
        Process* nextProcess = activeAndExpiredQueues[indexOfActiveQ].front();
        activeAndExpiredQueues[indexOfActiveQ].pop_front();
        return nextProcess;
    }
    else if(!activeAndExpiredQueues[indexOfExpiredQ].empty()){

        //activeQ is empty, switch indexes and ask again.
        int oldIndexOfActiveQ = indexOfActiveQ;
        indexOfActiveQ = indexOfExpiredQ;
        indexOfExpiredQ = oldIndexOfActiveQ;
        return get_next_process();

    }
    
    return nullptr;

}

bool PREPRIOScheduler::test_preempt(Process *proc, Process* currentProcess){

    if(currentProcess != nullptr){

        return (proc->dynamicPriority > currentProcess->dynamicPriority);
    }

    return false;

    
}









