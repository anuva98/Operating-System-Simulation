#include "../include/Process.h"


Process::Process(int proc_id, vector<VMA> list_of_vmas){
    this->proc_id = proc_id;
    this->vma_list = list_of_vmas;
    Process::ZeroPageTable();
}

void Process::ZeroPageTable(){

    PTE default_PTE = {0, 0, 0, 0 , 0, 0, 0};

    std::fill_n(this->page_table, MAX_VPAGES, default_PTE);
}

bool Process::IsValidPageInVma(int vpage){

    for (vector<VMA>::iterator it = vma_list.begin(); it != vma_list.end(); ++it) {
        
        if(vpage >= it->start_vpage & vpage <= it->end_vpage) return true;
    }

    return false;

}

bool Process::IsFileMapped(int vpage){

    for (vector<VMA>::iterator it = vma_list.begin(); it != vma_list.end(); ++it) {
        
        if(vpage >= it->start_vpage && vpage <= it->end_vpage) {
            
            if(it->file_mapped == (unsigned) 1) return true;
        
        }
    }

    return false;
}

bool Process::IsWriteProtected(int vpage){

    for (vector<VMA>::iterator it = vma_list.begin(); it != vma_list.end(); ++it) {
        
        if(vpage >= it->start_vpage && vpage <= it->end_vpage) {
            
            if(it->write_protected == (unsigned) 1) return true;
        
        }
    }

    return false;
}