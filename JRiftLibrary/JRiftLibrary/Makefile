UNAME := $(shell uname)

OCULUS_SDK:=..

ifeq ($(UNAME), Darwin)

all: libJRiftLibrary64.jnilib


JRiftLibrary.o:
	g++ \
	-fno-rtti \
	"-I/System/Library/Frameworks/JavaVM.framework/Versions/Current/Headers" \
	"-I/Developer/SDKs/MacOSX10.6.sdk/System/Library/Frameworks/JavaVM.framework/Versions/A/Headers" \
	"-I${OCULUS_SDK}/LibOVR/Include" \
	-c JRiftLibrary.cpp

libJRiftLibrary64.jnilib: JRiftLibrary.o
	g++ \
		-dynamiclib \
		-fno-rtti \
		-framework ApplicationServices \
		-framework IOKit \
		-o libJRiftLibrary.jnilib \
		JRiftLibrary.o \
		${OCULUS_SDK}/LibOVR/Src/libOVR.a

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

libJRiftLibrary.so: JRiftLibrary.o ${OCULUS_SDK}/LibOVR/Src/libOVR32.a
	g++ \
		-m32 \
		-fno-rtti \
		-shared \
		-o $@ \
		"-I/usr/lib/jvm/java-6-openjdk-amd64/include/" \
		$< \
		${OCULUS_SDK}/LibOVR/Src/libOVR32.a

libJRiftLibrary64.so: JRiftLibrary64.o ${OCULUS_SDK}/LibOVR/Src/libOVR.a
	g++ \
		-fno-rtti \
		-shared \
		-o $@ \
		"-I/usr/lib/jvm/java-6-openjdk-amd64/include/" \
		$< \
		${OCULUS_SDK}/LibOVR/Src/libOVR.a

${OCULUS_SDK}/LibOVR/Src/libOVR.a ${OCULUS_SDK}/LibOVR/Src/libOVR32.a : 
	make -C ${OCULUS_SDK}/LibOVR/Src
.PHONY: ${OCULUS_SDK}/LibOVR/Src/libOVR.a ${OCULUS_SDK}/LibOVR/Src/libOVR32.a
endif
endif

clean:
	rm -f *.o
	rm -f a.out
	rm -f *.jnilib
	rm -rf *.so

