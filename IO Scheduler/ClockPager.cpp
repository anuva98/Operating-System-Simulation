#include "../include/ClockPager.h"

ClockPager::ClockPager(int num_frames, map<int, Process*>* procDb){
    this->num_of_frames = num_frames;
    this->ProcessDB = procDb;
    handIndex = 0;
    PopulateFreePool();
}

Frame* ClockPager::select_victim_frame(int instrNo){
    if(handIndex == num_of_frames) handIndex = 0;

    int referenceBit = 1;
    Frame *selectedFrameToEvict;
    while (referenceBit)
    {
        if(handIndex == num_of_frames) handIndex = 0;

        int selectedVpage = frame_table[handIndex].vpage;
        int selectedProc = frame_table[handIndex].proc_id;
        for(int i = 0; i< (*ProcessDB).size() ; i++){

            if ((*ProcessDB)[i]->proc_id == selectedProc)
            {

                pte_t *selectedPte = &((*ProcessDB)[i]->page_table[selectedVpage]);
                int pteReferenceBit = selectedPte->referenced;
                referenceBit = pteReferenceBit;
                if(pteReferenceBit == 1) selectedPte->referenced = 0;
            }

        }
        
        selectedFrameToEvict = &frame_table[handIndex];
        handIndex++;
    }
    
    handIndex = selectedFrameToEvict->frame_number + 1;
    return selectedFrameToEvict;
}