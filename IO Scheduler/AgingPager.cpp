#include "../include/AgingPager.h"

AgingPager::AgingPager(int num_frames, map<int, Process*>* procDb){
    this->num_of_frames = num_frames;
    this->ProcessDB = procDb;
    handIndex = 0;
    PopulateFreePool();
}

Frame* AgingPager::select_victim_frame(int instrNo){

    if(handIndex == num_of_frames) handIndex = 0;
    bool selectedAgeSet = false;
    Frame *selectedFrameToEvict;
    uint32_t selectedAge;
    for(int i = 0; i< num_of_frames ; i++){

        if(handIndex == num_of_frames) handIndex = 0;

        int selectedVpage = frame_table[handIndex].vpage;
        int selectedProc = frame_table[handIndex].proc_id;
        Process* proc = (*ProcessDB)[selectedProc];
        pte_t *selectedPte = &(proc->page_table[selectedVpage]);
        Frame* tempSelection = &frame_table[handIndex];

        tempSelection->frameAge = tempSelection->frameAge >> 1;
        if(selectedPte->referenced == 1) tempSelection->frameAge |= 0x80000000;
        selectedPte->referenced = 0;

        if(selectedAgeSet == false || tempSelection->frameAge < selectedAge) {
            selectedFrameToEvict = tempSelection;
            selectedAge = tempSelection->frameAge;
            selectedAgeSet = true;
        }
        
        handIndex++;

    }
    
    handIndex = selectedFrameToEvict->frame_number % num_of_frames + 1;
    ResetAgeOfFrame(selectedFrameToEvict->frame_number);
    return selectedFrameToEvict;
}