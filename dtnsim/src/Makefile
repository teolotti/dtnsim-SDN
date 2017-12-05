#
# OMNeT++/OMNEST Makefile for dtnsim
#
# This file was generated with the command:
#  opp_makemake -f --deep -Iion/bp/include -Iion/ici/include -Iutils -Inode/app -Inode/fault -Inode/graphics -I. -Iion -Iion/bp/library/acs -Iion/bp/library/ext/bae -Iion/bp/library/ext/bpsec -Iion/bp/library/ext/bsp -Iion/bp/library/ext/cteb -Iion/bp/library/ext/ecos -Iion/bp/library/ext/meb -Iion/bp/library/ext/phn -Iion/bp/library/ext/snid -Iion/bp/library/ext -Inode -Ilogger -Inode/dtn/routing -Inode/com -Inode/dtn
#

# Name of target to be created (-o option)
TARGET = dtnsim$(EXE_SUFFIX)

# User interface (uncomment one) (-u option)
USERIF_LIBS = $(ALL_ENV_LIBS) # that is, $(TKENV_LIBS) $(QTENV_LIBS) $(CMDENV_LIBS)
#USERIF_LIBS = $(CMDENV_LIBS)
#USERIF_LIBS = $(TKENV_LIBS)
#USERIF_LIBS = $(QTENV_LIBS)

# C++ include paths (with -I)
INCLUDE_PATH = \
    -Iion/bp/include \
    -Iion/ici/include \
    -Iutils \
    -Inode/app \
    -Inode/fault \
    -Inode/graphics \
    -I. \
    -Iion \
    -Iion/bp/library/acs \
    -Iion/bp/library/ext/bae \
    -Iion/bp/library/ext/bpsec \
    -Iion/bp/library/ext/bsp \
    -Iion/bp/library/ext/cteb \
    -Iion/bp/library/ext/ecos \
    -Iion/bp/library/ext/meb \
    -Iion/bp/library/ext/phn \
    -Iion/bp/library/ext/snid \
    -Iion/bp/library/ext \
    -Inode \
    -Ilogger \
    -Inode/dtn/routing \
    -Inode/com \
    -Inode/dtn

# Additional object and library files to link with
EXTRA_OBJS =

# Additional libraries (-L, -l options)
LIBS =

# Output directory
PROJECT_OUTPUT_DIR = ../out
PROJECTRELATIVE_PATH = src
O = $(PROJECT_OUTPUT_DIR)/$(CONFIGNAME)/$(PROJECTRELATIVE_PATH)

# Object files for local .cc, .msg and .sm files
OBJS = \
    $O/central/Central.o \
    $O/ion/bp/cgr/libcgr.o \
    $O/ion/bp/library/bei.o \
    $O/ion/bp/library/dtn2fw.o \
    $O/ion/bp/library/eureka.o \
    $O/ion/bp/library/libbp.o \
    $O/ion/bp/library/libbpnm.o \
    $O/ion/bp/library/libbpP.o \
    $O/ion/bp/library/libdtn2fw.o \
    $O/ion/bp/library/libipnfw.o \
    $O/ion/bp/library/acs/acsappend.o \
    $O/ion/bp/library/acs/acsid.o \
    $O/ion/bp/library/acs/acsrx.o \
    $O/ion/bp/library/acs/acsserialize.o \
    $O/ion/bp/library/acs/acstx.o \
    $O/ion/bp/library/ext/bpextensions.o \
    $O/ion/bp/library/ext/bae/bae.o \
    $O/ion/bp/library/ext/bpsec/bpsec_bcb.o \
    $O/ion/bp/library/ext/bpsec/bpsec_bib.o \
    $O/ion/bp/library/ext/bpsec/bpsec_instr.o \
    $O/ion/bp/library/ext/bpsec/bpsec_util.o \
    $O/ion/bp/library/ext/bpsec/profiles.o \
    $O/ion/bp/library/ext/bsp/bspbab.o \
    $O/ion/bp/library/ext/bsp/bspbcb.o \
    $O/ion/bp/library/ext/bsp/bspbib.o \
    $O/ion/bp/library/ext/bsp/bsputil.o \
    $O/ion/bp/library/ext/bsp/ciphersuites.o \
    $O/ion/bp/library/ext/bsp/extbspbab.o \
    $O/ion/bp/library/ext/bsp/extbsppcb.o \
    $O/ion/bp/library/ext/bsp/extbsppib.o \
    $O/ion/bp/library/ext/bsp/extbsputil.o \
    $O/ion/bp/library/ext/bsp/ciphersuites/bab_hmac_sha1.o \
    $O/ion/bp/library/ext/bsp/ciphersuites/bcb_arc4.o \
    $O/ion/bp/library/ext/bsp/ciphersuites/bib_hmac_sha256.o \
    $O/ion/bp/library/ext/cteb/cteb.o \
    $O/ion/bp/library/ext/ecos/ecos.o \
    $O/ion/bp/library/ext/meb/meb.o \
    $O/ion/bp/library/ext/phn/phn.o \
    $O/ion/bp/library/ext/snid/snid.o \
    $O/ion/bp/utils/bpadmin.o \
    $O/ion/ici/library/bulk.o \
    $O/ion/ici/library/ion.o \
    $O/ion/ici/library/ionsec.o \
    $O/ion/ici/library/lyst.o \
    $O/ion/ici/library/memmgr.o \
    $O/ion/ici/library/platform.o \
    $O/ion/ici/library/psm.o \
    $O/ion/ici/library/rfx.o \
    $O/ion/ici/library/smlist.o \
    $O/ion/ici/library/smrbt.o \
    $O/ion/ici/library/sptrace.o \
    $O/ion/ici/library/zco.o \
    $O/ion/ici/sdr/sdrcatlg.o \
    $O/ion/ici/sdr/sdrhash.o \
    $O/ion/ici/sdr/sdrlist.o \
    $O/ion/ici/sdr/sdrmgt.o \
    $O/ion/ici/sdr/sdrstring.o \
    $O/ion/ici/sdr/sdrtable.o \
    $O/ion/ici/sdr/sdrxn.o \
    $O/ion/ici/utils/ionadmin.o \
    $O/node/app/App.o \
    $O/node/com/Com.o \
    $O/node/dtn/Contact.o \
    $O/node/dtn/ContactPlan.o \
    $O/node/dtn/CustodyModel.o \
    $O/node/dtn/Dtn.o \
    $O/node/dtn/SdrModel.o \
    $O/node/dtn/SdrStatus.o \
    $O/node/dtn/routing/RoutingCgrIon350.o \
    $O/node/dtn/routing/RoutingCgrModel350.o \
    $O/node/dtn/routing/RoutingCgrModel350_3.o \
    $O/node/dtn/routing/RoutingCgrModel350_Proactive.o \
    $O/node/dtn/routing/RoutingCgrModel350_Probabilistic.o \
    $O/node/dtn/routing/RoutingCgrModelRev17.o \
    $O/node/dtn/routing/RoutingCgrModelYen.o \
    $O/node/dtn/routing/RoutingDeterministic.o \
    $O/node/dtn/routing/RoutingDirect.o \
    $O/node/dtn/routing/RoutingEpidemic.o \
    $O/node/dtn/routing/RoutingSprayAndWait.o \
    $O/node/dtn/routing/RoutingStochastic.o \
    $O/node/fault/Fault.o \
    $O/node/graphics/Graphics.o \
    $O/utils/ContactPlanUtils.o \
    $O/utils/Lp.o \
    $O/utils/LpUtils.o \
    $O/utils/RouterUtils.o \
    $O/utils/Subject.o \
    $O/utils/TopologyUtils.o \
    $O/dtnsim_m.o

# Message files
MSGFILES = \
    dtnsim.msg

# SM files
SMFILES =

#------------------------------------------------------------------------------

# Pull in OMNeT++ configuration (Makefile.inc)

ifneq ("$(OMNETPP_CONFIGFILE)","")
CONFIGFILE = $(OMNETPP_CONFIGFILE)
else
ifneq ("$(OMNETPP_ROOT)","")
CONFIGFILE = $(OMNETPP_ROOT)/Makefile.inc
else
CONFIGFILE = $(shell opp_configfilepath)
endif
endif

ifeq ("$(wildcard $(CONFIGFILE))","")
$(error Config file '$(CONFIGFILE)' does not exist -- add the OMNeT++ bin directory to the path so that opp_configfilepath can be found, or set the OMNETPP_CONFIGFILE variable to point to Makefile.inc)
endif

include $(CONFIGFILE)

# Simulation kernel and user interface libraries
OMNETPP_LIBS = $(OPPMAIN_LIB) $(USERIF_LIBS) $(KERNEL_LIBS) $(SYS_LIBS)

COPTS = $(CFLAGS) $(IMPORT_DEFINES)  $(INCLUDE_PATH) -I$(OMNETPP_INCL_DIR)
MSGCOPTS = $(INCLUDE_PATH)
SMCOPTS =

# we want to recompile everything if COPTS changes,
# so we store COPTS into $COPTS_FILE and have object
# files depend on it (except when "make depend" was called)
COPTS_FILE = $O/.last-copts
ifneq ("$(COPTS)","$(shell cat $(COPTS_FILE) 2>/dev/null || echo '')")
$(shell $(MKPATH) "$O" && echo "$(COPTS)" >$(COPTS_FILE))
endif

#------------------------------------------------------------------------------
# User-supplied makefile fragment(s)
# >>>
# inserted from file 'makefrag':
#INCLUDE_PATH += -I"/home/madoery/boost_1_63_0"
#INCLUDE_PATH += -I"/home/madoery/cplex/cplex/include"
#INCLUDE_PATH += -I"/home/madoery/cplex/concert/include"

LIBS +=  -lpthread
#LIBS +=  -lconcert -lilocplex -lcplex -lm

#LDFLAGS += -L"/home/madoery/cplex/cplex/lib/x86-64_sles10_4.1/static_pic"
#LDFLAGS += -L"/home/madoery/cplex/concert/lib/x86-64_sles10_4.1/static_pic"

#COPTS += -DIL_STD=1

# <<<
#------------------------------------------------------------------------------

# Main target
all: $O/$(TARGET)
	$(Q)$(LN) $O/$(TARGET) .

$O/$(TARGET): $(OBJS)  $(wildcard $(EXTRA_OBJS)) Makefile $(CONFIGFILE)
	@$(MKPATH) $O
	@echo Creating executable: $@
	$(Q)$(CXX) $(LDFLAGS) -o $O/$(TARGET) $(OBJS) $(EXTRA_OBJS) $(AS_NEEDED_OFF) $(WHOLE_ARCHIVE_ON) $(LIBS) $(WHOLE_ARCHIVE_OFF) $(OMNETPP_LIBS)

.PHONY: all clean cleanall depend msgheaders smheaders

.SUFFIXES: .cc

$O/%.o: %.cc $(COPTS_FILE) | msgheaders smheaders
	@$(MKPATH) $(dir $@)
	$(qecho) "$<"
	$(Q)$(CXX) -c $(CXXFLAGS) $(COPTS) -o $@ $<

%_m.cc %_m.h: %.msg
	$(qecho) MSGC: $<
	$(Q)$(MSGC) -s _m.cc $(MSGCOPTS) $?

%_sm.cc %_sm.h: %.sm
	$(qecho) SMC: $<
	$(Q)$(SMC) -c++ -suffix cc $(SMCOPTS) $?

msgheaders: $(MSGFILES:.msg=_m.h)

smheaders: $(SMFILES:.sm=_sm.h)

clean:
	$(qecho) Cleaning...
	$(Q)-rm -rf $O
	$(Q)-rm -f $(TARGET)
	$(Q)-rm -f $(call opp_rwildcard, . , *_m.cc *_m.h *_sm.cc *_sm.h)

cleanall: clean
	$(Q)-rm -rf $(PROJECT_OUTPUT_DIR)

# include all dependencies
-include $(OBJS:%.o=%.d)
