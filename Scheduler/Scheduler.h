#include "Process.h"
#include <deque>
#include <iostream>
#include "Event.h"

#ifndef SCHEDULER_H
#define SCHEDULER_H

using namespace std;

class Scheduler{

    public:    
             
    static std::deque<Process*> RunQ;
    virtual void add_process(Process *proc) = 0;
    virtual Process* get_next_process() = 0;

    virtual bool test_preempt(Process *proc, Process* currProc){

        return false;
    }
    
        
};

#endif 