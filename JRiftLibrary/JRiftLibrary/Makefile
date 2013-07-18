UNAME := $(shell uname)

OCULUS_SDK:=..

ifeq ($(UNAME), Darwin)

all: libJRiftLibrary64.jnilib libJRiftLibrary.jnilib

JRiftLibrary.o: JRiftLibrary.cpp
	g++ \
	-fno-rtti \
	-m32 \
	"-I/System/Library/Frameworks/JavaVM.framework/Versions/Current/Headers" \
	"-I/Developer/SDKs/MacOSX10.6.sdk/System/Library/Frameworks/JavaVM.framework/Versions/A/Headers" \
	"-I${OCULUS_SDK}/LibOVR/Include" \
	-c JRiftLibrary.cpp \
	-o $@

JRiftLibrary64.o: JRiftLibrary.cpp
	g++ \
	-fno-rtti \
	"-I/System/Library/Frameworks/JavaVM.framework/Versions/Current/Headers" \
	"-I/Developer/SDKs/MacOSX10.6.sdk/System/Library/Frameworks/JavaVM.framework/Versions/A/Headers" \
	"-I${OCULUS_SDK}/LibOVR/Include" \
	-c JRiftLibrary.cpp \
	-o $@

libJRiftLibrary.jnilib: JRiftLibrary.o
	g++ \
		-dynamiclib \
		-fno-rtti \
		-m32 \
		-framework ApplicationServices \
		-framework IOKit \
		-L ${OCULUS_SDK}/LibOVR/Lib/OSX/Release/i386/ \
		-o $@ \
		JRiftLibrary.o \
		-lovr

libJRiftLibrary64.jnilib: JRiftLibrary64.o
	g++ \
		-dynamiclib \
		-fno-rtti \
		-framework ApplicationServices \
		-framework IOKit \
		-L ${OCULUS_SDK}/LibOVR/Lib/OSX/Release/x86_64/ \
		-o $@ \
		JRiftLibrary64.o \
		-lovr

else
ifeq ($(UNAME), Linux)


all: libJRiftLibrary64.so libJRiftLibrary.so


JRiftLibrary.o: JRiftLibrary.cpp
	g++ \
	-m32 \
	-fPIC \
	-fno-rtti \
	"-I/usr/lib/jvm/java-6-openjdk-amd64/include/" \
	"-I${OCULUS_SDK}/LibOVR/Include" \
	-c $< \
	-o $@

JRiftLibrary64.o: JRiftLibrary.cpp
	g++ \
	-fPIC \
	-fno-rtti \
	"-I/usr/lib/jvm/java-6-openjdk-amd64/include/" \
	"-I${OCULUS_SDK}/LibOVR/Include" \
	-c $< \
	-o $@

libJRiftLibrary.so: JRiftLibrary.o ${OCULUS_SDK}/LibOVR/Linux/Release/i386/libOVR.a 
	g++ \
		-m32 \
		-fno-rtti \
		-shared \
		-o $@ \
		"-I/usr/lib/jvm/java-6-openjdk-amd64/include/" \
		-L ${OCULUS_SDK}/LibOVR/Lib/Linux/Release/i386/ \
		$< \
		-lovr -l:libudev.so.0 -l:libXinerama.so.1

libJRiftLibrary64.so: JRiftLibrary64.o ${OCULUS_SDK}/LibOVR/Lib/Linux/Release/x86_64/libOVR.a 
	g++ \
		-fno-rtti \
		-shared \
		-o $@ \
		"-I/usr/lib/jvm/java-6-openjdk-amd64/include/" \
		-L ${OCULUS_SDK}/LibOVR/Lib/Linux/Release/x86_64/ \
		$< \
		-lovr -ludev -lXinerama

${OCULUS_SDK}/LibOVR/Lib/Linux/Release/x86_64/libOVR.a:
	make -C ${OCULUS_SDK}/LibOVR
${OCULUS_SDK}/LibOVR/Linux/Release/i386/libOVR.a : 
	linux32 make -C ${OCULUS_SDK}/LibOVR
.PHONY: ${OCULUS_SDK}/LibOVR/Lib/Linux/Release/x86_64/libOVR.a ${OCULUS_SDK}/LibOVR/Linux/Release/i386/libOVR.a
endif
endif

clean:
	rm -f *.o
	rm -f a.out
	rm -f *.jnilib
	rm -rf *.so

