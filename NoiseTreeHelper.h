#ifndef NoiseTreeHelper_h_
#define NoiseTreeHelper_h_

#include <cassert>
#include "HcalNoiseTree.h"

// Number of time slices
#define N_TIME_SLICES 10U

class NoiseTreeHelper : public HcalNoiseTree
{
public:
    NoiseTreeHelper(TTree *tree=0);
    inline virtual ~NoiseTreeHelper() {}

    // Set the min/max time slices for energy determination.
    // Min time slice will be included and max excluded.
    inline void setEMinMaxTS(const unsigned tsMin, const unsigned tsMax) 
    {
        assert(tsMin < tsMax);
        assert(tsMax <= N_TIME_SLICES);
        eMinTS_ = tsMin;
        eMaxTS_ = tsMax;
    }

    inline unsigned eMinTS() const {return eMinTS_;}
    inline unsigned eMaxTS() const {return eMaxTS_;}

    // Energy is calculated a-la "Method 0"
    double energy(unsigned channelIndex) const;

private:
    unsigned eMinTS_;
    unsigned eMaxTS_;
};

#endif // NoiseTreeHelper_h_
