#==============================================================================
# This is a generic makefile that you may use to build an application or plugin
# module which utilizes Crystal Space.  It relies upon the `cs-config' utility
# script, which is installed along with the Crystal Space SDK, to determine
# build flags needed to incorporate Crystal Space into your project.
#
# You will need to edit this makefile template so it is suitable for your
# project.  You may need to change at least the following variables after
# copying this template makefile into your project:
#
#	TARGET_TYPE -- either `exe' or `plugin'
#       TARGET_NAME -- name of the executable or plugin (sans .exe, .dll)
#       SRC -- all source files comprising your program or plugin
#       LINKFLAGS -- linker flags and libraries that your project requires
#	VERSION -- product version number
#	RELEASE_DATE -- date of product release
#
# If you installed Crystal Space in a non-standard location, then you will also
# need to set the CRYSTAL environment variable to point at the root of the
# installed SDK.
#
# This makefile supports the following targets: all, clean, depend.
#==============================================================================

VERSION = 1.0
RELEASE_DATE = 01-Jan-2005
SRCDIR = .

#------
# Type of project: `exe' or `plugin'
#------
TARGET_TYPE = exe
#TARGET_TYPE = plugin

#------
# Name of program or plugin
#------
TARGET_NAME = map2cs

ifeq ($(TARGET_TYPE),exe)
  TARGET=$(TARGET_NAME)$(EXE)
else
  TARGET=$(TARGET_NAME)$(DLL)
endif

#------
# Location of sources and object files
#------
SRC=$(wildcard *.cpp)
OBJS=$(addsuffix .o, $(basename $(SRC)))
OUT=.

#------
# Tools to use
#------
CXX=g++ 
LINK=$(CXX)
RM=rm -rf

#------
# Abstractions
#------
LFLAGS.L = -L
LFLAGS.l = -l
CFLAGS.D = -D
CFLAGS.I = -I

#------
# Flags for compiler and linker.
# Make sure to update the required libraries for your own project
#------
CFLAGS := $(shell cs-config --cflags) -g
CXXFLAGS := $(shell cs-config --cxxflags) -g -DCS_DEBUG -Iinclude
LINKFLAGS := $(shell cs-config --libs crystalspace) -g

DO.SHARED.PLUGIN.CORE=$(LINK) $(LFLAGS.DLL) -o $@ $^ $(PLUGIN.POSTFLAGS)
DO.PLUGIN = \
  $(DO.SHARED.PLUGIN.PREAMBLE) \
  $(DO.SHARED.PLUGIN.CORE) $(LINKFLAGS) \
  $(DO.SHARED.PLUGIN.POSTAMBLE)
DO.EXE = $(LINK) -o $@ $^ $(LFLAGS.EXE) $(LINKFLAGS) $(LIBS.EXE.PLATFORM)

ifeq ($(TARGET_TYPE),exe)
  DO.TARGET = $(DO.EXE)
else
  DO.TARGET = $(DO.PLUGIN)
endif

CSCONFIG.MAK=csconfig.mak
-include $(CSCONFIG.MAK)

#------
# Rules
#------
.PHONY: all depend clean
.SUFFIXES: .cpp

.cpp.o: $<
	$(CXX) $(CXXFLAGS) -o $@ -c $<

all: $(CSCONFIG.MAK) $(TARGET)

$(TARGET): $(OBJS) 
	$(DO.TARGET)

clean:
	$(RM) $(TARGET) $(TARGET).app $(OBJS) $(CSCONFIG.MAK) \
	makefile.dep *.def

#------
# Create dependencies
#------
depend: $(CSCONFIG.MAK)
	gcc -MM $(CXXFLAGS) $(SRC) > makefile.dep

#------
# Re-create the config flags include file
#------
$(CSCONFIG.MAK):
	cs-config --makevars > $(CSCONFIG.MAK)

#------
# Include dependencies
#------
-include makefile.dep

