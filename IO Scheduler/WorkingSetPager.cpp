#include "../include/WorkingSetPager.h"
#include <climits>

WorkingSetPager::WorkingSetPager(int num_frames, map<int, Process*>* procDb){
    this->num_of_frames = num_frames;
    this->ProcessDB = procDb;
    handIndex = 0;
    PopulateFreePool();
}

Frame* WorkingSetPager::select_victim_frame(int instrNo){

    if(handIndex == num_of_frames) handIndex = 0;
    int handIndexPlaceholder = handIndex;
    Frame *selectedFrameToEvict = nullptr;
    int minAge = INT_MAX;
    for (int i = 0; i < num_of_frames; i++)
    {
        if (handIndex == num_of_frames)
        {
            handIndex = 0;
        }

        int selectedVpage = frame_table[handIndex].vpage;
        int selectedProc = frame_table[handIndex].proc_id;
        Process *proc = (*ProcessDB)[selectedProc];
        pte_t *selectedPte = &(proc->page_table[selectedVpage]);
        Frame *tempSelection = &frame_table[handIndex];

        if (selectedPte->referenced == 1)
        {
            tempSelection->frameAge = instrNo;
            selectedPte->referenced = 0;
            handIndex++;
        }

        else
        {

            if (instrNo - tempSelection->frameAge > tau)
            {
                selectedFrameToEvict = tempSelection;
                handIndex++;
                break;
            }
            else
            {
                if (int(tempSelection->frameAge) < minAge)
                {
                    minAge = tempSelection->frameAge;
                    selectedFrameToEvict = tempSelection;
                    
                }

                handIndex++;
            }
        }
    }

    if(selectedFrameToEvict == nullptr) selectedFrameToEvict = &frame_table[handIndexPlaceholder];
    handIndex = selectedFrameToEvict->frame_number % num_of_frames + 1;
    selectedFrameToEvict->frameAge = instrNo;
    return selectedFrameToEvict;
}