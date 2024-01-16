#include "../include/RandomPager.h"

RandomPager::RandomPager(int num_frames){
    this->num_of_frames = num_frames;    
    this->ofs = 0;
    PopulateFreePool();
}

Frame* RandomPager::select_victim_frame(int instrNo){
    int rof = myrandom();
    Frame* selectedFrameToEvict = &frame_table[rof];
    return selectedFrameToEvict;
}

int RandomPager::myrandom()
{
    if (ofs >= randNumberList[0]) ofs = 1;
    else ofs++;
    return (randNumberList[ofs] % num_of_frames);
}

void RandomPager::SetRandNumberFile(vector<int> randNumFile){
    this->randNumberList = randNumFile;
}