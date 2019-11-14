#
# OMNeT++/OMNEST Makefile for dtnsim
#
# This file was generated with the command:
#  opp_makemake -f --deep -O out -I.
#

# Name of target to be created (-o option)
TARGET = dtnsim$(D)$(EXE_SUFFIX)
TARGET_DIR = .

# User interface (uncomment one) (-u option)
USERIF_LIBS = $(ALL_ENV_LIBS) # that is, $(TKENV_LIBS) $(QTENV_LIBS) $(CMDENV_LIBS)
#USERIF_LIBS = $(CMDENV_LIBS)
#USERIF_LIBS = $(TKENV_LIBS)
#USERIF_LIBS = $(QTENV_LIBS)

# C++ include paths (with -I)
INCLUDE_PATH = -I.

# Additional object and library files to link with
EXTRA_OBJS =

# Additional libraries (-L, -l options)
LIBS =

# Output directory
PROJECT_OUTPUT_DIR = out
PROJECTRELATIVE_PATH =
O = $(PROJECT_OUTPUT_DIR)/$(CONFIGNAME)/$(PROJECTRELATIVE_PATH)

# Object files for local .cc, .msg and .sm files
OBJS = \
    $O/src/central/Central.o \
    $O/src/ion/bp/cgr/libcgr.o \
    $O/src/ion/bp/library/bei.o \
    $O/src/ion/bp/library/dtn2fw.o \
    $O/src/ion/bp/library/eureka.o \
    $O/src/ion/bp/library/libbp.o \
    $O/src/ion/bp/library/libbpnm.o \
    $O/src/ion/bp/library/libbpP.o \
    $O/src/ion/bp/library/libdtn2fw.o \
    $O/src/ion/bp/library/libipnfw.o \
    $O/src/ion/bp/library/acs/acsappend.o \
    $O/src/ion/bp/library/acs/acsid.o \
    $O/src/ion/bp/library/acs/acsrx.o \
    $O/src/ion/bp/library/acs/acsserialize.o \
    $O/src/ion/bp/library/acs/acstx.o \
    $O/src/ion/bp/library/ext/bpextensions.o \
    $O/src/ion/bp/library/ext/bae/bae.o \
    $O/src/ion/bp/library/ext/bpsec/bpsec_bcb.o \
    $O/src/ion/bp/library/ext/bpsec/bpsec_bib.o \
    $O/src/ion/bp/library/ext/bpsec/bpsec_instr.o \
    $O/src/ion/bp/library/ext/bpsec/bpsec_util.o \
    $O/src/ion/bp/library/ext/bpsec/profiles.o \
    $O/src/ion/bp/library/ext/bsp/bspbab.o \
    $O/src/ion/bp/library/ext/bsp/bspbcb.o \
    $O/src/ion/bp/library/ext/bsp/bspbib.o \
    $O/src/ion/bp/library/ext/bsp/bsputil.o \
    $O/src/ion/bp/library/ext/bsp/ciphersuites.o \
    $O/src/ion/bp/library/ext/bsp/extbspbab.o \
    $O/src/ion/bp/library/ext/bsp/extbsppcb.o \
    $O/src/ion/bp/library/ext/bsp/extbsppib.o \
    $O/src/ion/bp/library/ext/bsp/extbsputil.o \
    $O/src/ion/bp/library/ext/bsp/ciphersuites/bab_hmac_sha1.o \
    $O/src/ion/bp/library/ext/bsp/ciphersuites/bcb_arc4.o \
    $O/src/ion/bp/library/ext/bsp/ciphersuites/bib_hmac_sha256.o \
    $O/src/ion/bp/library/ext/cteb/cteb.o \
    $O/src/ion/bp/library/ext/ecos/ecos.o \
    $O/src/ion/bp/library/ext/meb/meb.o \
    $O/src/ion/bp/library/ext/phn/phn.o \
    $O/src/ion/bp/library/ext/snid/snid.o \
    $O/src/ion/bp/utils/bpadmin.o \
    $O/src/ion/ici/library/bulk.o \
    $O/src/ion/ici/library/ion.o \
    $O/src/ion/ici/library/ionsec.o \
    $O/src/ion/ici/library/lyst.o \
    $O/src/ion/ici/library/memmgr.o \
    $O/src/ion/ici/library/platform.o \
    $O/src/ion/ici/library/psm.o \
    $O/src/ion/ici/library/rfx.o \
    $O/src/ion/ici/library/smlist.o \
    $O/src/ion/ici/library/smrbt.o \
    $O/src/ion/ici/library/sptrace.o \
    $O/src/ion/ici/library/zco.o \
    $O/src/ion/ici/sdr/sdrcatlg.o \
    $O/src/ion/ici/sdr/sdrhash.o \
    $O/src/ion/ici/sdr/sdrlist.o \
    $O/src/ion/ici/sdr/sdrmgt.o \
    $O/src/ion/ici/sdr/sdrstring.o \
    $O/src/ion/ici/sdr/sdrtable.o \
    $O/src/ion/ici/sdr/sdrxn.o \
    $O/src/ion/ici/utils/ionadmin.o \
    $O/src/node/app/App.o \
    $O/src/node/com/Com.o \
    $O/src/node/dtn/Contact.o \
    $O/src/node/dtn/ContactPlan.o \
    $O/src/node/dtn/CustodyModel.o \
    $O/src/node/dtn/Dtn.o \
    $O/src/node/dtn/SdrModel.o \
    $O/src/node/dtn/SdrStatus.o \
    $O/src/node/dtn/routing/RoutingCgrIon350.o \
    $O/src/node/dtn/routing/RoutingCgrModel350.o \
    $O/src/node/dtn/routing/RoutingCgrModel350_2Copies.o \
    $O/src/node/dtn/routing/RoutingCgrModel350_Hops.o \
    $O/src/node/dtn/routing/RoutingCgrModel350_Probabilistic.o \
    $O/src/node/dtn/routing/RoutingCgrModelRev17.o \
    $O/src/node/dtn/routing/RoutingCgrModelYen.o \
    $O/src/node/dtn/routing/RoutingDeterministic.o \
    $O/src/node/dtn/routing/RoutingDirect.o \
    $O/src/node/dtn/routing/RoutingEpidemic.o \
    $O/src/node/dtn/routing/RoutingSprayAndWait.o \
    $O/src/node/dtn/routing/RoutingStochastic.o \
    $O/src/node/fault/Fault.o \
    $O/src/node/graphics/Graphics.o \
    $O/src/utils/ContactPlanUtils.o \
    $O/src/utils/Lp.o \
    $O/src/utils/LpUtils.o \
    $O/src/utils/RouterUtils.o \
    $O/src/utils/Subject.o \
    $O/src/utils/TopologyUtils.o \
    $O/src/dtnsim_m.o

# Message files
MSGFILES = \
    src/dtnsim.msg

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
# <<<
#------------------------------------------------------------------------------

# Main target
all: $(TARGET_DIR)/$(TARGET)

$(TARGET_DIR)/% :: $O/%
	@mkdir -p $(TARGET_DIR)
	$(Q)$(LN) $< $@
ifeq ($(TOOLCHAIN_NAME),clangc2)
	$(Q)-$(LN) $(<:%.dll=%.lib) $(@:%.dll=%.lib)
endif

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
	$(Q)$(MSGC) -s _m.cc -MD -MP -MF $O/$(basename $<)_m.h.d $(MSGCOPTS) $?

%_sm.cc %_sm.h: %.sm
	$(qecho) SMC: $<
	$(Q)$(SMC) -c++ -suffix cc $(SMCOPTS) $?

msgheaders: $(MSGFILES:.msg=_m.h)

smheaders: $(SMFILES:.sm=_sm.h)

clean:
	$(qecho) Cleaning $(TARGET)
	$(Q)-rm -rf $O
	$(Q)-rm -f $(TARGET_DIR)/$(TARGET)
	$(Q)-rm -f $(TARGET_DIR)/$(TARGET:%.dll=%.lib)
	$(Q)-rm -f $(call opp_rwildcard, . , *_m.cc *_m.h *_sm.cc *_sm.h)

cleanall:
	$(Q)$(MAKE) -s clean MODE=release
	$(Q)$(MAKE) -s clean MODE=debug
	$(Q)-rm -rf $(PROJECT_OUTPUT_DIR)

# include all dependencies
-include $(OBJS:%=%.d) $(MSGFILES:%.msg=$O/%_m.h.d)