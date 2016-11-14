#
# OMNeT++/OMNEST Makefile for dtnsim
#
# This file was generated with the command:
#  opp_makemake -f --deep
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
    -Inode \
    -Inode/app \
    -Inode/mac \
    -Inode/net \
    -Iwin

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
    $O/node/app/App.o \
    $O/node/mac/Mac.o \
    $O/node/net/RoutingCgrIon350.o \
    $O/node/net/RoutingCgrModel.o \
    $O/node/net/RoutingDirect.o \
    $O/node/net/Net.o \
    $O/node/net/Contact.o \
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
	$(Q)-rm -f node/*_m.cc node/*_m.h node/*_sm.cc node/*_sm.h
	$(Q)-rm -f node/app/*_m.cc node/app/*_m.h node/app/*_sm.cc node/app/*_sm.h
	$(Q)-rm -f node/mac/*_m.cc node/mac/*_m.h node/mac/*_sm.cc node/mac/*_sm.h
	$(Q)-rm -f node/net/*_m.cc node/net/*_m.h node/net/*_sm.cc node/net/*_sm.h
	$(Q)-rm -f win/*_m.cc win/*_m.h win/*_sm.cc win/*_sm.h

cleanall: clean
	$(Q)-rm -rf $(PROJECT_OUTPUT_DIR)

depend:
	$(qecho) Creating dependencies...
	$(Q)$(MAKEDEPEND) $(INCLUDE_PATH) -f Makefile -P\$$O/ -- $(MSG_CC_FILES) $(SM_CC_FILES)  ./*.cc node/*.cc node/app/*.cc node/mac/*.cc node/net/*.cc win/*.cc

# DO NOT DELETE THIS LINE -- make depend depends on it.
$O/dtnsim_m.o: dtnsim_m.cc \
	dtnsim_m.h
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
	node/app/App.h \
	node/net/Contact.h \
	node/net/ContactPlan.h \
	node/net/Net.h \
	node/net/Routing.h \
	node/net/RoutingCgrIon350.h \
	node/net/RoutingCgrModel.h \
	node/net/RoutingDirect.h
$O/node/net/RoutingCgrIon350.o: node/net/RoutingCgrIon350.cc \
	dtnsim_m.h \
	node/net/Contact.h \
	node/net/ContactPlan.h \
	node/net/Routing.h \
	node/net/RoutingCgrIon350.h
$O/node/net/RoutingCgrModel.o: node/net/RoutingCgrModel.cc \
	dtnsim_m.h \
	node/net/Contact.h \
	node/net/ContactPlan.h \
	node/net/Routing.h \
	node/net/RoutingCgrModel.h
$O/node/net/RoutingDirect.o: node/net/RoutingDirect.cc \
	dtnsim_m.h \
	node/net/Contact.h \
	node/net/ContactPlan.h \
	node/net/Routing.h \
	node/net/RoutingDirect.h

