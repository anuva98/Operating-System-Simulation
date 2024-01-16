#include <iostream>
#include "FrameTableEntry.h"
#include <deque>
#include <vector>

#ifndef PAGER_H
#define PAGER_H

using namespace std;

class Pager {

    public:

        const static int MAX_FRAMES = 128;
        int num_of_frames;
        deque<Frame*> free_pool;
        Frame frame_table[MAX_FRAMES] = {};
        virtual Frame* select_victim_frame(int instrNo = -1);
        //we populate with proc_id=-1 (because the frame doesn't belong to any proc right now and frame# = 0 to num_frames-1)
        void PopulateFreePool();
        Frame* AllocateFrame(int instrNo = -1);
        virtual void ResetAgeOfFrame(int frameNum);
        virtual void SetRandNumberFile (std::vector<int> randNumFile);
};

#endif 