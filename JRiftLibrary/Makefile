all:	32 64 copy

32:	build32 copy

64:	build64 copy	

build32:	
	make 32 -C JRiftLibrary

build64:	
	make 64 -C JRiftLibrary

copy:
	@echo "Windows builds needed manually"
	-@cp JRiftLibrary/*.so natives/linux/
	-@cp JRiftLibrary/*jnilib natives/osx/
	-@cp Release/*dll natives/windows/
	-@cp x64/Release/*dll natives/windows/

deploy:
	mvn deploy
	cd ../../repo/; git add de
	cd ../../repo/; git commit -m "Updating JRiftLibrary natives"

clean:
	make clean -C JRiftLibrary
