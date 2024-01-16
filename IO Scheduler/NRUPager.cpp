#include "../include/NRUPager.h"

NRUPager::NRUPager(int num_frames, map<int, Process*>* procDb){
    this->num_of_frames = num_frames;
    this->ProcessDB = procDb;
    handIndex = 0;
    lastInstructionWhenRefBitWasSet = 0;
    PopulateFreePool();
}

Frame* NRUPager::select_victim_frame(int instrNo){

    if(handIndex == num_of_frames) handIndex = 0;
    bool refBitNeedsToBeReset = false;

    if((instrNo - lastInstructionWhenRefBitWasSet) >= tau) {
        refBitNeedsToBeReset = true;
        lastInstructionWhenRefBitWasSet = instrNo;
    }
    
    Frame *selectedFrameToEvict;
    int evictionFrameClass = 4;
    for(int i = 0; i< num_of_frames ; i++){

        if(handIndex == num_of_frames) handIndex = 0;
        int selectedVpage = frame_table[handIndex].vpage;
        int selectedProc = frame_table[handIndex].proc_id;
        Process* proc = (*ProcessDB)[selectedProc];
        pte_t *selectedPte = &(proc->page_table[selectedVpage]);
        int calcClass = 2* selectedPte->referenced + selectedPte->modified;
        if(calcClass < evictionFrameClass){
            selectedFrameToEvict = &frame_table[handIndex];
            evictionFrameClass = calcClass;
            if(calcClass == 0){
        
                if(!refBitNeedsToBeReset) {
                    handIndex++;
                    break;
                }
            }
        }
        if(refBitNeedsToBeReset) selectedPte->referenced = 0;
        handIndex++;

    }
    
    handIndex = selectedFrameToEvict->frame_number % num_of_frames + 1;
    return selectedFrameToEvict;
}