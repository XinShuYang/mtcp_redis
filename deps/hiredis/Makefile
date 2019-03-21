
# Hiredis Makefile
# Copyright (C) 2010-2011 Salvatore Sanfilippo <antirez at gmail dot com>
# Copyright (C) 2010-2011 Pieter Noordhuis <pcnoordhuis at gmail dot com>
# This file is released under the BSD license, see the COPYING file

#mtcp
CC=gcc -g -O3 -Wall -Werror -fgnu89-inline
DPDK=1
PS=0
NETMAP=0
ONVM=0
CFLAGS=-DMAX_CPUS=20

# DPDK LIBRARY and HEADER
DPDK_INC=/local/onvm/openNetVM/dpdk/x86_64-native-linuxapp-gcc/include
DPDK_LIB=/local/onvm/openNetVM/dpdk/x86_64-native-linuxapp-gcc/lib/

MTCP_FLD    =../../mtcp
MTCP_INC    =-I${MTCP_FLD}/include
MTCP_LIB    =-L${MTCP_FLD}/lib
MTCP_TARGET = ${MTCP_LIB}/libmtcp.a

UTIL_FLD = ../../util
UTIL_INC = -I${UTIL_FLD}/include
UTIL_OBJ = ${UTIL_FLD}/http_parsing.o ${UTIL_FLD}/tdate_parse.o ${UTIL_FLD}/netlib.o


PS_DIR = ../io_engine/
PS_INC = ${PS_DIR}/include
INC = -I./include/ ${UTIL_INC} ${MTCP_INC} -I${UTIL_FLD}/include
LIBS = ${MTCP_LIB}
ifeq ($(PS),1)
INC += -I{PS_INC}
LIBS += -lmtcp -L${PS_DIR}/lib -lps -lpthread -lnuma -lrt
endif

ifeq ($(NETMAP),1)
LIBS += -lmtcp -lpthread -lnuma -lrt
endif
# CFLAGS for DPDK-related compilation
INC += ${MTCP_INC}
ifeq ($(DPDK),1)
DPDK_MACHINE_FLAGS = $(shell cat /local/onvm/openNetVM/dpdk/x86_64-native-linuxapp-gcc/include/cflags.txt)
INC += ${DPDK_MACHINE_FLAGS} -I${DPDK_INC} -include $(DPDK_INC)/rte_config.h
endif

ifeq ($(shell uname -m),x86_64)
LIBS += -m64
endif

ifeq ($(DPDK),1)
DPDK_LIB_FLAGS = $(shell cat /local/onvm/openNetVM/dpdk/x86_64-native-linuxapp-gcc/lib/ldflags.txt)
#LIBS += -m64 -g -O3 -pthread -lrt -march=native -Wl,-export-dynamic ${MTCP_FLD}/lib/libmtcp.a -L../../dpdk/lib -Wl,-lnuma -Wl,-lmtcp -Wl,-lpthread -Wl,-lrt -Wl,-ldl -Wl,${DPDK_LIB_FLAGS}
LIBS += -g -O3 -pthread -lrt -march=native -export-dynamic ${MTCP_FLD}/lib/libmtcp.a -L../../dpdk/lib -lnuma -lmtcp -lpthread -lrt -ldl ${DPDK_LIB_FLAGS}
else
#LIBS += -m64 -g -O3 -pthread -lrt -march=native -Wl,-export-dynamic ${MTCP_FLD}/lib/libmtcp.a -L../../dpdk/lib -Wl,-lnuma -Wl,-lmtcp -Wl,-lpthread -Wl,-lrt -Wl,-ldl -Wl,${DPDK_LIB_FLAGS}
LIBS += -g -O3 -pthread -lrt -march=native -export-dynamic ${MTCP_FLD}/lib/libmtcp.a -L../../dpdk/lib -lnuma -lmtcp -lpthread -lrt -ldl ${DPDK_LIB_FLAGS}
endif

ifeq ($(ONVM),1)
ifeq ($(RTE_TARGET),)
$(error "Please define RTE_TARGET environment variable")
endif

INC += -I/onvm_nflib
INC += -DENABLE_ONVM
LIBS += /onvm_nflib/$(RTE_TARGET)/libonvm.a
endif
ifeq ($V,) # no echo
        export MSG=@echo
        export HIDE=@
else
        export MSG=@\#
        export HIDE=
endif
#-------------------------------------------

OBJ=net.o hiredis.o sds.o async.o read.o
EXAMPLES=hiredis-example hiredis-example-libevent hiredis-example-libev hiredis-example-glib
TESTS=hiredis-test
LIBNAME=libhiredis
PKGCONFNAME=hiredis.pc

HIREDIS_MAJOR=$(shell grep HIREDIS_MAJOR hiredis.h | awk '{print $$3}')
HIREDIS_MINOR=$(shell grep HIREDIS_MINOR hiredis.h | awk '{print $$3}')
HIREDIS_PATCH=$(shell grep HIREDIS_PATCH hiredis.h | awk '{print $$3}')
HIREDIS_SONAME=$(shell grep HIREDIS_SONAME hiredis.h | awk '{print $$3}')

# Installation related variables and target
PREFIX?=/usr/local
INCLUDE_PATH?=include/hiredis
LIBRARY_PATH?=lib
PKGCONF_PATH?=pkgconfig
INSTALL_INCLUDE_PATH= $(DESTDIR)$(PREFIX)/$(INCLUDE_PATH)
INSTALL_LIBRARY_PATH= $(DESTDIR)$(PREFIX)/$(LIBRARY_PATH)
INSTALL_PKGCONF_PATH= $(INSTALL_LIBRARY_PATH)/$(PKGCONF_PATH)

# redis-server configuration used for testing
REDIS_PORT=56379
REDIS_SERVER=redis-server
define REDIS_TEST_CONFIG
	daemonize yes
	pidfile /tmp/hiredis-test-redis.pid
	port $(REDIS_PORT)
	bind 127.0.0.1
	unixsocket /tmp/hiredis-test-redis.sock
endef
export REDIS_TEST_CONFIG
DPDK_LIB_FLAGS = $(shell cat /local/onvm/openNetVM/dpdk/x86_64-native-linuxapp-gcc/lib/ldflags.txt)
# Fallback to gcc when $CC is not in $PATH.
CC:=$(shell sh -c 'type $${CC%% *} >/dev/null 2>/dev/null && echo $(CC) || echo gcc')
CXX:=$(shell sh -c 'type $${CXX%% *} >/dev/null 2>/dev/null && echo $(CXX) || echo g++')
OPTIMIZATION?=-O3
WARNINGS=-Wall -W -Wstrict-prototypes -Wwrite-strings
DEBUG_FLAGS?= -g -ggdb ${MTCP_FLD}/lib/libmtcp.a -L ../../dpdk/lib -lnuma -lmtcp -lpthread -lrt -ldl ${DPDK_LIB_FLAGS}
REAL_CFLAGS=$(OPTIMIZATION) -fPIC $(CPPFLAGS) $(CFLAGS) $(WARNINGS) $(DEBUG_FLAGS)
REAL_LDFLAGS=$(LDFLAGS)
DYLIBSUFFIX=so
STLIBSUFFIX=a
DYLIB_MINOR_NAME=$(LIBNAME).$(DYLIBSUFFIX).$(HIREDIS_SONAME)
DYLIB_MAJOR_NAME=$(LIBNAME).$(DYLIBSUFFIX).$(HIREDIS_MAJOR)
DYLIBNAME=$(LIBNAME).$(DYLIBSUFFIX)
DYLIB_MAKE_CMD=$(CC) -shared -Wl,-soname,$(DYLIB_MINOR_NAME) -o $(DYLIBNAME) $(LDFLAGS)
STLIBNAME=$(LIBNAME).$(STLIBSUFFIX)
STLIB_MAKE_CMD=ar rcs $(STLIBNAME)

# Platform-specific overrides
uname_S := $(shell sh -c 'uname -s 2>/dev/null || echo not')
ifeq ($(uname_S),SunOS)
  REAL_LDFLAGS+= -ldl -lnsl -lsocket
  DYLIB_MAKE_CMD=$(CC) -G -o $(DYLIBNAME) -h $(DYLIB_MINOR_NAME) $(LDFLAGS)
endif
ifeq ($(uname_S),Darwin)
  DYLIBSUFFIX=dylib
  DYLIB_MINOR_NAME=$(LIBNAME).$(HIREDIS_SONAME).$(DYLIBSUFFIX)
  DYLIB_MAKE_CMD=$(CC) -dynamiclib -Wl,-install_name,$(PREFIX)/$(LIBRARY_PATH)/$(DYLIB_MINOR_NAME) -o $(DYLIBNAME) $(LDFLAGS)
endif

all: $(DYLIBNAME) $(STLIBNAME) hiredis-test $(PKGCONFNAME)

# Deps (use make dep to generate this)
async.o: async.c fmacros.h async.h hiredis.h read.h sds.h net.h dict.c dict.h
dict.o: dict.c fmacros.h dict.h
hiredis.o: hiredis.c fmacros.h hiredis.h read.h sds.h net.h
net.o: net.c fmacros.h net.h hiredis.h read.h sds.h ../../src/mod.h
read.o: read.c fmacros.h read.h sds.h
sds.o: sds.c sds.h
test.o: test.c fmacros.h hiredis.h read.h sds.h

$(DYLIBNAME): $(OBJ)
	$(DYLIB_MAKE_CMD) $(OBJ)

$(STLIBNAME): $(OBJ)
	$(STLIB_MAKE_CMD) $(OBJ)

dynamic: $(DYLIBNAME)
static: $(STLIBNAME)

# Binaries:
hiredis-example-libevent: examples/example-libevent.c adapters/libevent.h $(STLIBNAME)
	$(CC) -o examples/$@ ${MTCP_FLD}/lib/libmtcp.a  $(REAL_CFLAGS) $(REAL_LDFLAGS) -I. $< -levent $(STLIBNAME)

hiredis-example-libev: examples/example-libev.c adapters/libev.h $(STLIBNAME)
	$(CC) -o examples/$@ ${MTCP_FLD}/lib/libmtcp.a  $(REAL_CFLAGS) $(REAL_LDFLAGS) -I. $< -lev $(STLIBNAME)

hiredis-example-glib: examples/example-glib.c adapters/glib.h $(STLIBNAME)
	$(CC) -o examples/$@ ${MTCP_FLD}/lib/libmtcp.a  $(REAL_CFLAGS) $(REAL_LDFLAGS) -I. $< $(shell pkg-config --cflags --libs glib-2.0) $(STLIBNAME)

hiredis-example-ivykis: examples/example-ivykis.c adapters/ivykis.h $(STLIBNAME)
	$(CC) -o examples/$@ ${MTCP_FLD}/lib/libmtcp.a  $(REAL_CFLAGS) $(REAL_LDFLAGS) -I. $< -livykis $(STLIBNAME)

hiredis-example-macosx: examples/example-macosx.c adapters/macosx.h $(STLIBNAME)
	$(CC) -o examples/$@ ${MTCP_FLD}/lib/libmtcp.a  $(REAL_CFLAGS) $(REAL_LDFLAGS) -I. $< -framework CoreFoundation $(STLIBNAME)

ifndef AE_DIR
hiredis-example-ae:
	@echo "Please specify AE_DIR (e.g. <redis repository>/src)"
	@false
else
hiredis-example-ae: examples/example-ae.c adapters/ae.h $(STLIBNAME)
	$(CC) -o examples/$@ $(REAL_CFLAGS) $(REAL_LDFLAGS) -I. -I$(AE_DIR) $< $(AE_DIR)/ae.o $(AE_DIR)/zmalloc.o $(AE_DIR)/../deps/jemalloc/lib/libjemalloc.a -pthread $(STLIBNAME)
endif

ifndef LIBUV_DIR
hiredis-example-libuv:
	@echo "Please specify LIBUV_DIR (e.g. ../libuv/)"
	@false
else
hiredis-example-libuv: examples/example-libuv.c adapters/libuv.h $(STLIBNAME)
	$(CC) -o examples/$@ $(REAL_CFLAGS) $(REAL_LDFLAGS) -I. -I$(LIBUV_DIR)/include $< $(LIBUV_DIR)/.libs/libuv.a -lpthread -lrt $(STLIBNAME)
endif

ifeq ($(and $(QT_MOC),$(QT_INCLUDE_DIR),$(QT_LIBRARY_DIR)),)
hiredis-example-qt:
	@echo "Please specify QT_MOC, QT_INCLUDE_DIR AND QT_LIBRARY_DIR"
	@false
else
hiredis-example-qt: examples/example-qt.cpp adapters/qt.h $(STLIBNAME)
	$(QT_MOC) adapters/qt.h -I. -I$(QT_INCLUDE_DIR) -I$(QT_INCLUDE_DIR)/QtCore | \
	    $(CXX) -x c++ -o qt-adapter-moc.o -c - $(REAL_CFLAGS) -I. -I$(QT_INCLUDE_DIR) -I$(QT_INCLUDE_DIR)/QtCore
	$(QT_MOC) examples/example-qt.h -I. -I$(QT_INCLUDE_DIR) -I$(QT_INCLUDE_DIR)/QtCore | \
	    $(CXX) -x c++ -o qt-example-moc.o -c - $(REAL_CFLAGS) -I. -I$(QT_INCLUDE_DIR) -I$(QT_INCLUDE_DIR)/QtCore
	$(CXX) -o examples/$@ $(REAL_CFLAGS) $(REAL_LDFLAGS) -I. -I$(QT_INCLUDE_DIR) -I$(QT_INCLUDE_DIR)/QtCore -L$(QT_LIBRARY_DIR) qt-adapter-moc.o qt-example-moc.o $< -pthread $(STLIBNAME) -lQtCore
endif

hiredis-example: examples/example.c $(STLIBNAME)
	$(CC) -o examples/$@ ${MTCP_FLD}/lib/libmtcp.a  $(REAL_CFLAGS) $(REAL_LDFLAGS) -I. $< $(STLIBNAME)

examples: $(EXAMPLES)

hiredis-test:  test.o $(STLIBNAME)
	${CC} ${LIBS} ${UTIL_OBJ} -o $@ $^ ${MTCP_FLD}/lib/libmtcp.a 

hiredis-%: %.o $(STLIBNAME)
	$(CC) $(REAL_CFLAGS) -o $@ $(REAL_LDFLAGS) $< $(STLIBNAME)

test: hiredis-test
	./hiredis-test

check: hiredis-test
	@echo "$$REDIS_TEST_CONFIG" | $(REDIS_SERVER) -
	$(PRE) ./hiredis-test -h 127.0.0.1 -p $(REDIS_PORT) -s /tmp/hiredis-test-redis.sock || \
			( kill `cat /tmp/hiredis-test-redis.pid` && false )
	kill `cat /tmp/hiredis-test-redis.pid`

.c.o:
	$(CC) -std=c99 -pedantic -c $(REAL_CFLAGS) $<

clean:
	rm -rf $(DYLIBNAME) $(STLIBNAME) $(TESTS) $(PKGCONFNAME) examples/hiredis-example* *.o *.gcda *.gcno *.gcov

dep:
	$(CC) -MM *.c

INSTALL?= cp -pPR

$(PKGCONFNAME): hiredis.h
	@echo "Generating $@ for pkgconfig..."
	@echo prefix=$(PREFIX) > $@
	@echo exec_prefix=\$${prefix} >> $@
	@echo libdir=$(PREFIX)/$(LIBRARY_PATH) >> $@
	@echo includedir=$(PREFIX)/$(INCLUDE_PATH) >> $@
	@echo >> $@
	@echo Name: hiredis >> $@
	@echo Description: Minimalistic C client library for Redis. >> $@
	@echo Version: $(HIREDIS_MAJOR).$(HIREDIS_MINOR).$(HIREDIS_PATCH) >> $@
	@echo Libs: -L\$${libdir} -lhiredis >> $@
	@echo Cflags: -I\$${includedir} -D_FILE_OFFSET_BITS=64 >> $@

install: $(DYLIBNAME) $(STLIBNAME) $(PKGCONFNAME)
	mkdir -p $(INSTALL_INCLUDE_PATH) $(INSTALL_INCLUDE_PATH)/adapters $(INSTALL_LIBRARY_PATH)
	$(INSTALL) hiredis.h async.h read.h sds.h $(INSTALL_INCLUDE_PATH)
	$(INSTALL) adapters/*.h $(INSTALL_INCLUDE_PATH)/adapters
	$(INSTALL) $(DYLIBNAME) $(INSTALL_LIBRARY_PATH)/$(DYLIB_MINOR_NAME)
	cd $(INSTALL_LIBRARY_PATH) && ln -sf $(DYLIB_MINOR_NAME) $(DYLIBNAME)
	$(INSTALL) $(STLIBNAME) $(INSTALL_LIBRARY_PATH)
	mkdir -p $(INSTALL_PKGCONF_PATH)
	$(INSTALL) $(PKGCONFNAME) $(INSTALL_PKGCONF_PATH)

32bit:
	@echo ""
	@echo "WARNING: if this fails under Linux you probably need to install libc6-dev-i386"
	@echo ""
	$(MAKE) CFLAGS="-m32" LDFLAGS="-m32"

32bit-vars:
	$(eval CFLAGS=-m32)
	$(eval LDFLAGS=-m32)

gprof:
	$(MAKE) CFLAGS="-pg" LDFLAGS="-pg"

gcov:
	$(MAKE) CFLAGS="-fprofile-arcs -ftest-coverage" LDFLAGS="-fprofile-arcs"

coverage: gcov
	make check
	mkdir -p tmp/lcov
	lcov -d . -c -o tmp/lcov/hiredis.info
	genhtml --legend -o tmp/lcov/report tmp/lcov/hiredis.info

noopt:
	$(MAKE) OPTIMIZATION=""

.PHONY: all test check clean dep install 32bit 32bit-vars gprof gcov noopt
