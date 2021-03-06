#pragma once

#include "util.h"

class Quest
{
        Lesson * pl;
        int item;
        int ip;
        int quse; // 0 trn or 1 lis

        void expect(uchar c, double bkpenms, double time_window);
        ///void expect(uchar c, double bkpenms, char prompt);
        string selector1(Edu edu);
        string selector2(Edu edu);
        void update(float x);
        bool adjust_prn(int k);

    public:
        int qstat[2];

        Quest(Lesson * les, int itm, int q);
        ///void ask(bool letsel2, Edu edu);
        void ask(bool letsel2);
        bool done() const;
        bool last();
};
