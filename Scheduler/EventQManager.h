#include "Event.h"
#include <list>

#ifndef EVENTQMANAGER_H
#define EVENTQMANAGER_H

class EventQManager{

    public:

    list<Event*> eventQ; 
    Event* GetEvent();
    void PutEvent(Event* event); 
    void RmEvent(Event* event = nullptr);
    int get_next_event_time();
    Event* GetEventForProcess(Process* proc);
    
};

#endif 