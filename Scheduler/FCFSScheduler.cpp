#include "../include/Process.h"
#include "../include/FCFSScheduler.h"
#include <deque>



FCFSScheduler::FCFSScheduler(){
}

void FCFSScheduler::add_process(Process *proc) {

    RunQ.push_back(proc);
    for (deque<Process*>::iterator it = RunQ.begin(); it != RunQ.end(); ++it) {

            Process* process = *it; 
    }
}

Process* FCFSScheduler::get_next_process() {

    if (!RunQ.empty()) {
        Process* nextProcess = RunQ.front();
        RunQ.pop_front();
        return nextProcess;
    }

    return nullptr; 
}




