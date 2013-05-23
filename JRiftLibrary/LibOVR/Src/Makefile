SRCS:= OVR_DeviceHandle.cpp \
OVR_DeviceImpl.cpp \
OVR_LatencyTestImpl.cpp \
OVR_SensorFilter.cpp \
OVR_SensorFusion.cpp \
OVR_SensorImpl.cpp \
OVR_ThreadCommandQueue.cpp \
Kernel/OVR_Alg.cpp \
Kernel/OVR_Allocator.cpp \
Kernel/OVR_Atomic.cpp \
Kernel/OVR_File.cpp \
Kernel/OVR_FileFILE.cpp \
Kernel/OVR_Log.cpp \
Kernel/OVR_Math.cpp \
Kernel/OVR_RefCount.cpp \
Kernel/OVR_Std.cpp \
Kernel/OVR_String.cpp \
Kernel/OVR_String_FormatUtil.cpp \
Kernel/OVR_String_PathUtil.cpp \
Kernel/OVR_SysFile.cpp \
Kernel/OVR_System.cpp \
Kernel/OVR_ThreadsPthread.cpp \
Kernel/OVR_Timer.cpp \
Kernel/OVR_UTF8Util.cpp \
Util/Util_LatencyTest.cpp \
Util/Util_MagCalibration.cpp \
Util/Util_Render_Stereo.cpp 

UNAME := $(shell uname)

OCULUS_SDK:=..

LIBS=libOVR.a

ifeq ($(UNAME), Darwin)

SRCS+= OVR_OSX_DeviceManager.cpp \
OVR_OSX_HIDDevice.cpp \
OVR_OSX_HMDDevice.cpp \
OVR_OSX_SensorDevice.cpp

else
SRCS+= OVR_Linux_DeviceManager.cpp \
OVR_Linux_HIDDevice.cpp \
OVR_Linux_HMDDevice.cpp \
OVR_Linux_SensorDevice.cpp

LIBS += libOVR32.a

endif

OBJS=$(SRCS:.cpp=.o)
OBJS32=$(SRCS:.cpp=.ox)


all: $(LIBS)

.cpp.o:
	g++ -Os -fPIC -I. -c $< -o $@ -Wno-attributes

%.ox: %.cpp
	g++ -Os -fPIC -m32 -I. -c $< -o $@ -Wno-attributes

clean:
	find -name '*.ox' -delete -o -name '*.o' -delete
	rm libOVR*a

libOVR.a: $(OBJS)
	ar rcs $@ $(OBJS) 

libOVR32.a: $(OBJS32)
	ar rcs $@ $(OBJS32) 
