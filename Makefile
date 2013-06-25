all: java bin

java:
	make -C JRift

bin:
	make -C JRiftLibrary/JRiftLibrary

clean:
	make -C JRiftLibrary/LibOVR/Src clean
	make -C JRiftLibrary/JRiftLibrary clean
	make -C JRift clean

install-bin: bin
	cd JRiftLibrary/JRiftLibrary/; zip ../../JRift.jar lib*so lib*jnilib *dll
	cd JRiftLibrary/x64/Release; zip ../../../JRift.jar *dll
	cd JRiftLibrary/Release/; zip ../../JRift.jar *dll

install-java:
	cp JRift/target/ovr-1.0-SNAPSHOT.jar JRift.jar

run:
	make -C JRift run
	
