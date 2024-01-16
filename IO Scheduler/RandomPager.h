#include "Pager.h"

#ifndef RANDOMPAGER_H
#define RANDOMPAGER_H

using namespace std;

class RandomPager : public Pager {
public:
    RandomPager(int num_frames);
    int ofs;
    std::vector<int> randNumberList;
    Frame* select_victim_frame(int instrNo = -1) override;
    int myrandom();
    void SetRandNumberFile(vector<int> randNumFile) override;
};

#endif 