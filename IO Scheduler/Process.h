#include <iostream>
#include <vector>
#include "VMA.h"
#include "pte_t.h"


#ifndef PROCESS_H
#define PROCESS_H

using namespace std;

class Process {

    public:
        vector<VMA> vma_list;
        const static int MAX_VPAGES = 64;
        PTE page_table[MAX_VPAGES] = {};
        int proc_id;
        Process(int proc_id, std::vector<VMA> list_of_vmas);
        //if page_table[] empty then create all zero entries else reset all bits
        void ZeroPageTable();
        bool IsValidPageInVma(int vpage);
        bool IsFileMapped(int vpage);
        bool IsWriteProtected(int vpage);
};

#endif 