#include <iostream>

#ifndef PTE_H
#define PTE_H

using namespace std;

typedef struct pte_t{
    unsigned present:1;
    unsigned referenced:1;
    unsigned modified:1;
    unsigned write_protect:1;
    unsigned paged_out:1;
    unsigned page_frame_number:7;
    unsigned util_number:20;
}PTE;

#endif 
