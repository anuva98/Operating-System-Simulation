#include <iostream>

#ifndef FRAMETABLEENETRY_H
#define FRAMETABLEENETRY_H

using namespace std;

typedef struct FrameTableEntry {

    int proc_id;
    int vpage;
    int frame_number;
    uint32_t frameAge;
    
}Frame;

#endif 