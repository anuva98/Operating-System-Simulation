#include "../include/Process.h"
#include "../include/SRTFScheduler.h"
#include <deque>

SRTFScheduler::SRTFScheduler()
{
}

void SRTFScheduler::add_process(Process *proc)
{

    int remTime = proc->totalCPUTime - proc->cpuTimeDone;

    std::deque<Process *>::iterator it = RunQ.begin();
    for (int i = 0; it != RunQ.end(); ++it, i++)
    {
        int remTimeOfProcInRunQ = (*it)->totalCPUTime - (*it)->cpuTimeDone;
        if (remTime < remTimeOfProcInRunQ)
        {
            std::deque<Process *>::iterator insertPosition = std::next(RunQ.begin(), i);
            RunQ.insert(insertPosition, proc);
            break;
        }

        if (remTime == remTimeOfProcInRunQ)
        {

            std::deque<Process *>::iterator peekAtNextElement = it;
            std::advance(peekAtNextElement, 1);
            // in case we have 2/3.. same arrival timestamps
            if (peekAtNextElement != RunQ.end() && remTime == ((*peekAtNextElement)->totalCPUTime - (*peekAtNextElement)->cpuTimeDone))
            {
                continue;
            }

            std::deque<Process *>::iterator insertPosition = std::next(RunQ.begin(), i + 1);
            RunQ.insert(insertPosition, proc);
            break;
        }
    }

    if (it == RunQ.end())
    {

        RunQ.push_back(proc);
    }

    for (deque<Process *>::iterator it = RunQ.begin(); it != RunQ.end(); ++it)
    {

        Process *process = *it;
    }
}

Process *SRTFScheduler::get_next_process()
{

    if (!RunQ.empty())
    {
        Process *nextProcess = RunQ.front();
        RunQ.pop_front();
        return nextProcess;
    }

    return nullptr;
}
