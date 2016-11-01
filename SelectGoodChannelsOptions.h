#ifndef SelectGoodChannelsOptions_h_
#define SelectGoodChannelsOptions_h_

#include <iostream>

#include "CmdLine.hh"
#include "inputValidation.hh"

//
// Class SelectGoodChannelsOptions must have
//
// 1) Default constructor
//
// 2) Copy constructor (usually auto-generated)
//
// 3) Method "void parse(CmdLine& cmdline)"
//
// 4) Method "void listOptions(std::ostream& os) const" for printing
//    available options
//
// 5) Method "void usage(std::ostream& os) const" for printing usage
//    instructions
//
// Preferably, this class should also have "operator<<" for printing
// the option values actually used.
//
// This class works in tandem with the analysis class.
// SelectGoodChannelsOptions object is a "const" member in the analysis
// class, so it is safe to make SelectGoodChannelsOptions a struct.
//
// The "parse" method must use normal methods of "CmdLine"
// ("option", "has", and "require") to fill the members of
// this class. Note that, if you find yourself using method
// "option" to assign values to some members, you should
// initialize these members in the default constructor.
//
// Do not use here switches reserved for use by the main program.
// These switches are:
//   "-h", "--histogram"
//   "-n", "--maxEvents"
//   "-s", "--noStats"
//   "-t", "--treeName"
//   "-v", "--verbose"
//
struct SelectGoodChannelsOptions
{
    SelectGoodChannelsOptions()
        : hbGeometryFile("Geometry/hb.ctr"),
          heGeometryFile("Geometry/he.ctr"),
          channelSelector("FFTJetChannelSelector"),
          pattRecoScale(0.2),
          etaToPhiBandwidthRatio(1.0),
          coneSize(0.5),
          peakEtCutoff(5.0),
          jetPtCutoff(20.0),
          minRecHitTime(-1.0e30),
          maxRecHitTime(1.0e30),
          etFractionCutoff(0.02),
          minResponseTS(3),
          maxResponseTS(8)
    {
    }

    void parse(CmdLine& cmdline)
    {
        storeSelectedOnly = cmdline.has(NULL, "--storeSelectedOnly");
        cmdline.option(NULL, "--hbgeo") >> hbGeometryFile;
        cmdline.option(NULL, "--hegeo") >> heGeometryFile;
        cmdline.option(NULL, "--channelSelector") >> channelSelector;

        cmdline.option(NULL, "--pattRecoScale") >> pattRecoScale;
        cmdline.option(NULL, "--etaToPhiBandwidthRatio") >> etaToPhiBandwidthRatio;
        cmdline.option(NULL, "--coneSize") >> coneSize;
        cmdline.option(NULL, "--peakEtCutoff") >> peakEtCutoff;
        cmdline.option(NULL, "--jetPtCutoff") >> jetPtCutoff;
        cmdline.option(NULL, "--etFractionCutoff") >> etFractionCutoff;
        cmdline.option(NULL, "--minRecHitTime") >> minRecHitTime;
        cmdline.option(NULL, "--maxRecHitTime") >> maxRecHitTime;

        cmdline.option(NULL, "--minResponseTS") >> minResponseTS;
        cmdline.option(NULL, "--maxResponseTS") >> maxResponseTS;

        validateRangeLELT(minResponseTS, "minResponseTS", 0U, 9U);
        validateRangeLELT(maxResponseTS, "maxResponseTS", minResponseTS+1U, 10U);
    }

    void listOptions(std::ostream& os) const
    {
        os << " [--storeSelectedOnly]"
           << " [--hbgeo filename]"
           << " [--hegeo filename]"
           << " [--channelSelector classname]"
           << " [--pattRecoScale value]"
           << " [--etaToPhiBandwidthRatio value]"
           << " [--coneSize value]"
           << " [--peakEtCutoff value]"
           << " [--jetPtCutoff value]"
           << " [--etFractionCutoff value]"
           << " [--minRecHitTime value]"
           << " [--maxRecHitTime value]"
           << " [--minResponseTS value]"
           << " [--maxResponseTS value]"
            ;
    }

    void usage(std::ostream& os) const
    {
        os << " --hbgeo             File containing HB geometry description. The default\n"
           << "                     value of this option is \"Geometry/hb.ctr\". If this\n"
           << "                     value is incorrect (i.e., if the program is run from\n"
           << "                     some directory other than the source directory),\n"
           << "                     correct value of this option must be provided.\n\n";
        os << " --hegeo             File containing HE geometry description. The default\n"
           << "                     value of this option is \"Geometry/he.ctr\". If this\n"
           << "                     value is incorrect (i.e., if the program is run from\n"
           << "                     some directory other than the source directory),\n"
           << "                     correct value of this option must be provided.\n\n";
        os << " --channelSelector   Class to use for selecting good channels. Valid\n"
              "                     values of this option are \"FFTJetChannelSelector\"\n"
              "                     and \"AllChannelSelector\". Default is\n"
              "                     \"FFTJetChannelSelector\".\n\n";
        os << " --pattRecoScale     Pattern recognition scale for FFTJet jet reconstruction.\n"
           << "                     Default value is 0.2.\n\n";
        os << " --etaToPhiBandwidthRatio   Eta/phi pattern recognition bandwidth ratio and\n"
           << "                            cone axis ratio for FFTJet. Default value is 1.0.\n\n";
        os << " --coneSize          Geometric mean of eta-phi cone axes for jet\n"
           << "                     reconstruction. Default is 0.5.\n\n";
        os << " --peakEtCutoff      Peak magnitude cutoff (local Et) for jet reconstruction.\n"
           << "                     Default is 5.0.\n\n";
        os << " --jetPtCutoff       Minimum transverse momentum for \"good\" jets. Default\n"
           << "                     value is 20.0.\n\n";
        os << " --etFractionCutoff  Fraction of jet Et left out by the channels included into\n"
           << "                     the analysis. Default is 0.02\n\n";
        os << " --minRecHitTime     Minimum RecHitTime for \"good\" channels. This option\n"
           << "                     is currently unused.\n\n";
        os << " --maxRecHitTime     Maximum RecHitTime for \"good\" channels. This option\n"
           << "                     is currently unused.\n\n";
        os << " --minResponseTS     Minimum time slice (included) for defining the \"real\"\n"
           << "                     signal charge. Default is 3.\n\n";
        os << " --maxResponseTS     Maximum time slice (excluded) for defining the \"real\"\n"
           << "                     signal charge. Default is 8.\n\n";
        os << " --storeSelectedOnly    Store only the channels chosen by the channel selector.\n"
           << "                        Can be used to reduce the channel ntuple size.\n\n";
    }

    std::string hbGeometryFile;
    std::string heGeometryFile;
    std::string channelSelector;

    double pattRecoScale;
    double etaToPhiBandwidthRatio;
    double coneSize;
    double peakEtCutoff;
    double jetPtCutoff;
    double minRecHitTime;
    double maxRecHitTime;
    double etFractionCutoff;

    unsigned minResponseTS;
    unsigned maxResponseTS;
    bool storeSelectedOnly;
};

std::ostream& operator<<(std::ostream& os, const SelectGoodChannelsOptions& o)
{
    os << ", hbgeo = \"" << o.hbGeometryFile << '"'
       << ", hegeo = \"" << o.heGeometryFile << '"'
       << ", channelSelector = \"" << o.channelSelector << '"'
       << ", pattRecoScale = \"" << o.pattRecoScale << '"'
       << ", etaToPhiBandwidthRatio = \"" << o.etaToPhiBandwidthRatio << '"'
       << ", coneSize = \"" << o.coneSize << '"'
       << ", peakEtCutoff = \"" << o.peakEtCutoff << '"'
       << ", jetPtCutoff = \"" << o.jetPtCutoff << '"'
       << ", minRecHitTime = \"" << o.minRecHitTime << '"'
       << ", maxRecHitTime = \"" << o.maxRecHitTime << '"'
       << ", etFractionCutoff = \"" << o.etFractionCutoff << '"'
       << ", minResponseTS = " << o.minResponseTS
       << ", maxResponseTS = " << o.maxResponseTS
       << ", storeSelectedOnly = " << o.storeSelectedOnly
        ;
    return os;
}

#endif // SelectGoodChannelsOptions_h_
