#include "Pager.h"
#include "Process.h"
#include <map>

#ifndef WORKINGSETPAGER_H
#define WORKINGSETPAGER_H

using namespace std;

class WorkingSetPager : public Pager {
public:
    int handIndex;
    int tau = 49;
    map<int, Process*>* ProcessDB;
    WorkingSetPager(int num_frames, map<int, Process*>* procDb);
    Frame* select_victim_frame(int instrNo = -1) override;
};

#endif 