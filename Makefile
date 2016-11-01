OFILES = HistogramManager.o HcalNoiseTree.o NoiseTreeHelper.o HcalDetId.o \
         HBHEChannelGeometry.o HBHEChannelMap.o HcalHPDRBXMap.o

PROGRAMS = exampleTreeAnalysis.ana runSelectGoodChannels.ana

ROOTCONFIG   := root-config

ARCH         := $(shell $(ROOTCONFIG) --arch)
PLATFORM     := $(shell $(ROOTCONFIG) --platform)

CXX           =  g++

ROOTCFLAGS   := $(shell $(ROOTCONFIG) --cflags)
ROOTLDFLAGS  := $(shell $(ROOTCONFIG) --ldflags)
ROOTLIBS     := $(shell $(ROOTCONFIG) --libs)
ROOTGLIBS    := $(shell $(ROOTCONFIG) --glibs)
HASTHREAD    := $(shell $(ROOTCONFIG) --has-thread)

CXXFLAGS     += $(ROOTCFLAGS)
LDFLAGS      += $(ROOTLDFLAGS)

FFTJET_DIR = /usr/local
FFTJET_LIB = $(FFTJET_DIR)/lib
FFTJET_INC = $(FFTJET_DIR)/include

LIBS = $(ROOTLIBS) -L$(FFTJET_LIB) -L/usr/lib64 -lfftjet -lfftw3 -ldl -lm

CXXFLAGS = -fPIC -Wall -g -std=c++11 $(ROOTCFLAGS) -I$(FFTJET_INC) -I.
LINKFLAGS = -fPIC -g -std=c++11 $(LIBS)

%.o : %.C
	$(CXX) -c $(CXXFLAGS) -MD $< -o $@
	@sed -i 's,\($*\.o\)[:]*\(.*\),$@: $$\(wildcard\2\)\n\1:\2,g' $*.d

%.C : %.ana
	rm -f $@
	sed "s/ANALYSIS_HEADER_FILE/$</g" analysisExecutableTemplate.C > $@

BINARIES = $(PROGRAMS:.ana=)

all: $(BINARIES)

$(BINARIES): % : %.o $(OFILES); g++ $(OPTIMIZE) -fPIC -o $@ $^ $(LIBS)

clean:
	rm -f $(BINARIES) $(PROGRAMS:.ana=.C) core.* *.o *.d *~

-include $(OFILES:.o=.d)
-include $(PROGRAMS:.ana=.d)
