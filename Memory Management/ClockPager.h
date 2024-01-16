#include "Pager.h"
#include "Process.h"
#include <map>

#ifndef CLOCKPAGER_H
#define CLOCKPAGER_H

using namespace std;

class ClockPager : public Pager {
public:
    int handIndex;
    map<int, Process*>* ProcessDB;
    ClockPager(int num_frames, map<int, Process*>* procDb);
    Frame* select_victim_frame(int instrNo = -1) override;
};

#endif 