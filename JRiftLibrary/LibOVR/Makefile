#############################################################################
#
# Filename    : Makefile
# Content     : Makefile for building linux / OSX versions of: libovr
# Created     : 2013
# Authors     : Simon Hallam and Peter Giokaris
# Copyright   : Copyright 2013 OculusVR, Inc. All Rights Reserved
# Instruction : The g++ compiler and standard lib packages need to be 
#               installed on the system.  Navigate in a shell to the 
#               directory where this Makefile is located and enter:
#
#               make                builds the release version for the 
#                                   current architecture
#               make clean          delete intermediate release object files 
#                                   and the library file
#               make DEBUG=1        builds the debug version for the current
#                                   architecture
#               make clean DEBUG=1  deletes intermediate debug object files 
#                                   and the library file
#               make ARCH=32        builds the release version for i386 
#                                   architecture
#               make clean ARCH=32  delete intermediate release object files 
#                                   and the library file for i386
#               make ARCH=64        builds the release version for amd64
#                                   architecture
#               make clean ARCH=64  delete intermediate release object files 
#                                   and the library file for amd64
#
# Output      : Relative to the directory this Makefile lives in, libraries
#               are built at the following locations depending upon the
#               architecture of the system you are running:
#
#               ./Lib/Linux/Debug/i386/libovr.a
#               ./Lib/Linux/Debug/x86_64/libovr.a
#               ./Lib/Linux/Release/i386/libovr.a
#               ./Lib/Linux/Release/x86_64/libovr.a
#
#               ./Lib/OSX/Debug/i386/libovr.a
#               ./Lib/OSX/Debug/x86_64/libovr.a
#               ./Lib/OSX/Release/i386/libovr.a
#               ./Lib/OSX/Release/x86_64/libovr.a
#
#############################################################################

UNAME:= $(shell uname)

ifeq ($(shell uname), Darwin)

LIBDIR:=OSX
#MAC_SDK:=/Developer/SDKs/MacOSX10.6.sdk
MAC_SDK:=/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.6.sdk
MAC_MIN_VER:=10.6

CXXFLAGS = -isysroot ${MAC_SDK} -mmacosx-version-min=${MAC_MIN_VER}
INCPATH  = -I${MAC_SDK}/usr/include
           
ADDOBJS :=  \
		$(OBJPATH)/OVR_OSX_HIDDevice.o \
		$(OBJPATH)/OVR_OSX_SensorDevice.o \
		$(OBJPATH)/OVR_OSX_DeviceManager.o \
		$(OBJPATH)/OVR_OSX_HMDDevice.o
else

LIBDIR=Linux
ADDOBJS :=  \
		$(OBJPATH)/OVR_Linux_HIDDevice.o \
		$(OBJPATH)/OVR_Linux_SensorDevice.o \
		$(OBJPATH)/OVR_Linux_DeviceManager.o \
		$(OBJPATH)/OVR_Linux_HMDDevice.o
endif

####### Compiler, tools and options

CXX           = g++
LINK          = ar rvs
DELETEFILE    = rm -f

####### Detect debug or release

DEBUG         = 0
ifeq ($(DEBUG), 1)
	CXXFLAGS      += -pipe -DDEBUG -g -fPIC
	RELEASETYPE   = Debug
else
	CXXFLAGS      += -pipe -O2 -fPIC
	RELEASETYPE   = Release
endif

####### Detect system architecture

ifeq ($(ARCH),64)
SYSARCH       = x86_64
CXXFLAGS     += -m64
LDFLAGS      += -m64
else ifeq ($(ARCH),32)
SYSARCH       = i386
CXXFLAGS     += -m32
LDFLAGS      += -m32
else ifeq ($(shell uname -m),x86_64)
SYSARCH       = x86_64
CXXFLAGS     += -m64
LDFLAGS      += -m64
else
SYSARCH       = i386
CXXFLAGS     += -m32
LDFLAGS      += -m32
endif

####### Paths

LIBOVRPATH    = .
3RDPARTYPATH  = ../3rdParty
INCPATH      += -I. -I.. -I$(LIBOVRPATH)/Include -I$(LIBOVRPATH)/Src
OBJPATH       = ./Obj/$(LIBDIR)/$(RELEASETYPE)/$(SYSARCH)

####### Files

TARGET        = ./Lib/$(LIBDIR)/$(RELEASETYPE)/$(SYSARCH)/libovr.a

OBJECTS       = $(OBJPATH)/OVR_DeviceHandle.o \
		$(OBJPATH)/OVR_DeviceImpl.o \
		$(OBJPATH)/OVR_JSON.o \
		$(OBJPATH)/OVR_LatencyTestImpl.o \
		$(OBJPATH)/OVR_Profile.o \
		$(OBJPATH)/OVR_SensorFilter.o\
		$(OBJPATH)/OVR_SensorFusion.o\
		$(OBJPATH)/OVR_SensorImpl.o \
		$(OBJPATH)/OVR_ThreadCommandQueue.o \
		$(OBJPATH)/OVR_Alg.o \
		$(OBJPATH)/OVR_Allocator.o \
		$(OBJPATH)/OVR_Atomic.o \
		$(OBJPATH)/OVR_File.o \
		$(OBJPATH)/OVR_FileFILE.o \
		$(OBJPATH)/OVR_Log.o \
		$(OBJPATH)/OVR_Math.o \
		$(OBJPATH)/OVR_RefCount.o \
		$(OBJPATH)/OVR_Std.o \
		$(OBJPATH)/OVR_String.o \
		$(OBJPATH)/OVR_String_FormatUtil.o \
		$(OBJPATH)/OVR_String_PathUtil.o \
		$(OBJPATH)/OVR_SysFile.o \
		$(OBJPATH)/OVR_System.o \
		$(OBJPATH)/OVR_Timer.o \
		$(OBJPATH)/OVR_UTF8Util.o \
		$(OBJPATH)/Util_LatencyTest.o \
		$(OBJPATH)/Util_Render_Stereo.o \
		$(OBJPATH)/OVR_ThreadsPthread.o \
		$(OBJPATH)/tinyxml2.o

ifeq ($(shell uname), Darwin)

LIBDIR:=OSX

OBJECTS+=  \
		$(OBJPATH)/OVR_OSX_HIDDevice.o \
		$(OBJPATH)/OVR_OSX_SensorDevice.o \
		$(OBJPATH)/OVR_OSX_DeviceManager.o \
		$(OBJPATH)/OVR_OSX_HMDDevice.o
else

LIBDIR=Linux
OBJECTS+=  \
		$(OBJPATH)/OVR_Linux_HIDDevice.o \
		$(OBJPATH)/OVR_Linux_SensorDevice.o \
		$(OBJPATH)/OVR_Linux_DeviceManager.o \
		$(OBJPATH)/OVR_Linux_HMDDevice.o
endif
####### Rules

all: $(OBJPATH)   $(TARGET)

$(TARGET):  $(OBJECTS)
	mkdir -p $(dir $(TARGET))
	$(LINK) $(TARGET) $(OBJECTS)

$(OBJPATH):
	mkdir -p $(OBJPATH)

$(OBJPATH)/%.o: $(LIBOVRPATH)/Src/Util/%.cpp 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o $@ $<
$(OBJPATH)/%.o: $(LIBOVRPATH)/Src/Kernel/%.cpp 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o $@ $<
$(OBJPATH)/%.o: $(LIBOVRPATH)/Src/%.cpp 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o $@ $<

$(OBJPATH)/tinyxml2.o: $(3RDPARTYPATH)/TinyXml/tinyxml2.cpp 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o $@ $<

clean:
	-$(DELETEFILE) $(OBJECTS)
	-$(DELETEFILE) $(TARGET)

