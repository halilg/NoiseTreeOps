#include <set>
#include <cmath>
#include <cassert>
#include <algorithm>
#include <stdexcept>

#include "HBHEChannelMap.h"

void HBHEChannelMap::getChannelTriple(const unsigned index, unsigned* depth,
                                      int* ieta, unsigned* iphi) const
{
    if (index >= ChannelCount)
        throw std::out_of_range("In HBHEChannelMap::getChannelTriple: "
                                "input index out of range");
    const HBHEChannelId& id = lookup_[index];
    if (depth)
        *depth = id.depth();
    if (ieta)
        *ieta = id.ieta();
    if (iphi)
        *iphi = id.iphi();
}

void HBHEChannelMap::channelSetNeighbors(const std::vector<unsigned>& input,
                                         std::vector<unsigned>* output) const
{
    assert(output);
    output->clear();
    const unsigned nIn = input.size();
    assert(nIn <= 18);

    unsigned cands[18*8];
    unsigned nCands = 0;
    for (unsigned inp=0; inp<nIn; ++inp)
    {
        const std::vector<unsigned>& chNeighbors(
            channelNeigborsFromOtherHPDs(input[inp]));
        const unsigned nNeighbors = chNeighbors.size();
        assert(nNeighbors <= 8);
        for (unsigned ineib=0; ineib<nNeighbors; ++ineib)
            cands[nCands++] = chNeighbors[ineib];
    }
    std::sort(cands, cands+nCands);
    std::unique_copy(cands, cands+nCands, std::back_inserter(*output));
}

void HBHEChannelMap::calculateHPDNeighbors(const unsigned hpd,
                                           std::vector<unsigned>* vec) const
{
    std::set<unsigned> neighborSet;

    const std::vector<unsigned>& hpdChannels(hpd_channel_lookup_[hpd]);
    const unsigned nChan = hpdChannels.size();
    for (unsigned ichan=0; ichan<nChan; ++ichan)
    {
        const std::vector<unsigned>& chNeighbors(
            channelNeigborsFromOtherHPDs(hpdChannels[ichan]));
        const unsigned nNeighbors = chNeighbors.size();
        for (unsigned ineib=0; ineib<nNeighbors; ++ineib)
            neighborSet.insert(chNeighbors[ineib]);
    }

    vec->clear();
    vec->reserve(neighborSet.size());
    const std::set<unsigned>::const_iterator iend = neighborSet.end();
    for (std::set<unsigned>::const_iterator it = neighborSet.begin();
         it != iend; ++it)
        vec->push_back(*it);
    std::sort(vec->begin(), vec->end());
}

void HBHEChannelMap::calculateNeighborList(const unsigned index,
                                           std::vector<unsigned>* vec) const
{
    unsigned neighborChannels[8];
    unsigned nNeighbors = 0;

    const unsigned depth = lookup_[index].depth();
    const int eta0 = lookup_[index].ieta();
    const int phi0 = lookup_[index].iphi();
    const unsigned myHPD = getHPD(index);

    for (int etaShift=-1; etaShift<2; ++etaShift)
    {
        int iEta = eta0 + etaShift;
        // Jump over 0
        if (iEta == 0)
            iEta += etaShift;
        for (int phiShift=-1; phiShift<2; ++phiShift)
            if (etaShift || phiShift)
            {
                // Wrap 0 to 72 and 73 to 1
                int iPhi = phi0 + phiShift;
                assert(iPhi >= 0 && iPhi <= 73);
                if (iPhi == 0)
                    iPhi = 72;
                else if (iPhi == 73)
                    iPhi = 1;
                ChannelMap::const_iterator it = inverse_.find(HBHEChannelId(depth, iEta, iPhi));
                if (it != inverse_.end())
                {
                    const unsigned neighbor = it->second;
                    if (myHPD != getHPD(neighbor))
                        neighborChannels[nNeighbors++] = neighbor;
                }
            }
    }

    vec->clear();
    vec->reserve(nNeighbors);
    for (unsigned i=0; i<nNeighbors; ++i)
        vec->push_back(neighborChannels[i]);
    std::sort(vec->begin(), vec->end());
}

const std::vector<unsigned>& HBHEChannelMap::channelNeigborsFromOtherHPDs(
    const unsigned index) const
{
    if (index >= ChannelCount)
        throw std::out_of_range("In HBHEChannelMap::channelNeigborsFromOtherHPDs: "
                                "input index out of range");
    if (!neighborInfoFilled_)
    {
        HBHEChannelMap* m = const_cast<HBHEChannelMap*>(this);
        for (unsigned i=0; i<ChannelCount; ++i)
            calculateNeighborList(i, &m->channel_neighbors_[i]);
        m->neighborInfoFilled_ = true;
    }
    return channel_neighbors_[index];
}

const std::vector<unsigned>& HBHEChannelMap::getHPDNeigbors(
    const unsigned hpd) const
{
    const unsigned hpdMax = static_cast<unsigned>(HcalHPDRBXMap::NUM_HPDS);
    if (hpd >= hpdMax)
        throw std::out_of_range("In HBHEChannelMap::getHPDNeigbors: "
                                "input index out of range");
    if (!hpdNeighborsFilled_)
    {
        HBHEChannelMap* m = const_cast<HBHEChannelMap*>(this);
        for (unsigned i=0; i<hpdMax; ++i)
            calculateHPDNeighbors(i, &m->hpd_neighbors_[i]);
        m->hpdNeighborsFilled_ = true;
    }
    return hpd_neighbors_[hpd];
}

unsigned HBHEChannelMap::getHPD(const unsigned index) const
{
    if (index >= ChannelCount)
        throw std::out_of_range("In HBHEChannelMap::getHPD: "
                                "input index out of range");
    return hpd_lookup_[index];
}

unsigned HBHEChannelMap::getChannelInHPD(const unsigned index) const
{
    if (index >= ChannelCount)
        throw std::out_of_range("In HBHEChannelMap::getChannelInHPD: "
                                "input index out of range");
    return chan_in_hpd_lookup_[index];
}

unsigned HBHEChannelMap::getRBX(const unsigned index) const
{
    if (index >= ChannelCount)
        throw std::out_of_range("In HBHEChannelMap::getRBX: "
                                "input index out of range");
    return rbx_lookup_[index];
}

unsigned HBHEChannelMap::getChannelInRBX(const unsigned index) const
{
    if (index >= ChannelCount)
        throw std::out_of_range("In HBHEChannelMap::getChannelInRBX: "
                                "input index out of range");
    return chan_in_rbx_lookup_[index];
}

bool HBHEChannelMap::isValidTriple(const unsigned depth, const int ieta,
                                   const unsigned iphi) const
{
    ChannelMap::const_iterator it = inverse_.find(HBHEChannelId(depth, ieta, iphi));
    return it != inverse_.end();
}

unsigned HBHEChannelMap::linearIndex(const unsigned depth, const int ieta,
                                     const unsigned iphi) const
{
    ChannelMap::const_iterator it = inverse_.find(HBHEChannelId(depth, ieta, iphi));
    if (it == inverse_.end())
        throw std::invalid_argument("In HBHEChannelMap::linearIndex: "
                                    "invalid channel triple");
    return it->second;
}

HBHEChannelMap::HBHEChannelMap()
    : hpd_channel_lookup_(HcalHPDRBXMap::NUM_HPDS),
      rbx_channel_lookup_(HcalHPDRBXMap::NUM_RBXS),
      neighborInfoFilled_(false),
      hpdNeighborsFilled_(false)
{
    unsigned l = 0;
    unsigned depth = 1;

    for (int ieta = -29; ieta <= -21; ++ieta)
        for (unsigned iphi=1; iphi<72; iphi+=2)
            lookup_[l++] = HBHEChannelId(depth, ieta, iphi);

    for (int ieta = -20; ieta <= 20; ++ieta)
        if (ieta)
            for (unsigned iphi=1; iphi<=72; ++iphi)
                lookup_[l++] = HBHEChannelId(depth, ieta, iphi);

    for (int ieta = 21; ieta <= 29; ++ieta)
        for (unsigned iphi=1; iphi<72; iphi+=2)
            lookup_[l++] = HBHEChannelId(depth, ieta, iphi);

    depth = 2;

    for (int ieta = -29; ieta <= -21; ++ieta)
        for (unsigned iphi=1; iphi<72; iphi+=2)
            lookup_[l++] = HBHEChannelId(depth, ieta, iphi);

    for (int ieta = -20; ieta <= -18; ++ieta)
        for (unsigned iphi=1; iphi<=72; ++iphi)
            lookup_[l++] = HBHEChannelId(depth, ieta, iphi);

    for (int ieta = -16; ieta <= -15; ++ieta)
        for (unsigned iphi=1; iphi<=72; ++iphi)
            lookup_[l++] = HBHEChannelId(depth, ieta, iphi);

    for (int ieta = 15; ieta <= 16; ++ieta)
        for (unsigned iphi=1; iphi<=72; ++iphi)
            lookup_[l++] = HBHEChannelId(depth, ieta, iphi);

    for (int ieta = 18; ieta <= 20; ++ieta)
        for (unsigned iphi=1; iphi<=72; ++iphi)
            lookup_[l++] = HBHEChannelId(depth, ieta, iphi);

    for (int ieta = 21; ieta <= 29; ++ieta)
        for (unsigned iphi=1; iphi<72; iphi+=2)
            lookup_[l++] = HBHEChannelId(depth, ieta, iphi);

    depth = 3;

    for (int ieta = -28; ieta <= -27; ++ieta)
        for (unsigned iphi=1; iphi<72; iphi+=2)
            lookup_[l++] = HBHEChannelId(depth, ieta, iphi);

    for (int ieta = -16; ieta <= -16; ++ieta)
        for (unsigned iphi=1; iphi<=72; ++iphi)
             lookup_[l++] = HBHEChannelId(depth, ieta, iphi);

    for (int ieta = 16; ieta <= 16; ++ieta)
        for (unsigned iphi=1; iphi<=72; ++iphi)
             lookup_[l++] = HBHEChannelId(depth, ieta, iphi);

    for (int ieta = 27; ieta <= 28; ++ieta)
        for (unsigned iphi=1; iphi<72; iphi+=2)
            lookup_[l++] = HBHEChannelId(depth, ieta, iphi);

    assert(l == ChannelCount);

    for (unsigned i=0; i<ChannelCount; ++i)
    {
        const HBHEChannelId& cid = lookup_[i];
        inverse_[cid] = i;

        const HcalSubdetector sub = HBHEChannelMap::getSubdetector(
            cid.depth(), cid.ieta());
        HcalDetId id(sub, cid.ieta(), cid.iphi(), cid.depth());

        const int hpd = HcalHPDRBXMap::indexHPD(id);
        assert(hpd >= 0 && hpd < HcalHPDRBXMap::NUM_HPDS);
        hpd_lookup_[i] = hpd;
        chan_in_hpd_lookup_[i] = hpd_channel_lookup_[hpd].size();
        hpd_channel_lookup_[hpd].push_back(i);

        const int rbx = HcalHPDRBXMap::indexRBXfromHPD(hpd);
        assert(rbx >= 0 && rbx < HcalHPDRBXMap::NUM_RBXS);
        rbx_lookup_[i] = rbx;
        chan_in_rbx_lookup_[i] = rbx_channel_lookup_[rbx].size();
        rbx_channel_lookup_[rbx].push_back(i);
    }
}

HcalSubdetector HBHEChannelMap::getSubdetector(const unsigned depth,
                                               const int ieta)
{
    const unsigned abseta = std::abs(ieta);

    // Make sure the arguments are in range
    assert(abseta <= 29U);
    assert(depth > 0U && depth < 4U);
    if (abseta == 29U)
        assert(depth <= 2U);

    if (abseta <= 15U)
        return HcalBarrel;
    else if (abseta == 16U)
        return depth <= 2U ? HcalBarrel : HcalEndcap;
    else
        return HcalEndcap;
}

unsigned HBHEChannelMap::maxChannelsPerHPD() const
{
    const unsigned nhpds = hpd_channel_lookup_.size();
    unsigned maxcount = 0;
    for (unsigned i=0; i<nhpds; ++i)
    {
        const unsigned nchan = hpd_channel_lookup_[i].size();
        if (nchan > maxcount)
            maxcount = nchan;
    }
    return maxcount;
}

unsigned HBHEChannelMap::maxChannelsPerRBX() const
{
    const unsigned nrbxs = rbx_channel_lookup_.size();
    unsigned maxcount = 0;
    for (unsigned i=0; i<nrbxs; ++i)
    {
        const unsigned nchan = rbx_channel_lookup_[i].size();
        if (nchan > maxcount)
            maxcount = nchan;
    }
    return maxcount;
}
