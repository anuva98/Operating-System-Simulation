#include "../include/FIFOPager.h"

FIFOPager::FIFOPager(int num_frames){
    this->num_of_frames = num_frames;
    handIndex = 0;
    PopulateFreePool();
}

Frame* FIFOPager::select_victim_frame(int instrNo){
    if(handIndex == num_of_frames) handIndex = 0;
    Frame* selectedFrameToEvict = &frame_table[handIndex];
    handIndex++;
    return selectedFrameToEvict;
}