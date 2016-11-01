The code in this directory can be used as a starting point for
selecting HCAL channels with "good" signals. It utilizes a root-based
software framework which is described in some detail in the file
"analysis_framework.txt".

The HCAL channel selection analysis code and cuts are defined in the
following four files:

SelectGoodChannels.h        -- analysis class declarations
SelectGoodChannels.icc      -- analysis class implementation
SelectGoodChannelsOptions.h -- analysis options
runSelectGoodChannels.ana   -- analysis definition for the Makefile

This code does the following. It takes the signals stored in the
"NoiseTreeData" TTree and runs jet reconstruction on it. This, of
course, is going to be pure HCAL jet reconstruction, without ECAL or
tracker data, so it is going to be rather limited. We will not see
photons, pi0, or low-pt charged tracks. Anyway, it is still useful --
if we see a jet in HCAL, it is likely to be associated with a real
physics signal.

Jet reconstruction is performed by FFTJet (http://fftjet.hepforge.org).
It works, roughly, by discretizing the transverse energy flow in the
eta-phi space on a grid, so the result is kind of like a grayscale
image. Then it runs a Gaussian filter on this image. After filtering,
peaks above a certain cutoff are found -- these are the candidate jet
locations. Then something like a k-means clustering is run, using
these candidate locations as seeds.

The channels are associated with jets by looking up the closest
reconstructed jet (within a certain radius). For each jet, the channels
with the lowest transverse energy are discarded in such a way that
the sum of their transverse energy does not exceed 2% of the jet
transverse energy. Channels not associated with jets are discarded
as well. Finally, channels associated with jets whose Pt is less than
20 GeV/c (this cutoff is, of course, configurable) are also discarded.

In order to compile the code, download and install the latest version
of the FFTJet package from https://www.hepforge.org/downloads/fftjet.
After this installation, edit the "Makefile" and change the value of
"FFTJET_DIR" variable. Its value should normally correspond to the
value of the "--prefix" option with which you run the "configure"
script of the FFTJet package. Then you should be able to run "make"
and compile the "runSelectGoodChannels" executable.

The program writes out a simple ntuple of selected channels plus some
information about jets. The typical way to run it would be like this:

./runSelectGoodChannels -h '.*' -v --storeSelectedOnly output.root input1.root input2.root ...

Run the program without any arguments to see its usage instructions
and explanations of various options -- the default settings are
reasonable, but at some point you might want to change them.
We will need to run this or similar channel selection code on the
heavy ion data in order to build the signal model for the HCAL pulse.

Igor Volobouev
October 2016
