DIST=jpz4085-FakePCIID
BUILDDIR=./build

.PHONY: all
all:
	xcodebuild -configuration Release
	xcodebuild -configuration Debug

.PHONY: clean
clean:
	xcodebuild clean -configuration Release
	xcodebuild clean -configuration Debug

.PHONY: distribution
distribution:
	if [ -e ./Distribution ]; then rm -r ./Distribution; fi
	mkdir ./Distribution
	cp README.md ./Distribution
	cp -R $(BUILDDIR)/Debug ./Distribution
	cp -R $(BUILDDIR)/Release ./Distribution
	mkdir ./Distribution/injectors
	cp -R AppleIntelKBLGraphicsFramebufferInjector_3e9x.kext ./Distribution/injectors
	cp -R BroadcomWiFiInjector.kext ./Distribution/injectors
	find ./Distribution -path *.DS_Store -delete
	find ./Distribution -path *.dSYM -exec echo rm -r {} \; >/tmp/org.voodoo.rm.dsym.sh
	chmod +x /tmp/org.voodoo.rm.dsym.sh
	/tmp/org.voodoo.rm.dsym.sh
	rm /tmp/org.voodoo.rm.dsym.sh
	ditto -c -k --sequesterRsrc --zlibCompressionLevel 9 ./Distribution ./Archive.zip
	mv ./Archive.zip ./Distribution/`date +$(DIST)-%Y-%m%d.zip`
