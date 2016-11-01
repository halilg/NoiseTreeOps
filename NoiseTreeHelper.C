#include "NoiseTreeHelper.h"

NoiseTreeHelper::NoiseTreeHelper(TTree *tree)
    : HcalNoiseTree(tree),
      eMinTS_(3U),
      eMaxTS_(8U)
{
}

double NoiseTreeHelper::energy(const unsigned idx) const
{
    assert(idx < static_cast<unsigned>(PulseCount));

    const double* q = &Charge[idx][0];
    const double* ped = &Pedestal[idx][0];
    const double* g = &Gain[idx][0];

    double e = 0.0;
    for (unsigned i=eMinTS_; i<eMaxTS_; ++i)
        e += (q[i] - ped[i])*g[i];

    return e;
}
