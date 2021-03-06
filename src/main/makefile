# -*- Mode: Makefile -*-


ifneq ($(CLASP_INTERNAL_BUILD_TARGET_DIR),)
  CLASP_BOEHM = $(CLASP_INTERNAL_BUILD_TARGET_DIR)/$(EXECUTABLE_DIR)/clasp_boehm_o -f bclasp
  CLASP_MPS = $(CLASP_INTERNAL_BUILD_TARGET_DIR)/$(EXECUTABLE_DIR)/clasp_mps_o
else
  CLASP_BOEHM = clasp_boehm_o
  CLASP_MPS = clasp_mps_o
endif

export CLASP_APP_RESOURCES_DIR=$(CLASP_INTERNAL_BUILD_TARGET_DIR)/Contents/Resources



all:
	make boehm
	make mps

boehmdc-release-cxx:
	$(BUILD) toolset=$(TOOLSET) -j$(PJOBS) link=static program=cando gc=boehmdc --prefix=$(EXECS)/boehmdc/release release cando_install;
boehmdc-release-cxx-a:
	$(BUILD) toolset=$(TOOLSET) -j$(PJOBS) link=static program=cando gc=boehmdc --prefix=$(EXECS)/boehmdc/release release cando_install -a ;
boehmdc-debug-cxx:
	$(BUILD) toolset=$(TOOLSET) -j$(PJOBS) link=static program=cando gc=boehmdc --prefix=$(EXECS)/boehmdc/debug debug cando_install ;

boehm-release-cxx:
	$(BUILD) toolset=$(TOOLSET) -j$(PJOBS) link=static program=cando gc=boehm --prefix=$(EXECS)/boehm/release release cando_install;
boehm-release-cxx-a:
	$(BUILD) toolset=$(TOOLSET) -j$(PJOBS) link=static program=cando gc=boehm --prefix=$(EXECS)/boehm/release release cando_install -a ;
boehm-debug-cxx:
	$(BUILD) toolset=$(TOOLSET) -j$(PJOBS) link=static program=cando gc=boehm --prefix=$(EXECS)/boehm/debug debug cando_install ;

executable-symlinks:
	ln -sf $(EXECS)/boehm/release/bin/cando $(BINDIR)/cando_boehm_o
	ln -sf $(EXECS)/boehmdc/release/bin/cando $(BINDIR)/cando_boehmdc_o
	ln -sf $(EXECS)/mps/release/bin/cando $(BINDIR)/cando_mps_o
	ln -sf $(EXECS)/boehm/debug/bin/cando $(BINDIR)/cando_boehm_d
	ln -sf $(EXECS)/boehmdc/debug/bin/cando $(BINDIR)/cando_boehmdc_d
	ln -sf $(EXECS)/mps/debug/bin/cando $(BINDIR)/cando_mps_d

application:
	cp -R $(CLASP_INTERNAL_BUILD_TARGET_DIR)/* $(CLASP_APP_DIR)

boehm-cleavir:
	make boehm
	make cleavir

cclasp:
	clasp_boehm_o  -l "buildCleavirClasp.lsp"

bclasp-to-cclasp:
	clasp_boehm_o  -l "buildBclaspToCclasp.lsp"



#############################################################
#
# Set up scraping
#
#

project-directories = $(CLASP_HOME)/src/gctools \
			$(CLASP_HOME)/src/core \
			$(CLASP_HOME)/src/clbind \
			$(CLASP_HOME)/src/cffi \
			$(CLASP_HOME)/src/asttooling \
			$(CLASP_HOME)/src/llvmo \
			$(CLASP_HOME)/src/serveEvent \
			$(CLASP_HOME)/src/sockets \
			../geom \
			../units \
			../adapt \
			../chem 

scrape-all:
	(export PYTHONPATH=$(CLASP_HOME)/src/common:$(PYTHONPATH); $(PYTHON2) $(CLASP_HOME)/src/common/symbolGroveler.py include/generated/symbols_scraped_inc.h $(project-directories))
	(export PYTHONPATH=$(CLASP_HOME)/src/common:$(PYTHONPATH); $(PYTHON2) $(CLASP_HOME)/src/common/classGroveler.py include/generated/initClasses_inc.h $(project-directories) 2>registerClasses.log)








#	make clasp-gc-interface



boehm:
	make min-boehm
	make full-boehm

boehm-recompile:
	make min-boehm-recompile
	make full-boehm

min-boehm:
	$(CLASP_BOEHM) -I -f ecl-min -l "buildClaspMinBoot.lsp"

min-boehm-recompile:
	$(CLASP_BOEHM) -f ecl-min -l "buildClaspMinRecompile.lsp"

full-boehm:
	$(CLASP_BOEHM) -f ecl-min -l "buildClasp.lsp"


mps:
	make min-mps
	make full-mps

mps-recompile:
	make min-mps-recompile
	make full-mps

min-mps:
	$(CLASP_MPS) -I -f ecl-min -l "buildClaspMinMps.lsp"

min-mps-recompile:
	$(CLASP_MPS) -f ecl-min -l "buildClaspMinRecompile.lsp"

full-mps:
	$(CLASP_MPS) -f ecl-min -l "buildClasp.lsp"


short-compile-mps:
	time $(CLASP_MPS) -f ecl-min -l "shortCompile.lsp"

short-compile-boehm:
	time $(CLASP_BOEHM) -f ecl-min -l "shortCompile.lsp"



clasp-gc-interface:
	$(CLASP_BOEHM) -l "buildClaspGC.lsp"

compile-commands:
	$(BUILD) -j$(PJOBS) toolset=$(TOOLSET) link=static program=cando gc=mps-prep release
	make compile-commands-rest

compile-commands-rest:
	bjam -j$(PJOBS) toolset=$(TOOLSET) link=static program=cando gc=mps-prep release -n -a |tee >bjam.out
	-install -d $(CLASP_APP_RESOURCES_DIR)/build-databases
	clasp_boehm_o -f bclasp \
		-l $(CLASP_HOME)/src/common/build-json.lisp \
		-e "(build-json:build-db \"bjam.out\" \"clasp_compile_commands\")" \
		-e "(quit)"

mps-interface-prep:
	(make -C $(CLASP_HOME)/src/main boehmdc-release-cxx)
	make -C $(CLASP_HOME)/src/main min-boehmdc
	make -C $(CLASP_HOME)/src/main/ bclasp-boehmdc-bitcode
	make -C $(CLASP_HOME)/src/main/ bclasp-boehmdc-fasl
	make -C $(CLASP_HOME)/src/main/ bclasp-boehmdc-addons

mps-interface-run:
	make compile-commands
	clasp_boehmdc_o -f bclasp -e "(require :clasp-analyzer)" \
	-e "(defparameter *db* (clasp-analyzer:load-compilation-database \"app-resources:build-databases;cando_compile_commands.json\"))" \
        -e "(clasp-analyzer:search/generate-code *db*)"

mps-interface-run-boehm:
	clasp_boehm_o -f bclasp  \
		-e "(require :clasp-analyzer)" \
		-e "(time (clasp-analyzer:search/generate-code (clasp-analyzer:setup-clasp-analyzer-compilation-tool-database \"app-resources:build-databases;clasp_compile_commands.json\")))" \
		-e "(quit)"

mps-interface-test:
	make compile-commands
	clasp_boehmdc_o -f bclasp -e "(require :clasp-analyzer)" \
	-e "(clasp-analyzer:load-compilation-database \"app-resources:build-databases;cando_compile_commands.json\")" \
        -e "(clasp-analyzer:serial-search-all-then-generate-code-and-quit :test (clasp-analyzer::lsel clasp-analyzer:$$\* \".*mol2\.cc$$\"))"

mps-interface-run-cclasp:
	make compile-commands
	clasp_boehm_o -f bclasp -e "(require :clasp-analyzer)" \
	-e "(defparameter *db* (clasp-analyzer:load-compilation-database \"app-resources:build-databases;cando_compile_commands.json\"))" \
        -e "(clasp-analyzer:search/generate-code *db*)"


mps-interface:
	make mps-interface-prep
	make mps-interface-run
