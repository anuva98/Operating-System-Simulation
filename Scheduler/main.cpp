#include "../include/Process.h"
#include "../include/Event.h"
#include "../include/EventQManager.h"
#include <iostream>
#include <sstream>
#include <string>
#include <fstream>
#include <deque>
#include <list>
#include <unistd.h>
#include <map>
#include "../include/ProcessState.h"
#include "../include/Scheduler.h"
#include "../include/FCFSScheduler.h"
#include "../include/LCFSScheduler.h"
#include "../include/SRTFScheduler.h"
#include "../include/RRScheduler.h"
#include "../include/PRIOScheduler.h"
#include "../include/PREPRIOScheduler.h"
#include <cstdlib>
#include <cstdio>
#include <cstring>

using namespace std;

int maxprio = 4;
static vector<int> randNumberList;
static char *cstr;
static int pidGenerator = 0;
static int ofs = 0;
bool eventExecutionTraceFlag = false;
bool showEventQBeforeAfterFlag = false;
bool preemptFlagForEScheduler = false;
bool verboseFlag = false;
int quantum = 10000;
static EventQManager eventQManager;
static int CURRENT_TIME = 0;
static bool CALL_SCHEDULER = false;
static Process *CURRENT_RUNNING_PROCESS = nullptr;
static Scheduler *scheduler;
static std::list<Process *> DoneAwayProcessQ;
static int timeIOWillStopNext = 0;
static int ioOverlap = 0;
static int numOfProcesses = 0;

// Function declarations
char *GetNextTokenInLine(string line, bool contd);
void ResetOfsIfReqd();
int myrandom(int burst)
{   
    ResetOfsIfReqd();
    ofs++;
    return 1 + (randNumberList[ofs] % burst);
}

int GetDynamicPrio(int staticPrio){

    return 1 + (std::rand() % staticPrio);
}


void ReadAndLoadRandomNUmberFile(string fileName);
void HandleSchedulerType(char *schedFlag);
void RunSimulation();
int SelectRandomCPUBurst(int cpuBurst, int totalCpuTime, int cpuTimeDone);
void PrintOutput();
void PutDoneAwayProcess(Process *proc);
void ResetDynamicPropertyIfRoundRobin(Process* process);
void CheckAndApplyPreemptivePriority(Process* proc);

namespace SchedulerNamespace
{

    enum SchedType
    {
        FCFS,
        LCFS,
        SRTF,
        RR,
        PRIO,
        PREPRIO
    };

    std::string SchedTypeToString(SchedType type)
    {

        std::map<SchedType, std::string> typeMap;
        typeMap[SchedType::FCFS] = "FCFS";
        typeMap[SchedType::LCFS] = "LCFS";
        typeMap[SchedType::SRTF] = "SRTF";
        typeMap[SchedType::RR] = "RR";
        typeMap[SchedType::PRIO] = "PRIO";
        typeMap[SchedType::PREPRIO] = "PREPRIO";

        auto it = typeMap.find(type);
        if (it != typeMap.end())
        {
            return it->second;
        }
        else
        {
            return "Unknown"; // Handle unknown values
        }
    }

}

using namespace SchedulerNamespace;

static SchedType schedulerType;

int main(int argc, char *argv[])
{

    int option;
    while ((option = getopt(argc, argv, "vteps:")) != -1)
    {
        switch (option)
        {
        case 'v':
            verboseFlag = true;
            break;
        case 't':
            eventExecutionTraceFlag = true;
            break;
        case 'e':
            showEventQBeforeAfterFlag = true;
            break;
        case 'p':
            preemptFlagForEScheduler = true;
            break;
        case 's':
            HandleSchedulerType(optarg);
            break;
        case '?':
            if (optopt == 's')
            {
                std::cerr << "Option -s requires a valid argument." << std::endl;
            }
            else if (isprint(optopt))
            {
                std::cerr << "Unknown option -" << static_cast<char>(optopt) << std::endl;
            }
            else
            {
                std::cerr << "Unknown option character" << std::endl;
            }
            return 1; // Or handle the error as needed
        default:
            // Handle other options or arguments
            break;
        }
    }

    string inputFileName;
    string randomNumberFile;

    for (int i = optind; i < argc; ++i)
    {

        inputFileName = argv[i];
        if (i + 1 < argc)
        {
            randomNumberFile = argv[i + 1];
            break;
        }
    }

    ReadAndLoadRandomNUmberFile(randomNumberFile);
    ifstream processFile(inputFileName); // Open the file for reading
    if (processFile.is_open())
    {

        std::string line;
        while (getline(processFile, line))
        {
            int arrivalTime = stoi(GetNextTokenInLine(line, false));
            int totalCPUTime = stoi(GetNextTokenInLine(line, true));
            int cpuBurst = stoi(GetNextTokenInLine(line, true));
            int ioBurst = stoi(GetNextTokenInLine(line, true));
            int staticPrio = myrandom(maxprio);
            Process *proc = new Process(pidGenerator, arrivalTime, totalCPUTime, cpuBurst, ioBurst, staticPrio);
            proc->dynamicPriority = (staticPrio) - 1;
            // Before putting in event queue change to READY
            Process_State currStateForProc = Process_State::CREATED;
            Process_State nextStateForEvent = Process_State::READY;
            proc->state_ts = proc->arrivalTime;
            proc->state = currStateForProc;

            // after creating new process objects (CREATED) we need to get the eventQ going by putting the processes in READY state
            // after this when we start the simulation
            Event *newEvent = new Event(arrivalTime, proc, currStateForProc, nextStateForEvent);

            eventQManager.PutEvent(newEvent);

            pidGenerator++;
            numOfProcesses++;
        }

        processFile.close();
    }

    else
    {

        cout << "Unable to open input file: " << inputFileName << endl;
    }

    RunSimulation();
    PrintOutput();

    return 0;
}

void ResetOfsIfReqd()
{
    if (ofs >= randNumberList[0])
        ofs = 0;
}

void ReadAndLoadRandomNUmberFile(string fileName)
{

    ifstream randNumberFile(fileName);

    if (randNumberFile.is_open())
    {

        int number;
        string line;
        while (getline(randNumberFile, line))
        {
            number = stoi(GetNextTokenInLine(line, false));
            randNumberList.push_back(number);
        }

        randNumberFile.close();
    }
    else
    {
        cout << "Unable to open input file: " << fileName << endl;
    }
}
char *GetNextTokenInLine(string line, bool contd)
{
    if (contd == false)
    {

        cstr = new char[line.length() + 1];
        strcpy(cstr, line.c_str());
        char *token = strtok(cstr, " ");
        return token;
    }

    return strtok(NULL, " ");
};

void HandleSchedulerType(char *schedFlag)
{

    if (schedFlag[0] == 'F')
    {
        schedulerType = SchedType::FCFS;
        scheduler = new FCFSScheduler();
        return;
    }

    if (schedFlag[0] == 'L')
    {
        schedulerType = SchedType::LCFS;
        scheduler = new LCFSScheduler();
        return;
    }

    if (schedFlag[0] == 'S')
    {
        schedulerType = SchedType::SRTF;
        scheduler = new SRTFScheduler();
        return;
    }

    if (schedFlag[0] == 'R')
    {

        schedulerType = SchedType::RR;
        char *schedFlagSubstr = schedFlag + 1;
        quantum = atoi(schedFlagSubstr);
        scheduler = new RRScheduler();
        return;
    }

    if (schedFlag[0] == 'P')
    {

        schedulerType = SchedType::PRIO;
        char *schedFlagSubstr = schedFlag + 1;

        std::istringstream iss(schedFlagSubstr);
        char colon = ':';
        iss >> quantum >> colon >> maxprio;

        if (!iss)
        {
            quantum = atoi(schedFlagSubstr);
        }

        scheduler = new PRIOScheduler();
        return;
    }

    if (schedFlag[0] == 'E')
    {

        schedulerType = SchedType::PREPRIO;
        char *schedFlagSubstr = schedFlag + 1;
        std::istringstream iss(schedFlagSubstr);
        char colon = ':';
        iss >> quantum >> colon >> maxprio;

        if (!iss)
        {
            quantum = atoi(schedFlagSubstr);
        }
   
        scheduler = new PREPRIOScheduler();
        return;
    }
}

void RunSimulation()
{
    
    Event *evnt;
    Event *newEvent;
    while ((evnt = eventQManager.GetEvent()))
    {
        Process *proc = evnt->process;
        CURRENT_TIME = evnt->eventTimestamp;
        Transition_State transition = ProcessStateToTransitionState(evnt->newState);
        int timeInPrevState = CURRENT_TIME - proc->state_ts;
        eventQManager.RmEvent();

        switch (transition)
        {
        case TRANS_TO_READY:
        {
            proc->cpuWaitTime = proc->cpuWaitTime + timeInPrevState;
            proc->state = Process_State::READY;
            proc->state_ts = CURRENT_TIME;
            //ready can preempt here: because it can only be from creation or blocked
            //makes sense to put here because in the To_BLOCK trans it would basically just remove an event
            //at future ts but would fail if there was something at curr ts anyway
            CheckAndApplyPreemptivePriority(proc);

            scheduler->add_process(proc);
            CALL_SCHEDULER = true;
            break;
        }

        case TRANS_TO_PREEMPT:
        {
            proc->cpuWaitTime = proc->cpuWaitTime + timeInPrevState;
            proc->state = Process_State::PREEMPT;
            proc->state_ts = CURRENT_TIME;
            scheduler->add_process(proc);

            CALL_SCHEDULER = true;
            break;
        }

        case TRANS_TO_RUN:
        {
            // For blocking:
            if (proc->cpuBurstRemAfterQuantum == 0)
            {
                proc->cpuTimeDone = proc->cpuTimeDone + timeInPrevState;
                proc->state_ts = CURRENT_TIME;
                proc-> dynamicPriority = proc->staticPriority -1; //After IO the dynamic property is set to staticPriority - 1
                CURRENT_RUNNING_PROCESS = nullptr;
                if (proc->cpuTimeDone != proc->totalCPUTime)
                {
                    int randomIOBurst = myrandom(proc->ioBurst);
                    //-----For overlapping IO Utilization------
                    if (CURRENT_TIME < timeIOWillStopNext)
                    {
                        if (randomIOBurst <= (timeIOWillStopNext - CURRENT_TIME))
                        {
                            ioOverlap += randomIOBurst;
                        }
                        else
                        {
                            ioOverlap += (timeIOWillStopNext - CURRENT_TIME);
                            timeIOWillStopNext = CURRENT_TIME + randomIOBurst;
                        }
                    }
                    else
                    {
                        timeIOWillStopNext = CURRENT_TIME + randomIOBurst;
                    }

                    proc->state = Process_State::RUNNING;
                    newEvent = new Event(CURRENT_TIME + randomIOBurst, proc, evnt->newState, Process_State::BLOCKED);
                    eventQManager.PutEvent(newEvent);
                    CALL_SCHEDULER = true;
                    break;
                }

                else
                {
                    proc->finishTime = CURRENT_TIME;
                    PutDoneAwayProcess(proc);
                    CALL_SCHEDULER = true;
                    break;
                }
            }

            // For preemption:
            else{

                // if it is being preempted then we don't need to check the cpuTimeDOne because we chose a certain
                //cpuBurst according to the cpuTimeDone/left. So no need to check if the process is done or not cause that will only
                //happen for blocked.
                proc->cpuTimeDone = proc->cpuTimeDone + timeInPrevState;
                proc->state_ts = CURRENT_TIME;
                proc-> dynamicPriority -= 1;
                ResetDynamicPropertyIfRoundRobin(proc);
                CURRENT_RUNNING_PROCESS = nullptr;
                proc->state = Process_State::RUNNING;
                newEvent = new Event(CURRENT_TIME, proc, evnt->newState, Process_State::PREEMPT);
                eventQManager.PutEvent(newEvent);
                CALL_SCHEDULER = true;
                break;
            }
        }

        case TRANS_TO_BLOCK:
        {
            // modify ioBursttime here of process not when you start it.
            proc->ioTimeDone = proc->ioTimeDone + timeInPrevState;
            proc->state_ts = CURRENT_TIME;
            proc->state = Process_State::BLOCKED;
            newEvent = new Event(CURRENT_TIME, proc, evnt->newState, Process_State::READY);
            eventQManager.PutEvent(newEvent);
            CALL_SCHEDULER = true;
            break;
        }

        default:
            break;
        }

        if (CALL_SCHEDULER)
        {
            if (eventQManager.get_next_event_time() == CURRENT_TIME)
                continue; // process next event from Event queue

            CALL_SCHEDULER = false; // reset global flag
            if (CURRENT_RUNNING_PROCESS == nullptr)
            {
                CURRENT_RUNNING_PROCESS = scheduler->get_next_process();
                if (CURRENT_RUNNING_PROCESS == nullptr)
                    continue;

                int randomCPUBurst;
                int runningProcessRemCpuBurst = CURRENT_RUNNING_PROCESS->cpuBurstRemAfterQuantum;
                if (runningProcessRemCpuBurst > 0)
                {

                    randomCPUBurst = min(runningProcessRemCpuBurst, quantum);
                    CURRENT_RUNNING_PROCESS->cpuBurstRemAfterQuantum = abs(runningProcessRemCpuBurst - randomCPUBurst);
                }
                else
                {

                    int selectedRandomCpuBurst = SelectRandomCPUBurst(CURRENT_RUNNING_PROCESS->cpuBurst, CURRENT_RUNNING_PROCESS->totalCPUTime, CURRENT_RUNNING_PROCESS->cpuTimeDone);
                    randomCPUBurst = min(selectedRandomCpuBurst, quantum);
                    CURRENT_RUNNING_PROCESS->cpuBurstRemAfterQuantum = abs(selectedRandomCpuBurst - randomCPUBurst);
                }

                CURRENT_RUNNING_PROCESS->cpuWaitTime = CURRENT_RUNNING_PROCESS->cpuWaitTime + (CURRENT_TIME - (CURRENT_RUNNING_PROCESS->state_ts));
                CURRENT_RUNNING_PROCESS->state_ts = CURRENT_TIME;
                newEvent = new Event(CURRENT_TIME + randomCPUBurst, CURRENT_RUNNING_PROCESS, CURRENT_RUNNING_PROCESS->state, Process_State::RUNNING);
                eventQManager.PutEvent(newEvent);
            }
        }
    }
}

int SelectRandomCPUBurst(int cpuBurst, int totalCpuTime, int cpuTimeDone)
{

    int randomCPUBurst = myrandom(cpuBurst);
    if (randomCPUBurst > (totalCpuTime - cpuTimeDone))
    {
        return (totalCpuTime - cpuTimeDone);
    }
    else
    {
        return randomCPUBurst;
    }
}

void ResetDynamicPropertyIfRoundRobin(Process* process){
    
    if(schedulerType == SchedType::RR){

        process->dynamicPriority = process->staticPriority - 1;
    }
}

void CheckAndApplyPreemptivePriority(Process *proc)
{
    if (scheduler->test_preempt(proc, CURRENT_RUNNING_PROCESS))
    {
        // there is no point doing the event timestamp check if the dynamicPrio doesn't satisfy.
        // Hence no point calling the eventmanager unnecessarily
        Event *nextEventForCurrentRunningProcess = eventQManager.GetEventForProcess(CURRENT_RUNNING_PROCESS);
        if (nextEventForCurrentRunningProcess->eventTimestamp > CURRENT_TIME)
        {
            CURRENT_RUNNING_PROCESS->cpuTimeDone = CURRENT_RUNNING_PROCESS->cpuTimeDone + CURRENT_TIME - CURRENT_RUNNING_PROCESS->state_ts;
            CURRENT_RUNNING_PROCESS->state_ts = CURRENT_TIME;
            CURRENT_RUNNING_PROCESS->cpuBurstRemAfterQuantum += (nextEventForCurrentRunningProcess->eventTimestamp - CURRENT_TIME);

            CURRENT_RUNNING_PROCESS->dynamicPriority -= 1;
            ResetDynamicPropertyIfRoundRobin(CURRENT_RUNNING_PROCESS);
            CURRENT_RUNNING_PROCESS->state = Process_State::RUNNING;

            eventQManager.RmEvent(nextEventForCurrentRunningProcess);

            Event *newEvent = new Event(CURRENT_TIME, CURRENT_RUNNING_PROCESS, CURRENT_RUNNING_PROCESS->state, Process_State::PREEMPT);
            eventQManager.PutEvent(newEvent);

            CURRENT_RUNNING_PROCESS = nullptr;
        }
    }
}

void PrintOutput()
{

    int simulationFinishTime = CURRENT_TIME;
    int totalCpuTimeForAllProc = 0;
    int totalIoTime = 0;
    int totalTurnaroundTime = 0;
    int totalWaitTime = 0;

    cout << SchedulerNamespace::SchedTypeToString(schedulerType);
    if(schedulerType == SchedType::RR|| schedulerType == SchedType::PRIO || schedulerType == SchedType::PREPRIO){
        cout<<" "<<quantum<<endl;
    }
    else{
        cout<<endl;
    }
    std::list<Process *>::iterator it;
    for (it = DoneAwayProcessQ.begin(); it != DoneAwayProcessQ.end(); ++it)
    {
        Process *proc = *it;
        int pid = proc->pid;
        int arrivalTime = proc->arrivalTime;
        int totalCPUTime = proc->totalCPUTime;
        int cpuBurst = proc->cpuBurst;
        int ioBurst = proc->ioBurst;
        int staticPriority = proc->staticPriority;
        int finishTime = proc->finishTime;
        int turnaroundTime = finishTime - arrivalTime;
        int ioTimeDone = proc->ioTimeDone;
        int cpuWaitTime = proc->cpuWaitTime;

        totalCpuTimeForAllProc += totalCPUTime;
        totalTurnaroundTime += turnaroundTime;
        totalWaitTime += cpuWaitTime;
        totalIoTime += ioTimeDone;

        printf("%04d: %4d %4d %4d %4d %1d | %5d %5d %5d %5d\n", pid, arrivalTime, totalCPUTime, cpuBurst, ioBurst, staticPriority, finishTime, turnaroundTime, ioTimeDone, cpuWaitTime);
    }

    double cpu_util = 100.0 * (totalCpuTimeForAllProc / (double)simulationFinishTime);
    double io_util = 100.0 * ((totalIoTime - (ioOverlap)) / (double)simulationFinishTime);
    double throughput = 100.0 * (numOfProcesses / (double)simulationFinishTime);
    double avgTurnaroundTime = ((double)totalTurnaroundTime / numOfProcesses);
    double avgWaitTime = ((double)totalWaitTime / numOfProcesses);

    printf("SUM: %d %.2lf %.2lf %.2lf %.2lf %.3lf\n", simulationFinishTime, cpu_util, io_util, avgTurnaroundTime, avgWaitTime, throughput);
}

void PutDoneAwayProcess(Process *proc)
{
    if (!DoneAwayProcessQ.empty())
    {

        std::list<Process *>::iterator it = DoneAwayProcessQ.begin();
        for (int i = 0; it != DoneAwayProcessQ.end(); ++it, i++)
        {

            if (proc->pid < (*it)->pid)
            {
                std::list<Process *>::iterator insertPosition = std::next(DoneAwayProcessQ.begin(), i);
                DoneAwayProcessQ.insert(insertPosition, proc);
                return;
            }
        }
    }

    DoneAwayProcessQ.push_back(proc);
}