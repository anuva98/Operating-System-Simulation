#include "../include/Process.h"
#include "../include/Scheduler.h"
#include <deque>

std::deque<Process*> Scheduler::RunQ;
void Scheduler::add_process(Process *proc){}
Process* Scheduler::get_next_process(){
    return nullptr;
}
