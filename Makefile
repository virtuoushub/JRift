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

zipmerge: install-bin
	zipmerge JRift.jar JRift/target/ovr-*.jar 
install-java:
	cp JRift/target/ovr-*.jar JRift.jar

run:
	make -C JRift run
	
