
#include <iostream>
#include <sstream>
#include <string>
#include <fstream>
#include <deque>
#include <list>
#include <unistd.h>
#include <map>
#include <cstdlib>
#include <cstdio>
#include <vector>
#include "../include/Instruction.h"
#include "../include/FIFOScheduler.h"
#include "../include/SSTFScheduler.h"
#include "../include/LookScheduler.h"
#include "../include/FLookScheduler.h"
#include "../include/CLookScheduler.h"
#include "../include/Scheduler.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <iomanip>


using namespace std;

int current_time = 0;
int io_util_time = 0;
int total_turnaround_time = 0;
int last_instruction_end_time = 0;
int total_wait_time = 0;
int current_head_position =0;
int last_head_position =0;
static int instruction_number = 0;
int total_head_movement = 0;
int max_wait_time = 0;
static Instruction* current_instruction = nullptr;
list<Instruction> instructions;

static Scheduler *scheduler;
static map<int, Instruction*> instruction_stats;



// Function declarations
void HandleAlgoType(char *algoFlag);
void RunSimulation();
void PrintDescription();
void PrintStatInfo();

namespace SchedulerNamespace
{

    enum IOAlgo
    {
        FIFO,
        SSTF,
        LOOK,
        CLOOK,
        FLOOK
    };

}

using namespace SchedulerNamespace;

static IOAlgo io_algo_type;

int main(int argc, char *argv[])
{
    int option;

    while ((option = getopt(argc, argv, "s:v:q:f:")) != -1)
    {
        switch (option)
        {
        case 'v':
            // do nothing
            break;
        case 'q':
            // do nothing
            break;
        case 'f':
            // do nothing
            break;
        case 's':
            HandleAlgoType(optarg);
            break;
        case '?':
            if (optopt == 's')
            {
                std::cerr << "Option -" << static_cast<char>(optopt) << " requires a valid argument." << std::endl;
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
    for (int i = optind; i < argc; ++i)
    {

        inputFileName = argv[i];
        break;
    }

    ifstream processFile(inputFileName);
    if (processFile.is_open())
    {

        std::string line;
        int timeOfIssue_arrTime = 0;
        int trackAccessed = 0;
        while (getline(processFile, line))
        {

            if(sscanf(line.c_str(), "%d %d" , &timeOfIssue_arrTime, &trackAccessed) == 2 )
            {
                Instruction instr = {instruction_number, timeOfIssue_arrTime, trackAccessed, -1, -1};
                instructions.push_back(instr);
                instruction_number++;
            }
            
        }

        processFile.close();
        RunSimulation();
    }

    else
    {

        cout << "Unable to open input file: " << inputFileName << endl;
    }

    return 0;
}

void HandleAlgoType(char *algoFlag)
{

    if (algoFlag[0] == 'N')
    {
        io_algo_type = IOAlgo::FIFO;
        scheduler = new FIFOScheduler();
        return;
    }

    if (algoFlag[0] == 'S')
    {
        io_algo_type = IOAlgo::SSTF;
        scheduler = new SSTFScheduler();
        return;
    }

    if (algoFlag[0] == 'L')
    {
        io_algo_type = IOAlgo::LOOK;
        scheduler = new LookScheduler();
        return;
    }

    if (algoFlag[0] == 'C')
    {
        io_algo_type = IOAlgo::CLOOK;
        scheduler = new CLookScheduler();
        return;
    }

    if (algoFlag[0] == 'F')
    {

        io_algo_type = IOAlgo::FLOOK;
        scheduler = new FLookScheduler();
        return;
    }

}

void RunSimulation(){

    while(true){

        if(!instructions.empty() && instructions.front().arrival_time == current_time){

            scheduler->add_IO_instruction(&instructions.front());
            instructions.pop_front();
        }

        if(current_instruction != nullptr && current_instruction->endTime == current_time){
            
            instruction_stats[current_instruction->instruction_id] = current_instruction;
            current_instruction = nullptr;
        }

        if(current_instruction == nullptr){
            Instruction* nextInstruction = scheduler->get_next_IO_instruction(current_head_position, last_head_position, instructions.empty());
            
            if(nextInstruction != nullptr){

                nextInstruction->startTime = current_time;
               
                int movement = abs(nextInstruction->track_accessed - current_head_position);
                total_head_movement+= movement;
                nextInstruction->endTime = nextInstruction->startTime + movement;
                last_instruction_end_time = nextInstruction->endTime;
                int turnaround_time = (nextInstruction->endTime - nextInstruction->arrival_time);
                total_turnaround_time += turnaround_time;
                io_util_time += (nextInstruction->endTime - nextInstruction->startTime);
                int wait_time = (current_time - nextInstruction->arrival_time);
                total_wait_time += wait_time;
                if(wait_time > max_wait_time) max_wait_time = wait_time;
                current_instruction = nextInstruction;
            }
            else{
                // "Queue needs more requests."

                if(instructions.empty()){

                    // "No new instructions will be added. Exit simulation."
                    break;
                }
            }
        }

        if(current_instruction != nullptr && current_instruction->endTime == current_time) continue;

        if(current_instruction != nullptr){
            // "Increasing head."
            last_head_position = current_head_position;
            if(current_instruction->track_accessed < current_head_position) current_head_position--;
            if(current_instruction->track_accessed > current_head_position) current_head_position++;
        }

        // "Increasing time."
        current_time++;
    }

    // "Instructions ended."
    PrintDescription();
    PrintStatInfo();
}

void PrintDescription(){

    int local_id = 0;
    while(local_id < instruction_number){

        Instruction* ins = instruction_stats[local_id];
        printf("%5d: %5d %5d %5d\n", ins->instruction_id, ins->arrival_time, ins->startTime, ins->endTime);
        local_id++;
    }

}

void PrintStatInfo()
{
    double io_util_ratio = double (io_util_time)/last_instruction_end_time;
    double avg_turnaround_time = double (total_turnaround_time) / (instruction_number);
    double avg_wait_time = double (total_wait_time) / (instruction_number);
    printf("SUM: %d %d %.4lf %.2lf %.2lf %d\n", last_instruction_end_time, total_head_movement, io_util_ratio, avg_turnaround_time, avg_wait_time, max_wait_time);

}