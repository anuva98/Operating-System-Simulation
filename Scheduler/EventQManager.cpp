#include "../include/Event.h"
#include "../include/EventQManager.h"
#include "../include/ProcessState.h"
#include <list>

Event* EventQManager::GetEvent(){

    Event* firstElement = nullptr;

    if (!eventQ.empty()) {
        firstElement = eventQ.front();
        
        for (list<Event*>::iterator it = eventQ.begin(); it != eventQ.end(); ++it) {

            Event* element = *it;  // Access the element through the iterator
        }
    }
    else {
    }

    return firstElement;
}

void EventQManager::PutEvent(Event* event){

    //always put new event in a timestamp sorted manner
    int newEvent_ts = event->eventTimestamp;
    std::list<Event*>::iterator it = eventQ.begin();
    for (int i = 0; it != eventQ.end(); ++it, ++i) {

        if(newEvent_ts < (*it)->eventTimestamp )
        {
            std::list<Event*>::iterator insertPosition = std::next(eventQ.begin(), i);
            eventQ.insert(insertPosition, event);
            break;

        }

        if(newEvent_ts == (*it)->eventTimestamp)
        {
            std::list<Event*>::iterator peekAtNextElement = it;
            std::advance(peekAtNextElement, 1);
            //in case we have 2/3.. same arrival timestamps
            if(peekAtNextElement != eventQ.end() && newEvent_ts == (*peekAtNextElement)->eventTimestamp){

                continue;
            }

            std::list<Event*>::iterator insertPosition = std::next(eventQ.begin(), i+1);
            eventQ.insert(insertPosition, event);
            break;

        }

    }

    //put at the end of the list if the new_ts is greater than current residents of the eventQ
    if(it == eventQ.end()){

        eventQ.push_back(event);
    }

        for (list<Event*>::iterator it = eventQ.begin(); it != eventQ.end(); ++it) {

            Event* element = *it;  // Access the element through the iterator
        }
    
}

void EventQManager::RmEvent(Event* event){

    if(event == nullptr){

        eventQ.pop_front();
    }
    else
    {

        for (std::list<Event *>::iterator it = eventQ.begin(); it != eventQ.end(); ++it)
        {
            if (*it == event)
            {
                eventQ.erase(it);
                break; // Exit the loop once the element is erased.
            }
        }
    }

    for (list<Event *>::iterator it = eventQ.begin(); it != eventQ.end(); ++it)
    {

        Event *element = *it; // Access the element through the iterator
    }
}

int EventQManager::get_next_event_time(){

    if(!eventQ.empty()){
        Event* eventAtFront = eventQ.front();
        return eventAtFront->eventTimestamp;
    }

    return -1;
    
}

Event* EventQManager::GetEventForProcess(Process* proc){

    Event* evt = nullptr;

    if (!eventQ.empty()) {

        for (list<Event*>::iterator it = eventQ.begin(); it != eventQ.end(); ++it) {

            Event* element = *it;  // Access the element through the iterator
            if((*element->process).pid == proc->pid){

                evt = element;
                return evt;
            }
            
        }
    }
   
    return evt;
}
