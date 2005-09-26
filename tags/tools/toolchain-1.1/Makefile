# Makefile to create a gcc toolchain for Topfield TAP development
#
# Mon Sep 12 21:49:51 EST 2005
# Created by Steve Bennett <steveb@workware.net.au>
# 
# Just type make
# 
# See README for full details

TARGET=mips
PREFIX=/usr/local/topfield-gcc
SDKDIR=$(PREFIX)/topfield-sdk

DIST=topfield-toolchain-1.1
PATCH=patch -g 0

# Add the target bin directory to the path so that
# post-binutils build steps can find the binutils tools
export PATH := $(PREFIX)/bin:$(PATH)

# These flags are all required when building Topfield TAP applications
# so use them when building libgcc.a and newlib (libc.a and libm.a)
# Note that we don't need to specify -msoft-float since that is covered by multilib
export TARGET_CFLAGS=-mlong-calls -mqnxpic -fno-delayed-branch

OPTS=--with-float=soft --disable-shared --disable-newlib-multithread --disable-threads --disable-nls --with-gnu-as --with-gnu-ld

all: targetdir build-gcc/.built taptest/.built
	@echo "************************************************"
	@echo
	@echo " Topfield toolchain built, installed and tested"
	@echo " Tools are installed in $(PREFIX)"
	@echo " See the taptest directory for sample TAPs"
	@echo
	@echo "          Steve Bennett <steveb@workware.net.au>"
	@echo
	@echo "************************************************"

#################################################
# Unpack and patch various packages
#
binutils-2.15/.unpacked: pkg/binutils-2.15.tar.bz2 patches/jeff-baker-binutils-qnx.patch patches/binutils-2.15-20040817.patch
	@echo "Unpacking and patching binutils-2.15"
	rm -rf binutils-2.15
	tar xfj pkg/binutils-2.15.tar.bz2
	$(PATCH) <patches/binutils-2.15-20040817.patch -p1 -d binutils-2.15
	$(PATCH) <patches/jeff-baker-binutils-qnx.patch -p0 -d binutils-2.15
	touch $@

binutils-2.10.1/.unpacked: pkg/binutils-2.10.1.tar.gz patches/qnx-binutils-2.10.1.patch patches/steveb-binutils-topfield-qnx-1.patch patches/cazlar-osx-binutils-1.patch
	@echo "Unpacking and patching binutils-2.10.1"
	rm -rf binutils-2.10.1
	tar xfz pkg/binutils-2.10.1.tar.gz
	$(PATCH) <patches/qnx-binutils-2.10.1.patch -p1 -d binutils-2.10.1
	$(PATCH) <patches/steveb-binutils-topfield-qnx-1.patch -p1 -d binutils-2.10.1
	$(PATCH) <patches/cazlar-osx-binutils-1.patch -p1 -d binutils-2.10.1
	touch $@

newlib-1.13.0/.unpacked: pkg/newlib-1.13.0.tar.gz
	@echo "Unpacking newlib"
	rm -rf newlib-1.13.0
	tar xfz pkg/newlib-1.13.0.tar.gz
	touch $@

gcc-2.95.2/.unpacked: pkg/gcc-2.95.2.tar.gz patches/qnx-gcc-2.95.2.patch patches/steveb-gcc-topfield-qnx-1.patch patches/cazlar-osx-gcc-1.patch
	@echo "Unpacking and patching gcc 2.95.2"
	rm -rf gcc-2.95.2
	tar xfz pkg/gcc-2.95.2.tar.gz
	$(PATCH) <patches/qnx-gcc-2.95.2.patch -p1 -d gcc-2.95.2
	$(PATCH) <patches/steveb-gcc-topfield-qnx-1.patch -p1 -d gcc-2.95.2
	$(PATCH) <patches/cazlar-osx-gcc-1.patch -p1 -d gcc-2.95.2
	touch $@

# Manually unpack everything
unpack: binutils-2.15/.unpacked binutils-2.10.1/.unpacked newlib-1.13.0/.unpacked gcc-2.95.2/.unpacked

#################################################
# Configure and build the various packages
#
build-binutils/.built: binutils-2.10.1/.unpacked
	@echo "Building binutils"
	@rm -rf build-binutils
	( \
		mkdir build-binutils; \
		cd build-binutils; \
		sh ../binutils-2.10.1/configure --target=$(TARGET) --prefix=$(PREFIX) $(OPTS) && \
		make && make install; \
	)
	touch $@

build-ld/.built: tap.ld binutils-2.15/.unpacked build-binutils/.built
	@echo "Building binutils (ld)"
	@rm -rf build-ld
	( \
		mkdir build-ld; \
		cd build-ld; \
		sh ../binutils-2.15/configure --target=$(TARGET) --prefix=$(PREFIX) $(OPTS) && \
		make all-ld && make install-ld && cp ../tap.ld $(PREFIX)/$(TARGET)/lib; \
	)
	touch $@

build-bootstrapgcc/.built: gcc-2.95.2/.unpacked build-binutils/.built build-ld/.built
	@echo "Building bootstrap gcc"
	@rm -rf build-bootstrapgcc
	( \
		mkdir build-bootstrapgcc; \
		cd build-bootstrapgcc; \
		sh ../gcc-2.95.2/configure --target=$(TARGET) --prefix=$(PREFIX) $(OPTS) \
			--without-headers --with-newlib --with-gnu-as --with-gnu-ld \
			--with-local-prefix=$(PREFIX)/local \
			--enable-haifa --enable-languages=c --disable-texinfo --verbose && \
		make all-gcc install-gcc; \
	)
	touch $@

build-gcc/.built: gcc-2.95.2/.unpacked build-newlib/.built
	@echo "Building gcc"
	@rm -rf build-gcc
	( \
		mkdir build-gcc; \
		cd build-gcc; \
		sh ../gcc-2.95.2/configure --target=$(TARGET) --prefix=$(PREFIX) $(OPTS) \
			--with-newlib --with-gnu-as --with-gnu-ld \
			--with-local-prefix=$(PREFIX)/local \
			--enable-haifa --enable-languages=c --disable-texinfo --verbose && \
		make && make install; \
	)
	touch $@

build-newlib/.built: newlib-1.13.0/.unpacked build-bootstrapgcc/.built
	@echo "Building newlib"
	/bin/rm -rf build-newlib
	( \
		mkdir build-newlib; \
		cd build-newlib; \
		TARGET_CFLAGS="$(TARGET_CFLAGS) -DMALLOC_PROVIDED -DNO_EXEC"; \
		sh ../newlib-1.13.0/configure --target=$(TARGET) --prefix=$(PREFIX) $(OPTS) --with-pic && \
		make && make install; \
	)
	touch $@

#################################################
# Unpack the Topfield TAP SDK
#
$(SDKDIR)/include/tap.h: pkg/TAP\ API_ver1.22_Samples_Utilities_2005June03.zip
	unzip -o -C "$^" -d pkg tap_and_samples_2005June03.zip
	mkdir -p $(SDKDIR)/include $(SDKDIR)/lib
	unzip -o -d $(SDKDIR)/lib -LL -C pkg/tap_and_samples_2005June03.zip \*.a
	unzip -o -d $(SDKDIR)/include -LL -C pkg/tap_and_samples_2005June03.zip \*.h
	touch $@

#################################################
# Build the test taps
#
taptest/.built: taptest/.unpacked $(SDKDIR)/include/tap.h
	@echo "Building taptest"
	$(MAKE) -C taptest
	touch $@

targetdir:
	@touch $(PREFIX)/.tmp || ( echo "Unable to write to $(PREFIX), make sure it exists and is writable"; exit 1 )
	@rm $(PREFIX)/.tmp
	
#################################################
# Fetch any missing packages automagically
#
pkg/binutils-2.15.tar.bz2:
	@sh fetch $@ http://ftp.gnu.org/gnu/binutils/binutils-2.15.tar.bz2

pkg/binutils-2.10.1.tar.gz:
	@sh fetch $@ http://ftp.gnu.org/gnu/binutils/binutils-2.10.1.tar.gz

pkg/newlib-1.13.0.tar.gz:
	@sh fetch $@ ftp://sources.redhat.com/pub/newlib/newlib-1.13.0.tar.gz

pkg/gcc-2.95.2.tar.gz:
	@sh fetch $@ http://ftp.gnu.org/gnu/gcc/gcc-2.95.2.tar.gz

pkg/TAP\ API_ver1.22_Samples_Utilities_2005June03.zip:
	@sh fetch "$@" http://www.topfield.co.kr/data/product/firmware/TAP%20API_ver1.22_Samples_Utilities_2005June03.zip

#################################################
# Distribution maintenance
#
dist: README.html
	@rm -rf $(DIST)
	@mkdir -p $(DIST)/patches $(DIST)/pkg $(DIST)/taptest
	@cp README README.html Makefile fetch tap.ld $(DIST)
	@cp patches/* $(DIST)/patches
	@cp taptest/*.[ch] taptest/Makefile taptest/*.mak taptest/README $(DIST)/taptest
	@tar cfz $(DIST).tar.gz $(DIST)
	@rm -rf $(DIST)
	@echo "Created $(DIST).tar.gz"
	@tar cfz taptest.tar.gz taptest/[^.]*
	@echo "Created taptest.tar.gz"
	
clean:
	rm -rf build-*
	if [ -d taptest ]; then $(MAKE) -C taptest clean; fi
	rm -f taptest/.built

distclean: clean
	rm -rf binutils-2.15 binutils-2.10.1 gcc-2.95.2 newlib-1.13.0 $(DIST)

README.html: README
	text2html -n 4 README >$@ || ( rm $@; exit 1)
