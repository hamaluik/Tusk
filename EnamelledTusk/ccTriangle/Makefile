CC=g++
CFLAGS=-DSTANDALONE -D__STDC_CONSTANT_MACROS -D__STDC_LIMIT_MACROS -DTARGET_POSIX -D_LINUX -fPIC -DPIC -D_REENTRANT -D_LARGEFILE64_SOURCE -D_FILE_OFFSET_BITS=64 -U_FORTIFY_SOURCE -Wall -g -DHAVE_LIBOPENMAX=2 -DOMX -DOMX_SKIP64BIT -ftree-vectorize -pipe -DUSE_EXTERNAL_OMX -DHAVE_LIBBCM_HOST -DUSE_EXTERNAL_LIBBCM_HOST -DUSE_VCHIQ_ARM -Wno-psabi
LDFLAGS=-L$(SDKSTAGE)/opt/vc/lib/ -lGLESv2 -lEGL -lopenmaxil -lbcm_host -lvcos -lvchiq_arm -lpthread -lrt -L../libs/ilclient
INCLUDES=-I$(SDKSTAGE)/opt/vc/include/ -I$(SDKSTAGE)/opt/vc/include/interface/vcos/pthreads -I./ -I../libs/ilclient
DEBUG=-g

bin/triangle: o/triangle.o o/eglManager.o o/oglManager.o
	$(CC) -o bin/triangle -Wl,--whole-archive $(LDFLAGS) -Wl,--no-whole-archive -rdynamic o/oglManager.o o/eglManager.o o/triangle.o

o/triangle.o: src/triangle.cc
	$(CC) $(CFLAGS) $(INCLUDES) $(DEBUG) -Wno-deprecated-declarations -c -o o/triangle.o src/triangle.cc

o/eglManager.o: src/eglManager.h src/eglManager.cc
	$(CC) $(CFLAGS) $(INCLUDES) $(DEBUG) -Wno-deprecated-declarations -c -o o/eglManager.o src/eglManager.cc
	
o/oglManager.o: src/oglManager.h src/oglManager.cc
	$(CC) $(CFLAGS) $(INCLUDES) $(DEBUG) -Wno-deprecated-declarations -c -o o/oglManager.o src/oglManager.cc
	
.PHONY: run
run:
	./bin/triangle
	
.PHONY: clean
clean:
	rm -v o/* bin/*