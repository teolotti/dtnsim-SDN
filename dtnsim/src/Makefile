#
# OMNeT++/OMNEST Makefile for dtnsim
#
# This file was generated with the command:
#  opp_makemake -f --deep -Xion-3.5.0
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
    -I. \
    -IcontactPlan \
    -Iion \
    -Iion/bp \
    -Iion/bp/cgr \
    -Iion/bp/include \
    -Iion/bp/library \
    -Iion/bp/library/acs \
    -Iion/bp/library/ext \
    -Iion/bp/library/ext/bae \
    -Iion/bp/library/ext/bpsec \
    -Iion/bp/library/ext/bsp \
    -Iion/bp/library/ext/bsp/ciphersuites \
    -Iion/bp/library/ext/cteb \
    -Iion/bp/library/ext/ecos \
    -Iion/bp/library/ext/meb \
    -Iion/bp/library/ext/phn \
    -Iion/bp/library/ext/snid \
    -Iion/bp/utils \
    -Iion/ici \
    -Iion/ici/include \
    -Iion/ici/library \
    -Iion/ici/sdr \
    -Iion/ici/utils \
    -Inode \
    -Inode/app \
    -Inode/mac \
    -Inode/net

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
    $O/ion/Ion.o \
    $O/ion/bp/cgr/libcgr.o \
    $O/ion/bp/library/bei.o \
    $O/ion/bp/library/libdtn2fw.o \
    $O/ion/bp/library/libipnfw.o \
    $O/ion/bp/library/dtn2fw.o \
    $O/ion/bp/library/libbpnm.o \
    $O/ion/bp/library/libbpP.o \
    $O/ion/bp/library/eureka.o \
    $O/ion/bp/library/libbp.o \
    $O/ion/bp/library/acs/acsid.o \
    $O/ion/bp/library/acs/acsappend.o \
    $O/ion/bp/library/acs/acstx.o \
    $O/ion/bp/library/acs/acsserialize.o \
    $O/ion/bp/library/acs/acsrx.o \
    $O/ion/bp/library/ext/bpextensions.o \
    $O/ion/bp/library/ext/bae/bae.o \
    $O/ion/bp/library/ext/bpsec/bpsec_util.o \
    $O/ion/bp/library/ext/bpsec/bpsec_bib.o \
    $O/ion/bp/library/ext/bpsec/bpsec_bcb.o \
    $O/ion/bp/library/ext/bpsec/bpsec_instr.o \
    $O/ion/bp/library/ext/bpsec/profiles.o \
    $O/ion/bp/library/ext/bsp/ciphersuites.o \
    $O/ion/bp/library/ext/bsp/bspbib.o \
    $O/ion/bp/library/ext/bsp/extbsppcb.o \
    $O/ion/bp/library/ext/bsp/bsputil.o \
    $O/ion/bp/library/ext/bsp/extbspbab.o \
    $O/ion/bp/library/ext/bsp/bspbab.o \
    $O/ion/bp/library/ext/bsp/bspbcb.o \
    $O/ion/bp/library/ext/bsp/extbsppib.o \
    $O/ion/bp/library/ext/bsp/extbsputil.o \
    $O/ion/bp/library/ext/bsp/ciphersuites/bab_hmac_sha1.o \
    $O/ion/bp/library/ext/bsp/ciphersuites/bib_hmac_sha256.o \
    $O/ion/bp/library/ext/bsp/ciphersuites/bcb_arc4.o \
    $O/ion/bp/library/ext/cteb/cteb.o \
    $O/ion/bp/library/ext/ecos/ecos.o \
    $O/ion/bp/library/ext/meb/meb.o \
    $O/ion/bp/library/ext/phn/phn.o \
    $O/ion/bp/library/ext/snid/snid.o \
    $O/ion/bp/utils/bpadmin.o \
    $O/ion/ici/library/zco.o \
    $O/ion/ici/library/ionsec.o \
    $O/ion/ici/library/rfx.o \
    $O/ion/ici/library/psm.o \
    $O/ion/ici/library/sptrace.o \
    $O/ion/ici/library/lyst.o \
    $O/ion/ici/library/bulk.o \
    $O/ion/ici/library/ion.o \
    $O/ion/ici/library/smrbt.o \
    $O/ion/ici/library/memmgr.o \
    $O/ion/ici/library/smlist.o \
    $O/ion/ici/library/platform.o \
    $O/ion/ici/sdr/sdrstring.o \
    $O/ion/ici/sdr/sdrxn.o \
    $O/ion/ici/sdr/sdrmgt.o \
    $O/ion/ici/sdr/sdrhash.o \
    $O/ion/ici/sdr/sdrtable.o \
    $O/ion/ici/sdr/sdrcatlg.o \
    $O/ion/ici/sdr/sdrlist.o \
    $O/ion/ici/utils/ionadmin.o \
    $O/node/app/App.o \
    $O/node/mac/Mac.o \
    $O/node/net/Contact.o \
    $O/node/net/RoutingCgrModelYen.o \
    $O/node/net/SdrModel.o \
    $O/node/net/RoutingDirect.o \
    $O/node/net/RoutingCgrIon350.o \
    $O/node/net/Net.o \
    $O/node/net/SdrStatus.o \
    $O/node/net/RoutingCgrModel.o \
    $O/node/net/ContactPlan.o \
    $O/dtnsim_m.o

# Message files
MSGFILES = \
    dtnsim.msg

# SM files
SMFILES =

#------------------------------------------------------------------------------

# Pull in OMNeT++ configuration (Makefile.inc or configuser.vc)

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
OMNETPP_LIB_SUBDIR = $(OMNETPP_LIB_DIR)/$(TOOLCHAIN_NAME)
OMNETPP_LIBS = -L"$(OMNETPP_LIB_SUBDIR)" -L"$(OMNETPP_LIB_DIR)" -loppmain$D $(USERIF_LIBS) $(KERNEL_LIBS) $(SYS_LIBS)

COPTS = $(CFLAGS)  $(INCLUDE_PATH) -I$(OMNETPP_INCL_DIR)
MSGCOPTS = $(INCLUDE_PATH)
SMCOPTS =

# we want to recompile everything if COPTS changes,
# so we store COPTS into $COPTS_FILE and have object
# files depend on it (except when "make depend" was called)
COPTS_FILE = $O/.last-copts
ifneq ($(MAKECMDGOALS),depend)
ifneq ("$(COPTS)","$(shell cat $(COPTS_FILE) 2>/dev/null || echo '')")
$(shell $(MKPATH) "$O" && echo "$(COPTS)" >$(COPTS_FILE))
endif
endif

#------------------------------------------------------------------------------
# User-supplied makefile fragment(s)
# >>>
# <<<
#------------------------------------------------------------------------------

# Main target
all: $O/$(TARGET)
	$(Q)$(LN) $O/$(TARGET) .

$O/$(TARGET): $(OBJS)  $(wildcard $(EXTRA_OBJS)) Makefile
	@$(MKPATH) $O
	@echo Creating executable: $@
	$(Q)$(CXX) $(LDFLAGS) -o $O/$(TARGET)  $(OBJS) $(EXTRA_OBJS) $(AS_NEEDED_OFF) $(WHOLE_ARCHIVE_ON) $(LIBS) $(WHOLE_ARCHIVE_OFF) $(OMNETPP_LIBS)

.PHONY: all clean cleanall depend msgheaders smheaders

.SUFFIXES: .cc

$O/%.o: %.cc $(COPTS_FILE)
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
	$(Q)-rm -f dtnsim dtnsim.exe libdtnsim.so libdtnsim.a libdtnsim.dll libdtnsim.dylib
	$(Q)-rm -f ./*_m.cc ./*_m.h ./*_sm.cc ./*_sm.h
	$(Q)-rm -f contactPlan/*_m.cc contactPlan/*_m.h contactPlan/*_sm.cc contactPlan/*_sm.h
	$(Q)-rm -f ion/*_m.cc ion/*_m.h ion/*_sm.cc ion/*_sm.h
	$(Q)-rm -f ion/bp/*_m.cc ion/bp/*_m.h ion/bp/*_sm.cc ion/bp/*_sm.h
	$(Q)-rm -f ion/bp/cgr/*_m.cc ion/bp/cgr/*_m.h ion/bp/cgr/*_sm.cc ion/bp/cgr/*_sm.h
	$(Q)-rm -f ion/bp/include/*_m.cc ion/bp/include/*_m.h ion/bp/include/*_sm.cc ion/bp/include/*_sm.h
	$(Q)-rm -f ion/bp/library/*_m.cc ion/bp/library/*_m.h ion/bp/library/*_sm.cc ion/bp/library/*_sm.h
	$(Q)-rm -f ion/bp/library/acs/*_m.cc ion/bp/library/acs/*_m.h ion/bp/library/acs/*_sm.cc ion/bp/library/acs/*_sm.h
	$(Q)-rm -f ion/bp/library/ext/*_m.cc ion/bp/library/ext/*_m.h ion/bp/library/ext/*_sm.cc ion/bp/library/ext/*_sm.h
	$(Q)-rm -f ion/bp/library/ext/bae/*_m.cc ion/bp/library/ext/bae/*_m.h ion/bp/library/ext/bae/*_sm.cc ion/bp/library/ext/bae/*_sm.h
	$(Q)-rm -f ion/bp/library/ext/bpsec/*_m.cc ion/bp/library/ext/bpsec/*_m.h ion/bp/library/ext/bpsec/*_sm.cc ion/bp/library/ext/bpsec/*_sm.h
	$(Q)-rm -f ion/bp/library/ext/bsp/*_m.cc ion/bp/library/ext/bsp/*_m.h ion/bp/library/ext/bsp/*_sm.cc ion/bp/library/ext/bsp/*_sm.h
	$(Q)-rm -f ion/bp/library/ext/bsp/ciphersuites/*_m.cc ion/bp/library/ext/bsp/ciphersuites/*_m.h ion/bp/library/ext/bsp/ciphersuites/*_sm.cc ion/bp/library/ext/bsp/ciphersuites/*_sm.h
	$(Q)-rm -f ion/bp/library/ext/cteb/*_m.cc ion/bp/library/ext/cteb/*_m.h ion/bp/library/ext/cteb/*_sm.cc ion/bp/library/ext/cteb/*_sm.h
	$(Q)-rm -f ion/bp/library/ext/ecos/*_m.cc ion/bp/library/ext/ecos/*_m.h ion/bp/library/ext/ecos/*_sm.cc ion/bp/library/ext/ecos/*_sm.h
	$(Q)-rm -f ion/bp/library/ext/meb/*_m.cc ion/bp/library/ext/meb/*_m.h ion/bp/library/ext/meb/*_sm.cc ion/bp/library/ext/meb/*_sm.h
	$(Q)-rm -f ion/bp/library/ext/phn/*_m.cc ion/bp/library/ext/phn/*_m.h ion/bp/library/ext/phn/*_sm.cc ion/bp/library/ext/phn/*_sm.h
	$(Q)-rm -f ion/bp/library/ext/snid/*_m.cc ion/bp/library/ext/snid/*_m.h ion/bp/library/ext/snid/*_sm.cc ion/bp/library/ext/snid/*_sm.h
	$(Q)-rm -f ion/bp/utils/*_m.cc ion/bp/utils/*_m.h ion/bp/utils/*_sm.cc ion/bp/utils/*_sm.h
	$(Q)-rm -f ion/ici/*_m.cc ion/ici/*_m.h ion/ici/*_sm.cc ion/ici/*_sm.h
	$(Q)-rm -f ion/ici/include/*_m.cc ion/ici/include/*_m.h ion/ici/include/*_sm.cc ion/ici/include/*_sm.h
	$(Q)-rm -f ion/ici/library/*_m.cc ion/ici/library/*_m.h ion/ici/library/*_sm.cc ion/ici/library/*_sm.h
	$(Q)-rm -f ion/ici/sdr/*_m.cc ion/ici/sdr/*_m.h ion/ici/sdr/*_sm.cc ion/ici/sdr/*_sm.h
	$(Q)-rm -f ion/ici/utils/*_m.cc ion/ici/utils/*_m.h ion/ici/utils/*_sm.cc ion/ici/utils/*_sm.h
	$(Q)-rm -f node/*_m.cc node/*_m.h node/*_sm.cc node/*_sm.h
	$(Q)-rm -f node/app/*_m.cc node/app/*_m.h node/app/*_sm.cc node/app/*_sm.h
	$(Q)-rm -f node/mac/*_m.cc node/mac/*_m.h node/mac/*_sm.cc node/mac/*_sm.h
	$(Q)-rm -f node/net/*_m.cc node/net/*_m.h node/net/*_sm.cc node/net/*_sm.h

cleanall: clean
	$(Q)-rm -rf $(PROJECT_OUTPUT_DIR)

depend:
	$(qecho) Creating dependencies...
	$(Q)$(MAKEDEPEND) $(INCLUDE_PATH) -f Makefile -P\$$O/ -- $(MSG_CC_FILES) $(SM_CC_FILES)  ./*.cc contactPlan/*.cc ion/*.cc ion/bp/*.cc ion/bp/cgr/*.cc ion/bp/include/*.cc ion/bp/library/*.cc ion/bp/library/acs/*.cc ion/bp/library/ext/*.cc ion/bp/library/ext/bae/*.cc ion/bp/library/ext/bpsec/*.cc ion/bp/library/ext/bsp/*.cc ion/bp/library/ext/bsp/ciphersuites/*.cc ion/bp/library/ext/cteb/*.cc ion/bp/library/ext/ecos/*.cc ion/bp/library/ext/meb/*.cc ion/bp/library/ext/phn/*.cc ion/bp/library/ext/snid/*.cc ion/bp/utils/*.cc ion/ici/*.cc ion/ici/include/*.cc ion/ici/library/*.cc ion/ici/sdr/*.cc ion/ici/utils/*.cc node/*.cc node/app/*.cc node/mac/*.cc node/net/*.cc

# DO NOT DELETE THIS LINE -- make depend depends on it.
$O/dtnsim_m.o: dtnsim_m.cc \
	dtnsim_m.h
$O/ion/Ion.o: ion/Ion.cc \
	ion/Ion.h
$O/ion/bp/cgr/libcgr.o: ion/bp/cgr/libcgr.cc \
	ion/bp/include/bp.h \
	ion/bp/include/bpP.h \
	ion/bp/include/cgr.h \
	ion/config.h \
	ion/ici/include/ion.h \
	ion/ici/include/ionsec.h \
	ion/ici/include/lyst.h \
	ion/ici/include/memmgr.h \
	ion/ici/include/platform.h \
	ion/ici/include/psm.h \
	ion/ici/include/rfx.h \
	ion/ici/include/sdr.h \
	ion/ici/include/sdrlist.h \
	ion/ici/include/sdrmgt.h \
	ion/ici/include/sdrstring.h \
	ion/ici/include/sdrxn.h \
	ion/ici/include/smlist.h \
	ion/ici/include/smrbt.h \
	ion/ici/include/zco.h \
	node/net/SdrStatus.h
$O/ion/bp/library/bei.o: ion/bp/library/bei.cc \
	ion/bp/include/bei.h \
	ion/bp/include/bp.h \
	ion/bp/include/bpP.h \
	ion/bp/library/ext/bae/bae.h \
	ion/bp/library/ext/bpextensions.cc \
	ion/bp/library/ext/ecos/ecos.h \
	ion/bp/library/ext/meb/meb.h \
	ion/bp/library/ext/phn/phn.h \
	ion/config.h \
	ion/ici/include/ion.h \
	ion/ici/include/ionsec.h \
	ion/ici/include/lyst.h \
	ion/ici/include/memmgr.h \
	ion/ici/include/platform.h \
	ion/ici/include/psm.h \
	ion/ici/include/rfx.h \
	ion/ici/include/sdr.h \
	ion/ici/include/sdrlist.h \
	ion/ici/include/sdrmgt.h \
	ion/ici/include/sdrstring.h \
	ion/ici/include/sdrxn.h \
	ion/ici/include/smlist.h \
	ion/ici/include/smrbt.h \
	ion/ici/include/zco.h \
	node/net/SdrStatus.h
$O/ion/bp/library/dtn2fw.o: ion/bp/library/dtn2fw.cc \
	ion/bp/include/bp.h \
	ion/bp/include/bpP.h \
	ion/bp/include/dtn2fw.h \
	ion/config.h \
	ion/ici/include/ion.h \
	ion/ici/include/ionsec.h \
	ion/ici/include/lyst.h \
	ion/ici/include/memmgr.h \
	ion/ici/include/platform.h \
	ion/ici/include/psm.h \
	ion/ici/include/rfx.h \
	ion/ici/include/sdr.h \
	ion/ici/include/sdrlist.h \
	ion/ici/include/sdrmgt.h \
	ion/ici/include/sdrstring.h \
	ion/ici/include/sdrxn.h \
	ion/ici/include/smlist.h \
	ion/ici/include/smrbt.h \
	ion/ici/include/zco.h \
	node/net/SdrStatus.h
$O/ion/bp/library/eureka.o: ion/bp/library/eureka.cc \
	ion/bp/include/bp.h \
	ion/bp/include/bpP.h \
	ion/bp/include/cgr.h \
	ion/bp/include/dtn2fw.h \
	ion/bp/include/eureka.h \
	ion/bp/include/ipnfw.h \
	ion/config.h \
	ion/ici/include/ion.h \
	ion/ici/include/ionsec.h \
	ion/ici/include/lyst.h \
	ion/ici/include/memmgr.h \
	ion/ici/include/platform.h \
	ion/ici/include/psm.h \
	ion/ici/include/rfx.h \
	ion/ici/include/sdr.h \
	ion/ici/include/sdrlist.h \
	ion/ici/include/sdrmgt.h \
	ion/ici/include/sdrstring.h \
	ion/ici/include/sdrxn.h \
	ion/ici/include/smlist.h \
	ion/ici/include/smrbt.h \
	ion/ici/include/zco.h \
	node/net/SdrStatus.h
$O/ion/bp/library/libbp.o: ion/bp/library/libbp.cc \
	ion/bp/include/bp.h \
	ion/bp/include/bpP.h \
	ion/config.h \
	ion/ici/include/ion.h \
	ion/ici/include/ionsec.h \
	ion/ici/include/lyst.h \
	ion/ici/include/memmgr.h \
	ion/ici/include/platform.h \
	ion/ici/include/psm.h \
	ion/ici/include/rfx.h \
	ion/ici/include/sdr.h \
	ion/ici/include/sdrlist.h \
	ion/ici/include/sdrmgt.h \
	ion/ici/include/sdrstring.h \
	ion/ici/include/sdrxn.h \
	ion/ici/include/smlist.h \
	ion/ici/include/smrbt.h \
	ion/ici/include/zco.h \
	node/net/SdrStatus.h
$O/ion/bp/library/libbpP.o: ion/bp/library/libbpP.cc \
	ion/bp/include/bei.h \
	ion/bp/include/bp.h \
	ion/bp/include/bpP.h \
	ion/bp/include/cgr.h \
	ion/bp/include/dtn2fw.h \
	ion/bp/include/eureka.h \
	ion/bp/include/ipnfw.h \
	ion/config.h \
	ion/ici/include/ion.h \
	ion/ici/include/ionsec.h \
	ion/ici/include/lyst.h \
	ion/ici/include/memmgr.h \
	ion/ici/include/platform.h \
	ion/ici/include/psm.h \
	ion/ici/include/rfx.h \
	ion/ici/include/sdr.h \
	ion/ici/include/sdrhash.h \
	ion/ici/include/sdrlist.h \
	ion/ici/include/sdrmgt.h \
	ion/ici/include/sdrstring.h \
	ion/ici/include/sdrxn.h \
	ion/ici/include/smlist.h \
	ion/ici/include/smrbt.h \
	ion/ici/include/zco.h \
	node/net/SdrStatus.h
$O/ion/bp/library/libbpnm.o: ion/bp/library/libbpnm.cc \
	ion/bp/include/bei.h \
	ion/bp/include/bp.h \
	ion/bp/include/bpP.h \
	ion/bp/include/bpnm.h \
	ion/config.h \
	ion/ici/include/ion.h \
	ion/ici/include/ionsec.h \
	ion/ici/include/lyst.h \
	ion/ici/include/memmgr.h \
	ion/ici/include/platform.h \
	ion/ici/include/psm.h \
	ion/ici/include/rfx.h \
	ion/ici/include/sdr.h \
	ion/ici/include/sdrlist.h \
	ion/ici/include/sdrmgt.h \
	ion/ici/include/sdrstring.h \
	ion/ici/include/sdrxn.h \
	ion/ici/include/smlist.h \
	ion/ici/include/smrbt.h \
	ion/ici/include/zco.h \
	node/net/SdrStatus.h
$O/ion/bp/library/libdtn2fw.o: ion/bp/library/libdtn2fw.cc \
	ion/bp/include/bp.h \
	ion/bp/include/bpP.h \
	ion/bp/include/dtn2fw.h \
	ion/config.h \
	ion/ici/include/ion.h \
	ion/ici/include/ionsec.h \
	ion/ici/include/lyst.h \
	ion/ici/include/memmgr.h \
	ion/ici/include/platform.h \
	ion/ici/include/psm.h \
	ion/ici/include/rfx.h \
	ion/ici/include/sdr.h \
	ion/ici/include/sdrlist.h \
	ion/ici/include/sdrmgt.h \
	ion/ici/include/sdrstring.h \
	ion/ici/include/sdrxn.h \
	ion/ici/include/smlist.h \
	ion/ici/include/smrbt.h \
	ion/ici/include/zco.h \
	node/net/SdrStatus.h
$O/ion/bp/library/libipnfw.o: ion/bp/library/libipnfw.cc \
	ion/bp/include/bp.h \
	ion/bp/include/bpP.h \
	ion/bp/include/cgr.h \
	ion/bp/include/ipnfw.h \
	ion/config.h \
	ion/ici/include/ion.h \
	ion/ici/include/ionsec.h \
	ion/ici/include/lyst.h \
	ion/ici/include/memmgr.h \
	ion/ici/include/platform.h \
	ion/ici/include/psm.h \
	ion/ici/include/rfx.h \
	ion/ici/include/sdr.h \
	ion/ici/include/sdrlist.h \
	ion/ici/include/sdrmgt.h \
	ion/ici/include/sdrstring.h \
	ion/ici/include/sdrxn.h \
	ion/ici/include/smlist.h \
	ion/ici/include/smrbt.h \
	ion/ici/include/zco.h \
	node/net/SdrStatus.h
$O/ion/bp/library/acs/acsappend.o: ion/bp/library/acs/acsappend.cc \
	ion/bp/include/bei.h \
	ion/bp/include/bp.h \
	ion/bp/include/bpP.h \
	ion/bp/library/acs/acs.h \
	ion/bp/library/acs/acsP.h \
	ion/bp/library/ext/cteb/cteb.h \
	ion/config.h \
	ion/ici/include/ion.h \
	ion/ici/include/ionsec.h \
	ion/ici/include/lyst.h \
	ion/ici/include/memmgr.h \
	ion/ici/include/platform.h \
	ion/ici/include/psm.h \
	ion/ici/include/rfx.h \
	ion/ici/include/sdr.h \
	ion/ici/include/sdrlist.h \
	ion/ici/include/sdrmgt.h \
	ion/ici/include/sdrstring.h \
	ion/ici/include/sdrxn.h \
	ion/ici/include/smlist.h \
	ion/ici/include/smrbt.h \
	ion/ici/include/zco.h \
	node/net/SdrStatus.h
$O/ion/bp/library/acs/acsid.o: ion/bp/library/acs/acsid.cc \
	ion/bp/include/bei.h \
	ion/bp/include/bp.h \
	ion/bp/include/bpP.h \
	ion/bp/library/acs/acs.h \
	ion/bp/library/acs/acsP.h \
	ion/bp/library/ext/cteb/cteb.h \
	ion/config.h \
	ion/ici/include/ion.h \
	ion/ici/include/ionsec.h \
	ion/ici/include/lyst.h \
	ion/ici/include/memmgr.h \
	ion/ici/include/platform.h \
	ion/ici/include/psm.h \
	ion/ici/include/rfx.h \
	ion/ici/include/sdr.h \
	ion/ici/include/sdrhash.h \
	ion/ici/include/sdrlist.h \
	ion/ici/include/sdrmgt.h \
	ion/ici/include/sdrstring.h \
	ion/ici/include/sdrxn.h \
	ion/ici/include/smlist.h \
	ion/ici/include/smrbt.h \
	ion/ici/include/zco.h \
	node/net/SdrStatus.h
$O/ion/bp/library/acs/acsrx.o: ion/bp/library/acs/acsrx.cc \
	ion/bp/include/bei.h \
	ion/bp/include/bp.h \
	ion/bp/include/bpP.h \
	ion/bp/library/acs/acs.h \
	ion/bp/library/acs/acsP.h \
	ion/bp/library/ext/cteb/cteb.h \
	ion/config.h \
	ion/ici/include/ion.h \
	ion/ici/include/ionsec.h \
	ion/ici/include/lyst.h \
	ion/ici/include/memmgr.h \
	ion/ici/include/platform.h \
	ion/ici/include/psm.h \
	ion/ici/include/rfx.h \
	ion/ici/include/sdr.h \
	ion/ici/include/sdrlist.h \
	ion/ici/include/sdrmgt.h \
	ion/ici/include/sdrstring.h \
	ion/ici/include/sdrxn.h \
	ion/ici/include/smlist.h \
	ion/ici/include/smrbt.h \
	ion/ici/include/zco.h \
	node/net/SdrStatus.h
$O/ion/bp/library/acs/acsserialize.o: ion/bp/library/acs/acsserialize.cc \
	ion/bp/include/bei.h \
	ion/bp/include/bp.h \
	ion/bp/include/bpP.h \
	ion/bp/library/acs/acs.h \
	ion/bp/library/acs/acsP.h \
	ion/bp/library/ext/cteb/cteb.h \
	ion/config.h \
	ion/ici/include/ion.h \
	ion/ici/include/ionsec.h \
	ion/ici/include/lyst.h \
	ion/ici/include/memmgr.h \
	ion/ici/include/platform.h \
	ion/ici/include/psm.h \
	ion/ici/include/rfx.h \
	ion/ici/include/sdr.h \
	ion/ici/include/sdrlist.h \
	ion/ici/include/sdrmgt.h \
	ion/ici/include/sdrstring.h \
	ion/ici/include/sdrxn.h \
	ion/ici/include/smlist.h \
	ion/ici/include/smrbt.h \
	ion/ici/include/zco.h \
	node/net/SdrStatus.h
$O/ion/bp/library/acs/acstx.o: ion/bp/library/acs/acstx.cc \
	ion/bp/include/bei.h \
	ion/bp/include/bp.h \
	ion/bp/include/bpP.h \
	ion/bp/library/acs/acs.h \
	ion/bp/library/acs/acsP.h \
	ion/bp/library/ext/cteb/cteb.h \
	ion/config.h \
	ion/ici/include/ion.h \
	ion/ici/include/ionsec.h \
	ion/ici/include/lyst.h \
	ion/ici/include/memmgr.h \
	ion/ici/include/platform.h \
	ion/ici/include/psm.h \
	ion/ici/include/rfx.h \
	ion/ici/include/sdr.h \
	ion/ici/include/sdrhash.h \
	ion/ici/include/sdrlist.h \
	ion/ici/include/sdrmgt.h \
	ion/ici/include/sdrstring.h \
	ion/ici/include/sdrxn.h \
	ion/ici/include/smlist.h \
	ion/ici/include/smrbt.h \
	ion/ici/include/zco.h \
	node/net/SdrStatus.h
$O/ion/bp/library/ext/bpextensions.o: ion/bp/library/ext/bpextensions.cc \
	ion/bp/include/bei.h \
	ion/bp/include/bp.h \
	ion/bp/include/bpP.h \
	ion/bp/library/ext/bae/bae.h \
	ion/bp/library/ext/ecos/ecos.h \
	ion/bp/library/ext/meb/meb.h \
	ion/bp/library/ext/phn/phn.h \
	ion/config.h \
	ion/ici/include/ion.h \
	ion/ici/include/ionsec.h \
	ion/ici/include/lyst.h \
	ion/ici/include/memmgr.h \
	ion/ici/include/platform.h \
	ion/ici/include/psm.h \
	ion/ici/include/rfx.h \
	ion/ici/include/sdr.h \
	ion/ici/include/sdrlist.h \
	ion/ici/include/sdrmgt.h \
	ion/ici/include/sdrstring.h \
	ion/ici/include/sdrxn.h \
	ion/ici/include/smlist.h \
	ion/ici/include/smrbt.h \
	ion/ici/include/zco.h \
	node/net/SdrStatus.h
$O/ion/bp/library/ext/bae/bae.o: ion/bp/library/ext/bae/bae.cc \
	ion/bp/include/bei.h \
	ion/bp/include/bp.h \
	ion/bp/include/bpP.h \
	ion/bp/library/ext/bae/bae.h \
	ion/config.h \
	ion/ici/include/ion.h \
	ion/ici/include/ionsec.h \
	ion/ici/include/lyst.h \
	ion/ici/include/memmgr.h \
	ion/ici/include/platform.h \
	ion/ici/include/psm.h \
	ion/ici/include/rfx.h \
	ion/ici/include/sdr.h \
	ion/ici/include/sdrlist.h \
	ion/ici/include/sdrmgt.h \
	ion/ici/include/sdrstring.h \
	ion/ici/include/sdrxn.h \
	ion/ici/include/smlist.h \
	ion/ici/include/smrbt.h \
	ion/ici/include/zco.h \
	node/net/SdrStatus.h
$O/ion/bp/library/ext/bpsec/bpsec_bcb.o: ion/bp/library/ext/bpsec/bpsec_bcb.cc
$O/ion/bp/library/ext/bpsec/bpsec_bib.o: ion/bp/library/ext/bpsec/bpsec_bib.cc
$O/ion/bp/library/ext/bpsec/bpsec_instr.o: ion/bp/library/ext/bpsec/bpsec_instr.cc
$O/ion/bp/library/ext/bpsec/bpsec_util.o: ion/bp/library/ext/bpsec/bpsec_util.cc
$O/ion/bp/library/ext/bpsec/profiles.o: ion/bp/library/ext/bpsec/profiles.cc
$O/ion/bp/library/ext/bsp/bspbab.o: ion/bp/library/ext/bsp/bspbab.cc
$O/ion/bp/library/ext/bsp/bspbcb.o: ion/bp/library/ext/bsp/bspbcb.cc
$O/ion/bp/library/ext/bsp/bspbib.o: ion/bp/library/ext/bsp/bspbib.cc
$O/ion/bp/library/ext/bsp/bsputil.o: ion/bp/library/ext/bsp/bsputil.cc
$O/ion/bp/library/ext/bsp/ciphersuites.o: ion/bp/library/ext/bsp/ciphersuites.cc
$O/ion/bp/library/ext/bsp/extbspbab.o: ion/bp/library/ext/bsp/extbspbab.cc \
	ion/bp/include/bei.h \
	ion/bp/include/bp.h \
	ion/bp/include/bpP.h \
	ion/bp/include/crypto.h \
	ion/bp/library/ext/bsp/extbspbab.h \
	ion/bp/library/ext/bsp/extbsputil.h \
	ion/config.h \
	ion/ici/include/ion.h \
	ion/ici/include/ionsec.h \
	ion/ici/include/lyst.h \
	ion/ici/include/memmgr.h \
	ion/ici/include/platform.h \
	ion/ici/include/psm.h \
	ion/ici/include/rfx.h \
	ion/ici/include/sdr.h \
	ion/ici/include/sdrlist.h \
	ion/ici/include/sdrmgt.h \
	ion/ici/include/sdrstring.h \
	ion/ici/include/sdrxn.h \
	ion/ici/include/smlist.h \
	ion/ici/include/smrbt.h \
	ion/ici/include/zco.h \
	node/net/SdrStatus.h
$O/ion/bp/library/ext/bsp/extbsppcb.o: ion/bp/library/ext/bsp/extbsppcb.cc \
	ion/bp/include/bei.h \
	ion/bp/include/bp.h \
	ion/bp/include/bpP.h \
	ion/bp/include/crypto.h \
	ion/bp/library/ext/bsp/extbsppcb.h \
	ion/bp/library/ext/bsp/extbsputil.h \
	ion/config.h \
	ion/ici/include/ion.h \
	ion/ici/include/ionsec.h \
	ion/ici/include/lyst.h \
	ion/ici/include/memmgr.h \
	ion/ici/include/platform.h \
	ion/ici/include/psm.h \
	ion/ici/include/rfx.h \
	ion/ici/include/sdr.h \
	ion/ici/include/sdrlist.h \
	ion/ici/include/sdrmgt.h \
	ion/ici/include/sdrstring.h \
	ion/ici/include/sdrxn.h \
	ion/ici/include/smlist.h \
	ion/ici/include/smrbt.h \
	ion/ici/include/zco.h \
	node/net/SdrStatus.h
$O/ion/bp/library/ext/bsp/extbsppib.o: ion/bp/library/ext/bsp/extbsppib.cc \
	ion/bp/include/bei.h \
	ion/bp/include/bp.h \
	ion/bp/include/bpP.h \
	ion/bp/include/crypto.h \
	ion/bp/library/ext/bsp/extbsppib.h \
	ion/bp/library/ext/bsp/extbsputil.h \
	ion/config.h \
	ion/ici/include/ion.h \
	ion/ici/include/ionsec.h \
	ion/ici/include/lyst.h \
	ion/ici/include/memmgr.h \
	ion/ici/include/platform.h \
	ion/ici/include/psm.h \
	ion/ici/include/rfx.h \
	ion/ici/include/sdr.h \
	ion/ici/include/sdrlist.h \
	ion/ici/include/sdrmgt.h \
	ion/ici/include/sdrstring.h \
	ion/ici/include/sdrxn.h \
	ion/ici/include/smlist.h \
	ion/ici/include/smrbt.h \
	ion/ici/include/zco.h \
	node/net/SdrStatus.h
$O/ion/bp/library/ext/bsp/extbsputil.o: ion/bp/library/ext/bsp/extbsputil.cc \
	ion/bp/include/bei.h \
	ion/bp/include/bp.h \
	ion/bp/include/bpP.h \
	ion/bp/library/ext/bsp/extbsputil.h \
	ion/config.h \
	ion/ici/include/ion.h \
	ion/ici/include/ionsec.h \
	ion/ici/include/lyst.h \
	ion/ici/include/memmgr.h \
	ion/ici/include/platform.h \
	ion/ici/include/psm.h \
	ion/ici/include/rfx.h \
	ion/ici/include/sdr.h \
	ion/ici/include/sdrlist.h \
	ion/ici/include/sdrmgt.h \
	ion/ici/include/sdrstring.h \
	ion/ici/include/sdrxn.h \
	ion/ici/include/smlist.h \
	ion/ici/include/smrbt.h \
	ion/ici/include/zco.h \
	node/net/SdrStatus.h
$O/ion/bp/library/ext/bsp/ciphersuites/bab_hmac_sha1.o: ion/bp/library/ext/bsp/ciphersuites/bab_hmac_sha1.cc
$O/ion/bp/library/ext/bsp/ciphersuites/bcb_arc4.o: ion/bp/library/ext/bsp/ciphersuites/bcb_arc4.cc
$O/ion/bp/library/ext/bsp/ciphersuites/bib_hmac_sha256.o: ion/bp/library/ext/bsp/ciphersuites/bib_hmac_sha256.cc
$O/ion/bp/library/ext/cteb/cteb.o: ion/bp/library/ext/cteb/cteb.cc \
	ion/bp/include/bei.h \
	ion/bp/include/bp.h \
	ion/bp/include/bpP.h \
	ion/bp/library/acs/acs.h \
	ion/bp/library/acs/acsP.h \
	ion/bp/library/ext/cteb/cteb.h \
	ion/config.h \
	ion/ici/include/ion.h \
	ion/ici/include/ionsec.h \
	ion/ici/include/lyst.h \
	ion/ici/include/memmgr.h \
	ion/ici/include/platform.h \
	ion/ici/include/psm.h \
	ion/ici/include/rfx.h \
	ion/ici/include/sdr.h \
	ion/ici/include/sdrlist.h \
	ion/ici/include/sdrmgt.h \
	ion/ici/include/sdrstring.h \
	ion/ici/include/sdrxn.h \
	ion/ici/include/smlist.h \
	ion/ici/include/smrbt.h \
	ion/ici/include/zco.h \
	node/net/SdrStatus.h
$O/ion/bp/library/ext/ecos/ecos.o: ion/bp/library/ext/ecos/ecos.cc \
	ion/bp/include/bei.h \
	ion/bp/include/bp.h \
	ion/bp/include/bpP.h \
	ion/bp/library/ext/ecos/ecos.h \
	ion/config.h \
	ion/ici/include/ion.h \
	ion/ici/include/ionsec.h \
	ion/ici/include/lyst.h \
	ion/ici/include/memmgr.h \
	ion/ici/include/platform.h \
	ion/ici/include/psm.h \
	ion/ici/include/rfx.h \
	ion/ici/include/sdr.h \
	ion/ici/include/sdrlist.h \
	ion/ici/include/sdrmgt.h \
	ion/ici/include/sdrstring.h \
	ion/ici/include/sdrxn.h \
	ion/ici/include/smlist.h \
	ion/ici/include/smrbt.h \
	ion/ici/include/zco.h \
	node/net/SdrStatus.h
$O/ion/bp/library/ext/meb/meb.o: ion/bp/library/ext/meb/meb.cc \
	ion/bp/include/bei.h \
	ion/bp/include/bp.h \
	ion/bp/include/bpP.h \
	ion/bp/library/ext/meb/meb.h \
	ion/config.h \
	ion/ici/include/ion.h \
	ion/ici/include/ionsec.h \
	ion/ici/include/lyst.h \
	ion/ici/include/memmgr.h \
	ion/ici/include/platform.h \
	ion/ici/include/psm.h \
	ion/ici/include/rfx.h \
	ion/ici/include/sdr.h \
	ion/ici/include/sdrlist.h \
	ion/ici/include/sdrmgt.h \
	ion/ici/include/sdrstring.h \
	ion/ici/include/sdrxn.h \
	ion/ici/include/smlist.h \
	ion/ici/include/smrbt.h \
	ion/ici/include/zco.h \
	node/net/SdrStatus.h
$O/ion/bp/library/ext/phn/phn.o: ion/bp/library/ext/phn/phn.cc \
	ion/bp/include/bei.h \
	ion/bp/include/bp.h \
	ion/bp/include/bpP.h \
	ion/bp/library/ext/phn/phn.h \
	ion/config.h \
	ion/ici/include/ion.h \
	ion/ici/include/ionsec.h \
	ion/ici/include/lyst.h \
	ion/ici/include/memmgr.h \
	ion/ici/include/platform.h \
	ion/ici/include/psm.h \
	ion/ici/include/rfx.h \
	ion/ici/include/sdr.h \
	ion/ici/include/sdrlist.h \
	ion/ici/include/sdrmgt.h \
	ion/ici/include/sdrstring.h \
	ion/ici/include/sdrxn.h \
	ion/ici/include/smlist.h \
	ion/ici/include/smrbt.h \
	ion/ici/include/zco.h \
	node/net/SdrStatus.h
$O/ion/bp/library/ext/snid/snid.o: ion/bp/library/ext/snid/snid.cc \
	ion/bp/include/bei.h \
	ion/bp/include/bp.h \
	ion/bp/include/bpP.h \
	ion/bp/library/ext/snid/snid.h \
	ion/config.h \
	ion/ici/include/ion.h \
	ion/ici/include/ionsec.h \
	ion/ici/include/lyst.h \
	ion/ici/include/memmgr.h \
	ion/ici/include/platform.h \
	ion/ici/include/psm.h \
	ion/ici/include/rfx.h \
	ion/ici/include/sdr.h \
	ion/ici/include/sdrlist.h \
	ion/ici/include/sdrmgt.h \
	ion/ici/include/sdrstring.h \
	ion/ici/include/sdrxn.h \
	ion/ici/include/smlist.h \
	ion/ici/include/smrbt.h \
	ion/ici/include/zco.h \
	node/net/SdrStatus.h
$O/ion/bp/utils/bpadmin.o: ion/bp/utils/bpadmin.cc \
	ion/bp/include/bp.h \
	ion/bp/include/bpP.h \
	ion/bp/include/bpadmin.h \
	ion/bp/include/crypto.h \
	ion/config.h \
	ion/ici/include/csi.h \
	ion/ici/include/ion.h \
	ion/ici/include/ionsec.h \
	ion/ici/include/lyst.h \
	ion/ici/include/memmgr.h \
	ion/ici/include/platform.h \
	ion/ici/include/psm.h \
	ion/ici/include/rfx.h \
	ion/ici/include/sdr.h \
	ion/ici/include/sdrlist.h \
	ion/ici/include/sdrmgt.h \
	ion/ici/include/sdrstring.h \
	ion/ici/include/sdrxn.h \
	ion/ici/include/smlist.h \
	ion/ici/include/smrbt.h \
	ion/ici/include/zco.h \
	node/net/SdrStatus.h
$O/ion/ici/library/bulk.o: ion/ici/library/bulk.cc \
	ion/config.h \
	ion/ici/include/bulk.h \
	ion/ici/include/ion.h \
	ion/ici/include/memmgr.h \
	ion/ici/include/platform.h \
	ion/ici/include/psm.h \
	ion/ici/include/sdr.h \
	ion/ici/include/sdrlist.h \
	ion/ici/include/sdrmgt.h \
	ion/ici/include/sdrstring.h \
	ion/ici/include/sdrxn.h \
	ion/ici/include/smlist.h \
	ion/ici/include/smrbt.h \
	ion/ici/include/zco.h \
	node/net/SdrStatus.h
$O/ion/ici/library/ion.o: ion/ici/library/ion.cc \
	ion/config.h \
	ion/ici/include/ion.h \
	ion/ici/include/memmgr.h \
	ion/ici/include/platform.h \
	ion/ici/include/psm.h \
	ion/ici/include/rfx.h \
	ion/ici/include/sdr.h \
	ion/ici/include/sdrlist.h \
	ion/ici/include/sdrmgt.h \
	ion/ici/include/sdrstring.h \
	ion/ici/include/sdrxn.h \
	ion/ici/include/smlist.h \
	ion/ici/include/smrbt.h \
	ion/ici/include/zco.h \
	node/net/SdrStatus.h
$O/ion/ici/library/ionsec.o: ion/ici/library/ionsec.cc \
	ion/config.h \
	ion/ici/include/ion.h \
	ion/ici/include/ionsec.h \
	ion/ici/include/memmgr.h \
	ion/ici/include/platform.h \
	ion/ici/include/psm.h \
	ion/ici/include/sdr.h \
	ion/ici/include/sdrlist.h \
	ion/ici/include/sdrmgt.h \
	ion/ici/include/sdrstring.h \
	ion/ici/include/sdrxn.h \
	ion/ici/include/smlist.h \
	ion/ici/include/smrbt.h \
	ion/ici/include/zco.h \
	node/net/SdrStatus.h
$O/ion/ici/library/lyst.o: ion/ici/library/lyst.cc \
	ion/config.h \
	ion/ici/include/lyst.h \
	ion/ici/include/lystP.h \
	ion/ici/include/memmgr.h \
	ion/ici/include/platform.h \
	ion/ici/include/psm.h
$O/ion/ici/library/memmgr.o: ion/ici/library/memmgr.cc \
	ion/config.h \
	ion/ici/include/memmgr.h \
	ion/ici/include/platform.h \
	ion/ici/include/psm.h
$O/ion/ici/library/platform.o: ion/ici/library/platform.cc \
	ion/config.h \
	ion/ici/include/platform.h
$O/ion/ici/library/psm.o: ion/ici/library/psm.cc \
	ion/config.h \
	ion/ici/include/platform.h \
	ion/ici/include/psm.h \
	ion/ici/include/smlist.h \
	ion/ici/include/sptrace.h
$O/ion/ici/library/rfx.o: ion/ici/library/rfx.cc \
	ion/config.h \
	ion/ici/include/ion.h \
	ion/ici/include/lyst.h \
	ion/ici/include/memmgr.h \
	ion/ici/include/platform.h \
	ion/ici/include/psm.h \
	ion/ici/include/rfx.h \
	ion/ici/include/sdr.h \
	ion/ici/include/sdrlist.h \
	ion/ici/include/sdrmgt.h \
	ion/ici/include/sdrstring.h \
	ion/ici/include/sdrxn.h \
	ion/ici/include/smlist.h \
	ion/ici/include/smrbt.h \
	ion/ici/include/zco.h \
	node/net/SdrStatus.h
$O/ion/ici/library/smlist.o: ion/ici/library/smlist.cc \
	ion/config.h \
	ion/ici/include/platform.h \
	ion/ici/include/psm.h \
	ion/ici/include/smlist.h
$O/ion/ici/library/smrbt.o: ion/ici/library/smrbt.cc \
	ion/config.h \
	ion/ici/include/platform.h \
	ion/ici/include/psm.h \
	ion/ici/include/smrbt.h
$O/ion/ici/library/sptrace.o: ion/ici/library/sptrace.cc \
	ion/config.h \
	ion/ici/include/platform.h \
	ion/ici/include/psm.h \
	ion/ici/include/smlist.h \
	ion/ici/include/sptrace.h
$O/ion/ici/library/zco.o: ion/ici/library/zco.cc \
	ion/config.h \
	ion/ici/include/bulk.h \
	ion/ici/include/platform.h \
	ion/ici/include/psm.h \
	ion/ici/include/sdr.h \
	ion/ici/include/sdrlist.h \
	ion/ici/include/sdrmgt.h \
	ion/ici/include/sdrstring.h \
	ion/ici/include/sdrxn.h \
	ion/ici/include/zco.h
$O/ion/ici/sdr/sdrcatlg.o: ion/ici/sdr/sdrcatlg.cc \
	ion/config.h \
	ion/ici/include/lyst.h \
	ion/ici/include/memmgr.h \
	ion/ici/include/platform.h \
	ion/ici/include/psm.h \
	ion/ici/include/sdr.h \
	ion/ici/include/sdrP.h \
	ion/ici/include/sdrlist.h \
	ion/ici/include/sdrmgt.h \
	ion/ici/include/sdrstring.h \
	ion/ici/include/sdrxn.h \
	ion/ici/include/smlist.h \
	ion/ici/include/sptrace.h
$O/ion/ici/sdr/sdrhash.o: ion/ici/sdr/sdrhash.cc \
	ion/config.h \
	ion/ici/include/lyst.h \
	ion/ici/include/memmgr.h \
	ion/ici/include/platform.h \
	ion/ici/include/psm.h \
	ion/ici/include/sdrP.h \
	ion/ici/include/sdrhash.h \
	ion/ici/include/sdrlist.h \
	ion/ici/include/sdrmgt.h \
	ion/ici/include/sdrtable.h \
	ion/ici/include/sdrxn.h \
	ion/ici/include/smlist.h \
	ion/ici/include/sptrace.h
$O/ion/ici/sdr/sdrlist.o: ion/ici/sdr/sdrlist.cc \
	ion/config.h \
	ion/ici/include/lyst.h \
	ion/ici/include/memmgr.h \
	ion/ici/include/platform.h \
	ion/ici/include/psm.h \
	ion/ici/include/sdrP.h \
	ion/ici/include/sdrlist.h \
	ion/ici/include/sdrmgt.h \
	ion/ici/include/sdrxn.h \
	ion/ici/include/smlist.h \
	ion/ici/include/sptrace.h
$O/ion/ici/sdr/sdrmgt.o: ion/ici/sdr/sdrmgt.cc \
	ion/config.h \
	ion/ici/include/lyst.h \
	ion/ici/include/memmgr.h \
	ion/ici/include/platform.h \
	ion/ici/include/psm.h \
	ion/ici/include/sdrP.h \
	ion/ici/include/sdrmgt.h \
	ion/ici/include/sdrxn.h \
	ion/ici/include/smlist.h \
	ion/ici/include/sptrace.h
$O/ion/ici/sdr/sdrstring.o: ion/ici/sdr/sdrstring.cc \
	ion/config.h \
	ion/ici/include/lyst.h \
	ion/ici/include/memmgr.h \
	ion/ici/include/platform.h \
	ion/ici/include/psm.h \
	ion/ici/include/sdrP.h \
	ion/ici/include/sdrmgt.h \
	ion/ici/include/sdrstring.h \
	ion/ici/include/sdrxn.h \
	ion/ici/include/smlist.h \
	ion/ici/include/sptrace.h
$O/ion/ici/sdr/sdrtable.o: ion/ici/sdr/sdrtable.cc \
	ion/config.h \
	ion/ici/include/lyst.h \
	ion/ici/include/memmgr.h \
	ion/ici/include/platform.h \
	ion/ici/include/psm.h \
	ion/ici/include/sdrP.h \
	ion/ici/include/sdrmgt.h \
	ion/ici/include/sdrtable.h \
	ion/ici/include/sdrxn.h \
	ion/ici/include/smlist.h \
	ion/ici/include/sptrace.h
$O/ion/ici/sdr/sdrxn.o: ion/ici/sdr/sdrxn.cc \
	ion/config.h \
	ion/ici/include/lyst.h \
	ion/ici/include/memmgr.h \
	ion/ici/include/platform.h \
	ion/ici/include/psm.h \
	ion/ici/include/sdrP.h \
	ion/ici/include/sdrxn.h \
	ion/ici/include/smlist.h \
	ion/ici/include/sptrace.h
$O/ion/ici/utils/ionadmin.o: ion/ici/utils/ionadmin.cc \
	ion/config.h \
	ion/ici/include/ion.h \
	ion/ici/include/ionadmin.h \
	ion/ici/include/memmgr.h \
	ion/ici/include/platform.h \
	ion/ici/include/psm.h \
	ion/ici/include/rfx.h \
	ion/ici/include/sdr.h \
	ion/ici/include/sdrlist.h \
	ion/ici/include/sdrmgt.h \
	ion/ici/include/sdrstring.h \
	ion/ici/include/sdrxn.h \
	ion/ici/include/smlist.h \
	ion/ici/include/smrbt.h \
	ion/ici/include/zco.h \
	node/net/SdrStatus.h
$O/node/app/App.o: node/app/App.cc \
	dtnsim_m.h \
	node/app/App.h
$O/node/mac/Mac.o: node/mac/Mac.cc \
	dtnsim_m.h \
	node/app/App.h \
	node/mac/Mac.h
$O/node/net/Contact.o: node/net/Contact.cc \
	node/net/Contact.h
$O/node/net/ContactPlan.o: node/net/ContactPlan.cc \
	node/net/Contact.h \
	node/net/ContactPlan.h
$O/node/net/Net.o: node/net/Net.cc \
	dtnsim_m.h \
	ion/Ion.h \
	ion/bp/include/bp.h \
	ion/bp/include/bpP.h \
	ion/bp/include/bpadmin.h \
	ion/bp/include/cgr.h \
	ion/bp/include/crypto.h \
	ion/config.h \
	ion/ici/include/csi.h \
	ion/ici/include/ion.h \
	ion/ici/include/ionadmin.h \
	ion/ici/include/ionsec.h \
	ion/ici/include/lyst.h \
	ion/ici/include/memmgr.h \
	ion/ici/include/platform.h \
	ion/ici/include/psm.h \
	ion/ici/include/rfx.h \
	ion/ici/include/sdr.h \
	ion/ici/include/sdrlist.h \
	ion/ici/include/sdrmgt.h \
	ion/ici/include/sdrstring.h \
	ion/ici/include/sdrxn.h \
	ion/ici/include/smlist.h \
	ion/ici/include/smrbt.h \
	ion/ici/include/zco.h \
	node/app/App.h \
	node/net/Contact.h \
	node/net/ContactPlan.h \
	node/net/Net.h \
	node/net/Routing.h \
	node/net/RoutingCgrIon350.h \
	node/net/RoutingCgrModel.h \
	node/net/RoutingCgrModelYen.h \
	node/net/RoutingDirect.h \
	node/net/SdrModel.h \
	node/net/SdrStatus.h
$O/node/net/RoutingCgrIon350.o: node/net/RoutingCgrIon350.cc \
	dtnsim_m.h \
	ion/bp/include/bp.h \
	ion/bp/include/bpP.h \
	ion/bp/include/bpadmin.h \
	ion/bp/include/cgr.h \
	ion/bp/include/crypto.h \
	ion/config.h \
	ion/ici/include/csi.h \
	ion/ici/include/ion.h \
	ion/ici/include/ionadmin.h \
	ion/ici/include/ionsec.h \
	ion/ici/include/lyst.h \
	ion/ici/include/memmgr.h \
	ion/ici/include/platform.h \
	ion/ici/include/psm.h \
	ion/ici/include/rfx.h \
	ion/ici/include/sdr.h \
	ion/ici/include/sdrlist.h \
	ion/ici/include/sdrmgt.h \
	ion/ici/include/sdrstring.h \
	ion/ici/include/sdrxn.h \
	ion/ici/include/smlist.h \
	ion/ici/include/smrbt.h \
	ion/ici/include/zco.h \
	node/net/Contact.h \
	node/net/ContactPlan.h \
	node/net/Routing.h \
	node/net/RoutingCgrIon350.h \
	node/net/SdrModel.h \
	node/net/SdrStatus.h
$O/node/net/RoutingCgrModel.o: node/net/RoutingCgrModel.cc \
	dtnsim_m.h \
	node/net/Contact.h \
	node/net/ContactPlan.h \
	node/net/Routing.h \
	node/net/RoutingCgrModel.h \
	node/net/SdrModel.h \
	node/net/SdrStatus.h
$O/node/net/RoutingCgrModelYen.o: node/net/RoutingCgrModelYen.cc \
	dtnsim_m.h \
	node/net/Contact.h \
	node/net/ContactPlan.h \
	node/net/Routing.h \
	node/net/RoutingCgrModelYen.h \
	node/net/SdrModel.h \
	node/net/SdrStatus.h
$O/node/net/RoutingDirect.o: node/net/RoutingDirect.cc \
	dtnsim_m.h \
	node/net/Contact.h \
	node/net/ContactPlan.h \
	node/net/Routing.h \
	node/net/RoutingDirect.h \
	node/net/SdrModel.h \
	node/net/SdrStatus.h
$O/node/net/SdrModel.o: node/net/SdrModel.cc \
	dtnsim_m.h \
	node/net/Contact.h \
	node/net/ContactPlan.h \
	node/net/SdrModel.h \
	node/net/SdrStatus.h
$O/node/net/SdrStatus.o: node/net/SdrStatus.cc

