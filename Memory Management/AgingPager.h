#include "Pager.h"
#include "Process.h"
#include <map>

#ifndef AGINGPAGER_H
#define AGINGPAGER_H

using namespace std;

class AgingPager : public Pager {
public:
    int handIndex;
    map<int, Process*>* ProcessDB;
    AgingPager(int num_frames, map<int, Process*>* procDb);
    Frame* select_victim_frame(int instrNo = -1) override;
};

#endif 