CC=gcc
LD=gcc
NAME=texture

CFLAGS=-DSTANDALONE -D__STDC_CONSTANT_MACROS -D__STDC_LIMIT_MACROS -DTARGET_POSIX -D_LINUX -fPIC -DPIC -D_REENTRANT -D_LARGEFILE64_SOURCE -D_FILE_OFFSET_BITS=64 -U_FORTIFY_SOURCE -Wall -g -DHAVE_LIBOPENMAX=2 -DOMX -DOMX_SKIP64BIT -ftree-vectorize -pipe -DUSE_EXTERNAL_OMX -DHAVE_LIBBCM_HOST -DUSE_EXTERNAL_LIBBCM_HOST -DUSE_VCHIQ_ARM -Wno-psabi
LDFLAGS=-L$(SDKSTAGE)/opt/vc/lib/ -lGLESv2 -lEGL -lopenmaxil -lbcm_host -lvcos -lvchiq_arm -lpthread -lrt -L../libs/ilclient -L../libs/vgfont
INCLUDES=-I$(SDKSTAGE)/opt/vc/include/ -I$(SDKSTAGE)/opt/vc/include/interface/vcos/pthreads -I./ -I../libs/ilclient -I../libs/vgfont

bin/$(NAME): o/$(NAME).o
	$(CC) -o bin/$(NAME) -Wl,--whole-archive o/$(NAME).o $(LDFLAGS) -Wl,--no-whole-archive -rdynamic

o/$(NAME).o: src/$(NAME).c
	$(CC) $(CFLAGS) $(INCLUDES) -g -c src/$(NAME).c -o o/$(NAME).o -Wno-deprecated-declarations

.PHONY: clean
clean:
	rm o/$(NAME).o bin/$(NAME)

.PHONE: run
run:
	./bin/$(NAME)
