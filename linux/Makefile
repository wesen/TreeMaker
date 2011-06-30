### Makefile for building and installing TreeMaker 5 in Unix/Linux. 
### Requires GNU Make or a compatible tool.
###   Written by Carlos A. Furuti.
###   This is part of TreeMaker 5.0 and available under its release conditions.

### Check and optionally change the next few lines in order to match 
### your platform

# Where wxWidgets is installed
WXHOME = /opt/wxGTK-2.6.2
# Binary name
PROGRAM = TreeMaker
# Installation directory prefix (usually /usr/local); by wxWidgets
# convention for Unix, data files will always be created in a 
# subdirectory of $(INSTALL_PREFIX)/share
INSTALL_PREFIX = /usr/local
## Application name (part of the data files directory; by default 
## "TreeMaker 5") OBSOLETE - must think of a smarter way
#TM_APP_NAME_STR = "TreeMaker5"


### The remaining lines should be ok

#--- Compiling options
OPTIONS =
OPTIONS += -DAPPNAME=\"$(TM_APP_NAME_STR)\" 
OPTIONS += -DINSTALL_PREFIX=\"$(INSTALL_PREFIX)/\" 
# Temporary for experimental Linux development
OPTIONS += -DCAF

# Auxiliary wxWidgets apps
WXCONFIG = $(WXPATH)/wx-config
WXHLPCACHEDIR = $(WXPATH)/utils/hhp2cached

# Auxiliary program files
HELP = $(BUILDGENERIC)/help.zip
HELPCACHE = $(H2S)/help/treemaker.hhp.cached

BUILD ?= release

ifeq ($(BUILD),debug)
  CONDFLAGS = -DTMWX -D__WXDEBUG__ -g 
  WX = debug
else
ifeq ($(BUILD),devel)
  CONDFLAGS = -DTMWX -DTMDEBUG -g
  WX = release
else
ifeq ($(BUILD),profile)
  CONDFLAGS = -DTMWX -DTMPROFILE -O3 -g -pg
  WX = release
else
ifeq ($(BUILD),release)
  CONDFLAGS = -DTMWX -O3
  WX = release
endif
endif
endif
endif

CFLAGS = `$(WXCONFIG) --cxxflags` \
	-Wall $(CONDFLAGS) $(OPTIONS) $(INCCFLAGS) 
# Use either debug or release build directories of wxW
WXPATH = $(WXHOME)/build-$(WX)
# You may remake everything here anytime you want
BUILDGENERIC = build
# We'll leave dependency, object and binary files here
BUILDROOT = $(BUILDGENERIC)/$(BUILD)
# Relative path from here to source directory
H2S = ../Source
# Extended header path
INCCFLAGS = \
	-I$(H2S)/ \
	-I$(H2S)/tmModel \
	-I$(H2S)/tmModel/tmPtrClasses \
	-I$(H2S)/tmModel/tmOptimizers \
	-I$(H2S)/tmModel/tmSolvers \
	-I$(H2S)/tmModel/tmNLCO \
	-I$(H2S)/tmModel/tmTreeClasses \
	-I$(H2S)/tmModel/wnlib/cmp \
	-I$(H2S)/tmModel/wnlib/conjdir \
	-I$(H2S)/tmModel/wnlib/cpy \
	-I$(H2S)/tmModel/wnlib/list \
	-I$(H2S)/tmModel/wnlib/low \
	-I$(H2S)/tmModel/wnlib/mat \
	-I$(H2S)/tmModel/wnlib/mem \
	-I$(H2S)/tmModel/wnlib/random \
	-I$(H2S)/tmModel/wnlib/vect \
	-I$(H2S)/tmwxGUI/tmwxDocView \
	-I$(H2S)/tmwxGUI/tmwxCommon \
	-I$(H2S)/tmwxGUI/tmwxInspector \
	-I$(H2S)/tmwxGUI/tmwxFoldedForm \
	-I$(H2S)/tmwxGUI/tmwxPalette \
	-I$(H2S)/tmwxGUI/tmwxViewSettings \
	-I$(H2S)/tmwxGUI/tmwxLogFrame \
	-I$(H2S)/tmwxGUI/tmwxOptimizerDialog \
	-I$(H2S)/tmwxGUI/tmwxHtmlHelp \
	`pkg-config gtk+-2.0 --cflags`

#--- Source files (not all binaries need all of them)
PTRSRC =$(H2S)/tmModel/tmPtrClasses/tmDpptrTarget.cpp
NLCOSRC= \
	$(H2S)/tmModel/tmNLCO/tmNLCO_alm.cpp \
	$(H2S)/tmModel/tmNLCO/tmNLCO_cfsqp.cpp \
	$(H2S)/tmModel/tmNLCO/tmNLCO.cpp \
	$(H2S)/tmModel/tmNLCO/tmNLCO_rfsqp.cpp \
	$(H2S)/tmModel/tmNLCO/tmNLCO_wnlib.cpp \
	$(H2S)/tmModel/tmNLCO/tmNLCO_wnlibStub.c
WNSRC = \
	$(H2S)/tmModel/wnlib/cmp/wndcmp.c \
	$(H2S)/tmModel/wnlib/conjdir/wn1dmin.c \
	$(H2S)/tmModel/wnlib/conjdir/wncnjfg.c \
	$(H2S)/tmModel/wnlib/conjdir/wnconjg.c \
	$(H2S)/tmModel/wnlib/conjdir/wnnlp.c \
	$(H2S)/tmModel/wnlib/conjdir/wnparvect.c \
	$(H2S)/tmModel/wnlib/conjdir/wnqfit.c \
	$(H2S)/tmModel/wnlib/list/wnscnt.c \
	$(H2S)/tmModel/wnlib/list/wnsmk.c \
	$(H2S)/tmModel/wnlib/low/wnasrt.c \
	$(H2S)/tmModel/wnlib/mat/wnmmk.c \
	$(H2S)/tmModel/wnlib/mem/wnmbtr.c \
	$(H2S)/tmModel/wnlib/mem/wnmcpy.c \
	$(H2S)/tmModel/wnlib/mem/wnmemb.c \
	$(H2S)/tmModel/wnlib/mem/wnmem.c \
	$(H2S)/tmModel/wnlib/mem/wnmemg.c \
	$(H2S)/tmModel/wnlib/mem/wnmemn.c \
	$(H2S)/tmModel/wnlib/random/wnrdb.c \
	$(H2S)/tmModel/wnlib/random/wnrflt.c \
	$(H2S)/tmModel/wnlib/random/wnrnd.c \
	$(H2S)/tmModel/wnlib/random/wnrtab.c \
	$(H2S)/tmModel/wnlib/vect/wndot.c \
	$(H2S)/tmModel/wnlib/vect/wnpoly.c \
	$(H2S)/tmModel/wnlib/vect/wnvadd3.c \
	$(H2S)/tmModel/wnlib/vect/wnvcpy.c \
	$(H2S)/tmModel/wnlib/vect/wnvgen.c \
	$(H2S)/tmModel/wnlib/vect/wnvmk.c \
	$(H2S)/tmModel/wnlib/vect/wnvnrm.c \
	$(H2S)/tmModel/wnlib/vect/wnvprn.c
MDLSRC = $(PTRSRC) $(NLCOSRC) $(WNSRC) \
	$(H2S)/tmModel/tmOptimizers/tmConstraintFns.cpp \
	$(H2S)/tmModel/tmOptimizers/tmEdgeOptimizer.cpp \
	$(H2S)/tmModel/tmOptimizers/tmOptimizer.cpp \
	$(H2S)/tmModel/tmOptimizers/tmScaleOptimizer.cpp \
	$(H2S)/tmModel/tmOptimizers/tmStrainOptimizer.cpp \
	$(H2S)/tmModel/tmSolvers/tmStubFinder.cpp \
	$(H2S)/tmModel/tmTreeClasses/tmCluster.cpp \
	$(H2S)/tmModel/tmTreeClasses/tmCondition.cpp \
	$(H2S)/tmModel/tmTreeClasses/tmConditionEdgeLengthFixed.cpp \
	$(H2S)/tmModel/tmTreeClasses/tmConditionEdgesSameStrain.cpp \
	$(H2S)/tmModel/tmTreeClasses/tmConditionNodeCombo.cpp \
	$(H2S)/tmModel/tmTreeClasses/tmConditionNodeFixed.cpp \
	$(H2S)/tmModel/tmTreeClasses/tmConditionNodeOnCorner.cpp \
	$(H2S)/tmModel/tmTreeClasses/tmConditionNodeOnEdge.cpp \
	$(H2S)/tmModel/tmTreeClasses/tmConditionNodesCollinear.cpp \
	$(H2S)/tmModel/tmTreeClasses/tmConditionNodesPaired.cpp \
	$(H2S)/tmModel/tmTreeClasses/tmConditionNodeSymmetric.cpp \
	$(H2S)/tmModel/tmTreeClasses/tmConditionOwner.cpp \
	$(H2S)/tmModel/tmTreeClasses/tmConditionPathActive.cpp \
	$(H2S)/tmModel/tmTreeClasses/tmConditionPathAngleFixed.cpp \
	$(H2S)/tmModel/tmTreeClasses/tmConditionPathAngleQuant.cpp \
	$(H2S)/tmModel/tmTreeClasses/tmConditionPathCombo.cpp \
	$(H2S)/tmModel/tmTreeClasses/tmCrease.cpp \
	$(H2S)/tmModel/tmTreeClasses/tmCreaseOwner.cpp \
	$(H2S)/tmModel/tmTreeClasses/tmEdge.cpp \
	$(H2S)/tmModel/tmTreeClasses/tmEdgeOwner.cpp \
	$(H2S)/tmModel/tmTreeClasses/tmFacet.cpp \
	$(H2S)/tmModel/tmTreeClasses/tmFacetOwner.cpp \
	$(H2S)/tmModel/tmTreeClasses/tmNode.cpp \
	$(H2S)/tmModel/tmTreeClasses/tmNodeOwner.cpp \
	$(H2S)/tmModel/tmTreeClasses/tmPart.cpp \
	$(H2S)/tmModel/tmTreeClasses/tmPath.cpp \
	$(H2S)/tmModel/tmTreeClasses/tmPathOwner.cpp \
	$(H2S)/tmModel/tmTreeClasses/tmPoint.cpp \
	$(H2S)/tmModel/tmTreeClasses/tmPoly.cpp \
	$(H2S)/tmModel/tmTreeClasses/tmPolyOwner.cpp \
	$(H2S)/tmModel/tmTreeClasses/tmTreeCleaner.cpp \
	$(H2S)/tmModel/tmTreeClasses/tmTree.cpp \
	$(H2S)/tmModel/tmTreeClasses/tmTree_FacetOrder.cpp \
	$(H2S)/tmModel/tmTreeClasses/tmTree_IO.cpp \
	$(H2S)/tmModel/tmTreeClasses/tmTree_TestTrees.cpp \
	$(H2S)/tmModel/tmTreeClasses/tmVertex.cpp \
	$(H2S)/tmModel/tmTreeClasses/tmVertexOwner.cpp

#	$(H2S)/CFSQP/cfsqp_noprint.cpp \
#	$(H2S)/RFSQP/rfsqp_noprint.cpp

GUISRC = \
	$(H2S)/tmwxGUI/tmwxCommon/tmwxApp.cpp \
	$(H2S)/tmwxGUI/tmwxCommon/tmwxCommand.cpp \
	$(H2S)/tmwxGUI/tmwxCommon/tmwxDocManager.cpp \
	$(H2S)/tmwxGUI/tmwxCommon/tmwxGetUserInputDialog.cpp \
	$(H2S)/tmwxGUI/tmwxCommon/tmwxPersistentFrame.cpp \
	$(H2S)/tmwxGUI/tmwxCommon/tmwxStr.cpp \
	$(H2S)/tmwxGUI/tmwxDocView/tmwxDesignCanvas.cpp \
	$(H2S)/tmwxGUI/tmwxDocView/tmwxDesignFrame.cpp \
	$(H2S)/tmwxGUI/tmwxDocView/tmwxDoc.cpp \
	$(H2S)/tmwxGUI/tmwxDocView/tmwxDoc_Action.cpp \
	$(H2S)/tmwxGUI/tmwxDocView/tmwxDoc_Condition.cpp \
	$(H2S)/tmwxGUI/tmwxDocView/tmwxDoc_Debug.cpp \
	$(H2S)/tmwxGUI/tmwxDocView/tmwxDoc_Edit.cpp \
	$(H2S)/tmwxGUI/tmwxDocView/tmwxDoc_File.cpp \
	$(H2S)/tmwxGUI/tmwxDocView/tmwxDoc_View.cpp \
	$(H2S)/tmwxGUI/tmwxDocView/tmwxPrintout.cpp \
	$(H2S)/tmwxGUI/tmwxDocView/tmwxView.cpp \
	$(H2S)/tmwxGUI/tmwxFoldedForm/tmwxFoldedFormFrame.cpp \
	$(H2S)/tmwxGUI/tmwxHtmlHelp/tmwxHtmlHelpController.cpp \
	$(H2S)/tmwxGUI/tmwxHtmlHelp/tmwxHtmlHelpFrame.cpp \
	$(H2S)/tmwxGUI/tmwxInspector/tmwxConditionEdgeLengthFixedPanel.cpp \
	$(H2S)/tmwxGUI/tmwxInspector/tmwxConditionEdgesSameStrainPanel.cpp \
	$(H2S)/tmwxGUI/tmwxInspector/tmwxConditionListBox.cpp \
	$(H2S)/tmwxGUI/tmwxInspector/tmwxConditionNodeComboPanel.cpp \
	$(H2S)/tmwxGUI/tmwxInspector/tmwxConditionNodeFixedPanel.cpp \
	$(H2S)/tmwxGUI/tmwxInspector/tmwxConditionNodeOnCornerPanel.cpp \
	$(H2S)/tmwxGUI/tmwxInspector/tmwxConditionNodeOnEdgePanel.cpp \
	$(H2S)/tmwxGUI/tmwxInspector/tmwxConditionNodesCollinearPanel.cpp \
	$(H2S)/tmwxGUI/tmwxInspector/tmwxConditionNodesPairedPanel.cpp \
	$(H2S)/tmwxGUI/tmwxInspector/tmwxConditionNodeSymmetricPanel.cpp \
	$(H2S)/tmwxGUI/tmwxInspector/tmwxConditionPathActivePanel.cpp \
	$(H2S)/tmwxGUI/tmwxInspector/tmwxConditionPathAngleFixedPanel.cpp \
	$(H2S)/tmwxGUI/tmwxInspector/tmwxConditionPathAngleQuantPanel.cpp \
	$(H2S)/tmwxGUI/tmwxInspector/tmwxConditionPathComboPanel.cpp \
	$(H2S)/tmwxGUI/tmwxInspector/tmwxCreasePanel.cpp \
	$(H2S)/tmwxGUI/tmwxInspector/tmwxEdgePanel.cpp \
	$(H2S)/tmwxGUI/tmwxInspector/tmwxFacetPanel.cpp \
	$(H2S)/tmwxGUI/tmwxInspector/tmwxGroupPanel.cpp \
	$(H2S)/tmwxGUI/tmwxInspector/tmwxInspectorFrame.cpp \
	$(H2S)/tmwxGUI/tmwxInspector/tmwxInspectorPanel.cpp \
	$(H2S)/tmwxGUI/tmwxInspector/tmwxNodePanel.cpp \
	$(H2S)/tmwxGUI/tmwxInspector/tmwxPathPanel.cpp \
	$(H2S)/tmwxGUI/tmwxInspector/tmwxPolyPanel.cpp \
	$(H2S)/tmwxGUI/tmwxInspector/tmwxTreePanel.cpp \
	$(H2S)/tmwxGUI/tmwxInspector/tmwxVertexPanel.cpp \
	$(H2S)/tmwxGUI/tmwxLogFrame/tmwxLogFrame.cpp \
	$(H2S)/tmwxGUI/tmwxOptimizerDialog/tmwxOptimizerDialog_cmn.cpp \
	$(H2S)/tmwxGUI/tmwxOptimizerDialog/tmwxOptimizerDialog_gtk.cpp \
	$(H2S)/tmwxGUI/tmwxPalette/tmwxButtonMini.cpp \
	$(H2S)/tmwxGUI/tmwxPalette/tmwxButtonSmall.cpp \
	$(H2S)/tmwxGUI/tmwxPalette/tmwxCheckBox.cpp \
	$(H2S)/tmwxGUI/tmwxPalette/tmwxCheckBoxSmall.cpp \
	$(H2S)/tmwxGUI/tmwxPalette/tmwxPaletteFrame.cpp \
	$(H2S)/tmwxGUI/tmwxPalette/tmwxPalettePanel.cpp \
	$(H2S)/tmwxGUI/tmwxPalette/tmwxRadioBoxSmall.cpp \
	$(H2S)/tmwxGUI/tmwxPalette/tmwxStaticText.cpp \
	$(H2S)/tmwxGUI/tmwxPalette/tmwxTextCtrl.cpp \
	$(H2S)/tmwxGUI/tmwxViewSettings/tmwxViewSettings.cpp \
	$(H2S)/tmwxGUI/tmwxViewSettings/tmwxViewSettingsFrame.cpp \
	$(H2S)/tmwxGUI/tmwxViewSettings/tmwxViewSettingsPanel.cpp

SRC = $(MDLSRC) $(GUISRC)

#--- Object files
PTROBJS = $(patsubst $(H2S)%,$(BUILDROOT)%,\
         $(patsubst %.cpp,%.o,$(patsubst %.c,%.o,$(PTRSRC))))
MDLOBJS = $(patsubst $(H2S)%,$(BUILDROOT)%,\
         $(patsubst %.cpp,%.o,$(patsubst %.c,%.o,$(MDLSRC))))
NLCOOBJS = $(patsubst $(H2S)%,$(BUILDROOT)%,\
         $(patsubst %.cpp,%.o,$(patsubst %.c,%.o,$(NLCOSRC))))
WNOBJS = $(patsubst $(H2S)%,$(BUILDROOT)%,\
         $(patsubst %.cpp,%.o,$(patsubst %.c,%.o,$(WNSRC))))
OBJS = $(patsubst $(H2S)%,$(BUILDROOT)%,\
         $(patsubst %.cpp,%.o,$(patsubst %.c,%.o,$(SRC))))
DEPENDS = $(patsubst $(H2S)%,$(BUILDROOT)%,\
         $(patsubst %.cpp,%.dep,$(patsubst %.c,%.dep,$(SRC))))

#--- Main target
APP = $(BUILDROOT)/$(PROGRAM)

$(APP): $(OBJS)
	@echo Linking TreeMaker \($(PROGRAM)\)
	@$(CXX) -o $(APP) -L../tmModel $(OBJS) $(CFLAGS) \
	$(H2S)/tmHeader.cpp `$(WXCONFIG) --libs`

#--- Auxiliary and optional targets

TESTS = $(BUILDROOT)/test/tmArrayTester \
	$(BUILDROOT)/test/tmDpptrTester \
	$(BUILDROOT)/test/tmNewtonRaphsonTester \
	$(BUILDROOT)/test/tmModelTester \
	$(BUILDROOT)/test/tmNLCOTester

$(BUILDROOT)/test/tmArrayTester: $(H2S)/test/tmArrayTester.cpp \
	$(H2S)/tmHeader.cpp $(PTROBJS) 
	@echo Building $@
	@$(CXX) $(CFLAGS) -UTMWX -o $@ $(H2S)/tmHeader.cpp $< $(PTROBJS)
$(BUILDROOT)/test/tmDpptrTester: $(H2S)/test/tmDpptrTester.cpp \
	$(H2S)/tmHeader.cpp $(PTROBJS) 
	@echo Building $@
	@$(CXX) $(CFLAGS) -UTMWX -o $@ $< $(H2S)/tmHeader.cpp $(PTROBJS)
$(BUILDROOT)/test/tmNewtonRaphsonTester: $(H2S)/test/tmNewtonRaphsonTester.cpp \
	$(H2S)/tmHeader.cpp $(PTROBJS) 
	@echo Building $@
	@$(CXX) $(CFLAGS) -UTMWX -o $@ $< $(H2S)/tmHeader.cpp $(PTROBJS)
$(BUILDROOT)/test/tmNLCOTester: $(H2S)/test/tmNLCOTester/tmNLCOTester.cpp \
	$(H2S)/tmHeader.cpp $(PTROBJS) $(NLCOOBJS) $(WNOBJS)
	@echo Building $@
	@$(CXX) $(CFLAGS) -UTMWX -o $@ $< $(H2S)/tmHeader.cpp \
	  $(PTROBJS) $(NLCOOBJS) $(WNOBJS) `$(WXCONFIG) --libs`
$(BUILDROOT)/test/tmModelTester: $(H2S)/test/tmModelTester/tmModelTester.cpp \
	$(H2S)/tmHeader.cpp $(MDLOBJS)
	@echo Building $@
	@$(CXX) $(CFLAGS) -UTMWX -o $@ $< $(H2S)/tmHeader.cpp \
	  $(MDLOBJS) `$(WXCONFIG) --libs`

tests: buildprep $(TESTS)

aux: $(HELP) $(BUILDROOT)/tmpath

help: $(HELP)

$(HELP):  $(HELPCACHE)
	@echo Creating help file
	@(cd $(H2S)/help && zip -q - *) > $(HELP) || rm $(HELP)

# Currently hhp2cached doesn't correctly set exit status, so don't
# forget the "-" prefix; it also doesn't rewrite an existing cache file
$(HELPCACHE): $(WXHLPCACHEDIR)/hhp2cached \
	$(H2S)/help/*.htm $(H2S)/help/*.hhc $(H2S)/help/*.hhp \
	$(H2S)/help/*.hhk $(H2S)/help/*.css $(H2S)/help/*.png
	@echo Creating help cache
	-@rm $(HELPCACHE)
	-@$(WXHLPCACHEDIR)/hhp2cached $(H2S)/help/treemaker.hhp

#-$(WXHLPCACHEDIR)/hhp2cached $(H2S)/help/treemaker.hhp
$(WXHLPCACHEDIR)/hhp2cached:
	$(MAKE) -C $(WXHLPCACHEDIR)

$(BUILDROOT)/tmpath: tmpath.c $(H2S)/tmVersion.h
	@echo Compiling name helper
	@$(CC) $(CFLAGS) -o $(BUILDROOT)/tmpath tmpath.c

UNINSTALLER = $(BUILDROOT)/uninstall
install: $(APP) $(HELP) $(BUILDROOT)/tmpath
	@echo Installing binary program in $(INSTALL_PREFIX)/bin
	@-install -d $(INSTALL_PREFIX)/bin
	@install $(APP) $(INSTALL_PREFIX)/bin
	@echo Installing auxiliary files in \
          $(INSTALL_PREFIX)/share/`$(BUILDROOT)/tmpath`
	@-install -d "$(INSTALL_PREFIX)/share/`$(BUILDROOT)/tmpath`"
	@sh bundle/mkuninstall "$(UNINSTALLER)" "$(INSTALL_PREFIX)" \
	  "$(PROGRAM)" "`$(BUILDROOT)/tmpath`"
	@install $(UNINSTALLER)\
		"$(INSTALL_PREFIX)/share/`$(BUILDROOT)/tmpath`"
	@gzip -c ../LICENSE.txt > LICENSE.txt.gz
	@gzip -c ../change_log.txt > change_log.txt.gz
	@install -m 644 $(HELP) $(H2S)/about/about.htm \
		LICENSE.txt.gz change_log.txt.gz \
		$(H2S)/images/SplashScreen.png \
		resources/Icon_doc_48.png resources/Icon_app_48.png \
		resources/Icon_app.ppm \
		"$(INSTALL_PREFIX)/share/`$(BUILDROOT)/tmpath`"
	@rm LICENSE.txt.gz

dist: $(APP) $(HELP) $(BUILDROOT)/tmpath
	@sh bundle/buildbundle "`$(BUILDROOT)/tmpath -b`" \
	  $(BUILD) $(PROGRAM) $(H2S) "`$(BUILDROOT)/tmpath -p`" \
	  "`$(BUILDROOT)/tmpath -a`"

clean: FORCE
	-@rm $(OBJS) $(DEPENDS) $(HELP) $(HELPCACHE) $(TESTS) \
	$(BUILDROOT)/tmpath 2> /dev/null

.PHONY: clean
FORCE:

buildprep: FORCE
	-@mkdir -p $(BUILDROOT)/test

#--- Rules and suffixes 

include $(DEPENDS)

.SUFFIXES: .dep .cpp

# Both object file and dependency list depend on a source file and
# all its included files (recursively)
#  (supersedes sed 's!^.*\.o[ :]*!$*.o  $@ : !g' < /tmp/mkdep.$$$$ > ...)

$(BUILDROOT)%.dep: $(H2S)%.cpp
	@echo Remaking dependencies for $<
	@-mkdir -p $(dir $(patsubst $(H2S)%,$(BUILDROOT)%,$<))
	@$(CXX) -MT $(patsubst $(H2S)%,$(BUILDROOT)%,$(<:.cpp=.o)) -MM \
           $(CFLAGS) $< > /tmp/mkdep.$$$$ && \
	  sed '1i$(patsubst $(H2S)%,$(BUILDROOT)%,$(<:.cpp=.dep)) \\' \
           /tmp/mkdep.$$$$  > \
	  $(patsubst $(H2S)%,$(BUILDROOT)%,$(<:.cpp=.dep)) && \
	  rm -f /tmp/mkdep.$$$$
$(BUILDROOT)%.dep: $(H2S)%.c
	@echo Remaking dependencies for $<
	@-mkdir -p $(dir $(patsubst $(H2S)%,$(BUILDROOT)%,$<))
	@$(CXX) -MT $(patsubst $(H2S)%,$(BUILDROOT)%,$(<:.c=.o)) -MM \
           $(CFLAGS) $< > /tmp/mkdep.$$$$ && \
	  sed '1i$(patsubst $(H2S)%,$(BUILDROOT)%,$(<:.c=.dep)) \\' \
           /tmp/mkdep.$$$$  > \
	  $(patsubst $(H2S)%,$(BUILDROOT)%,$(<:.c=.dep)) && \
	  rm -f /tmp/mkdep.$$$$

$(BUILDROOT)%.o: $(H2S)%.cpp
	@echo Compiling $<
	@$(CXX) -c $(CFLAGS) \
          -o $(patsubst $(H2S)%,$(BUILDROOT)%,$(<:.cpp=.o)) $<
$(BUILDROOT)%.o: $(H2S)%.c
	@echo Compiling $<
	@$(CC) -c $(CFLAGS) -o $(patsubst $(H2S)%,$(BUILDROOT)%,$(<:.c=.o)) $<
