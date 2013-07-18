all:
	make -C JRiftLibrary
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
