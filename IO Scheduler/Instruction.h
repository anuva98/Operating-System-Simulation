#include <iostream>
#include <string>

#ifndef INSTRUCTION_H
#define INSTRUCTION_H

typedef struct Instruction{
    int instruction_id;
    int arrival_time;
    int track_accessed;
    int startTime;
    int endTime;
}Instruction;

#endif 