#include <fstream>
#include <stdexcept>

#include "HBHEChannelGeometry.h"
#include "HBHEChannelMap.h"
#include "fillTuplesFromText.h"

HBHEChannelGeometry::HBHEChannelGeometry(const char* hbFile, const char* heFile)
    : directions_(HBHEChannelMap::ChannelCount)
{
    HBHEChannelMap chmap;

    // Load the data from text files
    loadData(hbFile, chmap);
    loadData(heFile, chmap);

    // Check that we have directions for all channels
    TVector3 zero;
    for (unsigned i=0; i<HBHEChannelMap::ChannelCount; ++i)
        if (directions_[i] == zero)
        {
            unsigned depth, iphi;
            int ieta;
            chmap.getChannelTriple(i, &depth, &ieta, &iphi);
            std::ostringstream os;
            os << "In HBHEChannelGeometry constructor: no data for ieta "
               << ieta << ", iphi " << iphi << ", depth " << depth;
            throw std::runtime_error(os.str());
        }
}

void HBHEChannelGeometry::loadData(const char* filename,
                                   const HBHEChannelMap& chmap)
{
    // Order: ieta, iphi, depth, x, y, z
    typedef std::tuple<int, int, int, double, double, double> Tuple;

    std::ifstream is(filename);
    if (!is.is_open())
    {
        std::ostringstream os;
        os << "In HBHEChannelGeometry::loadData: failed to open file \""
           << filename << '"';
        throw std::invalid_argument(os.str());
    }

    std::vector<Tuple> nt;
    if (!fillTuplesFromText(is, &nt))
    {
        std::ostringstream os;
        os << "In HBHEChannelGeometry::loadData: failed to parse file \""
           << filename << '"';
        throw std::invalid_argument(os.str());
    }

    const unsigned nrows = nt.size();
    for (unsigned row=0; row<nrows; ++row)
    {
        const Tuple& t(nt[row]);
        const unsigned idx = chmap.linearIndex(std::get<2>(t), std::get<0>(t), std::get<1>(t));
        TVector3 vec(std::get<3>(t), std::get<4>(t), std::get<5>(t));
        directions_.at(idx) = vec.Unit();
    }
}
