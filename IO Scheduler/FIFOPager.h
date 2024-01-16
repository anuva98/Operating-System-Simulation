#include "Pager.h"

#ifndef FIFOPAGER_H
#define FIFOPAGER_H

using namespace std;

class FIFOPager : public Pager {
public:
    int handIndex;
    FIFOPager(int num_frames);
    Frame* select_victim_frame(int instrNo = -1) override;
};

#endif 