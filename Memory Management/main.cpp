
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
#include "../include/Pager.h"
#include "../include/Process.h"
#include "../include/FIFOPager.h"
#include "../include/ClockPager.h"
#include "../include/NRUPager.h"
#include "../include/AgingPager.h"
#include "../include/WorkingSetPager.h"
#include "../include/RandomPager.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <cstring>


using namespace std;


// change this, while reading
int num_of_frames;
int num_of_procs = -1;
vector<int> randNumberList;
static char *cstr;
// static std::deque<Process*> processQ;
static int pid = 0;
bool desc_info = false;
bool page_t_info = false;
bool frame_t_info = false;
bool stat_info = false;
bool num_of_procs_expected = true;
bool num_for_vmas_expected = false;
bool vma_spec = false;
bool instr_expected = false;
int instruction_number = 0;

//stats
typedef struct stats{
    int unmaps;
    int maps;
    int ins;
    int outs;
    int fins;
    int fouts;
    int zeros;
    int segv;
    int segprot; 
}stats;

static map<int, stats> ProcessStats;
int curr_proc_id;
int ctxtSwitchs = 0;
int reads = 0;
int writes = 0;
int exits = 0;

int cs_cycles = 130;
int read_write_cycles = 1;
int exit_cycles = 1230;
int maps=350;
int unmaps=410; 
int ins=3200;
int outs=2750;
int fins=2350;
int fouts=2800;
int zeros=150;
int segv=440;
int segprot=410;

unsigned long long total_cost = 0;

static Pager *pager;
//let's use map becuase while contxt switch, the lookup will be O(1)
static map<int, Process*> ProcessStore;
static Process *current_running_process = nullptr;


// Function declarations
char *GetNextTokenInLine(string line, bool contd);


void ReadAndLoadRandomNUmberFile(string fileName);
void HandlePagingAlgoType(char *algoFlag);
void HandlePrintingRequirements(char *printFlag);
void RunSimulation(char instr, int vpgae);
void PrintDescription(string printStr);
void PrintPageTableInfo();
void PrintFrameTableInfo();
void PrintStatInfo();

namespace PagingPolicyNamespace
{

    enum PagingAlgo
    {
        FIFO,
        CLOCK,
        RANDOM,
        NRU,
        AGING,
        WORKING_SET
    };

}

using namespace PagingPolicyNamespace;

static PagingAlgo paging_algo_type;

int main(int argc, char *argv[])
{
    int option;

    while ((option = getopt(argc, argv, "f:a:o:")) != -1)
    {
        switch (option)
        {
        case 'f':
            num_of_frames = stoi(optarg);
            break;
        case 'a':
            HandlePagingAlgoType(optarg);
            break;
        case 'o':
            HandlePrintingRequirements(optarg);
            break;
        case '?':
            if (optopt == 'f' || optopt == 'a' || optopt == 'o')
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
    ifstream processFile(inputFileName);
    if (processFile.is_open())
    {

        std::string line;
        int temp_proc_count = -1;
        int local_num_of_vmas = -1;
        while (getline(processFile, line))
        {

            if(num_of_procs_expected){
                sscanf(line.c_str(), "%d" , &num_of_procs);
                if(num_of_procs != -1){
                    temp_proc_count = num_of_procs;
                    num_of_procs_expected = false;
                    num_for_vmas_expected = true;
                    vma_spec = true;
                    continue;
                }
            }

            if (vma_spec && temp_proc_count != 0)
            {
                if (num_for_vmas_expected)
                {
                    sscanf(line.c_str(), "%d" , &local_num_of_vmas);
                    if(local_num_of_vmas != -1){
                        num_for_vmas_expected = false;
                    }

                }
                else{
                    
                    int start_vpage = 0;
                    int end_vpage = 0;
                    unsigned wp = 0;
                    unsigned fm = 0;
                    vector<VMA> vma_list;
                    for(int j = local_num_of_vmas ; j> 0 ; j--){
                        sscanf(line.c_str(), "%d %d %u %u" , &start_vpage, &end_vpage, &wp, &fm);
                        VMA vma = {start_vpage, end_vpage, wp, fm};
                        vma_list.push_back(vma);
                        getline(processFile, line);
                    }

                    Process *proc = new Process(pid, vma_list);
                    ProcessStore[pid] = proc;
                    ProcessStats[pid] = {0, 0, 0, 0, 0, 0, 0, 0, 0}; 
                    temp_proc_count--;
                    if(temp_proc_count != 0){
                        
                        local_num_of_vmas = -1;
                        num_for_vmas_expected = true;
                        pid++;
                    }
                    else{
                        instr_expected = true;
                        vma_spec = false;
                        num_for_vmas_expected = false;
                    }

                }

                continue;
            }


            if(instr_expected){
                //Now when we reach each instr we should run simulation after each.
                char instr;
                int instrVal = -1;
                sscanf(line.c_str(), "%c %d" , &instr, &instrVal);
                if(instrVal != -1 && instr != '#'){
                    PrintDescription(to_string(instruction_number)+": ==> "+line);
                    instruction_number++;
                    RunSimulation(instr, instrVal);
                }
            }


        }

        processFile.close();
        PrintPageTableInfo();
        PrintFrameTableInfo();
        PrintStatInfo();
    }

    else
    {

        cout << "Unable to open input file: " << inputFileName << endl;
    }

    return 0;
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
        pager->SetRandNumberFile(randNumberList);
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

void HandlePagingAlgoType(char *algoFlag)
{

    if (algoFlag[0] == 'f')
    {
        paging_algo_type = PagingAlgo::FIFO;
        pager = new FIFOPager(num_of_frames);
        return;
    }

    if (algoFlag[0] == 'c')
    {
        paging_algo_type = PagingAlgo::CLOCK;
        pager = new ClockPager(num_of_frames, &ProcessStore);
        return;
    }

    if (algoFlag[0] == 'r')
    {
        paging_algo_type = PagingAlgo::RANDOM;
        pager = new RandomPager(num_of_frames);
        return;
    }

    if (algoFlag[0] == 'e')
    {
        paging_algo_type = PagingAlgo::NRU;
        pager = new NRUPager(num_of_frames, &ProcessStore);
        return;
    }

    if (algoFlag[0] == 'a')
    {

        paging_algo_type = PagingAlgo::AGING;
        pager = new AgingPager(num_of_frames, &ProcessStore);
        return;
    }

    if (algoFlag[0] == 'w')
    {
        paging_algo_type = PagingAlgo::WORKING_SET;
        pager = new WorkingSetPager(num_of_frames, &ProcessStore);
        return;
    }

}

void HandlePrintingRequirements(char *printFlag)
{

    int i = -1;
    while (printFlag[i] != '\0')
    {
        i++;

        if (printFlag[i] == 'O')
        {
            desc_info = true;
            continue;
        }

        if (printFlag[i] == 'P')
        {
            page_t_info = true;
            continue;
        }

        if (printFlag[i] == 'F')
        {
            frame_t_info = true;
            continue;
        }

        if (printFlag[i] == 'S')
        {

            stat_info = true;
            continue;
        }
    }
}

void RunSimulation(char instr, int instrVal){
    
    switch(instr){
        case 'c':
            ctxtSwitchs ++;
            total_cost += cs_cycles;
            current_running_process = ProcessStore[instrVal];
            curr_proc_id = current_running_process->proc_id;
            break;
        case 'r':
        case 'w':
        {
            pte_t *pte = &current_running_process->page_table[instrVal];
            if ( ! pte->present) {
            // verify this is actually a valid page in a vma if not raise error and next inst
            if(!current_running_process->IsValidPageInVma(instrVal)){
                //raise error: write error here SEGV
                PrintDescription(" SEGV");
                ProcessStats[curr_proc_id].segv++;
                total_cost += segv;
                total_cost += read_write_cycles;
                if(instr == 'r') reads++ ;
                else writes++;
                break;
            }

            Frame *allocatedframe = pager->AllocateFrame(instruction_number);            
            if(allocatedframe->vpage == -1){
                //the frame was not allocated to anything so nothing to UNMAP
                if(current_running_process->IsFileMapped(instrVal)){
                    PrintDescription(" FIN");
                    ProcessStats[curr_proc_id].fins++;
                    total_cost += fins;
                }
                else{
                    if(!pte->paged_out) {
                        PrintDescription(" ZERO");
                        ProcessStats[curr_proc_id].zeros++;
                        total_cost += zeros;
                    }
                    else 
                    {
                        PrintDescription(" IN");
                        ProcessStats[curr_proc_id].ins++;
                        total_cost += ins;

                    }
                }
                
                //MAP now
                pte->page_frame_number = allocatedframe->frame_number;
                pte->present = 1;
                allocatedframe->proc_id = current_running_process->proc_id;
                allocatedframe->vpage = instrVal;
                string mapStr = " MAP "+ to_string(allocatedframe->frame_number);
                PrintDescription(mapStr);
                ProcessStats[curr_proc_id].maps++;
                total_cost += maps;
            }
            else{

                int oldVpage = allocatedframe->vpage;
                int oldProcId = allocatedframe->proc_id;
                Process* oldProc = ProcessStore[oldProcId];
                pte_t* oldPte = &oldProc->page_table[oldVpage];

                string unmapStr = " UNMAP "+ to_string(oldProcId)+ ":"+ to_string(oldVpage);
                PrintDescription(unmapStr);
                ProcessStats[oldProcId].unmaps++;
                total_cost += unmaps;

                if(oldPte->modified){
                    //if write: ref+mod: needs to be swapped to disk
                    if(oldProc->IsFileMapped(oldVpage)) {

                        PrintDescription(" FOUT");
                        ProcessStats[oldProcId].fouts++;
                        total_cost += fouts;
                    }
                    else {
                        
                        PrintDescription(" OUT");
                        ProcessStats[oldProcId].outs++;
                        total_cost += outs;
                        oldPte->paged_out = 1;
                    }
                }
                else if(oldPte->referenced && !oldPte->modified){
                    //if read was performed then we do nothing basically because we will have most recent image
                    //do nothing
                }
                //Reset oldPte
                oldPte->present = 0;
                oldPte->referenced = 0;
                oldPte->modified = 0;
                
                //now for new pte set bits and stuff: MAP and set
                if(current_running_process->IsFileMapped(instrVal)){
                    PrintDescription(" FIN");
                    ProcessStats[curr_proc_id].fins++;
                    total_cost += fins;

                }
                else{
                    if(!pte->paged_out) {
                        PrintDescription(" ZERO");
                        ProcessStats[curr_proc_id].zeros++;
                        total_cost += zeros;
                    }
                    else {
                        PrintDescription(" IN");
                        ProcessStats[curr_proc_id].ins++;
                        total_cost += ins;
                        }
                }

                // MAP now
                pte->page_frame_number = allocatedframe->frame_number;
                pte->present = 1;
                allocatedframe->proc_id = current_running_process->proc_id;
                allocatedframe->vpage = instrVal;
                string mapStr = " MAP " + to_string(allocatedframe->frame_number);
                PrintDescription(mapStr);
                ProcessStats[curr_proc_id].maps++;
                total_cost += maps;
            }

            }

            if (instr == 'r') 
            {   
                reads++;
                total_cost += read_write_cycles;
                pte->referenced = 1;
            }
            else
            {
                writes++;
                total_cost += read_write_cycles;
                if (current_running_process->IsWriteProtected(instrVal))
                {
                    // print ERROR: SEGPROT
                    PrintDescription(" SEGPROT");
                    ProcessStats[curr_proc_id].segprot++;
                    total_cost += segprot;
                    pte->referenced = 1;
                    pte->modified = 0;
                    pte->write_protect |= 1;
                }
                else
                {
                    pte->referenced = 1;
                    pte->modified = 1;
                }
            }

            break;

        }

        case 'e':
            exits++;
            total_cost += exit_cycles;
            PrintDescription("EXIT current process " + to_string(current_running_process->proc_id));
            for(int i = 0 ; i < current_running_process->MAX_VPAGES ; i++){

                pte_t *pte = &current_running_process->page_table[i];
                
                if(current_running_process->IsValidPageInVma(i)){
                    if(pte->present){

                        int frame_number_becoming_free = pte->page_frame_number;
                        
                        //UNMAP on exit()
                        string unmapStr = " UNMAP "+ to_string(current_running_process->proc_id)+ ":"+ to_string(i);
                        PrintDescription(unmapStr);
                        ProcessStats[current_running_process->proc_id].unmaps++;
                        total_cost += unmaps;

                        if(pte->modified && (current_running_process->IsFileMapped(i))){
                            PrintDescription(" FOUT");
                            ProcessStats[curr_proc_id].fouts++;
                            total_cost += fouts;
                        }

                        pte->present = 0;
                        
                        pager->frame_table[frame_number_becoming_free].proc_id = -1;
                        pager->frame_table[frame_number_becoming_free].vpage = -1;
                        pager->frame_table[frame_number_becoming_free].frameAge = 0;
                        pager->free_pool.push_back(&pager->frame_table[frame_number_becoming_free]);
                    }

                    pte->modified = 0;
                    pte->referenced = 0;
                    pte -> paged_out = 0;
                }

            }
            break;

        default:

            break;
        }

}

void PrintDescription(string printStr){

    if(desc_info){
        cout<<printStr<<endl;
    }
}

void PrintPageTableInfo()
{

    if (page_t_info)
    {

        for (int it = 0; it < num_of_procs ; it++)
        {
            
            int proc_id = it;
            Process *proc = ProcessStore[it];

            cout << "PT[" << proc_id << "]:";
            for (int i = 0; i < proc->MAX_VPAGES; i++)
            {
                if (!(proc->page_table[i].present))
                {
                    // the vpage is not valid
                    if (proc->page_table[i].paged_out == 1)
                    {
                        cout << " #";
                        continue;
                    }

                    else
                    {
                        cout << " *";
                        continue;
                    }
                }

                cout << " "<< i << ":";
                char ref;
                char mod;
                char swap;
                if(proc->page_table[i].referenced) cout<<"R";
                else cout<<"-";

                if(proc->page_table[i].modified) cout<<"M";
                else cout<<"-";

                if(proc->page_table[i].paged_out) cout<<"S";
                else cout<<"-";

            }

            cout<<endl;
        }

    }
}

void PrintFrameTableInfo()
{

    if (frame_t_info)
    {

        cout << "FT:";
        for (int i = 0; i < pager->num_of_frames ; i++)
        {
            if (pager->frame_table[i].vpage == -1)
            {

                cout << " *";
                continue;
            }

            cout << " "<< pager->frame_table[i].proc_id << ":" << pager->frame_table[i].vpage;
        }

        cout << endl;
    }
}

void PrintStatInfo()
{
    if (stat_info)
    {

        for (int it = 0; it < num_of_procs ; it++)
        {
            
            int proc_id = it;
            stats proc_stats = ProcessStats[it];

            cout << "PROC[" << proc_id << "]: ";
            cout<<"U="<<proc_stats.unmaps<<" ";
            cout<<"M="<<proc_stats.maps<<" ";
            cout<<"I="<<proc_stats.ins<<" ";
            cout<<"O="<<proc_stats.outs<<" ";
            cout<<"FI="<<proc_stats.fins<<" ";
            cout<<"FO="<<proc_stats.fouts<<" ";
            cout<<"Z="<<proc_stats.zeros<<" ";
            cout<<"SV="<<proc_stats.segv<<" ";
            cout<<"SP="<<proc_stats.segprot;

            cout<<endl;
        }


        int cs_cost = ctxtSwitchs*cs_cycles;
        int read_cost = reads*read_write_cycles;
        int write_cost = writes*read_write_cycles;
        int exit_cost = exits*exit_cycles;
        cout<<"TOTALCOST "<<(instruction_number)<<" "<<ctxtSwitchs<<" "<<exits<<" "<<total_cost<<" "<<sizeof(pte_t)<<endl;
    }

}