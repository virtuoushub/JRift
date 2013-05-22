all:
	make -C JRiftLibrary/JRiftLibrary
	make -C JRift

clean:
	make -C JRiftLibrary/LibOVR/Src clean
	make -C JRiftLibrary/JRiftLibrary clean
	make -C JRift clean

install: all
	cp JRift/target/ovr-1.0-SNAPSHOT.jar JRift.jar
	cd JRiftLibrary/JRiftLibrary/; zip ../../JRift.jar lib*so lib*dynlib


run:
	make -C JRift run
	
