#include "../include/Pager.h"

Frame* Pager::select_victim_frame(int instrNo){}
void Pager::SetRandNumberFile(vector<int> randNumFile){
    // Do nothing
};

void Pager::PopulateFreePool(){

    //we also add those frames in frame table entry and then populate free_pool with pointers to those
    for(int i = 0; i <num_of_frames ; i++){
        Frame newFrame;
        newFrame.proc_id = -1;
        newFrame.vpage = -1;
        newFrame.frame_number = i;
        newFrame.frameAge = 0;
        frame_table[i] = newFrame;
        free_pool.push_back(&frame_table[i]);
    }
}

Frame* Pager::AllocateFrame(int instrNo){

    if(!free_pool.empty()) {
        Frame* frame_t = free_pool.front();
        free_pool.pop_front();
        return frame_t;
    }
    
    return select_victim_frame(instrNo);
}

void Pager::ResetAgeOfFrame(int frameNum){
    frame_table[frameNum].frameAge = 0;
}