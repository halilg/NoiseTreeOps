#ifndef CheckMask_h_
#define CheckMask_h_

#include <cassert>

//
// A filter for use with automatic ntuples. Will check a value
// of a mask vector element
//
// I. Volobouev
// November 2013
//

class CheckMask
{
public:
    inline CheckMask(const std::vector<unsigned char>* a, const bool enabled)
        : arr_(a), enabled_(enabled) {if (enabled_) assert(arr_);}

    inline bool operator()(unsigned i) const
    {
        if (enabled_)
            return (*arr_)[i];
        else
            return true;
    }

private:
    CheckMask();

    const std::vector<unsigned char>* arr_;
    bool enabled_;
};

#endif // CheckMask_h_
