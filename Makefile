all: 
	make -C JRift

bin:
	make -C JRiftLibrary

clean:
	make -C JRiftLibrary/LibOVR/Src clean
	make -C JRiftLibrary clean
	make -C JRift clean
