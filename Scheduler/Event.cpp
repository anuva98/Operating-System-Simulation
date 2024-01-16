#include "../include/Process.h"
#include "../include/Event.h"
#include <iostream>
#include <string>

using namespace std;

Event::Event(int ts, Process* proc, Process_State oldState, Process_State newState){
    this->eventTimestamp = ts;
    this->process = proc;
    this->oldState = oldState;
    this->newState = newState;
}
