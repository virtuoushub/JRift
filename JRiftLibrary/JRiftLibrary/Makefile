UNAME := $(shell uname)

ifeq ($(UNAME), Darwin)

all: libJRiftLibrary.jnilib


JRiftLibrary.o:
	g++ \
	-fno-rtti \
	"-I/System/Library/Frameworks/JavaVM.framework/Versions/Current/Headers" \
	"-I/Developer/SDKs/MacOSX10.6.sdk/System/Library/Frameworks/JavaVM.framework/Versions/A/Headers" \
	"-I${OCULUS_SDK}/LibOVR/Include" \
	-c JRiftLibrary.cpp

libJRiftLibrary.jnilib: JRiftLibrary.o
	g++ \
		-dynamiclib \
		-fno-rtti \
		-framework ApplicationServices \
		-framework IOKit \
		-o libJRiftLibrary.jnilib \
		JRiftLibrary.o \
	 	${OCULUS_SDK}/LibOVR/Lib/MacOS/Release/libovr.a

else
ifeq ($(UNAME), Linux)

all: libJRiftLibrary64.so libJRiftLibrary.so


JRiftLibrary.o: JRiftStub.cpp
	g++ \
	-m32 \
	-fPIC \
	-fno-rtti \
	"-I/usr/lib/jvm/java-6-openjdk-amd64/include/" \
	-c JRiftStub.cpp \
	-o $@

libJRiftLibrary.so: JRiftLibrary.o
	g++ \
		-m32 \
		-fno-rtti \
		-shared \
		-o $@ \
		"-I/usr/lib/jvm/java-6-openjdk-amd64/include/" \
		$<

JRiftLibrary64.o: JRiftStub.cpp
	g++ \
	-fPIC \
	-fno-rtti \
	"-I/usr/lib/jvm/java-6-openjdk-amd64/include/" \
	-c JRiftStub.cpp \
	-o $@

libJRiftLibrary64.so: JRiftLibrary64.o
	g++ \
		-fno-rtti \
		-shared \
		-o $@ \
		"-I/usr/lib/jvm/java-6-openjdk-amd64/include/" \
		$<
endif
endif

clean:
	rm -f *.o
	rm -f a.out
	rm -f *.jnilib
	rm -rf *.so

