.SUFFIXES:
################################################################################
# What Is This?
# -------------
# The purpose of this Makefile make building Commander X16 programs from the
# command line easier.  Currently, only assemply source files are supported but
# that can be expanded in the future.  All files related to program are added
# to this Makefile, then this Makefile will invoke Makefile.X16 which does all
# the heavy lifting/building.
# 
# How To Use It
# -------------
# - Copy this file, Makefile, into your source directory
# - Fill in the variables below
# - Update the "include" at the bottom of this Makefile to the location of
#   Makefile.X16
# 
# Anything Else?
# --------------
# - To use the resources feature (RES), "ConverterX16.py" must be in your path.
#   - "ConverterX16.py" can be found in the "tool" directory
# - To build the "run" target, the Commander X16 Emulator (x16emu) must be in
#   your path.
# - To build the "sdcard" and "sdcard_clean" targets, the "sdcard.sh" script
#   must be in your path.
#   - "sdcard.sh" can be found in the "tool" directory
#   - May require root access to run
# - "Makefile.X16" is based on the "cc65" toolset.
#   - Set the CC65_DIR variable to the path to "cc65" directories
#   - "ca65" must be in your path
#   - "ld65" must be in your path
################################################################################

########################################
# Change the "a.prg" to be the name of
# the executable
PRG := a.prg

########################################
# Add all the source (*.s) files to this
# variable.
# Example 1:
# 	SRC := foo.s bar.s blah.s
#
# Example 2:
# 	SRC :=	\
#  	 	foo.s	\
#		bar.s	\
#		blah.s
SRC :=

########################################
# Palette files placed in this variable
# will be passed to the ConverterX16.py
# and changed to a loadable binary file.
#
# Example:
# 	RES := Resource.ini
RES := 

########################################
# Specify the SD card to be used by the
# emulator, no value means "No SD card".
#
# Example:
#	SDCARD := floppy.img
SDCARD :=

include ../Makefile.X16

