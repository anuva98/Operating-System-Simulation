#include "../include/Process.h"
#include "../include/LCFSScheduler.h"
#include <deque>



LCFSScheduler::LCFSScheduler(){
}

void LCFSScheduler::add_process(Process *proc) {

    RunQ.push_back(proc);
    for (deque<Process*>::iterator it = RunQ.begin(); it != RunQ.end(); ++it) {

            Process* process = *it;  
    }
}

Process* LCFSScheduler::get_next_process() {

    if (!RunQ.empty()) {
        Process* nextProcess = RunQ.back();
        RunQ.pop_back();
        return nextProcess;
    }

    return nullptr; 
}



