#include "Pager.h"
#include "Process.h"
#include <map>

#ifndef NRUPAGER_H
#define NRUPAGER_H

using namespace std;

class NRUPager : public Pager {
public:
    int handIndex;
    int lastInstructionWhenRefBitWasSet;
    int tau = 48;
    map<int, Process*>* ProcessDB;
    NRUPager(int num_frames, map<int, Process*>* procDb);
    Frame* select_victim_frame(int instrNo = -1) override;
};

#endif 