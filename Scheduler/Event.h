#include "Process.h"
#include <iostream>
#include <string>
#include "ProcessState.h"

#ifndef EVENT_H
#define EVENT_H

using namespace std;

class Event{

    public:    
             
    int eventTimestamp;       
    Process* process;
    Process_State oldState;
    Process_State newState;
    
    Event(int ts, Process* proc, Process_State oldState, Process_State newState);
        
};

#endif 