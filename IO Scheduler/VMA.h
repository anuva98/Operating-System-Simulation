#ifndef VMA_H
#define VMA_H

using namespace std;

typedef struct VMA{
    int start_vpage;
    int end_vpage;
    unsigned write_protected:1;
    unsigned file_mapped:1;
}VMA;

#endif 